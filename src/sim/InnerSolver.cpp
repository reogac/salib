/**
 @file InnerSolver.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "InnerSolver.h"

#include "common/Logger.h"

#include "Solver.h"

BIO_NAMESPACE_BEGIN

InnerSolver::InnerSolver(Solver* wrapper)
  : wrapper_(wrapper)
  , m_Time(0.0)
  , m_NumOdes(wrapper->m_Model->getNumOdes())
  , m_Errors(wrapper->m_Errors)
  , m_Data(wrapper->m_Data.get())
{
}

InnerSolver::~InnerSolver()
{ 
}

const SolverSettings* InnerSolver::getSettings() const 
{
  return wrapper_->m_Settings;
}


const OdeStruct* InnerSolver::getModel()
{
  return wrapper_->m_Model;
}
void InnerSolver::setInitialCondition()
{
  //set initial time
  m_Time = wrapper_->m_Model->getInitialTime();
  //set initial values for ode variables
  double* y = getY();
  const double* values = m_Data->getValues();
  for (unsigned int i=0; i<m_NumOdes; ++i)
    y[i] = values[i];
}

void InnerSolver::initialize()
{
  setInitialCondition();
  doInitialize();
}

void InnerSolver::reset()
{
  setInitialCondition();
  doReset();
}

BIO_NAMESPACE_END

