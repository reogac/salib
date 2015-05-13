/**
 @file CvodeSettings.cpp
 @brief implementation for CvodeSettings class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "CvodeSettings.h"

BIO_NAMESPACE_BEGIN

CvodeSettings::CvodeSettings()
  : SolverSettings(ODE_SOLVER_CVODE)
    , m_ATol(10e-6)
    , m_RTol(10e-5)
    , m_MaxInternalStep(10000)
    , m_MaxOrder(6)
{
}

CvodeSettings::CvodeSettings(const CvodeSettings& other)
  :SolverSettings(other)
{
  m_ATol = other.getATol();
  m_RTol = other.getRTol();
  m_MaxInternalStep = other.getMaxInternalStep();
  m_MaxOrder = other.getMaxOrder();
}
CvodeSettings* CvodeSettings::clone() const
{
  return new CvodeSettings(*this);
}

CvodeSettings& CvodeSettings::operator= (const CvodeSettings&
    other)
{
  if (this != &other)
  {
    SolverSettings::operator=(other);
    m_ATol = other.getATol();
    m_RTol = other.getRTol();
    m_MaxInternalStep = other.getMaxInternalStep();
    m_MaxOrder = other.getMaxOrder();
  }
  return *this;
}

CvodeSettings& CvodeSettings::setATol(const double atol)
{
  m_ATol = atol;
  return *this;
}

double CvodeSettings::getATol() const
{
  return m_ATol;
}

CvodeSettings& CvodeSettings::setRTol(const double rtol)
{
  m_RTol = rtol;
  return *this;
}

double CvodeSettings::getRTol() const
{
  return m_RTol;
}
    
int CvodeSettings::getMaxInternalStep() const
{
  return m_MaxInternalStep;

}

CvodeSettings& CvodeSettings::setMaxInternalStep(const int num)
{
  m_MaxInternalStep = num;
}

int CvodeSettings::getMaxOrder() const
{
  return m_MaxOrder;
}
  
CvodeSettings& CvodeSettings::setMaxOrder(const int order)
{
  m_MaxOrder = order;
  return *this;
}

BIO_NAMESPACE_END

