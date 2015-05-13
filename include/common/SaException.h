/**
 @file SaException.h
 @brief Base class for exceptions
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  SaException_INC
#define  SaException_INC

#include <string>

#include "namespace.h"

BIO_NAMESPACE_BEGIN

class SaException
{
  public:
    SaException(const std::string& message)
     : m_Message(message) {};
    virtual const char* what() const { return m_Message.c_str(); };
  protected:
    std::string m_Message;
};

class InputException : public SaException
{
  public:
    InputException(const std::string& message)
      : SaException(message) {} ;
};

class ModelParsingException : public SaException
{
  public:
    ModelParsingException(const std::string& message)
      : SaException(message) {} ;
};

BIO_NAMESPACE_END
#endif   /* ----- #ifndef SaException_INC  ----- */
