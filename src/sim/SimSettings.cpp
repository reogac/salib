/**
 @file SimSettings.cpp
 @brief Implementation for SimSettings class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */
#include "SimSettings.h"

#include <cstdio>

#include "CommonDefs.h"

BIO_NAMESPACE_BEGIN

SimSettings::SimSettings()
  :SimSettings(DEFAULT_SIM_NUMSTEPS, DEFAULT_SIM_DURATION)
{
}

SimSettings::SimSettings(const int numIntervals, const double timeValue, bool
    isDuration /* true */)
{
  if (isDuration)
  {
    double interval = timeValue/numIntervals;
    setTimeSteps(numIntervals, interval);
    m_TimeSteps[m_NumSteps-1] = timeValue; //make sure the last time value equals to 'duration'
  }
  else
    setTimeSteps(numIntervals, timeValue);
}

SimSettings::SimSettings(const double* timeSteps, const int numSteps)
{
  m_NumSteps = numSteps+1;
  m_TimeSteps = new double[m_NumSteps];
  m_TimeSteps[0] = 0;
  for (int iStep = 1; iStep < m_NumSteps; iStep++)
    m_TimeSteps[iStep] = timeSteps[iStep-1];

}

SimSettings::SimSettings(const SimSettings& other)
{
  m_NumSteps = other.getNumSteps();
  m_TimeSteps = new double[other.getNumSteps()];
  const double* timeSteps = other.getTimeSteps();
  for (int iStep=0; iStep < m_NumSteps; iStep++)
    m_TimeSteps[iStep] = timeSteps[iStep];
}

SimSettings::~SimSettings()
{
  delete m_TimeSteps;
}


int SimSettings::getNumSteps() const
{
  return m_NumSteps;
}

const double* SimSettings::getTimeSteps() const
{
  return m_TimeSteps;
}

double SimSettings::getTimeStep(const int step) const
{
  return m_TimeSteps[step];
}


void SimSettings::setTimeSteps(const int numIntervals, const double interval)
{
  m_NumSteps = numIntervals + 1;
  m_TimeSteps = new double[m_NumSteps];

  double offset = 0;

  m_TimeSteps[0] = offset;
  for (int iInterval =1; iInterval < m_NumSteps; iInterval++)
  {
    offset += interval;
    m_TimeSteps[iInterval] = offset;
  }
}


BIO_NAMESPACE_END

