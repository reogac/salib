/**
 @file OdeSim.cpp
 @brief Implementation for OdeSim class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "OdeSim.h"


#include "common/CommonDefs.h"
#include "common/SaError.h"
#include "common/SaException.h"

#include "OdeModel.h"
#include "SolverSettings.h"
#include "CvodeSettings.h"
#include "SolverBase.h"
#include "CvodeSolver.h"
#include "SolverException.h"

BIO_NAMESPACE_BEGIN

OdeSim::OdeSim(const OdeModel& model)
  : m_Model(model)
    , m_SolverSettings(nullptr)
    , m_Solver(nullptr)
    , m_SimValues(nullptr)
    , m_Timesteps{0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10}
    , m_CurrentStep(0)

{
}

OdeSim::~OdeSim()
{
}

OdeSim& OdeSim::setTimeSteps(const std::vector<double>& timesteps)
{
  if (timesteps.size() == 0)
  {
    throw InputException("No simulation time step");
  }

  auto it = timesteps.begin();
  double t1 = *it;
  double t2{};
  bool invalid = (t1 <= 0);
  it++;
  while (!invalid && it != timesteps.end())
  {
    t2 = *it;
    invalid = (t2<=0) || (t2<= t1);
    t1 = t2;
    it++;
  }
  if (invalid)
  {
    throw InputException( "time steps should make a positive ascending value sequence" );
  }
  /* add time step 'zero' at the begining */
  m_Timesteps.clear();
  m_Timesteps.emplace_back(0.0);

  for (auto& t : timesteps)
  {
    m_Timesteps.emplace_back(t);
  }
  m_IsTimeChanged = true;
  
  return *this;
}

OdeSim& OdeSim::setTimeStepsByDuration(const double duration, const int numSteps)
{
  if ( duration<=0 )
    throw InputException("Simulation duration must be a positive value");
  if (numSteps <=0)
    throw SolverException("Number of time steps must be positive");
  double interval = duration/numSteps;
  setTimeStepsByInterval(interval, numSteps);
  m_Timesteps.back() = duration;
  return *this;
}

OdeSim& OdeSim::setTimeStepsByInterval(const double interval, const int numIntervals)
{
  if ( interval<=0 )
    throw SolverException("Time interval must be positive");
  if (numIntervals <=0)
    throw SolverException("Number of interval must be positive");

  m_Timesteps.clear();
  m_Timesteps.reserve(numIntervals+1);
  double prev = 0.0;
  m_Timesteps.emplace_back(0.0);
  for (int i=0; i<numIntervals; ++i)
  {
    prev += interval;
    m_Timesteps.emplace_back(prev);
  }
  return *this;
}

OdeSim& OdeSim::setVaryingParameters( const std::set<int>& indexes)
{
  bool invalid =false;
  for (int index : indexes)
  {
    if ( index < 0 || index >= m_Model.getNumVariables())
    {
      invalid = true;
      break;
    }

    //TODO:we should check if the variable can be changed
  }
  if (invalid)
  {
    throw InputException( "Out of range variable index " );
  }
  m_VaryingParameters = indexes;
  m_IsParameterChanged = true;
  return *this;
}

/* Set solver settings for the simulator.
 * The function fails if model does not have a supported solver or the solver is
 * not suitable to the model 
 * NOTE: currently, only Cvode solver for ODE models is implemented*/
OdeSim& OdeSim::setSolverSettings(const SolverSettings* solverSettings)
{
  /* TODO: we should change if solver settings is valid regarding to the model
   * properties */
  if (solverSettings
      && solverSettings->getSolverType() == ODE_SOLVER_CVODE )
  {
    m_SolverSettings.reset(new SolverSettings(*solverSettings));
    m_IsSolverChanged = true;
    return *this;
  }

  throw InputException( "Unsupported model or invalid solver");
}

void OdeSim::createDefaultSolverSettings()
{
  if (m_Model.getNumOdes() <=0)
  {
    /* model has no ODE */
    
    /* currently we don't support models without ODEs */
    throw SolverException( "Model without ODEs is not supported");
  }

  if (m_Model.getNumAlgebraicEquations() >0)
  {
    /*  model has algebraic equations */

    /* currently we don't support models with algebraic equations */
    throw SolverException( "Model with algebraic equations is not supported");
  }

  /* create a default setting for CvodeSolver */
  m_SolverSettings.reset(new CvodeSettings());
}


