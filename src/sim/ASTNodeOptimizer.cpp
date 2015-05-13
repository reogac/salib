/**
 @file ASTNodeOptimizer.cpp
 @brief implementation for ASTNodeOptimizer class 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "ASTNodeOptimizer.h"

#include "OdeModelVariable.h"


BIO_NAMESPACE_BEGIN

ASTNodeOptimizer::ASTNodeOptimizer(const std::vector< std::unique_ptr<OdeModelVariable> >& variables
                                   , const std::set<int>& varIndexes)
  : variables_(variables)
    , varIndexes_(varIndexes)
{
}

ASTNode* ASTNodeOptimizer::copy(const ASTNode* node) const
{
  ASTNode* ret = node->deepCopy();
  /* TODO: optimize the math expression */
  return ret;
}

void ASTNodeOptimizer::simplify(ASTNode* node) const
{
  /* TODO: */
}

ASTNodeIndexer::ASTNodeIndexer(const std::vector< std::unique_ptr<OdeModelVariable> >& variables)
  : variables_(variables)
{

}

void ASTNodeIndexer::index(ASTNode* node) const
{
  if (! node)
    return;

  if (node->isName())
  {
    for (auto &variable : variables_)
    {
      if (strcmp(node->getName(), variable->getName().c_str()) == 0)
      {
        node->setUserData((void*) variable.get());
      }
    }
  }
  for (int iChild = 0; iChild < node->getNumChildren(); iChild++)
  {
    index(node->getChild(iChild));
  }
}

BIO_NAMESPACE_END

