/**
 @file SimError.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "SimError.h"

BIO_NAMESPACE_BEGIN

SimError::SimError(const int code, ErrorLevel_t level)
  : ErrorMessage(code, level)
{
}

SimError::SimError(int code, const std::string& message, ErrorLevel_t level)
  : ErrorMessage(code, message, level)
{
}
const char* SimError::getMessage() const
{
  return m_Message.c_str();
}
BIO_NAMESPACE_END


