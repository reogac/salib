/**
 @file TestFrameworks.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */
#include "TestFrameworks.h"
#include "common/SaException.h"
#include "common/CommonDefs.h"

#include <cmath>

#define MY_PI 3.141592653589793238462643383279502884L
#include <iostream>

using namespace reo;
TestFramework::TestFramework()
{
}

TestFramework* TestFramework::instance(const std::string& name)
{
  TestFramework* frm{nullptr};
  if (name == "portfolio")
  { 
    frm = new PortfolioFrm();
  } else
  {
    throw InputException("Unknown test model");
  }
  
  frm->init();
  return frm;
}

TestFramework* TestFramework::Gfunction(const double a[8])
{
  TestFramework* ret = new GfunctionModelFrm(a);
  ret->init();
  return ret;
}

TestFramework* TestFramework::Ishigami(const double a, const double b)
{ 
  TestFramework* ret = new IshigamiFrm(a, b);
  ret->init();
  return ret;
}

std::shared_ptr<ModelEvaluator> TestFramework::createModel(void* userdata)
{
  std::shared_ptr<ModelEvaluator> ret(createModel_());
  ret->setUserData(userdata);
  std::cout << "create a model \n";
  return ret;
}

PortfolioFrm::PortfolioFrm()
  : TestFramework()
{

}

void PortfolioFrm::init()
{
/*create input list and output list*/
  m_InputList.add("Ps")
    .setRef(0.0)
    .setNormal(0.0, 4.0);

  m_InputList.add("Pt")
    .setRef(0.0)
    .setNormal(0.0, 2.0);

  m_InputList.add("Pj")
    .setRef(0.0)
    .setNormal(0.0, 1.0);
  m_NumOutputs = 1;
}

PortfolioFrm::TestModel* PortfolioFrm::createModel_()
{
  return new TestModel();
}


PortfolioFrm::TestModel::TestModel()
  : ModelEvaluator(3,1)
{
}


int PortfolioFrm::TestModel::solve(const double* inputs, double* outputs) const
{
  outputs[0] = inputs[0]*Cs + inputs[1]*Ct + inputs[2]*Cj;
  return SATOOLS_SUCCESS;
}

GfunctionModelFrm::GfunctionModelFrm(const double a[8])
  : TestFramework()
{
  for (int i=0; i<8; ++i)
    a_[i] = a[i];
}



void GfunctionModelFrm::init()
{
  /*create input list and output list*/
  for (int i=0; i<8; ++i)
  {
    m_InputList.add("X" + std::to_string(i))
    .setUniform(0.0, 1.0);
  }

  m_NumOutputs = 1;
}

GfunctionModelFrm::TestModel* GfunctionModelFrm::createModel_()
{
  return new TestModel(a_);
}


GfunctionModelFrm::TestModel::TestModel(const double a[8])
  : ModelEvaluator(3,1)
{
  for (int i=0; i<8; ++i)
    a_[i] = a[i];
}


int GfunctionModelFrm::TestModel::solve(const double* inputs, double* outputs) const
{
  outputs[0] = 1;
  for (int i=0; i<8; ++i)
  {
    outputs[0] *= (abs(4*inputs[i] -2) + a_[i])/(1+a_[i]);
  }

  return SATOOLS_SUCCESS;
}

IshigamiFrm::IshigamiFrm(const double a, const double b)
  : TestFramework()
  , a_(a)
  , b_(b)
{
}

void IshigamiFrm::init()
{
  /*create input list and output list*/
  for (int i=0; i<3; ++i)
  {
    m_InputList.add("X" + std::to_string(i))
    .setUniform(-MY_PI, MY_PI);
  }

  m_NumOutputs = 1;
}

IshigamiFrm::TestModel* IshigamiFrm::createModel_()
{
  return new TestModel(a_, b_);
}


IshigamiFrm::TestModel::TestModel(const double a, const double b)
  : ModelEvaluator(3,1)
  , a_(a)
  , b_(b)
{
}


int IshigamiFrm::TestModel::solve(const double* inputs, double* outputs) const
{
  double x1 = sin(inputs[0]);
  double x2 = sin(inputs[2]);
  double x3 = inputs[2]*inputs[2];
  outputs[0] = x1 + a_*x2*x2 + b_*x3*x3*x1;

  return SATOOLS_SUCCESS;
}
