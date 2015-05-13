/**
 @file FAST.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  FAST_INC
#define  FAST_INC

#include "FASTBase.h"

BIO_NAMESPACE_BEGIN

class FAST : public FASTBase
{
  public:
    /**
     * @brief Constructor 
     * */
    FAST();
  private:
    void doSA() override;
    int getNumSens() const override;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef FAST_INC  ----- */



