/**
 @file SbmlSim1.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */
#include "SbmlSim1.h"

#include "common/Logger.h"
#include "OdeStructBuilder.h"
#include "SimException.h"
#include "CvodeSettings.h"
//#include "SimResultRecorder.h"

#include <iostream>
#include <iomanip>

BIO_NAMESPACE_BEGIN

SbmlSim1::SbmlSim1(const Model& model)
  : m_Model(model)
  , m_Settings(new CvodeSettings())
  , m_Status(SIM_NOT_READY)
  , m_CurrentStep(0)
  , m_NumSteps(100)
  , m_Times(new double [m_NumSteps+1])
 // , m_Results(nullptr)
{
  //create default timepoints
  for (unsigned int i=0; i<=m_NumSteps; ++i)
    m_Times[i] = i*5;
}

void SbmlSim1::initialize()
{
  m_Status = SIM_NOT_READY;

  if (!m_OdeStruct
      || m_VaryingFlag)                         /* needs to build ode structures */
  {
    OdeStructBuilder builder(m_Model);
    m_OdeStruct.reset(builder.build());
    m_Solver.reset(new Solver(m_Settings.get()));//solver has to be reset
    m_Solver->initialize(m_OdeStruct.get());
  } else if (m_SettingsFlag)                    /* solver settings were changed*/
  {
    m_Solver->reset(m_Settings.get());
  } else if (m_TimeFlag)                        /* simulation time was change */
  {
    m_Solver->reset();
  }
  m_CurrentStep = 0;
  m_Status = SIM_READY;
}

void SbmlSim1::setTime(const std::vector<double>& time)
{
  if (m_Status != SIM_NOT_READY
      && m_Status != SIM_DONE)
    throw SimException(SIM_SOLVER_BUSY);

  if (time.size() == 0)
    throw SimException(SIM_EMPTY_TIMEPOINT);

  //set time here
  std::unique_ptr<double []> ts(new double[time.size()+1]);
  ts[0] = 0.0;
  for (int i=0; i< time.size(); ++i)
  {
    if (time[i] <= ts[i] )
      throw SimException(SIM_INVALID_TIMEPOINT);
    ts[i+1] = time[i];
  }
  m_Times = std::move(ts);
  m_NumSteps = time.size();

  if (m_Status != SIM_NOT_READY)
    m_TimeFlag = true;
}


void SbmlSim1::setTime(const double duration, const int numSteps)
{
  if (m_Status != SIM_NOT_READY
      && m_Status != SIM_DONE)
    throw SimException(SIM_SOLVER_BUSY);

  if (duration<=0)
     throw SimException(SIM_INVALID_TIME_DURATION);
  if (numSteps <=0)
    throw SimException(SIM_INVALID_NUM_TIMESTEPS);
  
  std::unique_ptr<double[]> ts(new double[numSteps+1]);
  double interval = duration/numSteps;
  ts[0] = 0.0;
  for (int i=1; i<numSteps; ++i)
    ts[i] = i*interval;
  ts[numSteps] = duration;
  m_Times = std::move(ts);
  m_NumSteps = numSteps;

  if (m_Status != SIM_NOT_READY)
    m_TimeFlag = true;
}

void SbmlSim1::setSolverSettings(std::unique_ptr<SolverSettings>&& settings)
{
  if (m_Status != SIM_NOT_READY
      && m_Status != SIM_DONE)
    throw SimException(SIM_SOLVER_BUSY);

  m_Settings = std::move(settings);

  if (m_Status != SIM_NOT_READY)
    m_SettingsFlag = true;
}

void SbmlSim1::setSolverSettings(const SolverSettings& settings)
{
  if (m_Status != SIM_NOT_READY
      && m_Status != SIM_DONE)
    throw SimException(SIM_SOLVER_BUSY);

  m_Settings.reset(settings.clone());

  if (m_Status != SIM_NOT_READY)
    m_SettingsFlag = true;
}


void SbmlSim1::setVaryingVariables(std::vector<std::string>& variables)
{
  if (m_Status != SIM_NOT_READY
      && m_Status != SIM_DONE)
    throw SimException(SIM_SOLVER_BUSY);

  if (m_Status != SIM_NOT_READY)
    m_VaryingFlag = true;

}
bool SbmlSim1::isDone() const
{
  return m_Status == SIM_DONE;
}

void SbmlSim1::simulateOne()
{
  if (m_Status != SIM_READY)
    throw SimException(SIM_SOLVER_NOT_READY);

  m_Solver->solve(m_Times[m_CurrentStep+1]);
  m_CurrentStep++;
  if (m_CurrentStep == m_NumSteps)
    m_Status = SIM_DONE;
}

void SbmlSim1::simulate()
{
  if (m_Status != SIM_READY)
    throw SimException(SIM_SOLVER_NOT_READY);
  
  //m_Results.reset(new SimResults(m_Model, m_Times.get(), m_NumSteps+1));
  //SimResultRecorder recorder(m_Results.get(), m_OdeStruct.get());

  const double* values = m_Solver->getValues();
  const double* times = m_Times.get();
  //recorder.record(values, m_CurrentStep);

  while( m_CurrentStep<m_NumSteps )
  {
    m_Solver->solve(times[m_CurrentStep+1]);
    m_CurrentStep++;
    //recorder.record(values, m_CurrentStep);
  }

  m_Status = SIM_DONE;
  //std::cout << *m_Results;
}

void SbmlSim1::reset()
{
  m_Solver->reset();
  m_CurrentStep = 0;
  m_Status = SIM_READY;
}

void SbmlSim1::reset(const double* values)
{
  m_Solver->reset(m_VaryingVariableIndexes, values);
  m_CurrentStep = 0;
  m_Status = SIM_READY;
}
void SbmlSim1::printResults(const double* values)
{
  for (unsigned int i=0; i<m_OdeStruct->getNumVariables(); ++i)
  {
     // const OdeVariable* variable = m_OdeStruct->getVariable(i);
    std::cout << std::setw(6) << values[i] << " ";
  }
  std::cout << std::endl;
}
BIO_NAMESPACE_END


