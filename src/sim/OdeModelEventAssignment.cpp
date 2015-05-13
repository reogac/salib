/**
 @file OdeModelEventAssignment.cpp
 @brief Implementation for OdeModelEventAssignment class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "OdeModelEventAssignment.h"

BIO_NAMESPACE_BEGIN

  OdeModelEventAssignment::OdeModelEventAssignment(const int varId, const ASTNode* assignment)
  : ModelComponentWithMath()
  , variableId_(varId)
  , assignment_(nullptr)
{
  if (assignment)
    assignment_.reset(assignment->deepCopy());
}

OdeModelEventAssignment::OdeModelEventAssignment(const OdeModelEventAssignment& other)
  : ModelComponentWithMath(other)
  , variableId_(other.variableId_)
  , assignment_(nullptr)
{
  if (other.assignment_)
    assignment_.reset(other.assignment_->deepCopy());

}

OdeModelEventAssignment& OdeModelEventAssignment::operator=(const OdeModelEventAssignment& other)
{
  if (this != &other)
  {
    this->ModelComponentWithMath::operator=(other);
    variableId_ = other.variableId_;
    if (other.assignment_)
      assignment_.reset(other.assignment_->deepCopy());
    else
      assignment_.reset(nullptr);
  }
  return *this;
}

OdeModelEventAssignment* OdeModelEventAssignment::clone() const
{
  return new OdeModelEventAssignment(*this);
}

void OdeModelEventAssignment::index(const ASTNodeIndexer& indexer)
{
  indexer.index(assignment_.get());
}
void OdeModelEventAssignment::simplify(const ASTNodeOptimizer& simplifier)
{ 
  simplifier.simplify(assignment_.get());
}
int OdeModelEventAssignment::getVariableId() const
{
  return variableId_;
}

ASTNode* OdeModelEventAssignment::getAssignment() const
{
  return assignment_.get();
}
BIO_NAMESPACE_END
