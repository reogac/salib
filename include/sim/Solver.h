/**
 @file Solver.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#ifndef  Solver_INC
#define  Solver_INC

#include "SolverData.h"
#include "SolverSettings.h"
#include "CvodeSolver1.h"

#include <vector>

BIO_NAMESPACE_BEGIN


class Solver
{
  public:
    Solver(const SolverSettings* settings);

    void initialize(const OdeStruct* model);
    void reset(const SolverSettings* settings);
    void reset();
    void reset(const std::vector<int>& indexes, const double* values);
    void solve(const double time);
    const double* getValues() const { return m_Data->getValues(); }
  protected:

    const SolverSettings* m_Settings;
    const OdeStruct* m_Model;
    std::unique_ptr<SolverData> m_Data;
    std::unique_ptr<CvodeSolver1> m_Solver;
    ErrorList m_Errors;

  private:
    void checkErrors();
//    double m_T0;                                /* integration starting time */
//    double* m_Y;
    friend class InnerSolver;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef Solver_INC  ----- */

