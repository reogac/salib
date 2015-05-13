/**
 @file OdeModel.cpp
 @brief Implementation for OdeModel class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#include "OdeModel.h"

#include "common/CommonDefs.h"
#include "MathPrinter.h"
#include "ASTNodeOptimizer.h"

BIO_NAMESPACE_BEGIN

OdeModel::OdeModel()
  : ModelBase()
{
}

OdeModel::~OdeModel()
{
}

OdeModel::OdeModel(const OdeModel& other)
  : ModelBase(other)
{
  m_Variables.reserve(other.m_Variables.size());
  for (auto& variable : other.m_Variables)
  {
    m_Variables.emplace_back(variable->clone());
  }

  m_Odes.reserve(other.m_Odes.size());
  for (auto& eq : other.m_Odes)
  {
    m_Odes.emplace_back(eq->clone());
  }

  m_Assignments.reserve(other.m_Assignments.size());
  for (auto& eq : other.m_Assignments)
  {
    m_Assignments.emplace_back(eq->clone());
  }

  m_AlgebraicEquations.reserve(other.m_AlgebraicEquations.size());
  for (auto& eq : other.m_AlgebraicEquations)
  {
    m_AlgebraicEquations.emplace_back(eq->clone());
  }

  m_InitAssignments.reserve(other.m_InitAssignments.size());
  for (auto& eq : other.m_InitAssignments)
  {
    m_InitAssignments.emplace_back(eq->clone());
  }


  m_Events.reserve(other.m_Events.size());
  for (auto&event : other.m_Events)
  {
    m_Events.emplace_back(event->clone());
  }
}

OdeModel& OdeModel::operator=(const OdeModel& other)
{
  if (this != &other)
  {
    m_Variables.clear();
    m_Variables.reserve(other.m_Variables.size());
    for (auto& variable : other.m_Variables)
    {
      m_Variables.emplace_back(variable->clone());
    }

    m_Odes.clear();
    m_Odes.reserve(other.m_Odes.size());
    for (auto& eq : other.m_Odes)
    {
      m_Odes.emplace_back(eq->clone());
    }

    m_Assignments.clear();
    m_Assignments.reserve(other.m_Assignments.size());
    for (auto& eq : other.m_Assignments)
    {
      m_Assignments.emplace_back(eq->clone());
    }

    m_AlgebraicEquations.clear();
    m_AlgebraicEquations.reserve(other.m_AlgebraicEquations.size());
    for (auto& eq : other.m_AlgebraicEquations)
    {
      m_AlgebraicEquations.emplace_back(eq->clone());
    }

    m_InitAssignments.clear();
    m_InitAssignments.reserve(other.m_InitAssignments.size());
    for (auto& eq : other.m_InitAssignments)
    {
      m_InitAssignments.emplace_back(eq->clone());
    }

    m_Events.clear();
    m_Events.reserve(other.m_Events.size());
    for (auto&event : other.m_Events)
    {
      m_Events.emplace_back(event->clone());
    }
  }
  return *this;
}

OdeModel* OdeModel::clone() const
{
  return new OdeModel(*this);
}

void OdeModel::load(const char* filename)
{
}

void OdeModel::print() const
{
  if (m_Variables.size() <= 0)
    return;
  MathPrinter mathprinter;

  std::cout << "Model has " << m_Variables.size() << " parameter(s" << std::endl;
  
  for (auto& variable : m_Variables)
  {
    std::cout << variable->getName();
    if( variable->hasValue())
      std::cout << "\t\t\t" << variable->getValue();
    std::cout<< std::endl;
  }


  int index{};
  if (m_Odes.size()>0)
  {
    std::cout << std::endl;
    std::cout << m_Odes.size() << " ode(s):" << std::endl;
    for (auto& ode : m_Odes)
    {
      index = ode->getVariable();
      std::cout << "d(" << m_Variables[index]->getName().c_str() 
                << ")/dt = " << mathprinter.print(ode->getMath()).c_str() << std::endl;
    }
  }


  if (m_Assignments.size()>0)
  {
    std::cout<< std::endl;

    std::cout << m_Assignments.size() << " assignment rule(s):" << std::endl;
    for (auto& assignment : m_Assignments)
    {
      std::cout << m_Variables[index++]->getName().c_str()
                << " = "
                << mathprinter.print(assignment->getMath()).c_str() << std::endl;
    }
  }
}

int OdeModel::getNumOdes() const
{
  return m_Odes.size();
}

int OdeModel::getNumVariables() const
{
  return m_Variables.size();
}

int OdeModel::getNumAssignments() const
{
  return m_Assignments.size();
}

int OdeModel::getNumAlgebraicEquations() const
{
  return m_AlgebraicEquations.size();
}
int OdeModel::getNumInitAssignments() const
{
  return m_InitAssignments.size();
}

int OdeModel::getNumEvents() const
{
  return m_Events.size();
}

const OdeModelVariable* OdeModel::getVariable(const int index) const
{
  return m_Variables[index].get();
}

int OdeModel::getVariableIndex(const std::string& name) const
{
  int ret = -1;
  for (auto& variable : m_Variables)
  {
    if (variable->getName() == name)
    {
      ret = variable->getIndex();
      break;
    }
  }
  return ret;
}
const OdeModelEquation* OdeModel::getOde(const int index) const
{
  return m_Odes[index].get();
}

const OdeModelEquation* OdeModel::getAssignment(const int index) const
{
  return m_Assignments[index].get();
}

const OdeModelEquation* OdeModel::getAlgebraicEquation(const int index) const
{
  return m_AlgebraicEquations[index].get();
}

const OdeModelEquation* OdeModel::getInitAssignment(const int index) const
{
  return m_InitAssignments[index].get();
}


const OdeModelEvent* OdeModel::getEvent(const int index) const
{
  return m_Events[index].get();
}


const std::vector<std::unique_ptr<OdeModelEquation> >& 
  OdeModel::getOdes() const
{
  return m_Odes;
}

const std::vector<std::unique_ptr<OdeModelEquation> >& 
  OdeModel::getAssignments() const
{
  return m_Assignments;
}

const std::vector<std::unique_ptr<OdeModelEquation> >& 
  OdeModel::getInitAssignments() const
{
  return m_InitAssignments;
}

const std::vector<std::unique_ptr<OdeModelEquation> >& 
  OdeModel::getAlgebraticEquations() const
{
  return m_AlgebraicEquations;
}

const std::vector< std::unique_ptr<OdeModelVariable> >& 
  OdeModel::getVariables()  const
{
  return m_Variables;
}

const std::vector<std::unique_ptr<OdeModelEvent> >&
  OdeModel::getEvents() const
{
  return m_Events;
}

OdeModel* OdeModel::optimize(const std::set<int>& varIndexes) const
{
  OdeModel* ret = new OdeModel();
  try
  {
    /* 1. copy variables */
    for (auto& variable : m_Variables)
    {
      ret->m_Variables.emplace_back(new OdeModelVariable(*variable.get()));
    }
    
    ASTNodeOptimizer optimizer(ret->m_Variables, varIndexes);

    /* 2. copy odes */
    for (auto& ode : m_Odes)
    {
      ret->m_Odes.emplace_back(ode->optimize(optimizer));
    }

    /* 3. copy assignments */
    for (auto& assignment : m_Assignments)
    {
      ret->m_Assignments.emplace_back(assignment->optimize(optimizer));
    }

    /* 4. copy algebraic equations */

    for(auto& equation : m_AlgebraicEquations)
    {
      ret->m_AlgebraicEquations.emplace_back(equation->optimize(optimizer));
    }

    /* 5. copy init assignments */
    for (auto& assignment : m_InitAssignments)
    {
      ret->m_InitAssignments.emplace_back(assignment->optimize(optimizer));
    }

    /* 6 copy events */

    for (auto& event : m_Events)
    {
      ret->m_Events.emplace_back(event->optimize(optimizer));
    }

  } catch (...)
  {
    delete ret;
    throw;
  }
  return ret;
}

