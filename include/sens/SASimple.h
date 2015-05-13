/**
 @file SASimple.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#ifndef  SASimple_INC
#define  SASimple_INC

#include "SABase.h"

BIO_NAMESPACE_BEGIN

class SASimple : public SABase
{
  public:
    SASimple(const SAMethod_t method);

  private:
    void doSA();
    virtual void sample() = 0;
    virtual void estimate() = 0;
    virtual int getNumSamples() const = 0;
};

BIO_NAMESPACE_END
#endif   /* ----- #ifndef SASimple_INC  ----- */

