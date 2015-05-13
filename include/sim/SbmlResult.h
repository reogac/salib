/**
 @file SbmlResult.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  SbmlResult_INC
#define  SbmlResult_INC

#include <ostream>
#include <memory>
#include <string>
#include <vector>

#include <sbml/SBMLTypes.h>

#include "common/namespace.h"

BIO_NAMESPACE_BEGIN 
class SbmlTimecourse
{
  public:
    SbmlTimecourse(const std::vector<double>& timesteps);
    ~SbmlTimecourse();
    int length() const;
    void setName(const std::string& name);
    const std::string& getName() const;
    const double* getValues() const;
    double getValue(const int index) const;
    void setValue(const int index, const double value);
    double& operator[] (const int index);
    const double& operator[] (const int index) const;
    
  private:
    std::string name_;
    const std::vector<double>& timesteps_;
    double* values_;

    friend std::ostream& operator<< (std::ostream& out, const SbmlTimecourse& timecourse);
};

class SbmlTimecourseArray
{
  public:
    SbmlTimecourseArray(const std::vector<double>&timesteps, const
        std::vector<std::string>& names);
    ~SbmlTimecourseArray();
    SbmlTimecourse& operator[] (const int index);
    const SbmlTimecourse& operator[] (const int index) const;
  private:
    std::vector<std::unique_ptr<SbmlTimecourse> > timecourses_;
    const std::vector<double>& timesteps_;

    friend std::ostream& operator<< (std::ostream& out, const
        SbmlTimecourseArray& data);
};

class SbmlResult
{
  public:
    SbmlResult(const std::vector<double>&timesteps);
    ~SbmlResult();


    int allocate(const Model& model);
    const std::vector<std::string>& getSpeciesNames() const;
    const std::vector<std::string>& getCompartmentNames() const;
    const std::vector<std::string>& getReactionNames() const;
    const std::vector<std::string>& getParameterNames() const;

    SbmlTimecourseArray* getSpecies();
    SbmlTimecourseArray* getCompartments();
    SbmlTimecourseArray* getReactions();
    SbmlTimecourseArray* getParameters();
  private:
    std::vector<double> timesteps_;
    std::unique_ptr<SbmlTimecourseArray> species_;
    std::unique_ptr<SbmlTimecourseArray> compartments_;
    std::unique_ptr<SbmlTimecourseArray> reactions_;
    std::unique_ptr<SbmlTimecourseArray> parameters_;

    std::vector<std::string> speciesNames_;
    std::vector<std::string> compartmentNames_;
    std::vector<std::string> reactionNames_;
    std::vector<std::string> parameterNames_;

    friend std::ostream& operator<< (std::ostream& out, const
        SbmlResult& result);
};

BIO_NAMESPACE_END 
#endif   /* ----- #ifndef SbmlResult_INC  ----- */

