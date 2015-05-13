/**
 @file SolverHelper.cpp
 @brief Implementation for SolverHelper class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "SolverHelper.h"

#include "EventExecutionList.h"

#include "common/CommonDefs.h"
#include "OdeModel.h"
#include "SolverHelperBuilder.h"
#include "SolverError.h"
#include "SolverException.h"

BIO_NAMESPACE_BEGIN

#define MySQR(x) ((x)*(x))

SolverHelper::SolverHelper(const OdeModel& model, SolverError& errors)
  : m_OrigModel(model)
    , m_EventExecutions()
    , m_Errors(errors)
    , m_Odes{}
    , m_Assignments{}
    , m_InitAssignments{}
    , m_Time{}
    , m_RootFinders{}
    , m_NumRootFinders{}
{
  allocate();
}


SolverHelper::~SolverHelper()
{
  delete m_Values;
  delete m_Triggers;
  delete m_EvalStatus;
  
  delete [] m_Odes;
  delete [] m_Assignments;
  delete [] m_InitAssignments;

  for (int i=0; i < m_NumRootFinders; ++i)
    delete m_RootFinders[i];
  delete [] m_RootFinders;
}

void SolverHelper::allocate()
{
  int numOdes = m_OrigModel.getNumOdes();
  int numAssignments = m_OrigModel.getNumAssignments();
  int numInitAssignments = m_OrigModel.getNumInitAssignments();
  int numEvents = m_OrigModel.getNumEvents();
  int numVariables = m_OrigModel.getNumVariables();

  double* values(nullptr);
  int* triggers(nullptr);
  int* evalStatus(nullptr);

  Equation* odes(nullptr);
  Equation* assignments(nullptr);
  Equation* initAssignments(nullptr);

  try 
  {
    if (numEvents > 0)
      triggers = new int[numEvents];

    if (numVariables >0)
    {
      evalStatus = new int[numVariables];
      values = new double[numVariables];
    }

    
    if (numOdes > 0)
    {
      odes = new Equation[numOdes];
      for (int index = 0; index < numOdes; ++index)
      {
        odes[index].var_ = m_OrigModel.getOde(index)->getVariable();
     }
    }

    if (numAssignments >0)
    {
      assignments = new Equation[numAssignments];
      for (int index = 0; index < numAssignments; ++index)
      {
        assignments[index].var_ = m_OrigModel.getAssignment(index)->getVariable();
        assignments[index].wrapper_ = new ASTNode();
        assignments[index].wrapper_->setName(m_OrigModel.getVariable(assignments[index].var_)->getName().c_str());
      }
    }

    if (numInitAssignments > 0)
    {
      initAssignments = new Equation[numInitAssignments];

      for (int index = 0; index < numInitAssignments; ++index)
      {
        initAssignments[index].var_ = m_OrigModel.getInitAssignment(index)->getVariable();
        initAssignments[index].wrapper_ = new ASTNode();
        initAssignments[index].wrapper_->setName(m_OrigModel.getVariable(initAssignments[index].var_)->getName().c_str());
      }
    }
    
    m_Odes = odes;
    m_Assignments = assignments;
    m_InitAssignments = initAssignments;

    m_Values = values;
    m_Triggers = triggers;
    m_EvalStatus = evalStatus;
   
  } catch (std::exception& e)
  {
    delete [] odes;
    delete [] assignments;
    delete [] initAssignments;
    delete  triggers;
    delete  evalStatus;
    delete  values;
    throw;
  }
}
const double* SolverHelper::getValues() const
{
  return m_Values;
}

int SolverHelper::getNumRootFinders() const
{
  return m_NumRootFinders;
}

void SolverHelper::initStructures(const std::set<int>& varIndexes)
{
  /* copy any optimize the original model */
  m_Model.reset(m_OrigModel.clone());
  m_Model->simplify(varIndexes);
  /* index all ASTNode components in the model */
  m_Model->index();

  /* TODO:
   * 
   * 1. Search for order of the total set of algebraic rules and initial
   * assignment rules
   * 2. Search for order of algebraic rules that must be evaluated before odes 
   * 3. Search for */

  SolverHelperBuilder builder(*m_Model);
  builder.build();

  /* delete and reset previous root finders */
  for (int i=0 ; i< m_NumRootFinders; ++i)
    delete m_RootFinders[i];
  delete m_RootFinders;
  m_RootFinders = nullptr;

  std::vector<std::unique_ptr<ASTNode> >& rootEqs = builder.getRootFinders();
  m_NumRootFinders = rootEqs.size();

  if (m_NumRootFinders > 0)
  {
    m_RootFinders = new ASTNode*[m_NumRootFinders];
    for (int i=0; i < m_NumRootFinders; ++i)
    {
      m_RootFinders[i] = rootEqs[i].release();
      /* Indexes the node */
      m_Model->index(m_RootFinders[i]);
    }
  }
  

  for (int index = 0; index < m_Model->getNumOdes(); ++index)
  {
    m_Odes[index].math_ = m_Model->getOde(index)->getMath();
  }

  for (int index = 0; index < m_Model->getNumAssignments(); ++index)
  {
    m_Assignments[index].math_ = m_Model->getAssignment(index)->getMath();

    /* index assignments wrapper*/
    m_Model->index(m_Assignments[index].wrapper_);
  }

  for (int index = 0; index < m_Model->getNumInitAssignments(); ++index)
  {
    m_InitAssignments[index].math_ =  m_Model->getInitAssignment(index)->getMath();
    /* index init assignments wrapper*/
    m_Model->index(m_InitAssignments[index].wrapper_);
  }

}

