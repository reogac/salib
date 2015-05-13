/**
 @file ModelEvaluator.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#ifndef  ModelEvaluator_INC
#define  ModelEvaluator_INC
#include "common/namespace.h"

BIO_NAMESPACE_BEGIN

class ModelEvaluator
{
  public:
    ModelEvaluator(const int nInputs, const int nOutputs);
    virtual ~ModelEvaluator();

    virtual int solve(const double* inputs, double* outputs) const = 0;
    
    int getNumInputs() const;
    int getNumOutputs() const;
    void setUserData(void* data);
  protected:
    int m_NumInputs;
    int m_NumOutputs;
    void* m_UserData;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef ModelEvaluator_INC  ----- */
