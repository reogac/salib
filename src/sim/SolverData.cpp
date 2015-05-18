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
#include <sstream>
#include <iostream>

BIO_NAMESPACE_BEGIN

#define MySQR(x) ((x)*(x))

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
  double result{0};

  if ( !node ) 
  {
    m_Errors.add(new SimError(SIM_ERROR_EMPTY_AST, "Empy ASTNode"));
    return 0;
  }
  if ( node->isUnknown() ) 
  {
    m_Errors.add(new SimError(SIM_ERROR_UNKNOWN_AST, "Unknown ASTNode"));
    return 0;
  }

  int childnum = node->getNumChildren();
  int isTrue{};
  int i{};

  ASTNodeType_t type = node->getType();
  switch(type)
  {
    case AST_INTEGER:
      result = (double) node->getInteger();      
      break;
    case AST_REAL:
      result = node->getReal();
      break;
    case AST_REAL_E:
      result = node->getReal();
      break;      
    case AST_RATIONAL:
      result = node->getReal() ;
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
                std::ostringstream strstr;
                strstr << "Cyclic AST evaluation at " << node->getName();
                m_Errors.add(new SimError(SIM_ERROR_CYCLIC_AST, strstr.str()));          
                break;
              }
            case EVAL_DONE:
              result = m_Values[index];
              break;
            default:                             // EVAL_WAIT
              m_EvalStatus[index] = EVAL_DOING;
              if (variable->getType() == ODE_ASSIGNMENT_VARIABLE)
              {
                result=evaluate(m_Model->getAssignment(variable->getEqIndex()));

              } else if (variable->getInitEqIndex()>=0)
              {
                result=evaluate(m_Model->getInitAssignment(variable->getInitEqIndex()));
              } else 
              {
                //should never happen
                m_Errors.add(new SimError(SIM_UNKNOWN_ERROR, LEVEL_FATAL));          
              }
              m_Values[index] = result;
              m_EvalStatus[index] = EVAL_DONE;
              break;
          }
        } else 
        {
          const char* name = node->getName();          
          if ( strcmp(name, "t") == 0
              || strcmp(name,"T") == 0 
              || strcmp(name,"time") == 0 
              || strcmp(name, "Time") == 0 
              || strcmp(name, "TIME") == 0 ) 
          { 
            result = (double) m_Time; 
          } else 
          {
            std::ostringstream strstr;
            strstr << node->getName() << " has not beed defined";
            m_Errors.add(new SimError(SIM_ERROR_CYCLIC_AST, strstr.str()));          
            result = 0.0;
          }
        }
        break;
      }
    case AST_FUNCTION_DELAY:
      m_Errors.add(new SimError(SIM_WARNING_DELAY_ODE
          , "Solving ODEs with Delay is not implemented. "
          "Defaults to 0 to avoid program crash", LEVEL_WARNING));
      result = 0.0;
      break;
    case AST_NAME_TIME:
      result = (double) m_Time;
      break;

    case AST_CONSTANT_E:
      /** exp(1) is used to adjust exponentiale to machine precision */
      result = (double)exp(1.0);
      break;
    case AST_CONSTANT_FALSE:
      result = 0.0;
      break;
    case AST_CONSTANT_PI:
      /** pi = 4 * atan 1  is used to adjust Pi to machine precision */
      result = 4.*atan(1.);
      break;
    case AST_CONSTANT_TRUE:
      result = 1.0;
      break;

    case AST_PLUS:
      result = 0.0;
      for(i=0; i<childnum; ++i) 
        result += evaluate(node->getChild(i));   
      break;      
    case AST_MINUS:
      if(childnum<2)
        result = - (evaluate(node->getChild(0)));
      else
        result = evaluate(node->getChild(0)) - evaluate(node->getChild(1));
      break;
    case AST_TIMES:
      result = 1.0;
      for( i=0; i<childnum; ++i) 
        result *= evaluate(node->getChild(i));
      break;
    case AST_DIVIDE:
      result = evaluate(node->getChild(0)) / evaluate(node->getChild(1));
      break;
    case AST_POWER:
      result = pow(evaluate(node->getChild(0)),evaluate(node->getChild(1)));
      break;
    case AST_LAMBDA:
      m_Errors.add(new SimError(SIM_WARNING_LAMBDA_USING,
            "Lambda can only be used in function definitions."
            " Defaults to 0 to avoid program crash", LEVEL_WARNING));
      result = 0.0;
      break;
      /** FUNCTIONS: */
    case AST_FUNCTION:
      {
        std::ostringstream strstr;
        strstr << "The function " << node->getName() << "() has not been defined "
               << "in the SBML input model or as an externally "
               << "supplied function. Defaults to 0 to avoid "
               << "program crash";
      m_Errors.add(new SimError(SIM_WARNING_UNDEFINED_FUNCTION, strstr.str(), LEVEL_WARNING));
      result = 0.0;
      break;
      }
    case AST_FUNCTION_ABS:
      result = (double) fabs(evaluate(node->getChild(0)));
      break;
    case AST_FUNCTION_ARCCOS:
      result = acos(evaluate(node->getChild(0))) ;
      break;
    case AST_FUNCTION_ARCCOSH:
      result = acosh(evaluate(node->getChild(0)));
      break;
    case AST_FUNCTION_ARCCOT:
      /** arccot x =  arctan (1 / x) */
      result = atan(1./ evaluate(node->getChild(0)));
      break;
    case AST_FUNCTION_ARCCOTH:
      /** arccoth x = 1/2 * ln((x+1)/(x-1)) */
      result = ((1./2.)*log((evaluate(node->getChild(0))+1.)/
            (evaluate(node->getChild(0))-1.)) );
      break;
    case AST_FUNCTION_ARCCSC:
      /** arccsc(x) = Arctan(1 / sqrt((x - 1)(x + 1))) */
      result = atan( 1. / sqrt( (evaluate(node->getChild(0))-1.)*
            (evaluate(node->getChild(0))+1.) ) );
      break;
    case AST_FUNCTION_ARCCSCH:
      /** arccsch(x) = ln((1 + sqrt(1 + x^2)) / x) */
      result = log((1.+sqrt((1+MySQR(evaluate(node->getChild(0)))))) /
          evaluate(node->getChild(0)));
      break;
    case AST_FUNCTION_ARCSEC:
      /** arcsec(x) = arctan(sqrt((x - 1)(x + 1))) */   
      result = atan( sqrt( (evaluate(node->getChild(0))-1.)*
            (evaluate(node->getChild(0))+1.) ) );
      break;
    case AST_FUNCTION_ARCSECH:
      /** arcsech(x) = ln((1 + sqrt(1 - x^2)) / x) */
      result = log((1.+sqrt((1-MySQR(evaluate(node->getChild(0))))))/
          evaluate(node->getChild(0)));      
      break;
    case AST_FUNCTION_ARCSIN:
      result = asin(evaluate(node->getChild(0)));
      break;
    case AST_FUNCTION_ARCSINH:
      result = asinh(evaluate(node->getChild(0)));
      break;
    case AST_FUNCTION_ARCTAN:
      result = atan(evaluate(node->getChild(0)));
      break;
    case AST_FUNCTION_ARCTANH:
      result = atanh(evaluate(node->getChild(0)));
      break;
    case AST_FUNCTION_CEILING:
      result = ceil(evaluate(node->getChild(0)));
      break;
    case AST_FUNCTION_COS:
      result = cos(evaluate(node->getChild(0)));
      break;
    case AST_FUNCTION_COSH:
      result = cosh(evaluate(node->getChild(0)));
      break;
    case AST_FUNCTION_COT:
      /** cot x = 1 / tan x */
      result = (1./tan(evaluate(node->getChild(0))));
      break;
    case AST_FUNCTION_COTH:
      /** coth x = cosh x / sinh x */
      result = cosh(evaluate(node->getChild(0))) /
        sinh(evaluate(node->getChild(0)));
      break;  
    case AST_FUNCTION_CSC:
      /** csc x = 1 / sin x */
      result = (1./sin(evaluate(node->getChild(0))));
      break;
    case AST_FUNCTION_CSCH:
      /** csch x = 1 / sinh x  */
      result = (1./sinh(evaluate(node->getChild(0))));
      break;
    case AST_FUNCTION_EXP:
      result = exp(evaluate(node->getChild(0)));
      break;
    case AST_FUNCTION_FACTORIAL:
      {
        double floam_Timek = evaluate(node->getChild(0));
        int k = (int) floor(floam_Timek);
        if ( floam_Timek != k ) 
        {
          m_Errors.add(new SimError(SIM_WARNING_FACTORIAL_USING
              , "The factorial is only implemented "
              "for integer values. The floor value of the "
              "passed float is used for calculation!", LEVEL_WARNING));
        }

        for(result=1; k>1; --k)
          result *= k;
      }
      break;
    case AST_FUNCTION_FLOOR:
      result = floor(evaluate(node->getChild(0)));
      break;
    case AST_FUNCTION_LN:
      result = log(evaluate(node->getChild(0)));
      break;
    case AST_FUNCTION_LOG:
      /** log(x,y) = log10(y)/log10(x) (where x is the base)  */
      result = log10(evaluate(node->getChild(1))) /
        log10(evaluate(node->getChild(0)));
      break;
    case AST_FUNCTION_PIECEWISE:
      if ( evaluate(node->getChild(0)) ) {
        result = evaluate(node->getChild(1));
      }
      else {
        result = evaluate(node->getChild(2));
      }
      break;
    case AST_FUNCTION_POWER:
      result = pow(evaluate(node->getChild(0)),evaluate(node->getChild(1)));
      break;
    case AST_FUNCTION_ROOT:
      result = pow(evaluate(node->getChild(1)),
          (1./evaluate(node->getChild(0))));
      break;
    case AST_FUNCTION_SEC:
      /** sec x = 1 / cos x */
      result = 1./cos(evaluate(node->getChild(0)));
      break;
    case AST_FUNCTION_SECH:
      /** sech x = 1 / cosh x */
      result = 1./cosh(evaluate(node->getChild(0)));
      break;
    case AST_FUNCTION_SIN:
      result = sin(evaluate(node->getChild(0)));
      break;
    case AST_FUNCTION_SINH:
      result = sinh(evaluate(node->getChild(0)));
      break;
    case AST_FUNCTION_TAN:
      result = tan(evaluate(node->getChild(0)));
      break;
    case AST_FUNCTION_TANH:
      result = tanh(evaluate(node->getChild(0)));
      break;

    case AST_LOGICAL_AND:
      isTrue = 0;
      for ( i=0; i<childnum; ++i ) 
      {
        isTrue += (int)evaluate(node->getChild(i));
      }
      if ( isTrue == childnum ) 
      {
        result = 1.0;
      } else 
      {
        result = 0.0;
      }
      break;
    case AST_LOGICAL_NOT:
      result = (double) (!(evaluate(node->getChild(0))));
      break;
    case AST_LOGICAL_OR:
      isTrue = 0;
      for (i=0; i<childnum; ++i ) 
      {
        isTrue += (int)evaluate(node->getChild(i));
      }

      if ( isTrue > 0 ) 
      {
        result = 1.0;
      }
      else 
      {
        result = 0.0;
      }
      break;
    case AST_LOGICAL_XOR:
      /* n-ary: true if an odd number of children is true */
      isTrue = 0;
      for ( i=0; i<childnum; ++i ) 
      {
        isTrue += (int)evaluate(node->getChild(i));
      }
      if ( isTrue % 2 != 0 ) 
      {
        result = 1.0;
      } else 
      {
        result = 0.0;
      }
      break;
      /* !!! check n-ary definitions for relational operators !!! */
    case AST_RELATIONAL_EQ:
      /* n-ary: all children must be equal */
      result = 1.0;
      for ( i=1; i<childnum; ++i ) 
      {
        if ( (evaluate(node->getChild(0))) !=
            (evaluate(node->getChild(i))) ) 
        {
          result = 0.0;
        }
      }
      break;
    case AST_RELATIONAL_GEQ:
      /* n-ary: each child must be greater than or equal to the following */
      result = 1.0;
      for ( i=0; i < childnum-1; ++i ) 
      {
        if ( (evaluate(node->getChild(i))) <
            (evaluate(node->getChild(i+1))) ) 
        {
          result = 0.0;
        }
      }
      break;
    case AST_RELATIONAL_GT:
      /* n-ary: each child must be greater than the following */
      result = 1.0;
      for ( i=0; i<childnum-1; ++i ) 
      {
        if ( (evaluate(node->getChild(i))) <=
            (evaluate(node->getChild(i+1))) ) 
        {
          result = 0.0;
        }
      }
      break;
    case AST_RELATIONAL_LEQ:
      /* n-ary: each child must be lower than or equal to the following */
      result = 1.0;
      for ( i=0; i<childnum-1; ++i ) 
      {
        if ( (evaluate(node->getChild(i))) >
            (evaluate(node->getChild(i+1))) ) 
        {
          result = 0.0;
        }
      }
      break;
    case AST_RELATIONAL_LT :
      /* n-ary: each child must be lower the following */
      result = 1.0;
      for ( i=0; i<childnum-1; ++i ) 
      {
        if ( (evaluate(node->getChild(i))) >=
            (evaluate(node->getChild(i+1))) ) 
        {
          result = 0.0;
        }
      }
      break;
    default:
      result = 0;
      break;
  }
  return result;
}

bool isZero(const double d)
{
  return d<CONST_EPS && d>-CONST_EPS;
}

BIO_NAMESPACE_END


