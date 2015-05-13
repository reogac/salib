/**
 @file SolverBase.h
 @brief Header file for SolverBase class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */



#ifndef  SolverBase_INC
#define  SolverBase_INC

#include <memory>
#include <set>


#include "SolverError.h"

BIO_NAMESPACE_BEGIN

class OdeModel;
class SolverSettings;
class SolverHelper;

class SolverBase
{
  public:
    /** @brief Constructor
     *
     * @param model Ode model 
     *
     * @param error An error list for reporting errors
     * that occurrs in the class member methods.  */

    SolverBase(const OdeModel& model);
    /**
     * @brief Destructor
     * */
    virtual ~SolverBase();

    /**
     * @brief Initializes the solver for the first time.  
     * Firstly it calls the virtual method createSolverHelper() to create a
     * helper then ask the helper to initialize its internal structures and data
     * to get ready for ODE solving. It then calls the virtual method
     * createSolverStructure(const SolverSettings* ) to construct the ODE
     * problem. */
    void init(const SolverSettings* settings, const std::set<int>& varIndexes);

    /**
     * \@brief Reinitializes the solver due to a change of varying parameter
     * list.
     *
     * This method ask the solver's helper to re-initialize its internal
     * structures and data due to ta request of changing the list of varying parameters.
     *
     * @param varIndexes Index values of variables/parameters
     * 
     * @throw A SolverException is thrown if the solver fails to initialize
     * */
    void reinit(const std::set<int>& varIndexes);

    /**
     * @brief Re-initializes solver due to solver configuration changes
     *
     * @param settings Solver configuration 
     * @throw A SolverException is thrown if the solver fails to re-initialize
     */
    void reinit(const SolverSettings* settings);

    /**
     * @brief Resets solver to initial condition and set some variables to given values
     * @throw A SolverException is thrown if the solver fails to reset its
     * initial condition
     * */
    void restart(const std::set<int>& varIndexes, const double* values);

    /**
     * @brief Resets solver to initial condition 
     * @throw A SolverException is thrown if the solver fails to reset its
     * intial condition
     */
    void restart();

    /**
     * @brief Integrates util a specific time 
     * 
     * 
     * @param endTime stop time
     * @retval SATOOLS_SUCCESS The solver successfully reach to the expected
     * time @param endTime
     * @retval SATOOL_FAILURE The solver fails to reach the expected time @param
     * endTime
     */
    virtual int solve(const double endTime) = 0;

    /**
     * @brief Gets current simulation result values for all model's variables*/
    const double* getValues() const;

    /**
     * @brief Returns a reference to the list of internal errors which occurs
     * during integration
     * */
    const SolverError& getErrors() const;
 protected:
    /**
     * @brief Creates solver internal data structures
     *
     * @param settings Solver configuration
     * */
    virtual int createSolverStructure(const SolverSettings* settings) = 0;

    /**
     * @brief Resets solver to initial condition 
     *
     * */
    virtual int resetSolverStructure() = 0;

    /**
     * @brief Creates a solver helper
     *
     * This method is called during solver initialization to create a helper.*/
    virtual SolverHelper* createSolverHelper() = 0;

    const OdeModel& m_Model;                    /**< A ODE model*/

    std::unique_ptr<SolverHelper> m_Helper;     /**< A solver helper  */


    SolverError   m_Errors;                         /**< An error list for reporting
                                                  errors occur within the class
                                                  member methods */
    
    double m_CurrentTime;                       /**< A time value indicates where solver currently reaches  */

  private:

};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef SolverBase_INC  ----- */

