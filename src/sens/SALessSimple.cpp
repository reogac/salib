/**
 @file SALessSimple.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "SALessSimple.h"

BIO_NAMESPACE_BEGIN

SALessSimple::SALessSimple(const SAMethod_t method)
  : SABase(method)
  , m_SaveInput(false)
  , m_SaveOutput(false)
{ 
}

void SALessSimple::setSaveInput(const bool save)
{
  m_SaveInput = save;
}
void SALessSimple::setSaveOutput(const bool save)
{
  m_SaveOutput = save;
}

BIO_NAMESPACE_END
