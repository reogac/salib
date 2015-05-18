/**
 @file ErrorMessage.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#ifndef  ErrorMessage_INC
#define  ErrorMessage_INC


#include "Logger.h"
#include "utils.h"

BIO_NAMESPACE_BEGIN

#define LOG_ERROR(errMsg) Logger::instance() << errMsg;

#define  FATAL_UNKNOWN  0
#define  FATAL_OUT_OF_MEMORY 1

typedef enum {
  LEVEL_WARNING = 0,
  LEVEL_ERROR,
  LEVEL_FATAL
} ErrorLevel_t;

class ErrorMessage
{
  public:
    ErrorMessage(int code, ErrorLevel_t level = LEVEL_ERROR);
    ErrorMessage(int code, const std::string& message, ErrorLevel_t level = LEVEL_ERROR);

  virtual const char* getMessage() const;

  int getLevel() const;
  int getCode() const;
  protected:
    int m_Code;                        /** Error code number */
    int m_Level;                       /** Severity level */
    const std::string m_Message;

    ErrorMessage(const ErrorMessage& other) = delete;
    ErrorMessage& operator= (const ErrorMessage& other) = delete;
    
  private:
    friend Logger& operator<< (Logger& logger, const ErrorMessage& msg);
    virtual void write(Logger& logger) const;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef ErrorMessage_INC  ----- */
