/**
 @file ErrorList.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */
#include "ErrorList.h"

#include <cassert>
BIO_NAMESPACE_BEGIN

static ErrorMessage OutOfMemoryError(FATAL_OUT_OF_MEMORY, LEVEL_FATAL);
static ErrorMessage UnknowError(FATAL_UNKNOWN, LEVEL_FATAL);

ErrorList::ErrorList()
  : m_Valid(true)
  , m_NumErrors(0)
  , m_NumFatals(0)
{

}

ErrorList::ErrorList(ErrorList&& other)
  : m_Valid(other.m_Valid)
  , m_NumErrors(other.m_NumErrors)
  , m_NumFatals(other.m_NumFatals)
  , m_Errors(std::move(other.m_Errors))
{
}

int ErrorList::getNumMessages() const
{
  return m_Valid? m_Errors.size() : m_Errors.size() + 1;
}

int ErrorList::getNumErrors() const
{
  return m_NumErrors;
}

int ErrorList::getNumFatals() const
{
  return m_Valid ? m_NumFatals : m_NumFatals+1;
}

int ErrorList::getNumWarnings() const
{
  return m_Errors.size() - m_NumErrors - m_NumFatals;
}

const ErrorMessage& ErrorList::getError(const int index) const
{
  assert(index >=0
      && (index - m_Errors.size()) < (m_Valid? 0 : 1));
  if (!m_Valid
      && index == m_Errors.size())
    return OutOfMemoryError;
  
  return *m_Errors[index];
}


void ErrorList::add(ErrorMessage*&& message) noexcept
{
  if (message == nullptr)                       /* getting a null pointer means the message was not allocated */
  {
    m_Valid = false;
    return;
  }
  if (!m_Valid)
  {
    delete message;
    return;
  }
  try
  {
    m_Errors.emplace_back(std::move(message));
    if (m_Errors.back()->getLevel() == LEVEL_ERROR)
      m_NumErrors++;

    else if (m_Errors.back()->getLevel() == LEVEL_FATAL)
      m_NumFatals++;
  } catch (std::bad_alloc&)
  {
    m_Valid = false;
  }
}
void ErrorList::add(std::unique_ptr<ErrorMessage>&& message) noexcept
{
  if (!m_Valid)
    return;
  try
  {
    m_Errors.emplace_back(std::move(message));
    if (m_Errors.back()->getLevel() == LEVEL_ERROR)
      m_NumErrors++;

    else if (m_Errors.back()->getLevel() == LEVEL_FATAL)
      m_NumFatals++;
  } catch (std::bad_alloc&)
  {
    m_Valid = false;
  }
}

void ErrorList::clear()
{
  m_Errors.empty();
}

void ErrorList::write(Logger& logger) const
{
  for (auto& err : m_Errors)
  {
    logger << *err;
  }
  if (!m_Valid)
  {
    logger << OutOfMemoryError;
  }
}

Logger& operator<<(Logger& logger, const ErrorList& errors)
{
  errors.write(logger);
  return logger;
}

BIO_NAMESPACE_END


