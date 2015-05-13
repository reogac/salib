/**
 @file SaError.cpp
 @brief Implementation for SaError class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#include "SaError.h"

#include <iostream>
#include <cstdarg>

BIO_NAMESPACE_BEGIN

#define ERROR_MESSAGE_MAX_LENGTH 1024

SaErrorMessage_t SaError::g_OutOfMemory = { 0 , "out of memory" };

SaError::SaError()
  : m_Valid(true)
{

}

SaError::~SaError()
{
  clear();
}

void SaError::clear()
{
  int iError=0;
  for (iError=0; iError < m_Warnings.size(); iError++)
    delete m_Warnings[iError];
  m_Warnings.clear();

  for (iError=0; iError < m_Errors.size(); iError++)
    delete m_Errors[iError];
  m_Errors.clear();

  for (iError=0; iError < m_Fatals.size(); iError++)
    delete m_Fatals[iError];
  m_Fatals.clear();
}

void SaError::addError(ErrorType_t type, int code, const char* fmt, ...)
{
  char buffer[ERROR_MESSAGE_MAX_LENGTH];
  SaErrorMessage_t* error = nullptr;
  try {
    va_list args;
    va_start(args, fmt);
    vsnprintf(buffer, ERROR_MESSAGE_MAX_LENGTH, fmt, args);
    va_end(args);

    error = new SaErrorMessage_t();
    error->m_Code = code;
    error->m_Message = std::string(buffer);
    switch (type)
    {
      case ERROR_ERROR:        
        m_Errors.emplace_back(error);
        break;
      case ERROR_WARNING:
        m_Warnings.emplace_back(error);
        break;
      default:
        m_Fatals.emplace_back(error);
        break;
    }
  } catch (std::bad_alloc& )
  {
    delete error;
    m_Valid = false;
  }
}

void SaError::devalid()
{
  m_Valid = false;
}

int SaError::getNumErrors() const
{
  int numErrors = m_Valid? 0:1;
  numErrors += (m_Warnings.size() + m_Errors.size() + m_Fatals.size());
  return numErrors;
}

int SaError::getNumErrors(ErrorType_t type) const
{
  int numErrors = m_Valid? 0:1;
  switch (type)
  {
    case ERROR_WARNING:
      numErrors += m_Warnings.size();
      break;
    case ERROR_ERROR:
      numErrors += m_Errors.size();
      break;
    default:
      numErrors += m_Fatals.size();
  }
  return numErrors;
}

void SaError::dumpErrors()
{
  int iError = 0;
  //dump warnings
  if (m_Warnings.size() >0) {
    std::cout << "There was " << m_Warnings.size() << " warning(s)" << std::endl;
    for (iError=0; iError < m_Warnings.size(); iError++)
    {
      std::cout << "WARNING: " << m_Warnings[iError]->m_Message << std::endl;
      delete m_Warnings[iError];
    }
    m_Warnings.clear();
  }

  //dump normal errors
  if (m_Errors.size() >0) {
    std::cout << "There was " << m_Errors.size() << " error(s)" << std::endl;
    for (iError=0; iError < m_Errors.size(); iError++)
    {
      std::cout << "ERROR: " << m_Errors[iError]->m_Message << std::endl;
      delete m_Errors[iError];
    }
    m_Errors.clear();
  }

  //dump fatal errors
  if (m_Fatals.size() >0) {
    std::cout << "There was " << m_Fatals.size() << " fatal error(s)" << std::endl;
    for (iError=0; iError < m_Fatals.size(); iError++)
    {
     std::cout << "FATAL: " << m_Fatals[iError]->m_Message << std::endl;
     delete m_Fatals[iError];
    }
    m_Fatals.clear();
  }
}

BIO_NAMESPACE_END

