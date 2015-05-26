/**
 @file SimResultRecorder.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "SimResultRecorder.h"

#include "OdeStruct.h"
#include <iostream>
BIO_NAMESPACE_BEGIN


SimResultRecorder::SimResultRecorder(SimResults* results, const OdeStruct* model)
  : results_(results)
  , compartments_(nullptr)
  , parameters_(nullptr)
  , species_(nullptr)
  , reactions_(nullptr)
{
  int numCompartments = results->getCompartments().size();
  if (numCompartments>0)
  {
    compartments_.reset(new int[numCompartments]); 
    for (int i=0; i<numCompartments; ++i)
      compartments_[i] = model->getVariableIndex(results->getCompartments()[i].getName());
  }
  
  int numSpecies = results->getSpecies().size();
  if (numSpecies>0)
  {
    species_.reset(new int[numSpecies]); 
    for (int i=0; i<numSpecies; ++i)
      species_[i] = model->getVariableIndex(results->getSpecies()[i].getName());
  }

  int numParameters = results->getParameters().size();
  if (numParameters>0)
  {
    parameters_.reset(new int[numParameters]); 
    for (int i=0; i<numParameters; ++i)
      parameters_[i] = model->getVariableIndex(results->getParameters()[i].getName());
  }

  int numReactions = results->getReactions().size();
  if (numReactions>0)
  {
    reactions_.reset(new int[numReactions]); 
    for (int i=0; i<numReactions; ++i)
      reactions_[i] = model->getVariableIndex(results->getReactions()[i].getName());
  }

}

void SimResultRecorder::record(const double* values, const int step)
{
  const int* ci = compartments_.get();
  const int* si = species_.get();
  const int* ri = reactions_.get();
  const int* pi = parameters_.get();
  TimecourseArray& c = results_->getCompartments();
  TimecourseArray& s = results_->getSpecies();
  TimecourseArray& p = results_->getParameters();
  TimecourseArray& r = results_->getReactions();
  for (int i=0; i< c.size(); ++i)
  {
    c[i][step] = values[ci[i]];
  }
  for (int i=0; i< s.size(); ++i)
  {
    s[i][step] = values[si[i]];
  }
  for (int i=0; i< r.size(); ++i)
  {
    r[i][step] = values[ri[i]];
  } 
  for (int i=0; i< p.size(); ++i)
  {
    p[i][step] = values[pi[i]];
  }
}

BIO_NAMESPACE_END


