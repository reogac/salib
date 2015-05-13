/**
 @file SolverSettings.h
 @brief Base class for configuration of solver
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  SolverSettings_INC
#define  SolverSettings_INC

#include "common/namespace.h"

BIO_NAMESPACE_BEGIN

#define ODE_SOLVER_CVODE 0
#define ODE_SOLVER_LSODE 1

class SolverSettings
{
  public:
    SolverSettings(const int type);
    SolverSettings(const SolverSettings& other);
    virtual ~SolverSettings();

    SolverSettings& operator= (const SolverSettings& other);
    virtual SolverSettings* clone() const;

    int getSolverType() const;

  protected:
    int m_SolverType;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef SolverSettings_INC  ----- */


