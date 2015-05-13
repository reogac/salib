/**
  @file CvodeSolver1.h
  @brief 
  @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  CvodeSolver1_INC
#define  CvodeSolver1_INC

#include "InnerSolver.h"

#include <nvector/nvector_serial.h>

BIO_NAMESPACE_BEGIN

class Solver;

class CvodeSolver1 : public InnerSolver
{
  public:
    CvodeSolver1(Solver* wrapper);
    ~CvodeSolver1();

    double* getY();

    void solve(const double tout) override;
  private:
    void doInitialize() override;
    void doReset() override;
    static int f(double t, N_Vector y, N_Vector ydot, void* fdata);
    static void handleError(int code, const char* module, const char* function, char* msg, void* data);
    void free();

    N_Vector y_;
    N_Vector dy_;
    N_Vector atols_;
    void* cvode_;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef CvodeSolver1_INC  ----- */