SolverBase* OdeSim::createSolver()
{
  assert(m_SolverSettings);

  SolverBase* ret(nullptr);
  switch (m_SolverSettings->getSolverType())
  {
    case ODE_SOLVER_CVODE:
      ret = new CvodeSolver(m_Model);
      break;
    default:
      throw SolverException( "not implemented solver");
      break;
  }
  
  return ret;
}


void OdeSim::init()
{
  if ( (!m_Solver)
      || ( m_IsParameterChanged
           && m_IsSolverChanged) )
  {
    /*  Solver has not been created;
     *  Or both varying parameter list and solver configuration have been changed.
     *  In both cases, we create a new solver*/
    
    std::cout << "create solver settings \n";
    if (!m_SolverSettings)                        /* solver configuration have
                                                     not been set */
    {
      createDefaultSolverSettings();            /* create a default solver
                                                   settings */
    }

    std::unique_ptr<SolverBase> solver(createSolver()); /* create a solver */

    if (solver)
    {
      std::cout << "creates solver successfully\n";
      /* initialize the solver */
      solver->init(m_SolverSettings.get()
          , m_VaryingParameters);

      m_Solver = std::move(solver);
      std::cout << "init solver successfully\n";
      m_SimValues = m_Solver->getValues();
    }
  
    /* in all remaining cases, the solve has not been created or initialized
     * successfully,*/

  } else if (m_IsParameterChanged)
  {
    /* only varying parameter list
     * has been changed, we need to re-optimize the solver internal
     * structures */ 
    m_Solver->reinit(m_VaryingParameters);
  } else if (m_IsSolverChanged)
    /* only solver settings are changed, re-init the solver */
  {
    m_Solver->reinit(m_SolverSettings.get());
  }
  else if (m_IsTimeChanged)
    /* simulation timsteps changed*/
  {
    m_Solver->restart();
  }

  m_CurrentStep = 0;
  m_IsParameterChanged = false;
  m_IsSolverChanged = false;
  m_IsTimeChanged = false;
}

void OdeSim::checkReady()
{
  if (!m_Solver)
  {
    throw SolverException("Solver has not been initialized");
  }

  if (m_IsTimeChanged
      || m_IsSolverChanged
      || m_IsParameterChanged)
  {
    throw SolverException("Simulation parameters have been chaged, solve need to be re-initialized");
  }
}

int OdeSim::simulateOne()
{
  checkReady();

  /*  solver util the next time step value */
  int ret = m_Solver->solve(m_Timesteps[m_CurrentStep+1]);
  if (ret == SATOOLS_SUCCESS)
  {
    m_CurrentStep++;                            /* update current step if simulation is success */
  }

  return ret;
}

bool OdeSim::isCompleted() const
{
  return m_CurrentStep == m_Timesteps.size()-1;
}
int OdeSim::getCurrentStep() const
{
  return m_CurrentStep;
}

int OdeSim::simulate()
{
  int ret = SATOOLS_SUCCESS;
  m_CurrentStep = 0;                       /* should we start from initial
                                                 condition? */
  while(true)
  {
    /*
       This is where to retrieve simulation result at current time step
       */

    if (m_CurrentStep == m_Timesteps.size()-1)
    {
      break;
    }

    if (simulateOne() != SATOOLS_SUCCESS)
    {
      ret = SATOOLS_FAILURE;
      break;
    }
  }

  if (ret != SATOOLS_SUCCESS)
  {
    const SolverError& errors = m_Solver->getErrors();
    /* Prints errors 
     *
     *
     * */
  }
  return ret;
}

void OdeSim::restart()
{
  if (m_Solver)
  {
    m_Solver->restart();                  /* reset solver to time zero */
    m_CurrentStep = 0;                          /* reset current step to zero */
  }
}


void OdeSim::restart(const double* values)
{
  if (m_Solver)
  {
    m_Solver->restart(m_VaryingParameters, values);
    m_CurrentStep = 0;
  }
}

BIO_NAMESPACE_END

