/**
 @file Solver.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "Solver.h"

#include "common/Logger.h"

  
BIO_NAMESPACE_BEGIN

Solver::Solver(const SolverSettings* settings)
  : m_Errors()
  , m_Data(new SolverData(m_Errors))
  , m_Settings(settings)
  , m_Model(nullptr)
  , m_Solver(nullptr)
{
}
void Solver::checkErrors()
{
  if (m_Errors.getNumErrors() + m_Errors.getNumFatals() > 0)
    throw std::move(m_Errors);
}
void Solver::initialize(const OdeStruct* model)
{ 
  m_Errors.clear();
  m_Model = model;
  m_Data->initialize(model);
  checkErrors();
  //now setup solver
  //TODO: depends on the model and SolverSettings, create a right solver
  m_Solver.reset(new CvodeSolver1(this));

  m_Solver->initialize();
  checkErrors();
}

void Solver::reset(const SolverSettings* settings)
{
  m_Errors.clear();
  m_Settings = settings;

  //reset solver data
  m_Data->reset();
  checkErrors();
  //setup solver structure
  m_Solver->initialize();
  checkErrors();
}

void Solver::reset()
{
  m_Errors.clear();
  //reset solver data
//  m_T0 = m_Model->getInitialTime();
  m_Data->reset();
  checkErrors();
  m_Solver->reset();
  checkErrors();
}

void Solver::reset(const std::vector<int>& indexes, const double* values)
{
  m_Errors.clear();
  //reset solver data and solver itself
  m_Data->reset(indexes, values);
//  m_T0 = m_Model->getInitialTime();
  m_Solver->reset();
  checkErrors();
}

void Solver::solve(const double tout)
{
  //call the internal solver to avance the integration from time m_T0 to 'time'
  //internally it will call 'm_Data->f(time, m_Y, m_YDot)' multiple time to
  //evaluate the rhs of the odes, depending on the type of ode solver
  m_Solver->solve(tout);
  checkErrors();
  // if the solver successfully advance to 'time' update solver data
}

BIO_NAMESPACE_END


