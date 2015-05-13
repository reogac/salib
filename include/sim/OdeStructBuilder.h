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
    OdeStructBuilder(const Model& model);
    OdeStruct* build();
  private:
    std::unique_ptr<OdeVariable> createAssignmentVariable(const std::string& id);
    std::unique_ptr<OdeVariable> createOdeVariable(const std::string& id);
    std::unique_ptr<ASTNode> buildRateEquation(const std::string& cId);
    void setVariableValue(std::unique_ptr<OdeVariable>& variable, const bool hasValue, const double value);
    void scanAssignments();
    void transitions2Assignments();
    OdeStruct* buildStruct();
    void printOdeStruct(const OdeStruct* model) const;
    
    std::vector<std::unique_ptr<OdeVariable> > m_Variables;
    std::vector<std::unique_ptr<ASTNode> > m_Odes;
    std::vector<std::unique_ptr<ASTNode> > m_Assignments;
    std::vector<std::unique_ptr<ASTNode> > m_InitAssignments;

    ErrorList m_Errors;
    const Model& m_Model;
};
BIO_NAMESPACE_END

#endif   /* ----- #ifndef OdeStructBuilder_INC  ----- */


