/**
 @file InputDist.h
 @brief Input distribution
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  InputDist_INC
#define  InputDist_INC

#include "common/namespace.h"

BIO_NAMESPACE_BEGIN

typedef enum
{
  DIST_UNKNOWN = 0,
  DIST_UNIFORM,
  DIST_NORMAL,
} InputDistType_t;

class InputDist
{
  public:
    InputDist(const InputDistType_t& type = DIST_UNKNOWN);
    int getType() const { return m_Type; }
  protected:
    int m_Type;

  private:
    InputDist(const InputDist& other) = delete;
    InputDist& operator=(const InputDist& other) = delete;
};

class InputUniform : public InputDist
{
  public:
    InputUniform(const double& lower, const double& upper);

    double getUpper() const;
    double getLower() const;

  private:
    InputUniform(const InputUniform& other) = delete;
    InputUniform& operator=(const InputUniform& other) = delete;

    double upper_;
    double lower_;
};

class InputNormal : public InputDist
{
  public:
    InputNormal(const double mean, const double std);

    double getMean() const;
    double getStd() const;

  private:
    InputNormal(const InputNormal& other) = delete;
    InputNormal& operator=(const InputNormal& other) = delete;

    double mean_;
    double std_;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef InputDist_INC  ----- */
