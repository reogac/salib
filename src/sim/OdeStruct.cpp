/**
 @file OdeStruct.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */
#include "OdeStruct.h"

#include "SimException.h"

#include <cstring>

BIO_NAMESPACE_BEGIN

OdeStruct::OdeStruct()
  : m_Variables(nullptr)
  , m_NumVariables(0)
  , m_Odes(nullptr)
  , m_NumOdes(0)
  , m_Assignments(nullptr)
  , m_NumAssignments(0)
  , m_NumAlgEquations(0)
  , m_AlgEquations(nullptr)
  , m_NumInitAssignments(0)
  , m_InitAssignments(nullptr)
  , m_T0(0.0)
{
}

OdeStruct::~OdeStruct()
{
  for (unsigned int i=0; i<m_NumOdes; ++i)
    delete m_Odes[i];
  delete [] m_Odes;

  for (unsigned int i=0; i< m_NumVariables; ++i)
    delete m_Variables[i];
  delete [] m_Variables;

  for (unsigned int i=0; i<m_NumAssignments; ++i)
    delete m_Assignments[i];
  delete [] m_Assignments;

  for (unsigned int i=0; i<m_NumAlgEquations; ++i)
    delete m_AlgEquations[i];
  delete [] m_AlgEquations;

  for (unsigned int i=0; i<m_NumInitAssignments; ++i)
    delete m_InitAssignments[i];
  delete [] m_InitAssignments;

}

void OdeStruct::index(ASTNode* math)
{
  if (math == nullptr)
    return;
  if (math->getType() == AST_NAME)
  {
    for (unsigned int iV=0; iV<m_NumVariables; ++iV)
    {
      if (strcmp(math->getName(), m_Variables[iV]->getName().c_str()) == 0)
      {
        math->setUserData(m_Variables[iV]);
        break;
      }
    }
  }

  for (unsigned int iC = 0; iC<math->getNumChildren(); ++iC)
  {
    index(math->getChild(iC));
  }
}

void OdeStruct::index()
{
  for (unsigned int iO=0; iO<m_NumOdes; ++iO)
    index(m_Odes[iO]);
  for (unsigned int iA=0; iA<m_NumAssignments; ++iA)
    index(m_Assignments[iA]);
  for (unsigned int iIA=0; iIA< m_NumInitAssignments; ++iIA)
    index(m_InitAssignments[iIA]);
}

int OdeStruct::getVariableIndex(const std::string& name) const
{
  for (int iV=0; iV< m_NumVariables; ++iV)
  {
    if (name == m_Variables[iV]->getName())
      return m_Variables[iV]->getIndex();
  }
  std::ostringstream strstr;
  strstr << name << " has not been defined in the model";
  throw SimException(SIM_EXCEPTION_VARIABLE_NOT_FOUND, strstr.str());
}

BIO_NAMESPACE_END