void OdeModel::simplify(const std::set<int>& varIndexes)
{
  ASTNodeOptimizer simplifier(m_Variables, varIndexes);

  /* 1. simplify odes */
  for (auto& ode : m_Odes)
  {
    ode->simplify(simplifier);
  }

  /* 2. simplify assignments */
  for (auto& assignment : m_Assignments)
  {
    assignment->simplify(simplifier);
  }

  /* 3. simplify algebraic equations */

  for(auto& equation : m_AlgebraicEquations)
  {
    equation->simplify(simplifier);
  }

  /* 4. simplify init assignments */
  for (auto& assignment : m_InitAssignments)
  {
    assignment->simplify(simplifier);
  }

  /* 5 simplify events */

  for (auto& event : m_Events)
  {
    event->simplify(simplifier);
  }
}

void OdeModel::index()
{
  ASTNodeIndexer indexer(m_Variables);
  for (auto&eq : m_Odes)
    eq->index(indexer);
  for (auto& eq : m_Assignments)
    eq->index(indexer);
  for (auto& eq : m_InitAssignments)
    eq->index(indexer);
  for (auto& eq : m_AlgebraicEquations)
    eq->index(indexer);
  for (auto& event : m_Events)
    event->index(indexer);
}

void OdeModel::index(ASTNode* node) const
{
  ASTNodeIndexer indexer(m_Variables);
  indexer.index(node);   
}

BIO_NAMESPACE_END

