/**
 @file OdeSim.h
 @brief Header file for OdeSim class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#ifndef  OdeSim_INC
#define  OdeSim_INC

#include <memory>
#include <set>
#include "common/SaError.h"

BIO_NAMESPACE_BEGIN

class OdeModel;
class SolverSettings;
class SolverBase;

class OdeSim
{
  public:
    /**
     * @brief Constructor
     *
     * @param model Ode model
     * */

    OdeSim(const OdeModel& model);
    /** 
     * @brief Destructor
     * */
    virtual ~OdeSim();

    /**
     * @brief Initializes the solver to get ready for simulation 
     * */
    void init();

    /**
     * @brief Calls solver to integrate for one time step 
     * */
    int simulateOne();

    /**
     * @brief Iteratively calls solver to simulate for all time steps 
     * */
    virtual int simulate();

    /**
     * @brief Returns true if the simulation has successfully reached to the last time step
     * */
    bool isCompleted() const;

    /**
     * @brief Changes solver configuration 
     * @param solverSettings input solver configuration.
     *
     * */
    OdeSim& setSolverSettings(const SolverSettings* solverSettings);

    /** 
     * @brief Sets up time steps for simulation 
     *
     * @param timestep A vector of time step values.
     * Values must be positive, unique and sorted in ascending order. (time step
     * 'zero' must not included.
     *
     * @throw a SolverException is thrown if input is invalid
     * */
    OdeSim& setTimeSteps(const std::vector<double>& timesteps);

    /** 
     * @brief Sets up time steps for simulation 
     *
     * @param duration a positive value represent simulation end time
     * @param numIntervals number of time steps not including the 'zero' time
     * step
     * @throw a SolverException is thrown if input is invalid
     * */
    OdeSim& setTimeStepsByDuration(const double duration, const int numSteps);

    /** 
     * @brief Sets up time steps for simulation 
     *
     * @param interval a positive value represents interval between two
     * consecutive time steps
     * @param numIntervals number of intervals. It is also the number of time
     * steps not including the zero time step.
     * @throw a SolverException is thrown if input is invalid
     * */
    OdeSim& setTimeStepsByInterval(const double interval, const int numIntervals);

    /**
     * @brief Sets which parameters whose values should be changed between simulations.
     *
     * This information helps to simplify (optimize) math expressions of the
     * model as the constant parameters can be replaced by their values 
     * 
     * @param indexes Indexes of variables */
    OdeSim& setVaryingParameters( const std::set<int>& indexes);

    /**
     * @brief Resets timestep and solver to initial condition 
     * */
    void restart();

    /**
     * @brief Resets solver to intial conditions and set initial values of
     * variables in the #m_VaryingVariables list
     *
     * @param values An array of variables values.
     * */
    void restart(const double* values);

    /** Returns the time steps where the solver has been successfully reached
      */
    int getCurrentStep() const;

  protected:
    /**
     * @brief Creates  a solver given its configuration in #m_SolverSettings.
     *
     * If one wants to develop his own ode solver, he should override this
     * method in order to choose the right solver for the simulator*/
    virtual SolverBase* createSolver(); 
    /**
     * @brief Checks if solver has been properly initialized
     *
     * @throw SolverException
     * */
    void checkReady();

    const OdeModel& m_Model;                    /**< A Ode model */

    std::unique_ptr<SolverSettings> m_SolverSettings; /**< A Solve configuration */

    std::unique_ptr<SolverBase> m_Solver;       /**< A numerical solver to solve
                                                  the Ode model */

    const double* m_SimValues;                  /**< Current values of all
                                                  variable/parameters of the
                                                  model  */
  
    std::vector<double> m_Timesteps;            /**< The simulation time step
                                                  values */

    std::set<int> m_VaryingParameters;          /**< An index list of variables
                                                   whose values are supposed to
                                                   be changed between simulation
                                                   runs */

  private:
    /**
     * @brief Creates a default solver configuration if it is not provided 
     * */
    void createDefaultSolverSettings();

    int m_CurrentStep;                          /**< Current step where the solver reaches */
    
    bool m_IsTimeChanged;                       /**< Has time step values got updated? */

    bool m_IsSolverChanged;                     /**< Has solver configuration got updated? */

    bool m_IsParameterChanged;                  /* Has the list of varying variables got updated? */

};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef OdeSim_INC  ----- */

