/**
 @file CvodeSolver.cpp
 @brief Implementation for CvodeSolver class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "CvodeSolver.h"

#include <cvode/cvode.h>
#include <cvode/cvode_dense.h>

#include "common/CommonDefs.h"
#include "OdeModel.h"
#include "CvodeHelper.h"
#include "CvodeSettings.h"
#include "SolverException.h"

BIO_NAMESPACE_BEGIN

CvodeSolver::CvodeSolver(const OdeModel& model)
  : SolverBase(model)
    , y_(nullptr)
    , dy_(nullptr)
    , atols_(nullptr)
    , cvode_(nullptr)
{
  /* Allocates solver data */
  bool success(false);
  int numOdes = model.getNumOdes();

  y_ = N_VNew_Serial(numOdes);
  if (y_)
  {
    dy_ = N_VNew_Serial(numOdes);
    if (dy_)
    {
      atols_ = N_VNew_Serial(numOdes);
      if (atols_)
        success = true;
    }
  }
  if (!success)
  {
    free();
    throw (std::bad_alloc());
  }
}

CvodeSolver::~CvodeSolver()
{
  free();
}

void CvodeSolver::free()
{
  if (y_)
    N_VDestroy_Serial(y_);
  if (dy_)
    N_VDestroy_Serial(dy_);
  if (atols_)
    N_VDestroy_Serial(atols_);
  if (cvode_)
    CVodeFree(&cvode_);
}


int CvodeSolver::solve(const double endTime)
{
  CvodeHelper* helper = (CvodeHelper*) m_Helper.get();

  int ret = SATOOLS_SUCCESS;
  bool isDone = false;                          /* has the integration reached to 'endTime'? */
  bool isEventTime = false;                     /* should the integration stop at the next event execution time ?*/
  double tout = endTime;
  while ( ret == SATOOLS_SUCCESS
      && !isDone )
  {
    /* Decide a stop time for cvode.  
     *
     * Normally it is the 'endTime', but if there is a pending event execution
     * before 'endTime', the stop time should be set according to the event time
     * */

    tout = endTime;
    isEventTime = false;
    /* get nearest event execution time */
    double te = helper->getNextEventExecutionTime();
    if (te >=0 && te <= endTime)
    {
      tout = te;
      isEventTime = true;
    }
    

    /* call CVode to move integration forward */
    int mode = isMoveOneInternalStep_ ? CV_ONE_STEP : CV_NORMAL;

    int flag = CVode(cvode_, tout, y_, &m_CurrentTime, mode);
    switch (flag)
    {
      case CV_TOO_CLOSE:                        /* tout is too close to the initial time value */
        isDone = (tout == endTime);
        ret = helperUpdate(tout);   
        break;
      case CV_ROOT_RETURN:                      /* root(s) found */
        isMoveOneInternalStep_ = true;          /* go one internal step in the next round in order to check for firing triggers */
        isDone = (m_CurrentTime >= endTime);
        ret = helperUpdate(tout);
        break;
      case CV_SUCCESS:                          /* success normally */
          isMoveOneInternalStep_ = false;       /* don't go for one internal step in the next round because no trigger's gonna fire */
          isDone = (m_CurrentTime >= endTime);
          ret = helperUpdate(tout);
        break;
      default:
        /* error  */
        ret = SATOOLS_FAILURE;
    }
  }
  return ret;
}

int CvodeSolver::helperUpdate(const double tout)
{
  CvodeHelper* helper = (CvodeHelper*) m_Helper.get();

  /*  update ode variable values */
  realtype* ydata = NV_DATA_S(y_);
  helper->setOdeVariableValues(ydata);

  /* handle events and update assignment variables */

  int ret = helper->update(tout);

  if (ret == SATOOLS_SUCCESS)
  {
    if (helper->isSolverResetRequired()) 
      /* executing of some event assigments have changed values of some Ode
       * variables, the solver therefore need to be re-initialized with the new
       * intial condition*/
      ret = resetSolverStructure();
  }
  return ret;
}

int CvodeSolver::createSolverStructure(const SolverSettings* settings)
{
  if (cvode_)
  {
    CVodeFree(&cvode_);
    cvode_ = nullptr;    
  }

  const CvodeSettings* cvodeSettings = (const CvodeSettings*) settings;

  double atol = cvodeSettings->getAError();
  realtype* y = NV_DATA_S(y_);
  realtype* atols = NV_DATA_S(atols_);
  const double* values = m_Helper->getValues();

  int numOdes = m_Model.getNumOdes();
  for (int iOde=0; iOde < numOdes; iOde++)
  {
    y[iOde] = values[iOde];
    atols[iOde] = atol;
  }
  
  cvode_ = CVodeCreate(CV_BDF, CV_NEWTON);

  bool success = ( cvode_ != nullptr );

  if (success)
    success = ( CVodeInit(cvode_, CvodeHelper::f, m_CurrentTime, y_) 
                  == CV_SUCCESS );

  if (success)
    success = ( CVodeSVtolerances(cvode_, cvodeSettings->getRError(), atols_) 
                  == CV_SUCCESS );
  if (success)
    success = ( CVodeSetUserData(cvode_, m_Helper.get())
                  == CV_SUCCESS );
  if (success)
    success = ( CVodeSetErrHandlerFn(cvode_, CvodeSolver::errorHandler, this)
                  == CV_SUCCESS);
  if (success)
    success = ( CVDense(cvode_, numOdes)
                  == CVDLS_SUCCESS );
  if (success)
    success = ( CVDlsSetDenseJacFn(cvode_, nullptr)
                  == CVDLS_SUCCESS );
  if (success)
    success = ( CVodeSetMaxNumSteps(cvode_, cvodeSettings->getMaxSteps())
                  == CV_SUCCESS );
  if (success)
    success = ( CVodeRootInit(cvode_, m_Helper->getNumRootFinders(), CvodeHelper::g)
                  == CV_SUCCESS );

  return success ? SATOOLS_SUCCESS : SATOOLS_FAILURE;
}
int CvodeSolver::resetSolverStructure()
{
  /*  m_CurrentTime = m_Helper->getTime() */
  const double* values = m_Helper->getValues();
  realtype *ydata = NV_DATA_S(y_);
  for ( int iValue = 0; iValue < m_Model.getNumOdes(); iValue++)
  {
    ydata[iValue] = values[iValue];
  }
  if (CVodeReInit(cvode_, m_CurrentTime, y_) == CV_SUCCESS)
  {
    return SATOOLS_SUCCESS;
  }
  return SATOOLS_FAILURE;
}

CvodeHelper* CvodeSolver::createSolverHelper()
{
  return new CvodeHelper(m_Model, m_Errors);  
}

void CvodeSolver::errorHandler(int error_code
        , const char* module, const char* function
        , char* msg, void* eh_data)
{
  CvodeSolver* solver = (CvodeSolver*) eh_data;
  solver->handleError(error_code, module, function, msg);
}


void CvodeSolver::handleError(int error_code, const char* module
        , const char* function, char* msg)
{
  std::cout << "Cvode solver error [" << error_code << "][" << module << "][" << function << "]: " << msg << "\n";
}

BIO_NAMESPACE_END

