/**
 @file SABase.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "SABase.h"

#include <cassert>

#include "common/CommonDefs.h"

BIO_NAMESPACE_BEGIN

SABase::SABase(const SAMethod_t method)
  : m_Method(method)
  , m_Sampling(LHS_SAMPLING)
  , m_SobolSkip(1000)
  , m_FailureRate(0.05)
  , m_InputList(nullptr)
  , m_NumOutputs(1)
  , m_Sens(nullptr)
{
}

SABase::~SABase()
{
}

void SABase::setSamplingMethod(const SamplingMethod_t method)
{
  m_Sampling = method;
}
void SABase::setSobolSkip(const unsigned int skip) 
{
  m_SobolSkip = skip;
}

void SABase::setFailureRate(const double rate)
{
  if (rate<0 || rate >0.2)
    throw SAException(ERROR_INVALID_FAILURE_RATE);
  m_FailureRate = rate;
}

void SABase::setModelInputList(const ModelInputList* list)
{
  if (list->size()<=0)
    throw SAException(ERROR_NONE_POSITIVE_INPUT_SIZE);
  m_InputList = list;
}

void SABase::setNumOutputs(const int num)
{
  if (num<=0)
    throw SAException(ERROR_NONE_POSITIVE_OUTPUT_SIZE);
  m_NumOutputs = num;
}

void SABase::setEval(std::function< std::shared_ptr<ModelEvaluator> (void*)> eval)
{
  m_Eval = eval;
}

const DMatrix* SABase::getSens() const
{
  return m_Sens.get();
}

void SABase::simulate(const DMatrix& inputs, ResultMatrix& outputs)
{
  INFO("ready for simulation... ");

  if (m_Eval == nullptr)
  {
    throw SAException(ERROR_NO_MODEL_EVALUATOR);
  }

 // #pragma omp parallel
  {
    std::shared_ptr<ModelEvaluator> solver = m_Eval(this);
    int* labels = outputs.getLabels();

   // #pragma omp parallel for  
    for (int i=0; i < inputs.getNumRows(); ++i)
    {

      if ( solver->solve(inputs.getRow(i), outputs.getRow(i)) 
          == SATOOLS_SUCCESS)
      {
        labels[i] = SIM_SUCCESS;
      } else 
      {
        labels[i] = SIM_FAILURE;
      }
    }
  }

}

void SABase::analyze()
{
  m_Sens.reset(new DMatrix(m_InputList->size(), getNumSens()));
  doSA();
}

BIO_NAMESPACE_END
