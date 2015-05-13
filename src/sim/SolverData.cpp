/**
 @file SolverData.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */
#include "SolverData.h"

#include "SimError.h"

#include "common/Logger.h"
#include "common/utils.h"

#include <cmath>
#include <cassert>
#include <limits>
#include <iostream>

BIO_NAMESPACE_BEGIN

static bool isZero(const double d);

constexpr double CONST_LN10 = log(10.0);
constexpr double CONST_E = exp(1.0);
constexpr double CONST_PI = 4.0*atan(1.0);
constexpr double CONST_EPS = std::numeric_limits<double>::epsilon();

SolverData::SolverData(ErrorList& errors)
  : m_Errors(errors)
  , m_Model(nullptr)
  , m_Time(0.0)
  , values_(nullptr)
  , m_Values(nullptr)
  , evalStatus_(nullptr)
  , m_EvalStatus(nullptr)
{
  //initialize internal data structure
}
const double* SolverData::getValues() const
{
  return m_Values;
}
void SolverData::initialize(const OdeStruct* model)
{
  m_Model = model;
  unsigned int numVariables = model->getNumVariables();
  //allocate internal data structure
  values_.reset(new double[numVariables]);
  m_Values = values_.get();
  evalStatus_.reset(new unsigned int[numVariables]);
  m_EvalStatus = evalStatus_.get();


  const OdeVariable* const* variables = model->getVariables();
  //setup intial state
  for (unsigned int iV = 0; iV<numVariables; ++iV)
  {
    m_Values[iV] = variables[iV]->getValue();
  }
  //evaluate (initial) assignments at the intial state
  update(model->getInitialTime());
}

void SolverData::reset()
{
  //reset to the intial state

  unsigned int numVariables = m_Model->getNumVariables();
  const OdeVariable* const* variables = m_Model->getVariables();
  //setup intial state
  for (unsigned int iV = 0; iV<numVariables; ++iV)
  {
    m_Values[iV] = variables[iV]->getValue();
  }

  update(m_Model->getInitialTime());
}

void SolverData::reset(const std::vector<int>& indexes, const double* values)
{
  //reset to the intial state
  
  unsigned int numVariables = m_Model->getNumVariables();
  const OdeVariable* const* variables = m_Model->getVariables();
  //setup intial state
  for (unsigned int iV = 0; iV<numVariables; ++iV)
  {
    m_Values[iV] = variables[iV]->getValue();
  }

  update(m_Model->getInitialTime());
}

void SolverData::update(const double time)
{
  m_Time = time;
  bool fZero = isZero(time-m_Model->getInitialTime());
  unsigned int numVariables = m_Model->getNumVariables();
  const OdeVariable* variable=nullptr;
  //set the assignment variables to evaluation waiting state
  for (unsigned int iV=0; iV< numVariables; ++iV)
  {
    variable = m_Model->getVariable(iV);
    if (variable->getType()
        == ODE_ASSIGNMENT_VARIABLE)
      m_EvalStatus[iV] = EVAL_WAIT;
    else if (fZero 
        && variable->hasInitAssignment())
    {
      m_EvalStatus[iV] = EVAL_WAIT;
    }
    else
      m_EvalStatus[iV] = EVAL_DONE;
  }
  //evaluate all assignments
  for (unsigned int iV=0; iV<numVariables; ++iV)
  {
    variable = m_Model->getVariable(iV);
    if (m_EvalStatus[iV] == EVAL_WAIT)
    {
      m_EvalStatus[iV] = EVAL_DOING;
      if (variable->getType() == ODE_ASSIGNMENT_VARIABLE)
        m_Values[iV] = evaluate(m_Model->getAssignment(variable->getEqIndex()));
      else                                      /* it must be a initial assignment */        
        m_Values[iV] = evaluate(m_Model->getInitAssignment(variable->getInitEqIndex()));
      m_EvalStatus[iV] = EVAL_DONE;
    }
  }
}

void SolverData::update(const double time, const double* odeValues)
{
  for (unsigned int iO = 0; iO< m_Model->getNumOdes(); ++iO)
  {
    m_Values[iO] = odeValues[iO]; 
  }
  update(time);
}

