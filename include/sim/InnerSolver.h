/**
 @file InnerSolver.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  InnerSolver_INC
#define  InnerSolver_INC

#include "common/ErrorList.h"

BIO_NAMESPACE_BEGIN

class Solver;
class SolverSettings;
class SolverData;
class OdeStruct;
class InnerSolver
{
  public:
    InnerSolver(Solver* wrapper);
    virtual ~InnerSolver();
    virtual double* getY() = 0;
    void initialize();
    void reset();
    virtual void solve(const double tout) = 0;
    double getTime() const { return m_Time;}
  protected:
    const SolverSettings* getSettings() const;
    const OdeStruct* getModel();
    void setInitialCondition();
    virtual void doInitialize() = 0;
    virtual void doReset() = 0;
    double m_Time;
    unsigned int m_NumOdes;
    ErrorList& m_Errors;
    SolverData* m_Data;
  private:
    Solver* wrapper_;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef InnerSolver_INC  ----- */

