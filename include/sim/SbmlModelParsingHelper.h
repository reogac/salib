/**
 @file SbmlModelParsingHelper.h
 @brief Header file for the SbmlModelParsingHelper class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  SbmlModelParsingHelper_INC
#define  SbmlModelParsingHelper_INC

#include "SbmlOdeModel.h"

#include <memory>
#include <string>
#include <vector>

BIO_NAMESPACE_BEGIN

class SbmlOdeModel::SbmlModelParsingHelper
{
  public:
    SbmlModelParsingHelper( const Model* model);

    void parse();

    std::vector< std::unique_ptr<ASTNode> >& getOdes();
    std::vector< std::unique_ptr<ASTNode> >& getAssignments();
    std::vector< std::unique_ptr<ASTNode> >& getAlgebraicEquations();
    std::vector< std::unique_ptr<OdeModelVariable> >& getOdeVariables();
    std::vector< std::unique_ptr<OdeModelVariable> >& getAlgebraicVariables();
    std::vector< std::unique_ptr<OdeModelVariable> >& getAssignmentVariables();
    std::vector< std::unique_ptr<OdeModelVariable> >& getParameters();
  private:
    void addRules();
    void inferRules();
    void addRemainingParameters();
    std::unique_ptr<ASTNode> odeFromReactions(const Species* species);

    std::unique_ptr<ASTNode> mathForSpeciesFromReaction(const SpeciesReference* sref, const char* reactionId, const char* speciesId);

  void addParameter(const std::string& name, bool hasValue, double value);

    const Model* m_Model;
    
    std::vector< std::unique_ptr<ASTNode> > m_Odes;
    
    std::vector< std::unique_ptr<ASTNode> > m_Assignments;
    
    std::vector< std::unique_ptr<ASTNode> > m_AlgebraicEquations;

    std::vector< std::unique_ptr<OdeModelVariable> > m_OdeVariables;
    
    std::vector< std::unique_ptr<OdeModelVariable> > m_AssignmentVariables;
    
    std::vector< std::unique_ptr<OdeModelVariable> > m_AlgebraicVariables;
    
    std::vector< std::unique_ptr<OdeModelVariable> > m_Parameters;

    std::vector<std::string> m_Errors;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef SbmlModelParsingHelper_INC  ----- */

