/**
 @file Jansen.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  Jansen_INC
#define  Jansen_INC

#include "SASimple.h"

BIO_NAMESPACE_BEGIN

class Jansen : public SASimple
{
  public:
    Jansen();
    void setR(const int r);
  private:
    int getNumSens() const override;
    int getNumSamples() const override;
    void sample() override;
    void estimate() override;

    int r_;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef Jansen_INC  ----- */

