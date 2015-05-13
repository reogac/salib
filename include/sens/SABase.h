/**
 @file SABase.h
 @brief Base class for all sensitivity analysis methods
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  SABase_INC
#define  SABase_INC


#include <memory>
#include <functional>

#include "common/namespace.h"
#include "common/Logger.h"
#include "ModelEvaluator.h"
#include "ModelInput.h"
#include "ResultMatrix.h"
#include "RNGWrapper.h"
#include "SAException.h"

BIO_NAMESPACE_BEGIN

typedef enum
{
  SOBOL_SAMPLING=0,
  LHS_SAMPLING,
  MC_SAMPLING
} SamplingMethod_t;

typedef enum
{
  SA_REGIONAL = 0,
  SA_SOBOL2002,
  SA_SOBOL2007,
  SA_JANSEN,
  SA_MORRIS,
  SA_DGSM,
  SA_FAST,
  SA_EFAST,
  SA_RBD
} SAMethod_t;


class SABase
{
  public:
    SABase(const SAMethod_t method);
    virtual ~SABase();

    void setSamplingMethod(const SamplingMethod_t method);
    void setSobolSkip(const unsigned int skip);
    void setFailureRate(const double rate);
    void setModelInputList(const ModelInputList* list);
    void setNumOutputs(const int num);
    void setEval(std::function< std::shared_ptr<ModelEvaluator> (void*)> eval);
    void analyze();

    const DMatrix* getSens() const;
  protected:
    void simulate(const DMatrix& inputs, ResultMatrix& outputs);

    std::unique_ptr<DMatrix> m_Sens;
    std::unique_ptr<DMatrix> m_InputData;
    std::unique_ptr<ResultMatrix> m_OutputData;
    const ModelInputList* m_InputList;
    int m_NumOutputs;

    RNGWrapper m_RNG;
    int m_Method;
    int m_Sampling;
    unsigned int m_SobolSkip;
    double m_FailureRate;
  private:
    std::function< std::shared_ptr<ModelEvaluator> (void* )> m_Eval;
    virtual int getNumSens() const = 0;
    virtual void doSA() = 0;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef SABase_INC  ----- */

