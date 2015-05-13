/**
 @file DGSM.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  DGSM_INC
#define  DGSM_INC

#include "SALessSimple.h"

BIO_NAMESPACE_BEGIN

class DGSM : public SALessSimple
{
  public:
    DGSM();
    /**
     * @brief Sets the number of */
    void setN(const int n);
    /**
     * @brief Sets the normalized delta value for local derivative estimation
     *
     * A value of 0.01 means the real delta is 1% of the factor range,
     * supposedly the factor is uniformly distributed 
     * */
    void setDelta(const double delta);
  private:
    int getNumSens() const;
    
    void doSA();

    int N_;
    double delta_;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef DGSM_INC  ----- */