int SolverData::f(const double time, const double* y, double* ydot)
{
  unsigned int numOdes = m_Model->getNumOdes();
  // update the values of ode variables
  for (unsigned int iO=0; iO<numOdes; ++iO)
    m_Values[iO] = y[iO];

  //setup status for assignment evaluation
  for (unsigned int iV=0; iV<m_Model->getNumVariables(); ++iV)
  {
    if (m_Model->getVariable(iV)->getType()
        == ODE_ASSIGNMENT_VARIABLE)
      m_EvalStatus[iV] = EVAL_WAIT;
    else
      m_EvalStatus[iV] = EVAL_DONE;
  }

  //evaluate all odes
  for (unsigned int iO=0; iO<numOdes; ++iO)
  {
    ydot[iO] = evaluate(m_Model->getOde(iO));
  }

  if (m_Errors.getNumErrors() + m_Errors.getNumFatals() >0)
    return -1;
  return 0;
}

double SolverData::evaluate(const ASTNode* node)
{
  double ret = 0.0;
  /* 
  if (node==nullptr)
  {
    m_Errors.add(new SimError(SIM_UNKNOWN_ERROR, LEVEL_FATAL));          
    return ret;
  }

  int type = node->getType();
  switch (type)
  {
    case AST_NUMBER:
      ret = node->getValue();
      break;
    case AST_CONST_E:
      ret = CONST_E;
      break;
    case AST_CONST_PI:
      ret = CONST_PI; 
      break;
    case AST_NAME_TIME:
      ret = m_Time;
      break;
    case AST_NAME:
      {
        const OdeVariable* variable = (const OdeVariable*) node->getUserData();
        if (variable)
        {

          int index = variable->getIndex();
          switch (m_EvalStatus[index])
          {
            case EVAL_DOING:
              {
                m_Errors.add(new SimError(SIM_CYCLIC_AST, formatMessage("cyclic AST evaluation at '%s'", node->getName())));          
                  break;
              }
            case EVAL_DONE:
              ret = m_Values[index];
              break;
            default:                             // EVAL_WAIT
              m_EvalStatus[index] = EVAL_DOING;
              if (variable->getType() == ODE_ASSIGNMENT_VARIABLE)
              {
                ret=evaluate(m_Model->getAssignment(variable->getEqIndex()));

              } else if (variable->getInitEqIndex()>=0)
              {
                ret=evaluate(m_Model->getInitAssignment(variable->getInitEqIndex()));
              } else 
              {
                //should never happen
                m_Errors.add(new SimError(SIM_UNKNOWN_ERROR, LEVEL_FATAL));          
              }
              m_Values[index] = ret;
              m_EvalStatus[index] = EVAL_DONE;
              break;
          }
        } else 
        {
          m_Errors.add(new SimError(SIM_EMPTY_AST, formatMessage("'%s' has not been defined", node->getName())));
          VERROR("'%s' has not been defined", node->getName())
        }
        break;
      }
    case AST_OP_PLUS:
      ret = evaluate(node->getChild(0)) + evaluate(node->getChild(1));
      break;
    case AST_OP_MINUS:
      ret = evaluate(node->getChild(0))-evaluate(node->getChild(1));
      break;
    case AST_OP_NEG:
      ret = - evaluate(node->getChild());
      break;
    case AST_OP_TIMES:
      ret = evaluate(node->getChild(0)) * evaluate(node->getChild(1));
      break;
    case AST_OP_DIVIDE:
      {
        ret = evaluate(node->getChild(0));
        double d = evaluate(node->getChild(1));
        if (isZero(d))
        {
          //TODO: add an error
          ret = 0.0;
        } else
          ret = ret / d;
        break;
      }
    case AST_OP_POW:
      ret = pow(evaluate(node->getChild(0)), evaluate(node->getChild(1)));
      //TODO: might need to check the pow function condition
      break;
    case AST_FUNC_SIN:
      ret = sin(evaluate(node->getChild()));
      break;
    case AST_FUNC_COS:
      ret = cos(evaluate(node->getChild()));
      break;
    case AST_FUNC_TAN:
      ret = tan(evaluate(node->getChild()));
      break;
    case AST_FUNC_LOG:
      ret = log(evaluate(node->getChild()))/CONST_LN10;
      break;
    case AST_FUNC_LN:
      ret = log(evaluate(node->getChild()));
      break;
    case AST_FUNC_EXP:
      ret = exp(evaluate(node->getChild()));
      break;
    case AST_FUNC_SQRT:
      ret = sqrt(evaluate(node->getChild()));
      break;
    default:
      m_Errors.add(new SimError(SIM_UNKNOWN_ERROR, LEVEL_FATAL));          
      break;
  }
  */
  return ret;
}

bool isZero(const double d)
{
  return d<CONST_EPS && d>-CONST_EPS;
}

BIO_NAMESPACE_END


