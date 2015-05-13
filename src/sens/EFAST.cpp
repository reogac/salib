/**
 @file EFAST.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "EFAST.h"

#include <cmath>
#include <iostream>
#include <memory>

#include "SAException.h"
#include "dft.h"

constexpr static double MY_PI = 4.0*atan(1);

BIO_NAMESPACE_BEGIN

EFAST::EFAST()
  : FASTBase(SA_EFAST)
  , Nr_(3)
{
}


void EFAST::setNr(const int nr)
{
  if (nr<=0)
    throw SAException(ERROR_EFAST_NONE_POSITIVE_NR);
  Nr_ = nr;
}

int EFAST::getNumSens() const
{
  return 2* m_NumOutputs;
}


void EFAST::doSA()
{
  if (N_ < 65)
    throw SAException(ERROR_EFAST_SMALL_SAMPLE_SIZE);

  
  int k = m_InputList->size();                  /* number of input factors */

  /*  1. Determine search curve length (N) */
  int N = N_ % 2 == 0 ? N_ : N_+1; /* Makes N an add number (although not important) */
  if (useFFT_)
    N = len4FFT(N_);

  /* 2. Initializes omega values */
  std::unique_ptr<int[]> omegas_ptr(new int[k]);
  int* omegas = omegas_ptr.get();
  initOmegas(omegas, N); 

  /* 3. Initialize s values */
  std::unique_ptr<double[]> sptr(new double[N]);
  double* s = sptr.get();
  for (int iN=0; iN<N; ++iN)
  {
    s[iN] = MY_PI*(2*iN+1-N)/N;
  }

  /* 4. Allocate a vector to hold phi values */
  std::unique_ptr<double[]> phis_ptr(new double[k]);
  double* phis = phis_ptr.get();
  
  /* 5. Allocates input/output data */

  std::unique_ptr<DMatrix> x(nullptr);
  std::unique_ptr<ResultMatrix> y(new ResultMatrix(1,1));

  double** xdata(nullptr);
  double** ydata(nullptr);
  int* labels(nullptr);

  if (m_SaveInput)
  {
    m_InputData.reset(new DMatrix(N*Nr_*k, k));
  } else
  {
    x.reset(new DMatrix(N, k));
    xdata = x->getData();
  }
  if (m_SaveOutput)
  {
    m_OutputData.reset(new ResultMatrix(N*Nr_*k, m_NumOutputs));
  } else
  {
    y.reset(new ResultMatrix(N, m_NumOutputs));
    ydata = y->getData();
    labels = y->getLabels();
  }

  /* 6. Allocates vector to store variances estimated for each search curve */
  std::unique_ptr<double[]> totalV(new double[m_NumOutputs]);
  std::unique_ptr<double[]> totalVi(new double[m_NumOutputs]);
  std::unique_ptr<double[]> totalVci(new double[m_NumOutputs]);

  /* 7. Allocates a vector to hold temporary column data of the outputs */
  std::unique_ptr<double[]> ycol_ptr(new double[N]);
  double* ycol = ycol_ptr.get();

  /* 8. Estimates sensitivity indices */
  for (int iFactor=0; iFactor<k; ++iFactor)                    /* for each input factor */
  {
    for (int iV=0; iV<m_NumOutputs; ++iV)
    {
      totalV[iV] = 0;
      totalVi[iV] = 0;
      totalVci[iV] = 0;
    }

    for (int iNr=0; iNr<Nr_; ++iNr)                /* for each search curve */
    {
      /* Randomly generate phi values */
      m_RNG.rand(phis, k);
      for (int iK=0; iK<k; ++iK)
      {
        phis[iK] *= 2*MY_PI;   
      }
      
      int startId = iFactor*Nr_*N + iNr*N;

      /**
       * Let x refers to its according submatrix of the allocated input data 
       * */
      if (m_SaveInput)
      {
        x = std::move(m_InputData->subMatrix(startId, N));
        xdata = x->getData();
      }
      /**
       * Let y refers to its according submatrix of the allocated output data
       * */
      if (m_SaveOutput)
      {
        y = std::move(m_OutputData->subMatrix(startId, N));
        ydata = y->getData();
        labels = y->getLabels();
      }

      /* Do sampling on x */
      for (int iK=0; iK<k; ++iK)
      {
        int omId = (iK + k - iFactor ) % k;  /* index of omega in the vector 'omegas' */
        for (int iN=0; iN<N; ++iN)
        {
          xdata[iN][iK] = 0.5 + asin(sin(omegas[omId]*s[iN] + phis[iK])) / MY_PI;
        }
      }
        
      /* Converts x data to target distributions */
      m_RNG.convert(*x, m_InputList);  
      
      /* Runs simulations */
      simulate(*x, *y);
      
      /* Estimates variance */
      for (int iOut=0; iOut < m_NumOutputs; ++iOut)
      {
        y->copyCol(iOut, ycol);                 /* makes a copy of the column */
        interpolate(ycol, labels, N);           /* fills missing values */
        double variances[3] = {0, 0, 0 };
        if (useFFT_)
        {
          fftVariances(ycol, N, omegas[0], variances);
        } else 
        {
          directVariances(ycol, s, N, omegas[0], variances);
        }

        totalV[iOut] += variances[0]; 
        totalVi[iOut] += variances[1];
        totalVci[iOut] += variances[2];
      }
    }  /* end of a search curve */
    /* Estimates sensitivity indices for current factor */
    double* sens= m_Sens->getRow(iFactor);
    for (int iOut=0; iOut<m_NumOutputs; ++iOut)
    {
      sens[iOut*m_NumOutputs] = totalVi[iOut] / totalV[iOut];
      sens[iOut*m_NumOutputs+1] = 1 - totalVci[iOut] / totalV[iOut];
    }
  }
}

