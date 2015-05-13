/**
 @file SbmlSim.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#ifndef  SbmlSim_INC
#define  SbmlSim_INC

#include <memory>

#include "OdeSim.h"
#include "SbmlResult.h"

BIO_NAMESPACE_BEGIN

class SbmlOdeModel;

class SbmlSim : public OdeSim
{
  public:
    SbmlSim(const SbmlOdeModel& model);
    ~SbmlSim();
    int simulate();
    const SbmlResult* getResult() const;
  private:
    std::unique_ptr<SbmlResult> result_;
   
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef SbmlSim_INC  ----- */