void SolverHelper::initValues()
{ 

  /* 1. set initial values for all variables */
  for (auto& var : m_Model->getVariables())
  {
    if (var->hasValue())
      m_Values[var->getIndex()] = var->getValue();
    else m_Values[var->getIndex()] = 0;
  }

  /* 2. set initial values of event triggers */
  for (int i = 0; i < m_Model->getNumEvents(); ++i)
  {
    m_Triggers[i] = m_Model->getEvent(i)->getInitValue();
  }

  /* 3. evaluate initial assignments to setup initial values for variables */
  if (evaluateInitAssignments() != SATOOLS_SUCCESS)
  {
    throw SolverException("Error occurrs while evaluating initial assignments");
  }

  /* 4. update assignment and handle event triggers */
  if ( update(0.0) != SATOOLS_SUCCESS)
  {
    throw SolverException("Error occurrs while updating variable values at the intial condition");
  }
}


void SolverHelper::initValues(const std::set<int>& varIndexes, const double* values)
{
  /* 1. set initial values for all variables */
  for (auto& var : m_Model->getVariables())
  {
    if (var->hasValue())
      m_Values[var->getIndex()] = var->getValue();
    else m_Values[var->getIndex()] = 0;
  }


  int index{0};
  for (auto& vindex : varIndexes)
  {
    m_Values[vindex] = values[index++];   
  }

  /* 2. set initial values of event triggers */
  for (int i = 0; i < m_Model->getNumEvents(); ++i)
  {
    m_Triggers[i] = m_Model->getEvent(i)->getInitValue();
  }

  /* 3. evaluate initial assignments to setup initial values for variables */
  if (evaluateInitAssignments() != SATOOLS_SUCCESS)
  {
    throw SolverException("Error occurrs while evaluating initial assignments");
    return;
  }

  /* 4. update assignment and handle event triggers */
  if ( update(0.0) != SATOOLS_SUCCESS)
  {
    throw SolverException("Error occurrs while updating variable values at the intial condition");
  }
}

int SolverHelper::evaluateInitAssignments()
{
  /* set up evaluation status for variables having assignments or initial assignments */
  for (auto& variable : m_Model->getVariables())
  {
    if (variable->getType() == ASSIGNMENT_VARIABLE
        || variable->getInitEquationIndex() >=0)
      m_EvalStatus[variable->getIndex()] = EVAL_WAITING;
    else m_EvalStatus[variable->getIndex()] = EVAL_DONE;
  }

  /* evaluate initial assignments.  the evaluation procedure is recurssive, thus
   * any variable having an assignment will be evaluated also if required */

  m_Time = 0;
  for (int i=0; i < m_Model->getNumInitAssignments(); ++i) 
  {
    evaluate(m_InitAssignments[i].wrapper_); 
  }

  return m_Errors.getNumErrors()>0 ? SATOOLS_FAILURE :SATOOLS_SUCCESS;
}

