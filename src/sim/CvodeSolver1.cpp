/**
 @file CvodeSolver1.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "CvodeSolver1.h"

#include "Solver.h"
#include "CvodeSettings.h"

#include <iostream>

#include "common/Logger.h"

#include <cvode/cvode.h>
#include <cvode/cvode_dense.h>

BIO_NAMESPACE_BEGIN

CvodeSolver1::CvodeSolver1(Solver* wrapper)
  : InnerSolver(wrapper)
  , y_(nullptr)
  , dy_(nullptr)
  , atols_(nullptr)
  , cvode_(nullptr)
{
  int errors = 0;
  y_= N_VNew_Serial(m_NumOdes);
  if (y_ == nullptr)
    errors++;
  if (errors == 0)
    dy_ = N_VNew_Serial(m_NumOdes);
  if (dy_ == nullptr)
    errors++;
  if (errors==0)
    atols_ = N_VNew_Serial(m_NumOdes);
  if (atols_ == nullptr)
    errors++;
  if (errors)
  {
    free();
    throw std::bad_alloc();
  }
}

CvodeSolver1::~CvodeSolver1()
{
  free();
  if (cvode_)
    CVodeFree(&cvode_);
}
void CvodeSolver1::free()
{
  N_VDestroy_Serial(y_);
  N_VDestroy_Serial(dy_);
  N_VDestroy_Serial(atols_);
}

void CvodeSolver1::doInitialize()
{
  if (cvode_)
    CVodeFree(&cvode_);
  const CvodeSettings* settings = (const CvodeSettings*) getSettings();

  double* aErrors = NV_DATA_S(atols_);
  for (unsigned int i=0; i<m_NumOdes; ++i)
    aErrors[i] = settings->getAError();

  cvode_ = CVodeCreate(CV_BDF, CV_NEWTON);
  CVodeInit(cvode_, CvodeSolver1::f, m_Time, y_);
  CVodeSVtolerances(cvode_, settings->getRError(), atols_) ;
  CVodeSetUserData(cvode_, m_Data);
  CVodeSetErrHandlerFn(cvode_, CvodeSolver1::handleError, this);
  CVDense(cvode_, m_NumOdes);
  CVDlsSetDenseJacFn(cvode_, nullptr);
  CVodeSetMaxNumSteps(cvode_, settings->getMaxSteps());
//  CVodeRootInit(cvode_, m_Helper->getNumRootFinders(), CvodeHelper::g);
}

void CvodeSolver1::doReset()
{
  CVodeReInit(cvode_, m_Time, y_);
}

void CvodeSolver1::solve(const double tout)
{
  //TODO: handler trigger (need root finder)
  CVode(cvode_, tout, y_, &m_Time, CV_NORMAL);
  m_Data->update(tout, NV_DATA_S(y_));
}

double* CvodeSolver1::getY()
{
  return NV_DATA_S(y_);
}

int CvodeSolver1::f(double t, N_Vector y, N_Vector ydot, void* fdata)
{
  SolverData* eval = (SolverData*) fdata;
  return eval->f(t, NV_DATA_S(y), NV_DATA_S(ydot));
}


void CvodeSolver1::handleError(int code, const char* module, const char* function, char* msg, void* data)
{
  std::cout << "Cvode solver error [" << code << "][" << module << "][" << function << "]: " << msg << "\n";
}
BIO_NAMESPACE_END

