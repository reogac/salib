/**
 @file RBD.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  RBD_INC
#define  RBD_INC

#include "FASTBase.h"

BIO_NAMESPACE_BEGIN

class RBD : public FASTBase
{
  public:
    RBD();
    /**
     * @brief Sets the base frequency
     *
     * @param om base frequency*/
    void setOmega(const int om);
  private:
    void doSA() override;
    int getNumSens() const override;
    int omega_;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef RBD_INC  ----- */
