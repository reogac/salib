/**
 @file TimecourseFeatures.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  TimecourseFeatures_INC
#define  TimecourseFeatures_INC
#include <vector>

#include "ModelOutput.h"

BIO_NAMESPACE_BEGIN

class TimecourseFeatures : public ModelOutput::Features
{
  public:
    TimecourseFeatures(const std::vector<double>& timesteps);
    int size() const;
  protected:
    const std::vector<double>& m_Timesteps;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef TimecourseFeatures_INC  ----- */

