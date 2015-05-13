/**
 @file RBD.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "RBD.h"

#include <cmath>
#include <memory>

#include "SAException.h"
#include "dft.h"

constexpr static double MY_PI = 4*atan(1);

BIO_NAMESPACE_BEGIN

RBD::RBD()
  : FASTBase(SA_RBD)
  , omega_(1)
{
}

void RBD::setOmega(const int om)
{
  if (om <= 0)
    throw SAException(ERROR_RBD_NONE_POSITIVE_OMEGA);
  omega_ = om;
}
int RBD::getNumSens() const
{
  return m_NumOutputs;
}

void RBD::doSA()
{
  /* 1. Determine sampling size */
  if (N_ < 2*omega_ + 1)
    throw SAException(ERROR_RBD_SMALL_SAMPLE_SIZE);

  int N = N_ % 2 == 1 ? N_ : N_ + 1;            /* Makes N an odd number (it is not important) */
  if (useFFT_)
    N = len4FFT(N_);

  /* 2. Allocates data */
  int k = m_InputList->size();
  std::unique_ptr<DMatrix> x(new DMatrix(N, k)); /* Input data */
  std::unique_ptr<IMatrix> p(new IMatrix(k, N)); /* Array of permutation vectors */
  std::unique_ptr<ResultMatrix> y(new ResultMatrix(N, m_NumOutputs)); /* Output data */

  double** xdata = x->getData();
  int** pdata = p->getData();
  double** ydata = y->getData();
  
  /* 3. Sets up permuation vectors */
  for (int iK=0; iK<k; ++iK)
  {
    for (int iN=0; iN<N; ++iN)
    {
      pdata[iK][iN] = iN;
    }
    m_RNG.shuffle(pdata[iK], N);
  }
  
  /* 4. Inits the pilot sample */
  std::unique_ptr<double[]> x0ptr(new double[2*N]); /* pilot sample */
  double* x0 = x0ptr.get();
  double* s = &x0[N]; 
  for (int i=0; i<N; ++i)
  {
    s[i] = MY_PI*(2*i+1-N)/N;

    x0[i] = 0.5 + asin(sin(omega_*s[i]));
  }

  /* 5. Samples input data */
  for (int iK=0; iK<k; ++iK)
  {
    for (int iN=0; iN<N; ++iN)
    {
      xdata[pdata[iK][iN]][iK] = x0[iN];
    }
  }

  /* 6. Converts input data to target distributions */
  m_RNG.convert(*x, m_InputList);

  /* 7. Runs simulation */
  simulate(*x, *y);
  

  /* 8. Estimates sensitivity indices */

  std::unique_ptr<double[]> ytmp_ptr(new double[N]);
  double* ytmp = ytmp_ptr.get();
  int* labels = y->getLabels();
  for (int iK=0; iK<k; ++iK)
  {
    for (int iOut=0; iOut< m_NumOutputs; ++iOut)
    {
      for (int iN=0; iN<N; ++iN)
      {
        ytmp[iN] = ydata[pdata[iK][iN]][iOut];
      }
      interpolate(ytmp, labels, N);

      if (useFFT_) {                            /* Estimate Fourier coeffients using FFT */
        sdft(ytmp, N);
        double V=0;
        for (int iN=1; iN<N/2; ++iN)
        {
          V += ytmp[iN];
        }
        double Vi = 0;
        for (int iM=1; iM<M_+1; ++iM)
        {
          Vi += ytmp[iM*omega_];
        }

        m_Sens->getRow(iK)[iOut]= Vi/V;
      }
      else                                      /* Estimates Fourier coefficients directly */
      {
        double V=0, f0=0;
        /* estimates total variance */
        for (int iN=0; iN<N; ++iN)
        {
          f0 += ytmp[iN];
          V += ytmp[iN] * ytmp[iN];
        }
        f0 /= N;
        V /= N;
        V -= f0*f0;
        
        /* estimate Fourier coefficients and Vi */
        double Vi = 0;
        for (int iM=1; iM<M_+1; ++iM)
        {
          double A=0, B=0;
          for (int iN = 0; iN<N; ++iN)
          {
            A += ytmp[iN] * cos(iM*omega_*s[iN]);
            B += ytmp[iN] * sin(iM*omega_*s[iN]);
          }       
          A /= N;
          B /= N;
          Vi += A*A + B*B;
        }
        m_Sens->getRow(iK)[iOut] = 2*Vi/V;
      }
    }
  }

  /* 9. Retains input/output data if needs */
  if (m_SaveInput)
    m_InputData = std::move(x);
  if (m_SaveOutput)
    m_OutputData = std::move(y);
}

BIO_NAMESPACE_END

