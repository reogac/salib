/**
 @file SolverSettings.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  SolverSettings_INC
#define  SolverSettings_INC

#include "common/namespace.h"

BIO_NAMESPACE_BEGIN

typedef enum
{
  ODE_SOLVER_RK45=0,
  ODE_SOLVER_CVODE,
} SolverType_t;

class SolverSettings
{
  public:
    SolverSettings(const SolverType_t type);
    SolverSettings(const SolverSettings& other);
    SolverSettings& operator= (const SolverSettings& other);
    virtual SolverSettings* clone() const = 0;

    void setMaxSteps(const int steps) { m_MaxSteps = steps; }
    int getMaxSteps() const  { return m_MaxSteps; }
    int getType() const { return m_Type; }
  protected:
    int m_Type;
    int m_MaxSteps;
};
BIO_NAMESPACE_END

#endif   /* ----- #ifndef SolverSettings_INC  ----- */

