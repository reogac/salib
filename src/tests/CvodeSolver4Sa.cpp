/**
 @file CvodeSolver4Sa.cpp
 @brief Implementation for CvodeSolver4Sa class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "CvodeSolver4Sa.h"

#include "../SaConfig.h"
#include "../OdeSim.h"


CvodeSolver4Sa::CvodeSolver4Sa(const OdeSim& solver, const SaConfig& cfg)
  : Solver4Sa(cfg.getNumInputs(), cfg.getNumOutputs())
  , m_Solver(solver)
  , m_SaCfg(cfg)
{

}


int CvodeSolver4Sa::solve(const double* inputs, double* outputs) const
{
  
  return 0;
}
