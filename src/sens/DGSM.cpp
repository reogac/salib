/**
 @file DGSM.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */
#include "DGSM.h"

#include "SAException.h"

BIO_NAMESPACE_BEGIN

DGSM::DGSM()
  : SALessSimple(SA_DGSM)
  , N_(500)
  , delta_(0.01)
{
}

void DGSM::setN(const int n)
{
  if (n<=10)
  {
    throw SAException(ERROR_TOO_SMALL_SAMPLE_SIZE);
  }

  N_ = n;
}

void DGSM::setDelta(const double delta)
{
  if (delta>=0.5)
    throw SAException(ERROR_DGSM_BIG_DELTA);
  delta_ = delta;
}

int DGSM::getNumSens() const
{
  return 3*m_NumOutputs;
}

void DGSM::doSA()
{
  int k = m_InputList->size();                  /* number of input factors */

  /* 1. Allocates input/output data */
  std::unique_ptr<DMatrix> X(nullptr);
  std::unique_ptr<ResultMatrix> y(nullptr);
  if (m_SaveInput)
  {
    m_InputData.reset(new DMatrix(N_*(k+1), k));
    X = std::move(m_InputData->subMatrix(0, N_));
  } else
  {
    X.reset(new DMatrix(N_, k));
  }

  if (m_SaveOutput)
  {
    m_OutputData.reset(new ResultMatrix(N_*(k+1), m_NumOutputs));
    y = std::move(m_OutputData->subMatrix(0, N_));
  } else
  {
    y.reset(new ResultMatrix(N_, m_NumOutputs));
  }

  std::unique_ptr<DMatrix> x(new DMatrix(N_, k));

  /* 2. Generates the first N samples */
  m_RNG.LHS(*x);

  /* Copies then converts to target distributions */
  x->copy(*X);
  m_RNG.convert(*X, m_InputList);
  
  /* 3. Runs simulation for the first N samples */
  simulate(*X, *y);

  /* 4. Allocates xdiff, ydiff if needs
   * xdiff is the 1-column different from X and ydiff is its corresponding
   * output
   * */
  std::unique_ptr<DMatrix> xdiff(nullptr);
  if (!m_SaveInput)
  {
    xdiff.reset(new DMatrix(N_, k));
  }

  std::unique_ptr<ResultMatrix> ydiff(nullptr);
  if (!m_SaveOutput)
  {
    ydiff.reset(new ResultMatrix(N_, m_NumOutputs));
  }

  std::unique_ptr<DMatrix> stats(new DMatrix(3, m_NumOutputs));
  
  const int* labels = y->getLabels();

  /* 5. Estimate sensitivity indices for each input factor */
  for (int iK=0; iK<k; ++iK)
  {
    /* Makes xdiff, ydiff refer to their correct location if needs */
    if (m_SaveInput)
    {
      xdiff = std::move(m_InputData->subMatrix((iK+1)*N_, N_));
    }

    if (m_SaveOutput)
    {
      ydiff = std::move(m_OutputData->subMatrix((iK+1)*N_, N_));
    }

    /* Copy the first N samples to xdiff and diffs the column iK*/
    x->copy(*xdiff);
    for (int iRow=0; iRow<N_; ++iRow)
    {
      /* Changes values in column iK */
      double* row = xdiff->getRow(iRow);
      if (row[iK] + delta_ >= 1)
        row[iK] -= delta_;
      else row[iK] += delta_;
    }
    
    /* Converts xdiff to target distributions */
    m_RNG.convert(*xdiff, m_InputList);
    
    /* Runs simulation */
    simulate(*xdiff, *ydiff);

    
    /* Estimates sensitivity indices */
    const int* labelsdiff = ydiff->getLabels();    
    double* sens = m_Sens->getRow(iK);
    for (int iOut=0; iOut< m_NumOutputs; ++iOut)
    {
      double mean=0, absmean=0, std=0;
      double derivative = 0;
      int cnt = 0;
      for (int iRow = 0; iRow < N_; ++iRow)
      {
        if (labels[iRow] == SIM_SUCCESS
            && labelsdiff[iRow] == SIM_SUCCESS)
        {
          derivative = (ydiff->getRow(iRow)[iOut] - y->getRow(iRow)[iOut])
                      / (X->getRow(iRow)[iK] - xdiff->getRow(iRow)[iK]);
          cnt++;
          mean += derivative;
          absmean += derivative > 0 ? derivative : -derivative;
          std += derivative*derivative;
        }   
      }
      if (cnt < N_ * (1-m_FailureRate))
        throw SAException(ERROR_EXCEEDING_FAILURE_RATE);

      mean /= cnt;
      absmean /= cnt;
      std = sqrt(std/cnt - mean*mean);
      sens[iOut*iK] = mean;
      sens[iOut*iK+1] = absmean;
      sens[iOut*iK+2] = std;    
    } 
  }
}

BIO_NAMESPACE_END

