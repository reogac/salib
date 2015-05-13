/**
 @file CvodeSolver.h
 @brief Header file for CvodeSolver class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#ifndef  CvodeSolver_INC
#define  CvodeSolver_INC

#include <nvector/nvector_serial.h>

#include "SolverBase.h"
#include "CvodeHelper.h"

BIO_NAMESPACE_BEGIN

class CvodeSolver : public SolverBase
{
  public:
    CvodeSolver(const OdeModel& model);
    ~CvodeSolver();

    int solve(const double endTime);

  protected:
    int createSolverStructure(const SolverSettings* settings);
    int resetSolverStructure();
    CvodeHelper* createSolverHelper();
  private:
    void free();
    int helperUpdate(const double tout);

    N_Vector y_;
    N_Vector dy_;
    N_Vector atols_;
    bool isMoveOneInternalStep_;                /* is there any possibly firing
                                                   trigger at the begining of
                                                   the next integration round?
                                                   */ 
    void* cvode_;
  private:
    static void errorHandler(int error_code
        , const char* module, const char* function
        , char* msg, void* eh_data);

    void handleError(int error_code, const char* module
        , const char* function, char* msg);
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef CvodeSolver_INC  ----- */


