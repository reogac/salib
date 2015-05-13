/**
 @file SbmlOdeModel.h
 @brief Header file for SbmlOdeModel class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */
#ifndef  SbmlOdeModel_INC
#define  SbmlOdeModel_INC


#include "OdeModel.h"
#include <memory>
#include <sbml/SBMLTypes.h>

BIO_NAMESPACE_BEGIN

class SbmlOdeModel : public OdeModel
{
  public:
    SbmlOdeModel();
    ~SbmlOdeModel();

    SbmlOdeModel(const SbmlOdeModel& other);
    SbmlOdeModel& operator=(SbmlOdeModel& other);

    SbmlOdeModel* clone() const;

    void loadSbml(const char* filename);
    const Model* getSbmlModel() const;
  protected:
    std::shared_ptr<SBMLDocument>  m_Doc;
  private:

    class SbmlModelParsingHelper;
    void buildModel(SBMLDocument* doc);
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef SbmlOdeModel_INC  ----- */
