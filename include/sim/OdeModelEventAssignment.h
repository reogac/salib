/**
 @file OdeModelEventAssignment.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#ifndef  OdeModelEventAssignment_INC
#define  OdeModelEventAssignment_INC

#include <memory>

#include "ModelBase.h"
#include "ASTNode.h"

BIO_NAMESPACE_BEGIN

class OdeModelEventAssignment : public ModelComponentWithMath
{
  public:
    OdeModelEventAssignment(const int varId, const ASTNode* assignment);
    OdeModelEventAssignment(const OdeModelEventAssignment& other);
    OdeModelEventAssignment& operator=(const OdeModelEventAssignment& other);
    OdeModelEventAssignment* clone() const;
    
    void index(const ASTNodeIndexer& indexer);
    void simplify(const ASTNodeOptimizer& simplifier);

    int getVariableId() const;
    ASTNode* getAssignment() const;
  private:
    std::unique_ptr<ASTNode> assignment_;
    int variableId_;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef OdeModelEventAssignment_INC  ----- */
