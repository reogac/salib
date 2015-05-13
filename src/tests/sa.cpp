/**
 @file sa.cpp
 @brief sensitivity analysis main program
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include <iostream>
#include <cstdio>
#include <cstdlib>

#include "../OdeModel.h"
#include "../OdeSim.h"
#include "TaskDesc.h"
#include "../SimSettings.h"
#include "../CvodeSolverSettings.h"
#include "../SaConfig.h"
#include "../SaBase.h"
#include "../SaFactory.h"
#include "CvodeSolver4Sa.h"


using namespace std;
int main(int argc, const char** argv)
{
  OdeModel model;

  if (argc !=3) {
    cout << "Usage: " << argv[0] << " model-file configuration-file" << endl;
    return 1;
  }

  cout << "Well come to global sensitivity analysis tool" << endl;
  //load sbml model
  model.load(argv[0]);

  //read task description
  TaskDesc desc(model); 
  desc.read(argv[1]);

  //read simulation settings and sensitivity analysis settings
  SimSettings simSettings;
  CvodeSolverSettings solverSettings;
  SaConfig saCfg;

  // ininitalze an Ode solver
  OdeSim solver(model);
  solver.init(&simSettings);
  //create a solver wrapper for sensitivity analysis
  CvodeSolver4Sa solverWrapper(solver, saCfg);
  
  //create a sensitivity analyzer
  SaFactory factory;
  SaBase *sa = NULL;

  sa = factory.createSa(saCfg, solverWrapper);

  //do sentitivity analysis
  sa->doSa();

  //report
  sa->report();

  delete sa;

  return 0;
}
