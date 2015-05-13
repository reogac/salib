/**
 @file SbmlResult.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "SbmlResult.h"
#include <iostream>

BIO_NAMESPACE_BEGIN

SbmlTimecourse::SbmlTimecourse(const std::vector<double>& timesteps)
  : timesteps_(timesteps)
  , values_{nullptr}
{
  if (timesteps.size() > 0)
    values_ = new double[timesteps.size()];
}
SbmlTimecourse::~SbmlTimecourse()
{
  delete [] values_;
}

int SbmlTimecourse::length() const
{
  return timesteps_.size();
}
void SbmlTimecourse::setName(const std::string& name)
{
  name_ = name;
}

const std::string& SbmlTimecourse::getName() const
{
  return name_;
}

const double* SbmlTimecourse::getValues() const
{
  return values_;
}
double SbmlTimecourse::getValue(const int index) const
{
  return values_[index];
}

void SbmlTimecourse::setValue(const int index, const double value)
{
  values_[index] = value;
}

double& SbmlTimecourse::operator[] (const int index)
{
  return values_[index];
}

const double& SbmlTimecourse::operator[] (const int index) const
{
  return values_[index];
}


std::ostream& operator<< (std::ostream& out, const SbmlTimecourse& timecourse)
{
  out << "\n##" << timecourse.getName() << " :\n";
  for (int i=0; i< timecourse.length(); ++i)
    out << timecourse.values_[i] << " ";
  return out;
}

SbmlTimecourseArray::SbmlTimecourseArray(const std::vector<double>&timesteps, const
        std::vector<std::string>& names)
  : timesteps_(timesteps)
{
  for (auto& name : names)
  {
    timecourses_.emplace_back(new SbmlTimecourse(timesteps));
    timecourses_.back()->setName(name);
  }
}
SbmlTimecourseArray::~SbmlTimecourseArray()
{
}

SbmlTimecourse& SbmlTimecourseArray::operator[] (const int index)
{
  return *(timecourses_[index].get());
}
const SbmlTimecourse& SbmlTimecourseArray::operator[] (const int index) const
{
  return *(timecourses_[index].get());
}

std::ostream& operator<< (std::ostream& out, const SbmlTimecourseArray&
    data)
{
  for (auto& t : data.timecourses_)
  {
    out << *(t.get());
  }
  out << "\n";
  return out;
}

SbmlResult::SbmlResult(const std::vector<double>& timesteps)
  : timesteps_(timesteps)
    , species_{nullptr}
    , compartments_{nullptr}
    , reactions_{nullptr}
    , parameters_{nullptr}
{
}

SbmlResult::~SbmlResult()
{

}
int SbmlResult::allocate(const Model& model)
{
  speciesNames_.clear();
  for (int i=0; i < model.getNumSpecies(); ++i)
  {
    speciesNames_.emplace_back(model.getSpecies(i)->getId());
  }

  if (speciesNames_.size()>0)
  {
    species_.reset(new SbmlTimecourseArray(timesteps_, speciesNames_));
  }

  compartmentNames_.clear();
  for (int i=0; i < model.getNumCompartments(); ++i)
  {
    compartmentNames_.emplace_back(model.getCompartment(i)->getId());
  }

  if (compartmentNames_.size()>0)
  {
    compartments_.reset(new SbmlTimecourseArray(timesteps_, compartmentNames_));
  }


  reactionNames_.clear();
  for (int i=0; i < model.getNumReactions(); ++i)
  {
    reactionNames_.emplace_back(model.getReaction(i)->getId());
  }

  if (reactionNames_.size()>0)
  {
    reactions_.reset(new SbmlTimecourseArray(timesteps_, reactionNames_));
  }

  parameterNames_.clear();
  for (int i=0; i < model.getNumParameters(); ++i)
  {
    parameterNames_.emplace_back(model.getParameter(i)->getId());
  }

  if (parameterNames_.size()>0)
  {
    parameters_.reset(new SbmlTimecourseArray(timesteps_, parameterNames_));
  }
}
const std::vector<std::string>& SbmlResult::getSpeciesNames() const
{
  return speciesNames_;
}
const std::vector<std::string>& SbmlResult::getCompartmentNames() const
{
  return compartmentNames_;
}

const std::vector<std::string>& SbmlResult::getReactionNames() const
{
  return reactionNames_;
}

const std::vector<std::string>& SbmlResult::getParameterNames() const
{
  return parameterNames_;
}

SbmlTimecourseArray* SbmlResult::getSpecies()
{
  return species_.get();
}
SbmlTimecourseArray* SbmlResult::getCompartments()
{
  return compartments_.get();
}

SbmlTimecourseArray* SbmlResult::getReactions()
{
  return reactions_.get();
}

SbmlTimecourseArray* SbmlResult::getParameters()
{
  return parameters_.get();
}

std::ostream& operator<< (std::ostream& out, const SbmlResult& result)
{

 if (result.compartments_)
  {
    out << "\n";
    out << "# Compartment(s)\n";
    out << *(result.compartments_.get());
  }

  if (result.species_)
  {
    out << "\n";
    out << "# Species\n";
    out << *(result.species_.get());
  }

 if (result.parameters_)
  {
    out << "\n";
    out << "# Parameter(s)\n";
    out << *(result.parameters_.get());
  }

 if (result.reactions_)
  {
    out << "\n";
    out << "# Reaction(s)\n";
    out << *(result.reactions_.get());
  }

  return out;
}

BIO_NAMESPACE_END

