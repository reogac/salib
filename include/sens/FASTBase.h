/**
 @file FASTBase.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  FASTBase_INC
#define  FASTBase_INC

#include "SALessSimple.h"

BIO_NAMESPACE_BEGIN

class FASTBase : public SALessSimple
{
  public:
    FASTBase(const SAMethod_t method);
    /**
     * @brief Sets the max spectral order
     *
     * @param m the maximum spectral order
     * */
    void setM(const int m);
    /**
     * @brief Sets the number of samples for a search curve
     * @param n number of samples
     * */
    void setN(const int n);
    /**
     * @brief Sets the method to estimate Fourier coefficients
     * 
     * @param fft true if Fast Fourier Transform is used
     * */
    void setUseFFT(const bool fft);
  protected:
    /**
     * @brief Returns a suitable length for FFT
     *
     * @param input length of the signal
     * @return the smallest 2^k >= n
     * */
    int len4FFT(const int n) const;
    /**
     * @brief Interpolates the missing values of the signal
     *
     * @param values input signal
     * @param labels an index vector to label missing values
     * @param length of the signal
     * */
    void interpolate(double* values, const int* labels, const int num);
    int M_;                                     /** Max spectral order */
    int N_;                                     /** Number of expected samples for a search curve */
    bool useFFT_;                               /** Use Fast Fourier Transform? */
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef FASTBase_INC  ----- */

