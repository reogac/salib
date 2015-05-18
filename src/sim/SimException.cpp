/**
 @file SimException.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "SimException.h"

BIO_NAMESPACE_BEGIN

static SimExceptionMessage_t SimExceptionTable[SIM_NUM_EXCEPTIONS] = 
{
  { SIM_EXCEPTION_NO_MODEL, "Input model has not been loaded"},
  { SIM_EXCEPTION_NO_TIMEPOINT, "Provide at least one time point"},
  { SIM_EXCEPTION_INVALID_TIMEPOINT, "Found a invalid timepoint (timepoints must be in strict ascending order"},
  { SIM_EXCEPTION_INVALID_TIME_DURATION, "Invalid time duration" },
  { SIM_EXCEPTION_INVALID_NUM_TIMESTEPS, "Invalid number of timesteps"},
  { SIM_EXCEPTION_UNKNOWN, "An unknown error"},
};


SimException::SimException(const int code)
  : Exception(code)
{
}

SimException::SimException(const int code, const std::string& message)
  : Exception(code, message)
{
}
SimException::SimException(SimException&& other)
  : Exception(std::move(other))
{
}

const char* SimException::what() const noexcept
{
  if (!m_Message.empty())
    return m_Message.c_str();
  if (m_Code>=0 && m_Code<SIM_NUM_EXCEPTIONS)
    return  SimExceptionTable[m_Code].message_;
  return UnknownErrorString;
}

BIO_NAMESPACE_END


