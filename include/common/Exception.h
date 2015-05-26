/**
 @file Exception.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  Exception_INC
#define  Exception_INC


#include "namespace.h"
#include "utils.h"

#include <stdexcept>
#include <sstream>

BIO_NAMESPACE_BEGIN


class Exception : public std::exception
{
  public:
    Exception(const int code);
    Exception(const int code, const std::string& message);
    Exception(Exception&& other);

  protected:
    Exception(const Exception& other) = delete;
    Exception& operator= (const Exception& other) = delete;

    int m_Code;
    const std::string m_Message;
};


BIO_NAMESPACE_END

#endif   /* ----- #ifndef Exception_INC  ----- */

