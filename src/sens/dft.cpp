/**
 @file dft.cpp
 @brief simple implementation of discret fuorier transform
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include <cmath>
#include <memory>

#include "dft.h"

constexpr static double MY_PI=atan(1)*4;

void dft(double* re, double* im, const int len)
{
  std::unique_ptr<double[]> dataptr(new double[2*len]);
  double* re2 = dataptr.get();
  double* im2 = &re2[len];

  #pragma omp parallel for  
  for (int i=0;i<len; ++i) {
    re2[i] = 0;
    im2[i] = 0;
    double arg = -2.0 * MY_PI * i / len;
    
    for (int k=0; k<len; ++k) {
      double cosarg = cos(k * arg);
      double sinarg = sin(k * arg);
      re2[i] += (re[k] * cosarg - im[k] * sinarg);
      im2[i] += (re[k] * sinarg + im[k] * cosarg);
    }
    
  }

  /*  Copy the data back */
  for (int i=0; i<len; ++i) {
    re[i] = re2[i] / len;
    im[i] = im2[i] / len;
  }
}

void sdft(double* re, const int len)
{
  std::unique_ptr<double[]> imptr(new double[len]);
  double* im = imptr.get();
  for (int i=0; i<len; ++i)
    im[i] = 0;

  dft(re, im, len);
  for (int i=0; i<len; ++i)
    re[i] = re[i]*re[i] + im[i]*im[i];
}
