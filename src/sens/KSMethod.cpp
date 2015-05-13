/**
 @file KSMethod.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */
#include "KSMethod.h"

#include <memory>

#include "RNGWrapper.h"

BIO_NAMESPACE_BEGIN

KSMethod::KSMethod()
  : SASimple(SA_REGIONAL)
{
}

KSMethod::~KSMethod()
{
}

void KSMethod::setN(const int N)
{
  N_ = N;
}
int KSMethod::getNumSens() const
{
  return m_NumOutputs;
}

int KSMethod::getNumSamples() const
{
  return  N_ + 1;
}

const double* KSMethod::getRefInput() const
{
  const double* ret = nullptr;
  if (m_InputData)
    ret = m_InputData->getRow(0);
  return ret;
}

const double* KSMethod::getRefOutput() const
{
  const double* ret = nullptr;
  if (m_OutputData)
    ret = m_OutputData->getRow(0);
  return ret;
}


void KSMethod::sample()
{
  int numsamples = getNumSamples();
  std::unique_ptr<double[]> valueptr(new double[numsamples]);
  double* values = valueptr.get();
  m_RNG.LHS(*m_InputData);
}

void KSMethod::estimate()
{
  double** inputs = m_InputData->getData();
  double** outputs = m_OutputData->getData();
  int* labels = m_OutputData->getLabels();
  int numSamples = getNumSamples();
  struct SortingStruct
  {
    double value_;
    int index_;
  };

  std::unique_ptr<SortingStruct> sortingColPtr(new SortingStruct[numSamples]); /* the first array member is not used */
  SortingStruct* sortingCol = sortingColPtr.get();

  std::unique_ptr<double[]> F1ptr(new double[numSamples]);
  std::unique_ptr<double[]> F2ptr(new double[numSamples]);
  double* F1 = F1ptr.get();
  double* F2 = F2ptr.get();

  /* for each input */
  for (int inputId=0; inputId<m_InputData->getNumCols(); ++inputId)
  {

    /* sort column 'inputId' of the inputs */
    for (int iSample=1; iSample<numSamples; ++iSample)
    {
      sortingCol[iSample] = {inputs[iSample][inputId], iSample};
    }
    std::sort(&sortingCol[1], &sortingCol[numSamples-1], [] (const SortingStruct& a, const SortingStruct& b)
        {
        return a.value_ < b.value_;
        });
    
    /* estimate KS score for each outputs */
    for (int iSens=0; iSens < getNumSens(); ++iSens)
    {
      /* TODO: ref value can be the average value of all responses of the 'iSens'
       * output; or it can be just the 'iSens' response value corresponding to the reference
       * inputs */
      double ref = m_OutputData->getRow(0)[iSens];
      F1[0] = 0;
      F2[0] = 0;
      int sortedId = 0;
      for (int iSample=1; iSample<numSamples; ++iSample)
      {
        sortedId =sortingCol[iSample].index_;
        if (labels[sortedId] == SIM_SUCCESS) /* the sample has a valid simulation result */
        {
          if (outputs[sortedId][iSens] <= ref)
          {
            F1[iSample] = F1[iSample-1] + 1;
            F2[iSample] = F2[iSample-1];
          } else
          {
            F2[iSample] = F2[iSample-1] + 1;
            F1[iSample] = F1[iSample-1];
          }
        } else                                  /* the sample has an invalid simulation result */
        {
          F1[iSample] =F1[iSample-1];
          F2[iSample] = F2[iSample-1];
        }
      }
      if (F1[numSamples-1] == 0
          || F2[numSamples-1] ==0)
      {
        m_Sens->getRow(inputId)[iSens] = 0;
      } else
      {
        for (int ii=1; ii<numSamples-1; ++ii)
        {
          F1[ii] /= F1[numSamples-1];
          F2[ii] /= F2[numSamples-1];
        }
        double sens = 0;
        for (int ii=1; ii<numSamples-1; ++ii)
        {
          double diff = abs(F1[ii]-F2[ii]);
          if (sens < diff)
          sens = diff;
        }
        m_Sens->getRow(inputId)[iSens] = sens;
      }
    }
  }
}
BIO_NAMESPACE_END

