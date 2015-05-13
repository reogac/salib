/**
 @file ModelEvaluator.cpp
 @brief Implementation for Solve4Sa class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#include "ModelEvaluator.h"

BIO_NAMESPACE_BEGIN

ModelEvaluator::ModelEvaluator(const int nInputs, const int nOutputs)
  : m_NumInputs(nInputs)
  , m_NumOutputs(nOutputs)
    , m_UserData(nullptr)
{

}

ModelEvaluator::~ModelEvaluator()
{

}

void ModelEvaluator::setUserData(void* data)
{
  m_UserData = data;
}

int ModelEvaluator::getNumInputs() const
{
  return m_NumInputs;
}

int ModelEvaluator::getNumOutputs() const
{
  return m_NumOutputs;
}

BIO_NAMESPACE_END

