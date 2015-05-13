/**
 @file SolverError.cpp
 @brief Implementation for SolverError class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#include "SolverError.h"

#include <iostream>
#include <cstdarg>

#define ERROR_MESSAGE_MAX_LENGTH 1024

BIO_NAMESPACE_BEGIN

SolverErrorMessage_t SolverError::g_OutOfMemory = { 0 , "out of memory" };

SolverError::SolverError()
  : m_Valid(true)
{

}

SolverError::~SolverError()
{
  clear();
}

void SolverError::clear(const SolverErrorType_t& type)
{
  if (type == SOLVER_WARNING)
  {
    for (auto& error : m_Warnings)
    {
      delete error;
    }
    m_Warnings.clear();
  } else if (type == SOLVER_ERROR)
  {
    for (auto& error : m_Errors)
    {
      delete error;
    }
    m_Errors.clear();
  }
}

void SolverError::clear()
{
  clear(SOLVER_WARNING);
  clear(SOLVER_ERROR);
}

void SolverError::addError(const SolverErrorType_t& type, int code, const char* fmt, ...)
{
  char buffer[ERROR_MESSAGE_MAX_LENGTH];
  SolverErrorMessage_t* error = nullptr;
  try {
    va_list args;
    vsnprintf(buffer, ERROR_MESSAGE_MAX_LENGTH, fmt, args);
    va_end(args);

    error = new SolverErrorMessage_t();
    error->m_Code = code;
    error->m_Message = std::string(buffer);
    switch (type)
    {
      case SOLVER_ERROR:        
        m_Errors.emplace_back(error);
        break;
      case SOLVER_WARNING:
        m_Warnings.emplace_back(error);
        break;
      default:
        delete error;
        break;
    }
  } catch (...)
  {
    delete error;
    throw;
  }
}

int SolverError::getNumErrors() const
{
  int numErrors = m_Valid? 0:1;
  numErrors += (m_Warnings.size() + m_Errors.size());
  return numErrors;
}

int SolverError::getNumErrors(const SolverErrorType_t& type) const
{
  int numErrors = m_Valid? 0:1;
  switch (type)
  {
    case SOLVER_WARNING:
      numErrors += m_Warnings.size();
      break;
    case SOLVER_ERROR:
      numErrors += m_Errors.size();
      break;
    default:
      break;
  }
  return numErrors;
}

void SolverError::dumpErrors()
{
  int iError = 0;
  //dump warnings
  if (m_Warnings.size() >0) {
    std::cout << "There was " << m_Warnings.size() << " warning(s)" << std::endl;
    for (auto& error : m_Warnings)
    {
      std::cout << "WARNING: " << error->m_Message << std::endl;
      delete error;
    }
    m_Warnings.clear();
  }

  //dump normal errors
  if (m_Errors.size() >0) {
    std::cout << "There was " << m_Errors.size() << " error(s)" << std::endl;
    for (auto& error : m_Errors)
    {
      std::cout << "ERROR: " << error->m_Message << std::endl;
      delete error;
    }
    m_Errors.clear();
  }  
}

BIO_NAMESPACE_END

