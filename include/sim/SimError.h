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
  SIM_WARNING_VARIABLE_NO_VALUE,
  SIM_ERROR_CONFLICT_RULE,
  SIM_EMPTY_AST,
  SIM_CYCLIC_AST,
  SIM_UNKNOWN_ERROR

};

class SimError : public ErrorMessage
{
  public:
    SimError(const int code, ErrorLevel_t level = LEVEL_ERROR);
    SimError(int code, std::unique_ptr<char[]>&&message, ErrorLevel_t level = LEVEL_ERROR);
  virtual const char* getMessage() const;
  private:
};
BIO_NAMESPACE_END

#endif   /* ----- #ifndef SimError_INC  ----- */

