/**
 @file SolverError.h
 @brief Header file for SolverError class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  SolverError_INC
#define  SolverError_INC

#include <string>
#include <vector>

#include "common/namespace.h"

BIO_NAMESPACE_BEGIN

typedef enum 
{
  SOLVER_WARNING = 0,
  SOLVER_ERROR = 1,

} SolverErrorType_t;

typedef struct
{
  int m_Code;
  std::string m_Message;
  
} SolverErrorMessage_t;


class SolverError
{
  public:
    SolverError();
    virtual ~SolverError();
    
    void addError(const SolverErrorType_t& type, int code, const char* fmt, ...);

    bool isValid();

    int getNumErrors() const;
    int getNumErrors(const SolverErrorType_t& type) const;
    void dumpErrors(); /* print error and empty list */
    
    void clear();
    void clear(const SolverErrorType_t& type);
  protected:

    std::vector<SolverErrorMessage_t*> m_Warnings;

    std::vector<SolverErrorMessage_t*> m_Errors;

    void devalid();
  private:
    static SolverErrorMessage_t g_OutOfMemory;
    bool m_Valid;

    SolverError(const SolverError& other) = delete;
    SolverError& operator= (const SolverError& other) = delete;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef SolverError_INC  ----- */

