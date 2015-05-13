/**
 @file ASTNodeOptimizer.h
 @brief helper class to copy and optimize ASTNode object
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#ifndef  ASTNodeOptimizer_INC
#define  ASTNodeOptimizer_INC

#include <vector>
#include <set>
#include <memory>

#include "common/namespace.h"

#include "ASTNode.h"
//#include <sbml/SBMLTypes.h>

BIO_NAMESPACE_BEGIN

class OdeModelVariable;
class ASTNodeOptimizer
{
  public:
    ASTNodeOptimizer(const std::vector< std::unique_ptr<OdeModelVariable> >& variables
                     , const std::set<int>& varIndexes);

    ASTNode* copy(const ASTNode* node) const;
    void simplify(ASTNode* node) const;
  private:
    void index(ASTNode* node) const;

    const std::vector< std::unique_ptr<OdeModelVariable> >& variables_;
    const std::set<int>& varIndexes_;
};

class ASTNodeIndexer
{
  public:
    ASTNodeIndexer(const std::vector< std::unique_ptr<OdeModelVariable> >& variables);
    void index(ASTNode* node) const;
  private:
    const std::vector< std::unique_ptr<OdeModelVariable> >& variables_;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef ASTNodeOptimizer_INC  ----- */


