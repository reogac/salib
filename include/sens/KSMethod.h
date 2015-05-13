/**
 @file KSMethod.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  KSMethod_INC
#define  KSMethod_INC

#include "SASimple.h"

BIO_NAMESPACE_BEGIN

class KSMethod : public SASimple
{
  public:
    KSMethod();
    ~KSMethod();
    const double* getRefInput() const;
    const double* getRefOutput() const;
    void setN(const int N);
  private:
    int getNumSens() const override;
    int getNumSamples() const override;
    void sample() override;
    void estimate() override;

    int N_;
};
BIO_NAMESPACE_END

#endif   /* ----- #ifndef KSMethod_INC  ----- */


