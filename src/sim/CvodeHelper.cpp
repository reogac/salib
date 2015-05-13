/**
 @file CvodeHelper.cpp
 @brief Implementation for CvodeHelper class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "CvodeHelper.h"

#include "common/CommonDefs.h"
#include "SolverError.h"

#include "OdeModel.h"
#include "CvodeSettings.h"

BIO_NAMESPACE_BEGIN

int CvodeHelper::f(realtype t, N_Vector y, N_Vector ydot, void *f_data)
{
  CvodeHelper* helper = (CvodeHelper*) f_data;
  return helper->evaluateF(t, y, ydot);
}

int CvodeHelper::g(realtype t, N_Vector y, realtype* gout, void* g_data)
{
  CvodeHelper* helper = (CvodeHelper*) g_data;
  return helper->evaluateG(t, y, gout);
}
CvodeHelper::CvodeHelper(const OdeModel& model, SolverError& errors)
  :SolverHelper(model, errors)
{
}

CvodeHelper::~CvodeHelper()
{

}

void CvodeHelper::setOdeVariableValues(const realtype* y)
{
  for (int iOde = 0; iOde < m_Model->getNumOdes(); iOde++)
    m_Values[iOde] = y[iOde];
}

int CvodeHelper::evaluateF(realtype t, N_Vector y, N_Vector ydot)
{
  realtype *ydata = NV_DATA_S(y);
  realtype* dydata = NV_DATA_S(ydot);
  /* set current time to 't' */
  m_Time = t;
  /* let all assignment variables wait for evaluation */
  for (int i=0; i < m_Model->getNumAssignments(); ++i)
  {
    m_EvalStatus[m_Assignments[i].var_] = EVAL_WAITING; 
  }

  /* update ode variable values from solver's internal structures */
  for (int iOde = 0; iOde < m_Model->getNumOdes(); ++iOde)
  {
    m_Values[m_Odes[iOde].var_] = ydata[iOde];
  }

  /* evaluate ydot */
  for (int iOde = 0; iOde < m_Model->getNumOdes(); ++iOde)
  {
    dydata[iOde] = evaluate(m_Odes[iOde].math_);
  }

  /* evaluate remaining assignment variables (some might have been evaluated in
   * the previous step*/

  for (int iAssignment=0; iAssignment < m_Model->getNumAssignments(); ++iAssignment)
  {
    if (m_EvalStatus[m_Assignments[iAssignment].var_] == EVAL_WAITING)
    {
      evaluate(m_Assignments[iAssignment].wrapper_);
    }
  }

  int numErrors = m_Errors.getNumErrors() ;
  return numErrors > 0 ? -1 : 0;
}

int CvodeHelper::evaluateG(realtype t, N_Vector y, realtype* gout)
{
  /* set current time to 't' */
  m_Time = t;
  realtype *ydata = NV_DATA_S(y);

  /* let all assignment variables wait for evaluation */
  for (int i=0; i < m_Model->getNumAssignments(); ++i)
  {
    m_EvalStatus[m_Assignments[i].var_] = EVAL_WAITING; 
  }

  /* update ode variable values from solver's internal structures */
  for (int iOde = 0; iOde < m_Model->getNumOdes(); ++iOde)
  {
    m_Values[m_Odes[iOde].var_] = ydata[iOde];
  }

  /*  evaluate root finder equations */
  for (int iEq=0; iEq < m_NumRootFinders; ++iEq)
  {
    gout[iEq] = evaluate(m_RootFinders[iEq]); 
  }

  int numErrors = m_Errors.getNumErrors() ;
  return numErrors > 0 ? -1 : 0;
}

BIO_NAMESPACE_END

