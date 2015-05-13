/**
 @file SAException.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  SAException_INC
#define  SAException_INC

#include <exception>

#include "common/namespace.h"

BIO_NAMESPACE_BEGIN
enum
{
  ERROR_UNKNOWN = 0,
  ERROR_NONE_POSITIVE_OUTPUT_SIZE,
  ERROR_NONE_POSITIVE_INPUT_SIZE,
  ERROR_NO_MODEL_EVALUATOR,
  ERROR_INVALID_FAILURE_RATE,
  ERROR_TOO_SMALL_SAMPLE_SIZE,
  ERROR_EXCEEDING_FAILURE_RATE,
  ERROR_SOBOL_EXCEEDING,
  ERROR_FAST_DIM_EXCEEDING,
  ERROR_FAST_SMALL_SAMPLE_SIZE,
  ERROR_FAST_INVALID_M,
  ERROR_FAST_NONE_POSITIVE_N,
  ERROR_EFAST_SMALL_SAMPLE_SIZE,
  ERROR_EFAST_NONE_POSITIVE_NR,
  ERROR_DGSM_BIG_DELTA,
  ERROR_RBD_SMALL_SAMPLE_SIZE,
  ERROR_RBD_NONE_POSITIVE_OMEGA,
  ERROR_MORRIS_TOO_SMALL_P,
  ERROR_MORRIS_TOO_SMALL_R
} SAExceptionCode_t;
class SAException : public std::exception
{
  public:
    SAException(int code);
    const char* what() const noexcept override;
  private:
    int m_Code;
  
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef SAException_INC  ----- */


