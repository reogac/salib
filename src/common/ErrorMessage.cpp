/**
 @file ErrorMessage.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "ErrorMessage.h"

BIO_NAMESPACE_BEGIN

ErrorMessage::ErrorMessage(int code, ErrorLevel_t level /* ERROR_ERROR */)
  : m_Code(code)
  , m_Level(level)
  , m_Message(nullptr)
{ 
}
ErrorMessage::ErrorMessage(int code, std::unique_ptr<char[]>&& message, ErrorLevel_t level /* ERROR_ERROR */)
  : m_Code(code)
  , m_Level(level)
  , m_Message(std::move(message))
{
}


const char* ErrorMessage::getMessage() const
{
  if (m_Message)
    return m_Message.get();
  return UnknownErrorString;
}

int ErrorMessage::getLevel() const
{
  return m_Level;
}

int ErrorMessage::getCode() const
{
  return m_Code;
}


void ErrorMessage::write(Logger& logger) const
{
  if (m_Level == LEVEL_WARNING)
    logger.write(LOG_WARNING, getMessage());
  else
    logger.write(LOG_ERROR, getMessage());
}

Logger& operator<< (Logger& logger, const ErrorMessage& msg)
{
  msg.write(logger);
  return logger;
}

BIO_NAMESPACE_END


