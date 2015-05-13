/**
 @file SolverBase.cpp
 @brief Implementation for SolverBase class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#include "SolverBase.h"

#include <cassert>

#include "common/CommonDefs.h"

#include "OdeModel.h"
#include "SolverSettings.h"
#include "SolverHelper.h"
#include "SolverException.h"

BIO_NAMESPACE_BEGIN

SolverBase::SolverBase(const OdeModel& model)
  : m_Model(model)
    , m_Helper(nullptr)
    , m_Errors()
    , m_CurrentTime(0)
{
}

SolverBase::~SolverBase()
{
}

const double* SolverBase::getValues() const
{
  m_Helper->getValues();
}

const SolverError& SolverBase:: getErrors() const
{
  return m_Errors;
}

void SolverBase::init(const SolverSettings* settings, const std::set<int>& varIndexes)
{

  /* 1. create new helper */
  m_Helper.reset(createSolverHelper());

  assert(m_Helper);
  
  std::cout << "create helper successfully\n";
  /* 2. construct structures for math expression evaluation, optimize them */
  m_Helper->initStructures(varIndexes);

  std::cout << "init helper structures successfully\n";
  /* 3. set initial condition */
  m_Helper->initValues();

  std::cout << "sets up intial condition successfully\n";
  /* 4. create solver structure */
  if (createSolverStructure(settings)!=SATOOLS_SUCCESS)
  {
    throw SolverException("Failed to create solver internal structures");
  }

  std::cout << "creates solver structures successfully\n";
  /* 5. set current time value to zero */
  m_CurrentTime = 0;

}

void SolverBase::reinit(const std::set<int>& varIndexes)
{
  m_Errors.clear();
  /* 1. re-construct structures for math expression evaluation, optimize them */
  m_Helper->initStructures(varIndexes);
  
  /* 2. reset initial conditions */
  m_Helper->initValues();
  
  /* 3. reset solver structure */
  if (resetSolverStructure() != SATOOLS_SUCCESS)
  {
    throw SolverException("Failed to reset solver internal structures");
  }
  
  /* reset time value to zero */
  m_CurrentTime = 0;
}


void SolverBase::reinit(const SolverSettings* settings) {

  m_Errors.clear();
  /* 1. reset initial condition */
  m_Helper->initValues();

  /* 2. create a new solver structure */
  if (createSolverStructure(settings) != SATOOLS_SUCCESS)
  {
    throw SolverException("Failed to create solver internal structures");
  }

  /* 3. reset time value to zero */
  m_CurrentTime = 0;
}

/*
 * This function resets the solver to initial condition. As the same time, its
 * change the intial values of some model variables according to the input
 * parameters of the function
 *
 * The solver first ask its helper to reset and set intial values of model's
 * variables according to the function's inputs. Then it re-initialize the
 * internal solver structures.
 */

void SolverBase::restart(const std::set<int>& varIndexes, const double* values)
{
  m_Errors.clear();
  /* 1. reset initial condition */
  m_Helper->initValues(varIndexes, values);
  
  /* 2. reset solver structure */
  if ( resetSolverStructure() != SATOOLS_SUCCESS )
  {
    throw SolverException("Failed to reset solver internal structures");
  }
  
  /* 3. reset time value to zero */
  m_CurrentTime = 0;
}

/*
 * This function resets the solver to initial condition.  
 *
 * The solver first ask its helper to reset to the intial condition. Then it
 * re-initialize the internal solver structures.  
 * */

void SolverBase::restart()
{
  m_Errors.clear();
  /* 1. reset initial condition */
  m_Helper->initValues();
  
  /* 2. reset solver structure */
  if (resetSolverStructure() != SATOOLS_SUCCESS)
  {
    throw SolverException("Failed to reset solver internal structures");
  }  
  /* 3. reset time value to zero */
  m_CurrentTime = 0;
}
BIO_NAMESPACE_END

