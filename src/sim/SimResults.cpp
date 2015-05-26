/**
 @file SimResults.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "SimResults.h"

BIO_NAMESPACE_BEGIN

std::ostream& operator<< (std::ostream& out, const Timecourse& tc)
{
  out << "#" << tc.name_ << std::endl;
  for (int i=0; i< tc.n_; ++i)
    out << tc[i] << " ";
  out << std::endl;
  return out;
}


SimResults::SimResults(const Model& model, const double* times, const int n)
  : results_("Simulation results")
{
  results_.add(TimecourseArray("Compartments"));
  for (int i=0; i< model.getNumCompartments(); ++i)
  {
    results_[0].add(Timecourse(times, n, model.getCompartment(i)->getId()));
  }
 
  results_.add(TimecourseArray("Species"));
  for (int i=0; i< model.getNumSpecies(); ++i)
  {
    results_[1].add(Timecourse(times, n, model.getSpecies(i)->getId()));
  }
  results_.add(TimecourseArray("Parameters"));
  for (int i=0; i<model.getNumParameters(); ++i)
  {
    const std::string& name = model.getParameter(i)->getId();
    results_[2].add(Timecourse(times, n, name));
  }
  results_.add(TimecourseArray("Reactions"));
  for (int i=0; i<model.getNumReactions(); ++i)
  {
    const std::string& name = model.getReaction(i)->getId();
    results_[3].add(Timecourse(times, n, name));
  }

}

TimecourseArray& SimResults::getCompartments()
{
  return results_[0];
}

TimecourseArray& SimResults::getParameters()
{
  return results_[2];
}

TimecourseArray& SimResults::getSpecies()
{
  return results_[1];
}

TimecourseArray& SimResults::getReactions()
{
  return results_[3];
}

int SimResults::getNumCompartments() const
{
  return results_[0].size();
}

int SimResults::getNumParameters() const
{
  return results_[1].size();
}

Timecourse& SimResults::getCompartment(const int index)
{
  return results_[0][index];
}

Timecourse& SimResults::getParameter(const int index)
{
  return results_[2][index];
}

Timecourse& SimResults::getSpecies(const int index)
{
  return results_[1][index];
}

Timecourse& SimResults::getReaction(const int index)
{
  return results_[3][index];
}


std::ostream& operator<< (std::ostream& out, const SimResults& rs)
{
  out << rs.results_;
  return out;
}
BIO_NAMESPACE_END


