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

#include <sbml/SBMLTypes.h>
#include <iostream>
#include <iomanip>

BIO_NAMESPACE_BEGIN

SbmlSim1::SbmlSim1()
  : m_Sbml(nullptr)
  , m_OdeStruct(nullptr)
  , m_Solver(nullptr)
  , m_Settings(new CvodeSettings())
  , m_CurrentStep(0)
  , m_NumSteps(100)
  , m_Times(new double [m_NumSteps+1])
 // , m_Results(nullptr)
{
  //create default timepoints
  for (unsigned int i=0; i<=m_NumSteps; ++i)
    m_Times[i] = i*5;
}

void SbmlSim1::loadSbml(const char* filename)
{
  SBMLReader reader;
  m_Sbml.reset(reader.readSBMLFromFile(filename));
  if (m_Sbml->getNumErrors() == 0)
  {
    if (m_Sbml->getLevel() <=2) 
    {//try to convert to level 2 version 4 Sbml format
      m_Sbml->setConsistencyChecksForConversion( LIBSBML_CAT_UNITS_CONSISTENCY, false);
      m_Sbml->setConsistencyChecksForConversion( LIBSBML_CAT_SBO_CONSISTENCY, false);

      if (m_Sbml->checkL2v4Compatibility() == 0)
        m_Sbml->setLevelAndVersion(2,4);
    }
  }

  if (m_Sbml->getNumErrors() >0)
  {
    //TODO: throw an exception
  }
}

void SbmlSim1::initialize()
{
  if (!m_Sbml)
  {
    //throw an exception
  }

  OdeStructBuilder builder(*(m_Sbml->getModel()));
  m_OdeStruct.reset(builder.build());
  m_Solver.reset(new Solver(m_Settings.get()));
  m_Solver->initialize(m_OdeStruct.get());
  m_CurrentStep = 0;
}

void SbmlSim1::setTime(const std::vector<double>& time)
{
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
}


void SbmlSim1::setTime(const double duration, const int numSteps)
{
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
}

void SbmlSim1::setSolverSettings(std::unique_ptr<SolverSettings>&& settings)
{
  m_Settings = std::move(settings);
}

void SbmlSim1::setSolverSettings(const SolverSettings& settings)
{
  m_Settings.reset(settings.clone());
}


void SbmlSim1::setVaryingVariables(std::vector<std::string>& variables)
{
}
bool SbmlSim1::isDone() const
{
  return (m_CurrentStep == m_NumSteps);
}

void SbmlSim1::simulateOne()
{
  m_Solver->solve(m_Times[m_CurrentStep+1]);
  m_CurrentStep++;
}

void SbmlSim1::simulate()
{
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

  //std::cout << *m_Results;
}

void SbmlSim1::reset()
{
  m_Solver->reset();
  m_CurrentStep = 0;
}

void SbmlSim1::reset(const double* values)
{
  m_Solver->reset(m_VaryingVariableIndexes, values);
  m_CurrentStep = 0;
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


