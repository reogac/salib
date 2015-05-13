/**
 @file EFAST.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  EFAST_INC
#define  EFAST_INC

#include "FASTBase.h"

BIO_NAMESPACE_BEGIN

class EFAST : public FASTBase
{
  public:
    EFAST();
    /**
     * @brief Sets the number of search curves
     *
     * @param nr number of search curves
     * */
    void setNr(const int nr);
  private:
    void doSA() override;
    int getNumSens() const override;
    void initOmegas(int* omegas, const int N) const;
    void directVariances(const double* y, const double* s, const int N, const int omega, double* v);
    void fftVariances(double* y, const int N, const int omega, double* v);
    int Nr_;                                    /* number of search curves */
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef EFAST_INC  ----- */


