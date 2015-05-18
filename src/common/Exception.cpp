/**
 @file Exception.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "Exception.h"


BIO_NAMESPACE_BEGIN

Exception::Exception(const int code)
  : m_Code(code)
{
}

Exception::Exception(const int code, const std::string& message)
  : m_Code(code)
  , m_Message(message)
{
}

Exception::Exception(Exception&& other)
  : std::exception(std::move(other))
  , m_Code(other.m_Code)
  , m_Message(other.m_Message)
{ 
}

BIO_NAMESPACE_END

