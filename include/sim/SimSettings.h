/**
 @file SimSettings.h
 @brief Class for setting up simulation paramaters 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  SimSettings_INC
#define  SimSettings_INC

#include "common/namespace.h"

BIO_NAMESPACE_BEGIN

#define DEFAULT_SIM_DURATION 1
#define DEFAULT_SIM_NUMSTEPS 100

class SimSettings
{
  public:
    SimSettings();
    SimSettings(const int numSteps, const double timeValue, bool isDuration =
        true);
    SimSettings(const double* timeSteps, const int numSteps);
    SimSettings(const SimSettings& other);
    virtual ~SimSettings();
    
    int getNumSteps() const;
    const double* getTimeSteps() const;
    double getTimeStep(const int step) const;

  protected:
    double* m_TimeSteps; //time step values
    int     m_NumSteps;   // number of time steps (including the time zero)

  private:
    void setTimeSteps(const int numIntervals, const double interval);
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef SimSettings_INC  ----- */

