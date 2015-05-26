/**
 @file SimResults.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  SimResults_INC
#define  SimResults_INC

#include "SimException.h"
#include <sbml/Model.h>

#include <memory>
#include <string>
#include <vector>
#include <ostream>

BIO_NAMESPACE_BEGIN

class Timecourse
{
  public:
    Timecourse(const double* times, const int n, const std::string& name="")
      : n_(n)
      , t_(times)
      , name_(name)
      , v_(new double[n]) { values_ = v_.get(); }
  
    Timecourse(Timecourse&& other)
      : n_(other.n_)
      , t_(other.t_)
      , name_(std::move(other.name_))
      , v_(std::move(other.v_))
      , values_(other.values_)  {}

    int size() const { return n_;}
    const std::string& getName() const { return name_; }
    void setName(const std::string& name) { name_ = name; }
    double& operator[] (const int index) { return values_[index]; }
    const double& operator[] (const int index) const{ return values_[index]; }

  private:
    const double* t_;                           /* times */
    std::unique_ptr<double[]> v_;               /* values */
    int n_;                                     /* #steps */
    std::string name_;
    double* values_;
  
    friend std::ostream& operator<< (std::ostream& out, const Timecourse& tc);
};

template <typename T>
class TArray
{
  public:
    TArray(const std::string& name)
      : name_(name) {}
    TArray(TArray&& other)
      : name_(std::move(other.name_))
      , elems_(std::move(other.elems_)) {}
    int size() const { return elems_.size(); }
    T& operator[] (const int index) { return elems_[index]; }
    const T& operator[] (const int index) const { return elems_[index]; }
    void add(T&& t)
    {
      elems_.emplace_back(std::move(t));
    }
    
  private:
    std::vector<T> elems_;
    std::string name_;
    friend std::ostream& operator<< (std::ostream& out, const TArray& array)
    {
      out << "#" << array.name_ << std::endl;

      for (const auto& e : array.elems_)
      {
        out << e << std::endl;
      }
      return out;
    }
};

typedef TArray<Timecourse> TimecourseArray;
typedef TArray<TimecourseArray> TimecourseMatrix;

class SimResults
{
  public:
    SimResults(const Model& model, const double* time, const int n);

    int getNumCompartments() const;
    int getNumParameters() const;
    Timecourse& getCompartment(const int index);
    Timecourse& getParameter(const int index);
    Timecourse& getSpecies(const int index);
    Timecourse& getReaction(const int index);

    TimecourseArray& getCompartments();
    TimecourseArray& getSpecies();
    TimecourseArray& getReactions();
    TimecourseArray& getParameters();
  private:
    TimecourseMatrix results_;
    friend std::ostream& operator<< (std::ostream& out, const SimResults& rs);
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef SimResults_INC  ----- */

