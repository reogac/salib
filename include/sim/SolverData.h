/**
 @file SolverData.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  SolverData_INC
#define  SolverData_INC

#include "common/namespace.h"

#include "OdeStruct.h"
#include "common/ErrorList.h"

#include <vector>
#include <memory>

BIO_NAMESPACE_BEGIN

enum
{
  EVAL_WAIT=0,
  EVAL_DOING,
  EVAL_DONE
} EvaluationStatus_t;
class SolverData
{
  public:
    SolverData(ErrorList& errors);
    void reset();
    void reset(const std::vector<int>& indexes, const double* value);
    void initialize(const OdeStruct* model);
    void update(const double time, const double* y);
    int f(const double time, const double* y, double* ydot);
    const double* getValues() const;
  protected:
    void update(const double time);
    double m_Time;
    const OdeStruct* m_Model;
    ErrorList& m_Errors;
    double* m_Values;
    unsigned int* m_EvalStatus;
  private:
    double evaluate(const ASTNode* node);

    std::unique_ptr<double []> values_;
    std::unique_ptr<unsigned int []> evalStatus_;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef SolverData_INC  ----- */


