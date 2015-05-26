/**
 @file SimResultRecorder.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  SimResultRecorder_INC
#define  SimResultRecorder_INC

#include "SimResults.h"

#include <memory>

BIO_NAMESPACE_BEGIN

class OdeStruct;
class SimResultRecorder
{
  public:
    SimResultRecorder(SimResults* results, const OdeStruct* model);
    void record(const double* values, const int step);
  private:
    SimResults* results_;
    std::unique_ptr<int[]> compartments_;
    std::unique_ptr<int[]> species_;
    std::unique_ptr<int[]> reactions_;
    std::unique_ptr<int[]> parameters_;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef SimResultRecorder_INC  ----- */

