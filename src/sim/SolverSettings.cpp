/**
 @file SolverSettings.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "SolverSettings.h"

BIO_NAMESPACE_BEGIN

SolverSettings::SolverSettings(const SolverType_t type)
  : m_Type(type)
  , m_MaxSteps(5000)
{
}

SolverSettings::SolverSettings(const SolverSettings& other)
  : m_Type(other.m_Type)
  , m_MaxSteps(other.m_MaxSteps)
{
}

SolverSettings& SolverSettings::operator= (const SolverSettings& other)
{
  if (this != &other)
  {
    m_Type = other.m_Type;
    m_MaxSteps = other.m_MaxSteps;
  }
  return *this;
}

BIO_NAMESPACE_END


