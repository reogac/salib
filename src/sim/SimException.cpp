/**
 @file SimException.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "SimException.h"

BIO_NAMESPACE_BEGIN

static SimExceptionMessage_t SimExceptionTable[SIM_NUM_EXCEPTIONS] = 
{
  { SIM_SOLVER_BUSY, "Solver is busy"},
  { SIM_SOLVER_NOT_READY, "Solver is not ready (not initialized)"},
  { SIM_EMPTY_TIMEPOINT, "Provide at least one time point"},
  { SIM_INVALID_TIMEPOINT, "Found a invalid timepoint (timepoints must be in strict ascending order"},
  { SIM_INVALID_TIME_DURATION, "Invalid time duration" },
  { SIM_INVALID_NUM_TIMESTEPS, "Invalid number of timesteps"},
  { SIM_UNKNOWN, "An unknown error"},
};


SimException::SimException(const int code)
  : Exception(code)
{
}

SimException::SimException(const int code, std::unique_ptr<char[]>&& message)
  : Exception(code, std::move(message))
{
}
SimException::SimException(SimException&& other)
  : Exception(std::move(other))
{
}

const char* SimException::what() const noexcept
{
  if (m_Message)
    return m_Message.get();
  if (m_Code>=0 && m_Code<SIM_NUM_EXCEPTIONS)
    return  SimExceptionTable[m_Code].message_;
  return UnknownErrorString;
}
BIO_NAMESPACE_END


