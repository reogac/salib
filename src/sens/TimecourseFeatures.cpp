/**
 @file TimecourseFeatures.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "TimecourseFeatures.h"

BIO_NAMESPACE_BEGIN

TimecourseFeatures::TimecourseFeatures(const std::vector<double>& timesteps)
  : m_Timesteps(timesteps)
{

}

int TimecourseFeatures::size() const
{
  return 1;
}

BIO_NAMESPACE_END

