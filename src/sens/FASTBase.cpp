/**
 @file FASTBase.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "FASTBase.h"

BIO_NAMESPACE_BEGIN

FASTBase::FASTBase(const SAMethod_t method)
  : SALessSimple(method)
  , M_(4)
  , N_(1000)
  , useFFT_(false)
{
}

void FASTBase::setM(const int m)
{
  if (m<4 || m>8)
    throw SAException(ERROR_FAST_INVALID_M);
  M_ = m;
}

void FASTBase::setN(const int n)
{
  if (n<=0)
    throw SAException(ERROR_FAST_NONE_POSITIVE_N);
  N_ = n;
}

void FASTBase::setUseFFT(const bool fft)
{
  useFFT_ = fft;
}

int FASTBase::len4FFT(const int n) const
{
  if (!n)
    return 0;

  int num = n;
  int ret = 1;
  while (num >>=1)
    ret <<=1;
  if (ret < n)
    ret <<=1;
  return ret;
}

void FASTBase::interpolate(double* values, const int* labels, const int num)
{
  /* TODO: Provides a simple interpolation algorithm */
}

BIO_NAMESPACE_END

