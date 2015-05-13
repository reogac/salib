/**
 @file dft.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  dft_INC
#define  dft_INC

#ifdef __cplusplus
extern "C"
{
#endif

/* discret fourier transform */
void dft(double* re, double* im, const int len);

/* spectral of discret fourier transform */
void sdft(double* re, const int len);

#ifdef __cplusplus
}
#endif
#endif   /* ----- #ifndef dft_INC  ----- */

