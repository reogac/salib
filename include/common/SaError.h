/**
 @file SaError.h
 @brief Header file for SaError class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  SaError_INC
#define  SaError_INC

#include <string>
#include <vector>

#include "namespace.h"

BIO_NAMESPACE_BEGIN

typedef enum 
{

  ERROR_WARNING = 0,
  ERROR_ERROR = 1,
  ERROR_FATAL = 2

} ErrorType_t;

typedef struct
{
  int m_Code;
  std::string m_Message;
  
} SaErrorMessage_t;


class SaError
{
  public:
    SaError();
    virtual ~SaError();
    
    void addError(ErrorType_t type, int code, const char* fmt, ...);

    bool isValid();

    int getNumErrors() const;
    int getNumErrors(ErrorType_t type) const;
    void dumpErrors(); /* print error and empty list */


  protected:
    void clear();

    std::vector<SaErrorMessage_t*> m_Warnings;

    std::vector<SaErrorMessage_t*> m_Errors;

    std::vector<SaErrorMessage_t*> m_Fatals;

    void devalid();
  private:
    static SaErrorMessage_t g_OutOfMemory;
    bool m_Valid;

    SaError(const SaError& other) = delete;
    SaError& operator= (const SaError& other) = delete;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef SaError_INC  ----- */

