/**
 @file SolverHelperBuilder.cpp
 @brief Implementation for SolverHelperBuilder class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "SolverHelperBuilder.h"

#include "common/SaException.h"

BIO_NAMESPACE_BEGIN

static void Trigger2AlgebraicEquation(const ASTNode* node, std::vector< std::unique_ptr<ASTNode> >& equations);

SolverHelperBuilder::SolverHelperBuilder(const OdeModel& model)
  : model_(model)
{
}


std::vector< std::unique_ptr<ASTNode> > & SolverHelperBuilder::getRootFinders()
{
  return rootFinders_;
}
void SolverHelperBuilder::build()
{
  buildRootFinders();
}

void SolverHelperBuilder::buildRootFinders()
{
  rootFinders_.clear();
  for (auto& event : model_.getEvents())
  {
    Trigger2AlgebraicEquation(event->getTrigger(), rootFinders_);
  }
  
}

void Trigger2AlgebraicEquation(const ASTNode* node, std::vector< std::unique_ptr<ASTNode> >& equations)
{
  int childnum = node->getNumChildren();
  switch(node->getType()) {
    case AST_LOGICAL_AND:
    case AST_LOGICAL_NOT:
    case AST_LOGICAL_OR:
    case AST_LOGICAL_XOR:
      {
        for ( int i=0; i < childnum; ++i )
        {
          Trigger2AlgebraicEquation(node->getChild(i), equations);
        }
      }
      break;
    case AST_RELATIONAL_EQ:
    case AST_RELATIONAL_GEQ:
    case AST_RELATIONAL_GT:
    case AST_RELATIONAL_LEQ:
    case AST_RELATIONAL_LT :
      {
        /* A opt B ==>  A - B where 'opt' is a comparison operator*/
        equations.emplace_back(new ASTNode());
        equations.back()->setCharacter('-');
        std::unique_ptr<ASTNode> child0(node->getChild(0)->deepCopy());
        std::unique_ptr<ASTNode> child1(node->getChild(1)->deepCopy());
        
        equations.back()->addChild(child0.get());
        equations.back()->addChild(child1.get());
        child0.release();
        child1.release();
      }
      break;
    default:
      equations.emplace_back(node->deepCopy());
      break;
  }
}

BIO_NAMESPACE_END
