/**
 @file Logger.cpp
 @
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */
#include "Logger.h"

#include <iostream>
#include <cstdarg>

BIO_NAMESPACE_BEGIN

#define ERROR_MESSAGE_MAX_LENGTH 1024

static const char* LogLevelText[] = {
  "Error",
  "Warning",
  "Info:",
  "Debug"
};

Logger Logger::instance_;

Logger::Logger()
  : stream_(&std::cerr)
  , level_(LOG_WARNING)
{
}

Logger& Logger::instance()
{
  return instance_;
}

void Logger::setWriter(std::ostream& stream)
{
  instance_.stream_ = &stream;
}

void Logger::setLevel(const LogLevel_t level)
{
  instance_.level_ = level;
}

void Logger::write(const LogLevel_t level, const char* message)
{
  if (level <= level_)
  {
    *stream_ << LogLevelText[level] << ": " << message << "\n";
  }
}

void Logger::vwrite(const LogLevel_t level, const char* fmt, ...)
{

  if (level > level_)
    return;

  char buffer[ERROR_MESSAGE_MAX_LENGTH];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buffer, ERROR_MESSAGE_MAX_LENGTH, fmt, args);
  va_end(args);
  *stream_ << LogLevelText[level] << ": " << buffer << "\n";
 }


BIO_NAMESPACE_END


