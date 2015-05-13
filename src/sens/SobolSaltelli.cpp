/**
 @file SobolSaltelli.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "SobolSaltelli.h"

#include <iostream>

BIO_NAMESPACE_BEGIN

SobolSaltelli::SobolSaltelli()
  : SALessSimple(SA_SOBOL2002)
  , N_(500)
  , estimator_(SOBOL2010)
{
  m_Sampling = SOBOL_SAMPLING;
}

void SobolSaltelli::setN(const int n)
{
  if (n<=5)
  {
    throw SAException(ERROR_TOO_SMALL_SAMPLE_SIZE);
  }

  N_ = n;
}

void SobolSaltelli::setEstimator(const SobolEstimator_t type)
{
  estimator_ = type;
}
int SobolSaltelli::getNumSens() const
{
  return 2*m_NumOutputs;
}

void SobolSaltelli::doSA()
{
  int k=m_InputList->size();                    /* number of factors */
  double minCnt = (1-m_FailureRate) * N_;

  /* a, b are pilot matrices, c has their column mixing following the sampling
   * design */
  std::unique_ptr<DMatrix> a(nullptr), b(nullptr), c(nullptr);
  std::unique_ptr<ResultMatrix> ya(nullptr), yb(nullptr), yc(nullptr);

  /* 1. Allocates input/output data if needs */
  if (m_SaveInput)
  {
    m_InputData.reset(new DMatrix((k+2)*N_, k));
    a = std::move(m_InputData->subMatrix(0, N_));
    b = std::move(m_InputData->subMatrix(N_, N_));
  } else
  {
    a.reset(new DMatrix(N_, k));
    b.reset(new DMatrix(N_, k));
    c.reset(new DMatrix(N_, k));
  }

  if (m_SaveOutput)
  {
    m_OutputData.reset(new ResultMatrix((k+2)*N_, m_NumOutputs));
    ya = std::move(m_OutputData->subMatrix(0, N_));
    yb = std::move(m_OutputData->subMatrix(N_, N_));
  } else
  {
    ya.reset(new ResultMatrix(N_, m_NumOutputs));
    yb.reset(new ResultMatrix(N_, m_NumOutputs));
    yc.reset(new ResultMatrix(N_, m_NumOutputs));
  }

  /* 2. Fill a and b with random samples */
  if (m_Sampling == MC_SAMPLING)
  {
    m_RNG.MC(*a, m_InputList);
    m_RNG.MC(*b, m_InputList);
  } else if (m_Sampling == LHS_SAMPLING)
  {
    m_RNG.LHS(*a, m_InputList);
    m_RNG.LHS(*b, m_InputList);
  } else                                        /* sobol sequence */
  {
    std::unique_ptr<DMatrix> pilot(new DMatrix(N_, 2*k));
    m_RNG.sobol(*pilot);
    /* copy pilot to a and b */
    for (int iRow=0; iRow<N_; ++iRow)
    {
      double* row = pilot->getRow(iRow);
      a->fillRow(iRow, row);
      b->fillRow(iRow, &row[k]);
    }
    pilot.release();
    /* convert a and b to target distribution */
    m_RNG.convert(*a, m_InputList);
    m_RNG.convert(*b, m_InputList);
  }

  /* 3. Simulate for the input matrices a and b */
  simulate(*a, *ya);
  simulate(*b, *yb);

  double** yadata = ya->getData();
  double** ybdata = yb->getData();
  int* la = ya->getLabels();
  int* lb = yb->getLabels();

  std::unique_ptr<double[]> acol(new double[N_]);;

  /* 4. For each input, estimate the sensitivity measures for all outputs */
  for (int iK=0; iK < k; ++iK)
  {
    /* Locates where are c and cy if needs */
    if (m_SaveInput)
    {
      c = std::move(m_InputData->subMatrix((2+iK)*N_, N_));     
    }
    if (m_SaveOutput)
    {
      yc = std::move(m_OutputData->subMatrix((2+iK)*N_, N_));
    }

    double* sens = m_Sens->getRow(iK);

    /* Fills content of the input matrix c */
    b->copy(*c);
    a->copyCol(iK, acol.get());
    c->fillCol(iK, acol.get());
    
    /* Simulates for the input matrix c */
    simulate(*c, *yc);
    
    /* Estimates sensitivity indices */
    double** ycdata = yc->getData();
    int* lc = yc->getLabels();
    for (int iOut=0; iOut < m_NumOutputs; ++ iOut)
    {
      /* calculate f0 and ya.ya, ya.yc and yb.yc for output iOut*/
      double f0 = 0;
      double D = 0;
      double Eyt1 = 0;
      double Eyt2 = 0;
      double Dy=0;
      double ybyc = 0;

      int validCnt[3] = {0, 0, 0};                         /* number of valid simulation
                                                   results */
      for (int iSample=0; iSample<N_; ++iSample)
      {
        if (la[iSample] == SIM_SUCCESS)
        {
          validCnt[0]++;
          f0 += yadata[iSample][iOut];
          D +=  yadata[iSample][iOut] * yadata[iSample][iOut];
        }


        if (la[iSample] == SIM_SUCCESS
            && lc[iSample] == SIM_SUCCESS
            && lb[iSample] == SIM_SUCCESS)
        {
          Dy += (ycdata[iSample][iOut] - ybdata[iSample][iOut]) * yadata[iSample][iOut];
          validCnt[1]++;
        }

        if (lb[iSample] == SIM_SUCCESS
            && lc[iSample] == SIM_SUCCESS)
        {
          ybyc += ybdata[iSample][iOut] * ycdata[iSample][iOut];
          Eyt1 += (ybdata[iSample][iOut]-ycdata[iSample][iOut]) * ybdata[iSample][iOut];
          Eyt2 += (ybdata[iSample][iOut]-ycdata[iSample][iOut]) *  (ybdata[iSample][iOut]-ycdata[iSample][iOut]);
            
          validCnt[2]++;
        }
      }

      /* check if the number if valid results is acceptable*/
      if (validCnt[0] < minCnt
          || validCnt[1] < minCnt
          || validCnt[2] < minCnt
          )
      {
        throw SAException(ERROR_EXCEEDING_FAILURE_RATE);
      }

      f0 = f0/validCnt[0];
      f0 *= f0;
      D /= validCnt[0];
      D -= f0;

      Dy /= validCnt[1];
      Eyt1 /= validCnt[2];
      Eyt2 /= 2*validCnt[2];
      ybyc /= validCnt[2];
      /* now estimate S[iOut] and St[iOut] */
      sens[2*iOut] = Dy/D;
      if (estimator_ == SOBOL2002)
      {
        sens[2*iOut+1] = 1-(ybyc-f0)/D; /* sobol 2002 */
      } else if (estimator_ == SOBOL2007)
      {
      sens[2*iOut+1] = Eyt1/D;                   /* sobol 2007 */
      } else
      {
        sens[2*iOut+1] = Eyt2/D;                   /* sobol 2010 */
      }
    }
  }
}
BIO_NAMESPACE_END

