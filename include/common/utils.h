/**
 @file utils.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#ifndef  utils_INC
#define  utils_INC

#include "namespace.h"

#include <memory>

BIO_NAMESPACE_BEGIN

extern "C" {
  const char UnknownErrorString[] = "Unknown error";
  std::unique_ptr<char[]> formatMessage(const char* fmt, ...) noexcept;
}

BIO_NAMESPACE_END


#endif   /* ----- #ifndef utils_INC  ----- */
