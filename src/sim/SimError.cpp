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

SimError::SimError(int code, std::unique_ptr<char[]>&&message, ErrorLevel_t level)
  : ErrorMessage(code, std::move(message), level)
{
}
const char* SimError::getMessage() const
{
  return m_Message.get();
}
BIO_NAMESPACE_END


