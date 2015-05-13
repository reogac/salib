/**
 @file FAST.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "FAST.h"

#include <memory>
#include <cmath>

#include "SAException.h"
#include "dft.h"

constexpr static double MY_PI = 4*atan(1);

BIO_NAMESPACE_BEGIN

FAST::FAST()
  : FASTBase(SA_FAST)
{
}

void FAST::doSA()
{
  static int otable[50] = { 0, 3, 1, 5, 11, 1, 17, 23, 19, 25,
                               41, 31, 23, 87, 67, 73, 85, 143, 149, 99, 
                               119, 237, 267, 283, 151, 385, 157, 215, 449, 163,
                               337, 253, 375, 441, 673, 773, 875, 873, 587, 849,
                               623, 637, 891, 943, 1171, 1225, 1335, 1725, 1663, 2019
                          };
  static int dtable[50] = { 4, 8, 6, 10, 20, 22, 32, 40, 38, 26,
                               56, 62, 46, 76, 96, 60, 86, 126, 134, 112,
                               92, 128, 154, 196, 34, 416, 106, 208, 328, 198,
                               382, 88, 348, 186, 140, 170, 284, 568, 302, 438, 
                               410, 248, 448, 388, 596, 216, 100, 488, 166, 0
                             };

  int k =m_InputList->size();                   /* number of input factor */
  
  if (k>50)
    throw SAException(ERROR_FAST_DIM_EXCEEDING);


  /* 1. Sets omega values */
  std::unique_ptr<int[]> omegas_ptr(new int[k]);
  int* omegas = omegas_ptr.get();
  omegas[0] = otable[k-1];
  for (int i=1; i<k; ++i)
  {
    omegas[i] = omegas[i-1] + dtable[k-i];
  }
  
  /* 2. Determines sampling size */
  int N = 2*M_*omegas[k-1]+1;                   /* Smallest acceptable sampling size */
  if (N_ < N)
    throw SAException(ERROR_FAST_SMALL_SAMPLE_SIZE);

  if (useFFT_) 
  {
    /* if using fft, increases the number of samples to the smallest 2^kr */
    N = len4FFT(N_);
  } else
  {
    /* if not using fft, makes sure the number of samples is odd */
    N = N_ % 2 == 1 ? N_ : N_ + 1;
  }
  
  std::unique_ptr<DMatrix> x(new DMatrix(N, k));
  std::unique_ptr<ResultMatrix> y(new ResultMatrix(N, m_NumOutputs));

  double** xdata = x->getData();
  double** ydata = y->getData();

  /* 3. Sets s values */
  std::unique_ptr<double[]> s_ptr(new double[N]);
  double* s = s_ptr.get();
  for (int iRow=0; iRow<N; ++iRow)
  {
    s[iRow] = 0.5*MY_PI*(2*iRow-N+1)/N;

    for (int iCol=0; iCol<k; ++iCol)
    {
      /* TODO need not to call trigonometric functions */
      xdata[iRow][iCol] = 0.5 + asin(sin(s[iRow] * omegas[iCol]))/MY_PI;
    }
  }
  /* 4. Converts to target distributions */
  m_RNG.convert(*x, m_InputList);

  /* 5. Runs simulations */
  simulate(*x, *y);

  /* 6. Estimate sensitivity indices */

  std::unique_ptr<double[]> ycol_ptr(new double[N]);

  double* ycol = ycol_ptr.get();
  int* labels = y->getLabels();

  for (int iOut=0; iOut < m_NumOutputs; ++iOut)
  { 
    y->copyCol(iOut, ycol);

    interpolate(ycol, labels, N);

    if (useFFT_)                                /* Estimate Fourier coefficients using FFT */
    {
      /* Discret Fourier transform */
      sdft(ycol, N);                            /* TODO: Provide a FFT implementation instead of DFT */

      /*  Estimates total variance*/
      double V=0;
      for (int iN=1; iN<N/2; ++iN)
        V += ycol[iN];

      for (int iK=0; iK<k; ++iK)
      {
        double Vi=0;
        for (int iOrder =1; iOrder < M_+1; ++iOrder)
        {
          Vi +=  ycol[iOrder*omegas[iK]];

        }
        m_Sens->getRow(iK)[iOut] = Vi/V;
      }

    } else                                      /* Estimate Fourier series coefficients directly */
    {
      /* Estimates total variance */  
      double f0=0;                                /* mean */
      double V=0;                                 /* variance */
      for (int iSample=0; iSample < N; ++iSample)
      {
        f0 += ycol[iSample];
        V += ycol[iSample] * ycol[iSample];
      }
      f0 /= N;
      V /= N;
      V -= f0*f0;

      int q = (N-1)/2;
      /* Estimates sensitivity indices */
      for (int iK=0; iK<k; ++iK)
      {
        double Vi = 0;
        for (int iOrder =1; iOrder < M_+1; ++iOrder)
        {
          int freq = iOrder*omegas[iK];
          double coeff = 0;
          if (freq % 2 == 0)
          {
            coeff = ycol[q+1];
            for (int iQ=1; iQ <=q; ++iQ)
            {
              coeff += (ycol[q+iQ] + ycol[q-iQ]) * cos(MY_PI*iQ*freq/N);
            }
          } else 
          {
            for (int iQ=1; iQ<=q; ++iQ)
            {
              coeff += (ycol[q+iQ] - ycol[q-iQ]) * sin(MY_PI*iQ*freq/N);
            }
          }

          coeff /= N;
          Vi += coeff * coeff;
        }
        m_Sens->getRow(iK)[iOut] = 2*Vi/V;
      }
    }
  }

  /* 7. Retains input/output data if needs*/
  if (m_SaveInput)
    m_InputData = std::move(x);
  if (m_SaveOutput)
    m_OutputData = std::move(y);
}

int FAST::getNumSens() const
{
  return m_NumOutputs;
}

BIO_NAMESPACE_END

