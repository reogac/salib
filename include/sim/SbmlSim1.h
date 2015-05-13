/**
 @file SbmlSim1.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  SbmlSim1_INC
#define  SbmlSim1_INC

#include "OdeStruct.h"
#include "Solver.h"
#include "SimException.h"
//#include "SimResults.h"

#include "common/ErrorList.h"
#include <sbml/Model.h>

#include <vector>
#include <memory>

BIO_NAMESPACE_BEGIN

enum
{
  SIM_NOT_READY=0,
  SIM_READY,
  SIM_DOING,
  SIM_DONE
} SimStatus_t;


class SbmlSim1
{
  public:
    SbmlSim1(const Model& model);
    
    void setTime(const std::vector<double>& times);
    void setTime(const double duration, const int numSteps);
    void setSolverSettings(std::unique_ptr<SolverSettings>&& settings);
    void setSolverSettings(const SolverSettings& settings);
    void setVaryingVariables(std::vector<std::string>& variables);
       
    void initialize();
    void simulate();
    void simulateOne();
    void reset();
    void reset(const double* values);
    bool isDone() const;
  protected:
  private:
    void printResults(const double* values);

    int m_Status;
    int m_CurrentStep;
    std::unique_ptr<double []> m_Times;
    int m_NumSteps;
    std::unique_ptr<Solver> m_Solver;
    const Model& m_Model;
    std::unique_ptr<SolverSettings> m_Settings;
    std::vector<int> m_VaryingVariableIndexes;
    std::unique_ptr<OdeStruct>  m_OdeStruct;

//    std::unique_ptr<SimResults> m_Results;

    bool m_TimeFlag;
    bool m_SettingsFlag;
    bool m_VaryingFlag;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef SbmlSim1_INC  ----- */


