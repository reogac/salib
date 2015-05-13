/**
 @file ErrorMessage.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "ErrorMessage.h"

BIO_NAMESPACE_BEGIN

ErrorMessage::ErrorMessage(int code, ErrorLevel_t level /* ERROR_ERROR */
    , std::string msg /* string("") */)
  : m_Code(code)
  , m_Level(level)
  , m_Message(msg)
{

}

ErrorMessage::~ErrorMessage()
{  
}

/* 
const std::string& ErrorMessage::getMessage() const
{
  return m_Message;
}
*/

const char* ErrorMessage::getMessage() const
{
  return m_Message.c_str();
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
    logger.write(LOG_WARNING, m_Message.c_str());
  else
    logger.write(LOG_ERROR, m_Message.c_str());
}

Logger& operator<< (Logger& logger, const ErrorMessage& msg)
{
  msg.write(logger);
  return logger;
}

BIO_NAMESPACE_END


