/**
 @file SbmlSim.cpp
 @brief Implementation for SbmlSim class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "SbmlSim.h"

#include "common/CommonDefs.h"
#include "SbmlOdeModel.h"
#include "SolverBase.h"

BIO_NAMESPACE_BEGIN

SbmlSim::SbmlSim(const SbmlOdeModel& model)
  : OdeSim(model)
    , result_{nullptr}
{
}

SbmlSim::~SbmlSim()
{
}

const SbmlResult* SbmlSim::getResult() const
{
  return result_.get();
}


int SbmlSim::simulate()
{
  checkReady();
  /* 1. Allocates data for simulation result*/

  const Model* sbmlmodel = ((const SbmlOdeModel&) m_Model)
                                  .getSbmlModel(); /* should never be a nullptr */


  std::unique_ptr<SbmlResult> result(new SbmlResult(m_Timesteps));

  result->allocate(*sbmlmodel);

  /* 2. Prepares indexes of species, reactions, compartments and parameters */
  std::vector<int> species;
  for (auto& name : result->getSpeciesNames())
  {
    int index = m_Model.getVariableIndex(name);
    /* index should always be valid, otherwise there must be some logical errors */
    species.emplace_back(index);
  }

  std::vector<int> compartments;
  for (auto& name : result->getCompartmentNames())
  {
    int index = m_Model.getVariableIndex(name);
    /* index should always be valid, otherwise there must be some logical errors */
    compartments.emplace_back(index);
  }

  std::vector<int> reactions;
  for (auto& name : result->getReactionNames())
  {
    int index = m_Model.getVariableIndex(name);
    /* index should always be valid, otherwise there must be some logical errors */
    reactions.emplace_back(index);
  }

  std::vector<int> parameters;
  for (auto& name : result->getParameterNames())
  {
    int index = m_Model.getVariableIndex(name);
    /* index should always be valid, otherwise there must be some logical errors */
    parameters.emplace_back(index);
  }

  /* 3. Simulates */

  int ret = SATOOLS_SUCCESS;
  while( true)
  {
    int step = getCurrentStep();

    /* Save current values of variables into the SbmlResult structure */
    for (int i=0; i < species.size(); i++)
    {
      (*(result->getSpecies()))[i][step] = m_SimValues[species[i]];
    }

    for (int i=0; i < compartments.size(); i++)
    {
      (*(result->getCompartments()))[i][step] = m_SimValues[compartments[i]];
    }

    for (int i=0; i < reactions.size(); i++)
    {
      (*(result->getReactions()))[i][step] = m_SimValues[reactions[i]];
    }

    for (int i=0; i < parameters.size(); i++)
    {
      (*(result->getParameters()))[i][step] = m_SimValues[parameters[i]];
    }

    if (isCompleted())
      break;

    /* Simulates one step */
    if (simulateOne() != SATOOLS_SUCCESS)
    {
      ret = SATOOLS_FAILURE;
      break;
    }
  }
  
  if (ret == SATOOLS_SUCCESS)
  {
    result_ = std::move(result);
  }

  return ret;
}

BIO_NAMESPACE_END

