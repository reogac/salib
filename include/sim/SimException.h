/**
 @file SimException.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */
#include "common/Exception.h"

#ifndef  SimException_INC
#define  SimException_INC

BIO_NAMESPACE_BEGIN

enum
{
  SIM_EXCEPTION_NO_MODEL,
  SIM_EXCEPTION_NO_TIMEPOINT,
  SIM_EXCEPTION_INVALID_TIMEPOINT,
  SIM_EXCEPTION_INVALID_TIME_DURATION,
  SIM_EXCEPTION_INVALID_NUM_TIMESTEPS,
  SIM_EXCEPTION_UNKNOWN,
  SIM_NUM_EXCEPTIONS,
  SIM_VARIABLE_NOT_FOUND
} SimExceptionCode_t;
class SimException : public Exception
{
  public:
    SimException(const int code);
    SimException(const int code, const std::string& message);
    SimException(SimException&& other);

    const char* what() const noexcept override;
  private:

};

typedef struct {
  int code_;
  const char* message_;
} SimExceptionMessage_t;

BIO_NAMESPACE_END

#endif   /* ----- #ifndef SimException_INC  ----- */
