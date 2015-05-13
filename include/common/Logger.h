/**
 @file Logger.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  Logger_INC
#define  Logger_INC

#include <ostream>

#include "namespace.h"

BIO_NAMESPACE_BEGIN

#define USE_LOGGER

#ifdef USE_LOGGER

#define VDEBUG(fmt, ...) Logger::instance().vwrite(LOG_DEBUG, fmt, __VA_ARGS__);
#define VINFO(fmt, ...) Logger::instance().vwrite(LOG_INFO, fmt, __VA_ARGS__);
#define VWARNING(fmt, ...) Logger::instance().vwrite(LOG_WARNING, fmt, __VA_ARGS__);
#define VERROR(fmt, ...) Logger::instance().vwrite(LOG_ERROR, fmt, __VA_ARGS__);

#define DEBUG(msg) Logger::instance().write(LOG_DEBUG, msg);
#define INFO(msg) Logger::instance().write(LOG_INFO, msg);
#define WARNING(msg) Logger::instance().write(LOG_WARNING, msg);
#define ERROR(msg) Logger::instance().write(LOG_ERROR, msg);

#else

#define VDEBUG(fmt, ...)
#define VINFO(fmt, ...) 
#define VWARNING(fmt, ...)
#define VERROR(fmt, ...)

#define DEBUG(msg)
#define INFO(msg)
#define ERROR(msg)
#define WARNING(msg)

#endif

typedef enum
{
  LOG_ERROR = 0,
  LOG_WARNING = 1,
  LOG_INFO = 2,
  LOG_DEBUG = 3
} LogLevel_t;

class Logger
{
  public:
    static Logger& instance();
    static void setWriter(std::ostream& stream);
    static void setLevel(const LogLevel_t level);

    void write(const LogLevel_t level, const char* message);
    void vwrite(const LogLevel_t level, const char* fmt, ...);

  private:
    Logger();
    Logger(const Logger& other) = delete;
    Logger& operator=(const Logger& other) = delete;
    
    std::ostream* stream_;
    unsigned int level_;

    static Logger instance_;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef Logger_INC  ----- */
