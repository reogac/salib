/**
 @file Exception.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "Exception.h"


BIO_NAMESPACE_BEGIN

Exception::Exception(const int code)
  : m_Code(code)
  , m_Message(nullptr)
{
}

Exception::Exception(const int code, std::unique_ptr<char[]>&& message)
  : m_Code(code)
  , m_Message(std::move(message))
{
}

Exception::Exception(Exception&& other)
  : std::exception(std::move(other))
  , m_Code(other.m_Code)
  , m_Message(std::move(other.m_Message))
{ 
}

BIO_NAMESPACE_END

