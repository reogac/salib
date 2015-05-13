/**
 @file CvodeSolver4Sa.h
 @brief Header file for CvodeSolver4Sa class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */



#ifndef  CvodeSolver4Sa_INC
#define  CvodeSolver4Sa_INC

#include "../Solver4Sa.h"

class SaConfig;
class OdeSim;

class CvodeSolver4Sa : public Solver4Sa
{
  public:
    CvodeSolver4Sa(const OdeSim& solver, const SaConfig& cfg);
    int solve(const double* inputs, double* outputs) const;

  protected:
    const SaConfig& m_SaCfg;
    const OdeSim& m_Solver;
  private:
    
};

#endif   /* ----- #ifndef CvodeSolver4Sa_INC  ----- */

