/**
 @file SALessSimple.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#ifndef  SALessSimple_INC
#define  SALessSimple_INC

#include "SABase.h"

BIO_NAMESPACE_BEGIN

class SALessSimple : public SABase
{
  public:
    SALessSimple(const SAMethod_t method);
    void setSaveInput(const bool save);
    void setSaveOutput(const bool save);
  protected:
    bool m_SaveInput;                           /* Save model input data? */
    bool m_SaveOutput;                          /* Save model output data? */
};

BIO_NAMESPACE_END


#endif   /* ----- #ifndef SALessSimple_INC  ----- */

