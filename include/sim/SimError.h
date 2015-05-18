/**
 @file SimError.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  SimError_INC
#define  SimError_INC

#include "common/ErrorMessage.h"

BIO_NAMESPACE_BEGIN

enum
{
  SIM_WARNING_COMPARTMENT_WITHOUT_RATE,
  SIM_WARNING_DELAY_ODE,
  SIM_WARNING_LAMBDA_USING,
  SIM_WARNING_UNDEFINED_FUNCTION,
  SIM_WARNING_FACTORIAL_USING,
  SIM_WARNING_VARIABLE_NO_VALUE,
  SIM_ERROR_CONFLICT_RULE,
  SIM_ERROR_EMPTY_AST,
  SIM_ERROR_UNKNOWN_AST,
  SIM_ERROR_CYCLIC_AST,
  SIM_ERROR_SBML_PARSING,
  SIM_UNKNOWN_ERROR

};

class SimError : public ErrorMessage
{
  public:
    SimError(const int code, ErrorLevel_t level = LEVEL_ERROR);
    SimError(int code, const std::string& message, ErrorLevel_t level = LEVEL_ERROR);
  virtual const char* getMessage() const;
  private:
};
BIO_NAMESPACE_END

#endif   /* ----- #ifndef SimError_INC  ----- */

