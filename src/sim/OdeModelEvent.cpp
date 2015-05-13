/**
 @file OdeModelEvent.cpp
 @brief Implementation for OdeModelEvent class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "OdeModelEvent.h"

#include <sbml/SBMLTypes.h>

BIO_NAMESPACE_BEGIN
  
OdeModelEvent::OdeModelEvent()
  : ModelComponentWithMath()
  , m_Trigger(nullptr)
  , m_Delay(nullptr)
  , m_Priority(nullptr)
  , m_InitValue(false)
  , m_Persist(false)
  , m_EvaluateNow(true)
{
}
/*  
OdeModelEvent::OdeModelEvent(const Event& event)
{
}
*/

OdeModelEvent::~OdeModelEvent()
{
}


OdeModelEvent::OdeModelEvent(const OdeModelEvent& other)
  : ModelComponentWithMath()
  , m_InitValue(other.m_InitValue)
  , m_Persist(other.m_Persist)
  , m_EvaluateNow(other.m_EvaluateNow)
  , m_Trigger(other.m_Trigger->deepCopy())
  , m_Delay(other.m_Delay->deepCopy())
  , m_Priority(other.m_Priority->deepCopy())
{
  for (auto& assignment : other.m_Assignments)
  {
    m_Assignments.emplace_back(assignment->clone());
  }
}

OdeModelEvent& OdeModelEvent::operator=(const OdeModelEvent& other)
{
  if (this != &other)
  {
    this->ModelComponentWithMath::operator = (other);

    m_InitValue   = other.m_InitValue;
    m_Persist     = other.m_Persist;
    m_EvaluateNow = other.m_EvaluateNow;

    m_Trigger.reset(other.m_Trigger->deepCopy());
    m_Delay.reset(other.m_Delay->deepCopy());
    m_Priority.reset(other.m_Priority->deepCopy());

    m_Assignments.reserve(other.m_Assignments.size());
    for (auto& assignment : other.m_Assignments)
    {
      m_Assignments.emplace_back(assignment->clone());
    }
  }
  return *this;
}

OdeModelEvent* OdeModelEvent::clone() const
{
  return new OdeModelEvent(*this);
}

OdeModelEvent*OdeModelEvent::optimize(const ASTNodeOptimizer& optimizer)
{
  OdeModelEvent* ret = nullptr;
  return ret;
}


void OdeModelEvent::simplify(const ASTNodeOptimizer& simplifier)
{
  simplifier.simplify(m_Trigger.get());
  simplifier.simplify(m_Delay.get());
  simplifier.simplify(m_Priority.get());
  for (auto& assignment : m_Assignments)
  {
    assignment->simplify(simplifier);
  }
}
void OdeModelEvent::index(const ASTNodeIndexer& indexer)
{
  indexer.index(m_Trigger.get());
  indexer.index(m_Delay.get());
  indexer.index(m_Priority.get());
  for (auto& assignment : m_Assignments)
  {
    assignment->index(indexer);
  }
}

void OdeModelEvent::setTrigger(const ASTNode* trigger)
{
  m_Trigger.reset(trigger->deepCopy());
}

const ASTNode* OdeModelEvent::getTrigger() const
{
  return m_Trigger.get();
}

void OdeModelEvent::setDelay(const ASTNode* delay)
{
  m_Delay.reset(delay->deepCopy());
}

const ASTNode* OdeModelEvent::getDelay() const
{
  return m_Delay.get();
}


void OdeModelEvent::setPriority(const ASTNode* priority)
{
  if (priority)
    m_Priority.reset(priority->deepCopy());
}

const ASTNode* OdeModelEvent::getPriority() const
{
  return m_Priority.get();
}

int OdeModelEvent::getNumAssignments() const
{
  return m_Assignments.size();
}

int OdeModelEvent::getVariableIndex(const int assignmentId) const
{
  return m_Assignments[assignmentId]->getVariableId();
}

const ASTNode* OdeModelEvent::getAssignment(const int assignmentId) const
{
  return m_Assignments[assignmentId]->getAssignment();
}

bool OdeModelEvent::getInitValue() const
{
  return m_InitValue;
}

bool OdeModelEvent::isPersist() const
{
  return m_Persist;
}

bool OdeModelEvent::isEvaluateNow() const
{
  return m_EvaluateNow;
}

BIO_NAMESPACE_END