int SolverHelper::update(const double tout)
{
  m_Time = tout;
  
  bool hasFiredTrigger = true;
  while( hasFiredTrigger )                                  /* until there is no firing trigger */
  {
    
    /* let all assignments wait for evaluation */
    for (int i=0; i < m_Model->getNumAssignments(); ++i)
    {
      m_EvalStatus[m_Assignments[i].var_] = EVAL_WAITING; 
    }

    /* execute pending event assignments */
    double time = m_EventExecutions.getNextExecutionTime();

    while (time >= 0 && time <= tout)           /* until there is no pending execution */
    {
      /* get the top event execution */
      EventExecution* ee = m_EventExecutions.pop();
      const OdeModelEvent* event = m_Model->getEvent(ee->getEventId());
      for (int i=0; i < event->getNumAssignments(); ++i) /* execute event assignments */
      {
        int varIndex = event->getVariableIndex(i);    

        /* evaluate rhs of the event assignment */
        m_Values[varIndex] = event->isEvaluateNow() ? ee->getValue(i) /* rhs was evaluated already */
                               : evaluate(event->getAssignment(i)); /* evaluate rhs */
          /* TODO: is ODE variables? */       
      }
      delete ee;

      /* get the time where the next event should be executed */
      time = m_EventExecutions.top()->getTime();

      /* any event assignment is executed may require some assignments have to
       * be re-evaluate.Let all assignments wait for evaluation */
      for (int i=0; i < m_Model->getNumAssignments(); ++i)
      {
        m_EvalStatus[m_Assignments[i].var_] = EVAL_WAITING; 
      }      
    }
    
    /* Evaluate triggers */
      bool hasFiring = false;
    for (int iEvent = 0; iEvent < m_Model->getNumEvents(); ++iEvent)
    {
      const OdeModelEvent* event = m_Model->getEvent(iEvent);
      int trigger = (int) evaluate(event->getTrigger());
      if (m_Triggers[iEvent] == 1 && trigger == 0) 
        /* a firing trigger need to be reset */
      { 
        if (!event->isPersist())
          /* this event not persistent, thus its execution has to be removed */
        {
          m_EventExecutions.removeById(iEvent);           
        }
      } else if (m_Triggers[iEvent] == 0 && trigger == 1) 
        /* a trigger fires */
      {
        hasFiring = true;
        /* evaluate all event content, create then add an event execution */
        EventExecution* ee = new EventExecution(iEvent, event->getNumAssignments());
        if (event->getPriority())
        {
          ee->setPriority(evaluate(event->getPriority()));
        }
        if (event->isEvaluateNow())
        {
          for (int index = 0; index < event->getNumAssignments(); ++index)
          {
            ee->setValue(index, evaluate(event->getAssignment(index)));
          }
        }
        if (event->getDelay())
        {
          ee->setTime( tout + evaluate(event->getDelay()));
        } else
        {
          ee->setTime(tout);
        }

        m_EventExecutions.add(ee);
      }

      /* update trigger values */
      m_Triggers[iEvent] = trigger;
     }
    if (!hasFiring)
      break;
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
  return m_Errors.getNumErrors() == 0 ? SATOOLS_SUCCESS : SATOOLS_FAILURE;
}


double SolverHelper::getNextEventExecutionTime() const
{
  return m_EventExecutions.getNextExecutionTime();
}

bool SolverHelper::isSolverResetRequired() const
{
  return true;
}

double SolverHelper::evaluate(const ASTNode *node) {
    double result{0};

    if ( !node ) 
    {
      m_Errors.addError(SOLVER_ERROR
          , ERROR_AST_EVALUATION
          , "evaluate: empty Abstract Syntax Tree (AST).");
        return 0;
    }
    if ( node->isUnknown() ) 
    {

      m_Errors.addError(SOLVER_ERROR
          , ERROR_AST_EVALUATION
          , "evaluate: unknown AST type.");
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
          const OdeModelVariable* variable = (const OdeModelVariable*)
                                                      node->getUserData(); 
          
          if(variable) 
          {
            int index = variable->getIndex();
            if (m_EvalStatus[index] == EVAL_DOING) 
            { 
              /* cyclic evalation occurred  */
              m_Errors.addError(SOLVER_ERROR
                  , ERROR_AST_EVALUATION
                  , "A cycle was found during mathematical evaluation of the model"); 

              result = 0; 
            } else if (m_EvalStatus[index] == EVAL_DONE ) /* value is there, just take it */
            { 
              result = m_Values[index]; 
            } else                              /* EVAL_WAIT, variable is has not been evaluated yet */
            {
              m_EvalStatus[index] = EVAL_DOING; 
              if (variable->getType() == ASSIGNMENT_VARIABLE) 
              { 
                result = evaluate(m_Assignments[variable->getEquationIndex()].math_);  
              } else if (variable->getInitEquationIndex() >=0 ) /* initial assignment */
              {
                result = evaluate(m_InitAssignments[variable->getInitEquationIndex()].math_);
              } else { 
                //should never happen
              } 

              m_Values[index] = result;
              m_EvalStatus[index] = EVAL_DONE;
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
              //should never happen
              m_Errors.addError(SOLVER_ERROR ,
                  ERROR_AST_EVALUATION 
                  , "No value found for AST_NAME %s . Defaults to"
                  " Zero to avoid program crash", name); 
              result = 0; 
              } 
            }
        }
        break;
    case AST_FUNCTION_DELAY:
        m_Errors.addError(SOLVER_ERROR
            , ERROR_AST_EVALUATION
            , "Solving ODEs with Delay is not implemented. "
            "Defaults to 0 to avoid program crash");
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
        m_Errors.addError(SOLVER_ERROR
            , ERROR_AST_EVALUATION
            , "Lambda can only be used in function definitions."
            " Defaults to 0 to avoid program crash");
        result = 0.0;
        break;
        /** FUNCTIONS: */
    case AST_FUNCTION:
        m_Errors.addError(SOLVER_ERROR
                , ERROR_AST_EVALUATION
                , "The function %s() has not been defined "
                "in the SBML input model or as an externally "
                "supplied function. Defaults to 0 to avoid "
                "program crash",
                node->getName());
        result = 0.0;
        break;
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
            m_Errors.addError(SOLVER_WARNING
                , ERROR_AST_EVALUATION
                , "The factorial is only implemented."
                "for integer values. The floor value of the "
                "passed float is used for calculation!");
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
BIO_NAMESPACE_END

