/**
 @file SolverSettings.cpp
 @brief Implementation for SolverSettings class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "SolverSettings.h"

BIO_NAMESPACE_BEGIN

SolverSettings::SolverSettings(const int type)
  : m_SolverType(type)
{
}
SolverSettings::~SolverSettings()
{
}

SolverSettings::SolverSettings(const SolverSettings& other)
{
  m_SolverType = other.getSolverType();
}

SolverSettings* SolverSettings::clone() const
{
  return new SolverSettings(*this);
}

SolverSettings& SolverSettings::operator= (const SolverSettings& other)
{
  if (this != &other )
  {
    m_SolverType = other.m_SolverType;
  }
  return *this;
}
int SolverSettings::getSolverType() const
{
  return m_SolverType;
}

BIO_NAMESPACE_END

