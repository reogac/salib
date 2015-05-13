/**
 @file CvodeSettings.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */
#include "CvodeSettings.h"

BIO_NAMESPACE_BEGIN

CvodeSettings::CvodeSettings()
  : SolverSettings(ODE_SOLVER_CVODE)
  , m_AError(10e-6)
  , m_RError(10e-6)
{
}

CvodeSettings::CvodeSettings(const CvodeSettings& other)
  : SolverSettings(other)
  , m_AError(other.m_AError)
  , m_RError(other.m_RError)
{
}
CvodeSettings& CvodeSettings::operator= (const CvodeSettings& other)
{
  if (this != &other)
  {
    SolverSettings::operator=(other);
    m_AError = other.m_AError;
    m_RError = other.m_RError;
  }
  return *this;
}

CvodeSettings* CvodeSettings::clone() const 
{
  return new CvodeSettings(*this);
}

BIO_NAMESPACE_END


