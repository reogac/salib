/**
 @file InputDist.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "InputDist.h"
#include "common/SaException.h"

BIO_NAMESPACE_BEGIN

InputDist::InputDist(const InputDistType_t& type /* DIST_UNKNOWN */)
  : m_Type(type)
{
}

InputUniform::InputUniform(const double& lower, const double& upper)
  : InputDist(DIST_UNIFORM)
    , lower_(lower)
    , upper_(upper)
{
  if (lower_ >= upper_)
  {
    throw InputException("Lower bound must be smaller than upper bound");
  }
}

double InputUniform::getLower() const
{
  return lower_;
}

double InputUniform::getUpper() const
{
  return upper_;
}

InputNormal::InputNormal(const double mean, const double std)
  : InputDist(DIST_NORMAL)
  , mean_(mean)
  , std_(std)
{
}

double InputNormal::getMean() const
{
  return mean_;
}

double InputNormal::getStd() const
{
  return std_;
}

BIO_NAMESPACE_END

