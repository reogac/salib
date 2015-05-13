/**
 @file testsens.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include <functional>
#include <memory>
#include <iostream>
#include <fstream>
#include "TestFrameworks.h"

#include <sens/SA.h>
#include <common/Logger.h>
#include <common/ErrorMessage.h>
#include <common/ErrorList.h>

//#include <sens/RNGWrapper.h>
//#include <sens/ModelInput.h>
//#include <sens/Matrix.h>

int main()
{
  using namespace reo;

//  std::ofstream ofs("sa.log");
//  Logger::setWriter(ofs);
//  DEBUG("debug")
//  ERROR("error")
//  INFO("info")
//  WARNING("waning")
//
  ErrorList errors;
  errors.add(new ErrorMessage(1,LEVEL_ERROR, "error"));
  errors.add(new ErrorMessage(2,LEVEL_FATAL, "fatal"));
  errors.add(new ErrorMessage(3,LEVEL_WARNING, "warning"));
  LOG_ERRORS(errors) 
  VERROR("a message %d", 10)
  try {
//  std::unique_ptr<TestFramework> frm(TestFramework::instance("portfolio"));
  double a[8] = {0,0,99,99,99,99,99,0};
  std::unique_ptr<TestFramework> frm(TestFramework::Gfunction(a));
//  std::unique_ptr<TestFramework> frm(TestFramework::Ishigami(7, 0.1));
//  KSMethod sa;
  SobolSaltelli sa;
  sa.setEstimator(SobolSaltelli::SOBOL2010);
//  DGSM sa;

//  Morris sa;
//  Jansen sa;
//  EFAST sa;
//  RBD sa;
//    FAST sa;
//    sa.setUseFFT(true);
//    sa.setN(2020);
  sa.setModelInputList(frm->getInputList());
  sa.setNumOutputs(frm->getNumOutputs());
//  sa.setSaveInput(true);
//  sa.setSaveOutput(true);
//  sa.setR(100);
//  sa.setN(5000);
  sa.setEval(std::bind(&PortfolioFrm::createModel, frm.get(), std::placeholders::_1));
  sa.analyze();
  std::cout << *sa.getSens();  
} catch (std::exception& e)
{
  std::cout << e.what() << "\n";
}
//  ofs.close();
  return EXIT_SUCCESS;
}