void EFAST::directVariances(const double* y, const double *s, const int N, const int omega, double* v)
{
  double f0=0;
  for (int iN=0; iN<N; ++iN)
  {
    f0 += y[iN];
    v[0] += y[iN] * y[iN];
  }
  f0 /= N;
  v[0] /= N;
  v[0] -= f0*f0;

  for (int iFreq=1; iFreq<omega/2; ++iFreq)
  {
    double A=0, B=0;
    for (int iN=0; iN<N; ++iN)
    {
      A += y[iN] * cos(iFreq*s[iN]);
      B += y[iN] * sin(iFreq*s[iN]);            
    }
    v[2] += A*A + B*B;
  }

  for (int iM=1; iM<=M_; ++iM)
  {
    double A=0, B=0;
    int iFreq = iM*omega;
    for (int iN=0; iN<N; ++iN)
    {
      A += y[iN] * cos(iFreq*s[iN]);
      B += y[iN] * sin(iFreq*s[iN]);            
    }
    v[1] += A*A + B*B;       
  }

  v[1] = 2*v[1]/N/N;
  v[2] = 2*v[2]/N/N;
  
}

void EFAST::fftVariances(double* y, const int N, const int omega, double* v)
{
  sdft(y, N);
  for (int iFreq=1; iFreq<N/2; ++iFreq)
    v[0] += y[iFreq];

  for (int iM=1; iM<=M_; ++iM)
    v[1] += y[iM*omega];

  for (int iFreq = 1; iFreq<omega/2; ++iFreq)
    v[2] += y[iFreq];
}

void EFAST::initOmegas(int* omegas, const int N) const
{
  omegas[0] = floor((N-1)/M_/2);
  int k = m_InputList->size();

  int m = floor(omegas[0]/M_/2);

  if (m>=k-1)
  {
    double offset = (double) m / (double)(k-1);
    for (int iK=1; iK<k; ++iK)
      omegas[iK] = floor(1+offset*(iK-1));
  } else
  {
    for (int iK=1; iK<k; ++iK)
    {
      omegas[iK] = (iK-1) % m + 1;
    }
  }
  for (int i=0; i<k; ++i)
    std::cout << omegas[i] << " ";

}

BIO_NAMESPACE_END

