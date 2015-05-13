/**
 @file OdeModelEquation.cpp
 @brief i
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "OdeModelEquation.h"

BIO_NAMESPACE_BEGIN

OdeModelEquation::OdeModelEquation(const EquationType type /* ODE_EQUATION */)
  : ModelComponentWithMath()
    , m_Type{type}
    , m_Variable{-1}
    , m_Math{nullptr}
{
}

OdeModelEquation::~OdeModelEquation()
{
  delete m_Math;
}


OdeModelEquation::OdeModelEquation(const OdeModelEquation& other)
{
  m_Type = other.m_Type;
  m_Variable = other.m_Variable;
  m_Math = other.m_Math->deepCopy();
}

OdeModelEquation& OdeModelEquation::operator=(const OdeModelEquation& other)
{
  if (this != &other)
  {
    m_Type = other.m_Type;
    m_Variable = other.m_Variable;
    setMath(other.m_Math->deepCopy());
  }
  return *this;
}

OdeModelEquation* OdeModelEquation::clone() const
{
  return new OdeModelEquation(*this);
}

void OdeModelEquation::setVariable(const int id)
{
  m_Variable = id;
}

int OdeModelEquation::getVariable() const
{
  return m_Variable;
}

void OdeModelEquation::setMath(ASTNode* math)
{
  delete m_Math;
  m_Math = math;
}

const ASTNode* OdeModelEquation::getMath() const
{
  return m_Math;
}


OdeModelEquation* OdeModelEquation::optimize(const ASTNodeOptimizer& optimizer) const
{
  OdeModelEquation* ret = new OdeModelEquation();
  try
  {
    ret->m_Type = m_Type;
    ret->m_Variable = m_Variable;
    ret->m_Math = optimizer.copy(m_Math);
  } catch (...)
  {
    delete ret;
    throw;
  }
  return ret;
}

void OdeModelEquation::simplify(const ASTNodeOptimizer& simplifier)
{
  simplifier.simplify(m_Math);
}
void OdeModelEquation::index(const ASTNodeIndexer& indexer) 
{
  indexer.index(m_Math);
}
BIO_NAMESPACE_END
