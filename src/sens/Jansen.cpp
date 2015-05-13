/**
 @file Jansen.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "Jansen.h"

#include <memory>

BIO_NAMESPACE_BEGIN

Jansen::Jansen()
  : SASimple(SA_JANSEN)
  , r_(500)
{
}

void Jansen::setR(const int r)
{
  if (r<=0)
  {
    throw SAException(ERROR_TOO_SMALL_SAMPLE_SIZE);
  }

  r_ = r;
}
int Jansen::getNumSens() const
{
  return 2*m_NumOutputs;
}

int Jansen::getNumSamples() const
{
  return (r_+1)*m_InputList->size();
}

void Jansen::sample()
{
  int k = m_InputData->getNumCols();            /* number of input factors */

  int N = getNumSamples();                      /* total number of samples to be generated */

  /*  Generates the first sample */
  double* curRow = m_InputData->getRow(0);
  m_RNG.rand(curRow, k);

  /* starts with the second column in winding stars design.
   * NOTE:the number of input factors must be greater than one
   * */
  int col = 1;

  for (int iSample=1; iSample<N; ++iSample)
  {
    /* Copies the previous row */
    m_InputData->fillRow(iSample, curRow);
    /* Moves to the next row */
    curRow = m_InputData->getRow(iSample);
    /* Alters the value of 'col' column of the current row */
    curRow[col] = m_RNG.rand();    
    /* Moves 'col' to the next column */
    col++;

    /* If 'col' passes the last column, go back to the first column */
    if (col >=k) col = 0;
  }

  /* Convert samples from the (0,1) uniform distribution to their target distributions */
  m_RNG.convert(*m_InputData, m_InputList);
}

void Jansen::estimate()
{
  int k = m_InputData->getNumCols();            /* number of input factors */
  int N = k*(r_+1);
  double minCnt = (1-m_FailureRate) * r_;
  int* labels = m_OutputData->getLabels();

  /* allocates a temporary vector of Di of input factors */
  std::unique_ptr<double[]> Dvec_ptr(new double[k]);
  double* Dvec = Dvec_ptr.get();

  /* allocate a temporary vector of Dt of input factors */
  std::unique_ptr<double[]> Dtvec_ptr(new double[k]);
  double* Dtvec = Dtvec_ptr.get();

  double ** outputs = m_OutputData->getData();

  for (int iOut = 0; iOut< m_OutputData->getNumCols(); ++iOut) /* for each output */
  {
    double D=0;                                   /* total output variance */
    double f0 = 0;                              /* mean of outputs */

    for (int iK=0; iK<k; ++iK)                  /* for each input factor */
    {
      int cnts[3] = {0, 0, 0};                  /* countt the numbers of valid simulation results */
      Dvec[iK] = 0;
      Dtvec[iK] = 0;
      double colD = 0;                             /* output variance of column iK */
      double f0 = 0;                            /* mean of column iK */

      /* Iterates from the first row to 'r' row of the winding stair design
       * matrix of outputs */
      for (int iR=0; iR<=r_; ++iR)
      {
        /* Identifies row ids of samples for variance estimation 
         * - a sample at baseId row belongs to column iK of winding stairs design
         *
         * Since the total number of samples is N=(r+1)*k, nextId and prevId
         * should never exceed N
         **/
        int baseId = iR*k + iK;          
        int prevId = baseId-1;         
        int nextId = baseId + k-1;
        if (labels[baseId] == SIM_SUCCESS)
        {
          cnts[0]++;
          f0 += outputs[baseId][iOut];
          colD += outputs[baseId][iOut]*outputs[baseId][iOut];

          if (nextId<N && labels[prevId] == SIM_SUCCESS)
          {
            cnts[1]++;
            double diff =  outputs[baseId][iOut] - outputs[nextId][iOut];
            Dvec[iK] += diff*diff;
          }

          if (prevId>0 && labels[nextId] == SIM_SUCCESS)
          {
            cnts[2]++;
            double diff =  outputs[baseId][iOut] - outputs[prevId][iOut];
            Dtvec[iK] += diff*diff;
          }
        }
      }
      if (cnts[0] < minCnt
          || cnts[1] < minCnt
          || cnts[2] < minCnt
          )
      {
        throw SAException(ERROR_EXCEEDING_FAILURE_RATE);
      }
      f0 /= cnts[0];
      D += (colD/cnts[0]-f0*f0);
      Dvec[iK] /= 2*cnts[1];
      Dtvec[iK] /= 2*cnts[2];
    }
    /* estimate D */
    D /= k;
    /* estimate sensitivity measures */
    for (int iK=0; iK<k; ++iK)
    {
      double* sens = m_Sens->getRow(iK);
      sens[2*iOut] = 1 - Dvec[iK]/D;
      sens[2*iOut+1] = Dtvec[iK]/D;
    }
  }
}

BIO_NAMESPACE_END

