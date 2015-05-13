/**
 @file SbmlOdeModel.cpp
 @brief Implementation for SbmlOdeModel class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#include "SbmlOdeModel.h"
#include "SbmlModelParsingHelper.h"


#include <cstdlib>
#include <memory>

#include <sbml/SBMLTypes.h>

#include "OdeModelVariable.h"
#include "KineticParameter.h"
#include "OdeModelEquation.h"

#include "common/CommonDefs.h"

BIO_NAMESPACE_BEGIN

SbmlOdeModel::SbmlOdeModel()
  : OdeModel()
    , m_Doc(nullptr)
{
}

SbmlOdeModel::~SbmlOdeModel()
{
}

SbmlOdeModel::SbmlOdeModel(const SbmlOdeModel& other)
  : OdeModel(other)
  , m_Doc(other.m_Doc)
{
}

SbmlOdeModel& SbmlOdeModel::operator=(SbmlOdeModel& other)
{
  if (this != &other)
  {
    this->OdeModel::operator=(other);
    m_Doc = other.m_Doc;
  }
  return *this;
}

SbmlOdeModel* SbmlOdeModel::clone() const
{
  return new SbmlOdeModel(*this);
}

const Model* SbmlOdeModel::getSbmlModel() const
{
  if (m_Doc)
    return m_Doc->getModel();
  
  return nullptr;
}
void SbmlOdeModel::loadSbml(const char* filename)
{
  bool isFailed = true;
  SBMLReader reader;
  std::shared_ptr<SBMLDocument> doc(reader.readSBMLFromFile(filename));
  if (doc->getNumErrors() == 0)
  {
    if (doc->getLevel() <=2) 
    {//try to convert to level 2 version 4 Sbml format
      doc->setConsistencyChecksForConversion( LIBSBML_CAT_UNITS_CONSISTENCY, false);
      doc->setConsistencyChecksForConversion( LIBSBML_CAT_SBO_CONSISTENCY, false);

      if (doc->checkL2v4Compatibility() == 0)
        doc->setLevelAndVersion(2,4);
    }
  }

  if (doc->getNumErrors() >0)
  {
    //TODO: create error message
  } else
  {
    buildModel(doc.get());
    m_Doc = doc;
  }
}

void SbmlOdeModel::buildModel(SBMLDocument* doc)
{
  const Model* model = doc->getModel();
  
  SbmlModelParsingHelper parser(model);

  parser.parse();

  /* concatinate variable lists to make a list of all variables
   * update variable indexes*/

  std::vector< std::unique_ptr<OdeModelVariable> > variables;

  int index{};
  for ( auto& variable : parser.getOdeVariables() )
  {
    variables.emplace_back(std::move(variable));
    variables.back()->setIndex(variables.size() -1);
    variables.back()->setEquationIndex(index++);
  }

  index = 0;
  for (auto& variable : parser.getAssignmentVariables() )
  {
    variables.emplace_back(std::move(variable));
    variables.back()->setIndex(variables.size() - 1);
    variables.back()->setEquationIndex(index++);
  }

  index = 0;
  for ( auto& variable : parser.getAlgebraicVariables())
  {
    variables.emplace_back(std::move(variable));
    variables.back()->setIndex(variables.size() - 1);
    variables.back()->setEquationIndex(index++);
  }

  for (auto& variable : parser.getParameters() )
  {
    variables.emplace_back(std::move(variable));
    variables.back()->setIndex(variables.size() - 1);
  }

 
  /* now that we have parse the sbml model successfully, let's fill class
   * members
   * */
  std::vector< std::unique_ptr< OdeModelEquation> > odes, assignments, initAssignments, algebraicEquations;
  index = 0;
  for (auto& eq : parser.getOdes() )
  {
    odes.emplace_back( new OdeModelEquation(ODE_EQUATION) );
    odes.back()->setVariable(index++);
    odes.back()->setMath(eq.release());
  }

  for (auto& eq : parser.getAssignments() )
  {
    assignments.emplace_back( new OdeModelEquation(ASSIGNMENT_EQUATION) );
    assignments.back()->setVariable(index++);
    assignments.back()->setMath(eq.release());
  }

  for (auto& eq : parser.getAlgebraicEquations() )
  {
    algebraicEquations.emplace_back( new OdeModelEquation(ODE_EQUATION) );
    algebraicEquations.back()->setVariable(index++);
    algebraicEquations.back()->setMath(eq.release());
  }

  for (int i=0 ; i < model->getNumInitialAssignments(); ++i)
  {
    initAssignments.emplace_back( new OdeModelEquation(INIT_ASSIGNMENT_EQUATION) );
    initAssignments.back()->setMath( model->getInitialAssignment(i)->getMath()->deepCopy());
  }

  for ( auto& variable : variables )
  {
    int index = variable->getInitEquationIndex();
    if (index >= 0)
    {
      for (int i=0; i < model->getNumInitialAssignments(); ++i)
      {
        if (variable->getName() == model->getInitialAssignment(i)->getSymbol())
        {
          initAssignments[i]->setVariable(variable->getIndex());
          break;
        }
      }
    }

  }

  m_Variables = std::move(variables);
  m_Odes = std::move(odes);
  m_Assignments = std::move(assignments);
  m_AlgebraicEquations = std::move(algebraicEquations);
  m_InitAssignments = std::move(initAssignments);

}
BIO_NAMESPACE_END

