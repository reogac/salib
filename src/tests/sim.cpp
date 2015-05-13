/**
  @file sim.cpp
  @brief simple simulation tool
  @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include <iostream>

#include "common/CommonDefs.h"
#include "sim/SbmlOdeModel.h"
#include "sim/SbmlSim.h"
#include "sim/SolverException.h"

#include "sens/RNGWrapper.h"

using namespace reo;

void test_rng()
{
  reo::RNGWrapper rng;
  int n=5;
  double* values = new double[n];
  rng.randomLHS(values, n);
  for (int i=0; i<n; ++i)
    std::cout << values[i] << " ";
  std::cout << "\n";
}


int main(int argc, const char** argv)
{
  test_rng();

  if (argc != 2) {
    std::cout << "Usage: " << argv[0] << " model-file" << std::endl;
    return EXIT_SUCCESS;
  }

  std::set<int> vars{};
  SbmlOdeModel model;

  SbmlSim sim(model);
  int ret = SATOOLS_SUCCESS;
  try {
    model.loadSbml(argv[1]);

    model.print();

//    std::vector<double> timesteps ={ 10000, 20000, 30000, 40000 };
//    sim.setTimeSteps(timesteps);
//
//    sim.setTimeStepsByDuration(40000, 15);
    sim.setTimeStepsByInterval(1000, 40)
       .init();
    ret = sim.simulate();
  } catch (const SolverException& e)
  {
    std::cout << e.what() << "\n";
    return EXIT_FAILURE;
  }

  if (ret == SATOOLS_SUCCESS)
  {
    const SbmlResult* result = sim.getResult();
    std::cout <<  *result;
  }

  if (ret == SATOOLS_SUCCESS)
  {
    std::cout << "\nSimulation Done!\n";
  }



  return EXIT_SUCCESS;
}

