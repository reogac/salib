/**
 @file CvodeHelper.h
 @brief helper class for CvodeSolver
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  CvodeHelper_INC
#define  CvodeHelper_INC

#include "SolverHelper.h"

#include <cvode/cvode.h>
#include <nvector/nvector_serial.h>

BIO_NAMESPACE_BEGIN

class CvodeHelper : public SolverHelper
{
  public:
    CvodeHelper(const OdeModel& model, SolverError& errors);
    ~CvodeHelper();

    void setOdeVariableValues(const realtype* y);

    static int f(realtype t, N_Vector y, N_Vector ydot, void *f_data);
    static int g(realtype t, N_Vector y, realtype* gout, void *g_data);
  
  protected:
  private:    
    CvodeHelper(const CvodeHelper& other) = delete;
    CvodeHelper& operator= (const CvodeHelper& other) = delete;

    int evaluateF(realtype t, N_Vector y, N_Vector ydot);
    int evaluateG(realtype t, N_Vector y, realtype* gout);
    
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef CvodeHelper_INC  ----- */


