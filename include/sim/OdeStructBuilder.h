/**
 @file OdeStructBuilder.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  OdeStructBuilder_INC
#define  OdeStructBuilder_INC

#include "common/ErrorList.h"
#include <sbml/Model.h>
#include "OdeStruct.h"
//#include "SimError.h"

#include <memory>
#include <vector>

BIO_NAMESPACE_BEGIN

class OdeStructBuilder
{
  public:
    OdeStructBuilder(const Model& m);
    OdeStruct* build();
  private:
    void collectRules();
    void buildRateRules();
    void rescanEntities();
    void addInitAssignments();
    OdeStruct* buildOdeStruct();

    void addRateRule(const std::string& id, std::unique_ptr<ASTNode> eq);
    void addAssignment(const std::string& id, std::unique_ptr<ASTNode> eq);
    OdeVariable* findVariable(const std::string& id);
    OdeVariable* addParameter(const std::string& id, bool search = false);

    std::unique_ptr<ASTNode> rateFromReactions(const Species* species);
    std::unique_ptr<ASTNode> mathForSpeciesFromReaction(const SpeciesReference* sref
                                                        , const char* reactionId
                                                        , const char* speciesId);
    
    void printOdeStruct(const OdeStruct* model) const;
    
    std::vector<std::unique_ptr<OdeVariable> > m_Variables;
    std::vector<std::unique_ptr<ASTNode> > m_Odes;
    std::vector<std::unique_ptr<ASTNode> > m_Assignments;
    std::vector<std::unique_ptr<ASTNode> > m_AlgEquations;
    std::vector<std::unique_ptr<ASTNode> > m_InitAssignments;

    ErrorList m_Errors;
    const Model& m_Model;
};
BIO_NAMESPACE_END

#endif   /* ----- #ifndef OdeStructBuilder_INC  ----- */


