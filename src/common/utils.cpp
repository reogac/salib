/**
 @file utils.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */
#include "utils.h"

#include <cstdarg>
#include <cstring>

BIO_NAMESPACE_BEGIN


#define MAX_EXCEPTION_MESSAGE_LENGTH 1024

std::unique_ptr<char[]> formatMessage(const char* fmt, ...) noexcept
{
  std::unique_ptr<char[]> ret(nullptr);

  static char buffer[MAX_EXCEPTION_MESSAGE_LENGTH];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, MAX_EXCEPTION_MESSAGE_LENGTH, fmt, args);
  va_end(args);
  try {
    ret.reset(new char[strlen(buffer)+1]);
    strcpy(ret.get(), buffer);
    return ret;
  } catch (std::bad_alloc)
  {
    return ret;
  }
}

BIO_NAMESPACE_END



