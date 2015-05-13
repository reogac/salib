/**
 @file SASimple.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */
#include "SASimple.h"

BIO_NAMESPACE_BEGIN

SASimple::SASimple(const SAMethod_t method)
  : SABase(method)
{
}

void SASimple::doSA()
{
  /* 1. Allocate input data and output data */
  m_InputData.reset(new DMatrix(getNumSamples(), m_InputList->size()));
  m_OutputData.reset(new ResultMatrix(getNumSamples(), m_NumOutputs));
  /* 2.Randomly generates input samples */
  sample();
  /* 3. Runs simulation to estimate outputs */
  simulate(*m_InputData, *m_OutputData);
  /* 4. Estimates sensitivity measures */
  estimate();

}

BIO_NAMESPACE_END
