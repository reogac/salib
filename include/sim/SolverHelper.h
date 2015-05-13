/**
 @file SolverHelper.h
 @brief Header file for SolverHelper class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#ifndef  SolverHelper_INC
#define  SolverHelper_INC

#include <memory>
#include <string>
#include <set>

#include "ASTNode.h"
#include "EventExecution.h"
#include "EventExecutionList.h"
#include "common/namespace.h"

BIO_NAMESPACE_BEGIN

#define EVAL_DONE 0
#define EVAL_WAITING 1
#define EVAL_DOING 2

class OdeModel;
class SolverError;

class SolverHelper
{
  public:
    /**
     * @brief Constructor
     *
     * @param model Ode model
     *
     * @param errors An error list for reporting errors that occurrs within
     * class member methods
     * */
    SolverHelper(const OdeModel& model, SolverError& errors);

    /**
     * @brief Destructor
     * */
    virtual ~SolverHelper();

    /** 
     * @brief Allocates and initializes internal data structures in order to get
     * ready for math expression evaluation
     *
     * @param varIndexes Indexes of variables who should not be replaced by
     * their values in the process of math expression simplification.
     * */
    void initStructures(const std::set<int>& varIndexes);

    /**
     * @brief Gets the number of algebraic equations used for root finding */
    int getNumRootFinders() const;

    /**
     * @brief Sets up data values for the internal structures to reflect the
     * model's initial condition at time zero
     * */
    virtual void initValues();

    /**
     * @brief Updates remaining variables values after the solver reach to given
     * time.
     * This method is called by the solver in order to update values of the remaining
     * variables.
     *
     * @param time Current time
     * */
    virtual int update(const double time);

    /** @brief Sets upt data values for the internal data structures to reflect
     * the model's initial condition at time 'zero'.
     * Intial values of some variables are taken from the method inputs instead
     * of the model data.
     *
     * @param varIndexes Indexes of variables whose values should be used to set
     * up the initial condition at time 'zero'
     *
     * @param values Values of the variables
     * */
    virtual void initValues(const std::set<int>& varIndexes, const double* values);

    /**
     * @brief Exposes the values of variable values to public */
    const double* getValues() const;

    /**
     * @brief Gets the time where the next event assignment should be executed */
    double getNextEventExecutionTime() const;

    /**
     * @brief Returning if the solver needs reset due to data changes.
     *
     * Any variable change its value (following an event assignment) may
     * potentially requires the solver to be re-initialized
     * */
    virtual bool isSolverResetRequired() const;

  protected:
    /**
     * @brief Evaluates a math expression */
    virtual double evaluate(const ASTNode* math);

    const OdeModel& m_OrigModel;                /* The original ode model */
    SolverError& m_Errors;                          /* An error list */

    EventExecutionList m_EventExecutions;       /* An event execution list */

    std::unique_ptr<OdeModel> m_Model;          /* A copy of original ode model with math expression simplified */
    ASTNode** m_RootFinders;                    /* A list of algebraic equations used for root finders */
    int m_NumRootFinders;                       /* The number of root finder equations */

    double* m_Values;                           /* Current values of model variables */
    int* m_Triggers;                            /* Current trigger states of model events */
    double m_Time;                              /* Current time, used for evaluation of math expression */
    int* m_EvalStatus;                          /* Evaluation status for assignments */

    int evaluateInitAssignments();              
    

    struct Equation
    {
      int var_ = -1;                            /* Index of the LHS variable of the equation */
      ASTNode* wrapper_ = nullptr;              /* ASTNode wrapper for the variable*/
      const ASTNode* math_ = nullptr;           /* The math expression */
      ~Equation() { delete wrapper_; }
    };

    Equation* m_Assignments;                    /* List of assignment rules */
    Equation* m_Odes;                           /* List of Odes */
    Equation* m_InitAssignments;                /* List of initial assignment rules */
  private:
    void allocate();
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef SolverHelper_INC  ----- */


