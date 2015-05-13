/**
 @file SAException.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "SAException.h"

BIO_NAMESPACE_BEGIN
static const char* SAExceptionMessages[] =
{
  /* ERROR_UNKNOWN */
  "unknown error",
  
  /* ERROR_NONE_POSITIVE_INPUT_SIZE */
  "expects a positive input size",

  /* ERROR_NONE_POSITIVE_OUTPUT_SIZE */
  "expects a positive output size",

  /* ERROR_NO_MODEL_EVALUATION */
  "sensitivity analysis need a model evaluator",

   /* ERROR_INVALID_FAILURE_RATE */
  "falure rate must be a real number in [0,0.2]",

  /* ERROR_TOO_SMALL_SAMPLE_SIZE */
  "the number of samples is too small",

  /* ERROR_EXCEEDING_FAILURE_RATE */
  "the number of model evaluation failures has exceeded the limit",

  /* ERROR_SOBOL_EXCEEDING */
  "the dimension exceeds the maximum value (1111) for sobol sequence generator",
  
   /* ERROR_FAST_DIM_EXCEEDING */
  "this implementation of FAST only supports less than 50 number of input \
    factors", 
  
  /* ERROR_FAST_SMALL_SAMPLE_SIZE */
  "number of samples for FAST method is too small", 
  
  /* ERROR_FAST_INVALID_M */
  "the spectral order must be in an integer in [4,8]", 
  
  /* ERROR_FAST_NONE_POSITIVE_N */
  "expects a positive number of samples in a search curve", 
  
  /* ERROR_EFAST_SMALL_SAMPLE_SIZE */
  "number of sample for EFAST method must be greater than 64",
  
  /* ERROR_EFAST_NONE_POSITIVE_NR */
  "expects a positive number of search curves", 

  /* ERROR_DGSM_BIG_DELTA */
  "delta is too big for DGSM method",            
  
  /* ERROR_RBD_SMALL_SAMPLE_SIZE */
  "number of samples for RBD method is too small",

  /* ERROR_RBD_NONE_POSITIVE_OMEGA */
  "expects a positive omega value",

  /* ERROR_MORRIS_TOO_SMALL_P */
  "the grid level must be greater then 1",

  /* ERROR_MORRIS_TOO_SMALL_R */
  "the number of trajectories must be greater than 10"

};

SAException::SAException(int code)
  : m_Code(code)
{
}

const char* SAException::what() const noexcept
{
  return SAExceptionMessages[m_Code];
}

BIO_NAMESPACE_END


