/**
 @file SbmlModelParsingHelper.cpp
 @brief Implementation for SbmlModelParsingHelper class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "SbmlModelParsingHelper.h"

#include <sstream>

#include <sbml/SBMLTypes.h>

#include "common/SaError.h"
#include "common/CommonDefs.h"

#include "OdeModelVariable.h"
#include "KineticParameter.h"
#include "SolverException.h"

BIO_NAMESPACE_BEGIN

SbmlOdeModel::SbmlModelParsingHelper::SbmlModelParsingHelper(const Model* model)
  : m_Model(model)
{
}

std::vector< std::unique_ptr<ASTNode> >& SbmlOdeModel::SbmlModelParsingHelper::getOdes()
{
  return m_Odes;
}
std::vector< std::unique_ptr<ASTNode> >& SbmlOdeModel::SbmlModelParsingHelper::getAssignments()
{
  return m_Assignments;
}

std::vector< std::unique_ptr<ASTNode> >& SbmlOdeModel::SbmlModelParsingHelper::getAlgebraicEquations()
{
  return m_AlgebraicEquations;
}

std::vector< std::unique_ptr<OdeModelVariable> >& SbmlOdeModel::SbmlModelParsingHelper::getOdeVariables()
{
  return m_OdeVariables;
}

std::vector< std::unique_ptr<OdeModelVariable> >& SbmlOdeModel::SbmlModelParsingHelper::getAlgebraicVariables()
{
  return m_AlgebraicVariables;
}

std::vector< std::unique_ptr<OdeModelVariable> >& SbmlOdeModel::SbmlModelParsingHelper::getAssignmentVariables()
{
  return m_AssignmentVariables;
}

std::vector< std::unique_ptr<OdeModelVariable> >& SbmlOdeModel::SbmlModelParsingHelper::getParameters()
{
  return m_Parameters;
}
  
void SbmlOdeModel::SbmlModelParsingHelper::parse()
{
  m_Errors.clear();
  m_Odes.clear();
  m_Assignments.clear();
  m_AlgebraicEquations.clear();

  m_OdeVariables.clear();
  m_AssignmentVariables.clear();
  m_AlgebraicEquations.clear();
  m_Parameters.clear();

  addRules();
  inferRules();
  addRemainingParameters();
  if (m_Errors.size() > 0)
  {
    /* TODO: report errors */
    throw SolverException("dump exception");
  }
}


void SbmlOdeModel::SbmlModelParsingHelper::addRules()
{
  for (unsigned int iRule = 0; iRule < m_Model->getNumRules(); iRule++) 
  {
    const Rule* rule = m_Model->getRule(iRule);
    if (!rule->isSetMath())
    {
      /* create an error */
      std::ostringstream messagefm;
      messagefm << "Rule on line" << rule->getLine() << "has no mathematical expression";
      m_Errors.emplace_back(messagefm.str());
      continue; /* move to next rule */
    }

    std::unique_ptr<ASTNode> ruleMath(rule->getMath()->deepCopy());


    if (rule->isRate()) /* rate rule */
    {
      m_Odes.emplace_back(rule->getMath()->deepCopy());
      m_OdeVariables.emplace_back(new OdeModelVariable(ODE_VARIABLE));
      m_OdeVariables.back()->setName(rule->getVariable());
    } else if (rule->isAssignment())
    {
      m_Assignments.emplace_back( rule->getMath()->deepCopy() );
      m_AssignmentVariables.emplace_back(new OdeModelVariable(ASSIGNMENT_VARIABLE));

      m_AssignmentVariables.back()->setName( rule->getVariable() );
    } else /* algebraic rule */
    {
      /* TODO: here we create dummy algebraic variables.
       * In the future they should be identified */
      m_AlgebraicEquations.emplace_back(rule->getMath()->deepCopy());
      m_AlgebraicVariables.emplace_back(new OdeModelVariable(ALGEBRAIC_VARIABLE));

      m_AlgebraicVariables.back()->setName("dummy_name");
    }
  }
  
}

void SbmlOdeModel::SbmlModelParsingHelper::inferRules()
{

  /* step 1.  turns reaction flows (kinetic math) into assignmenet rules
   *  globalizes local kinetic parameters
   *  replaces local parameters by globalized parameters*/  
  for(unsigned int iReaction=0; iReaction< m_Model->getNumReactions(); iReaction++)
  {
    const Reaction* reaction = m_Model->getReaction(iReaction);
    
    std::unique_ptr<ASTNode> assignment(nullptr);

    if (reaction->isSetKineticLaw())
    {
      const KineticLaw* kinetic = reaction->getKineticLaw();
      if (kinetic->isSetMath()) /* the reaction is valid (has kinetic law with math) */
      {
        assignment.reset(kinetic->getMath()->deepCopy());

        for (unsigned int iParameter=0;  iParameter < kinetic->getNumParameters(); iParameter++)
        {
          /* NOTE: we do not check the return values of ASTNode member
           * functions 
           * */    
          const Parameter* localParameter = kinetic->getParameter(iParameter);

          m_Parameters.emplace_back(new KineticParameter(reaction->getId(), localParameter->getId()));

          std::unique_ptr<ASTNode> mathNode(new ASTNode(AST_NAME));
          mathNode->setName(m_Parameters.back()->getName().c_str());
          if (localParameter->isSetValue())
          {
            m_Parameters.back()->setValue(localParameter->getValue());
          }
          
          assignment->replaceArgument(localParameter->getId(), mathNode.get());
          mathNode.release();
        }
      }
    }

    if (assignment) /* the reaction is valid */
    {
      m_AssignmentVariables.emplace_back(new OdeModelVariable(ASSIGNMENT_VARIABLE));
      m_Assignments.emplace_back( std::move(assignment) );

      m_AssignmentVariables.back()->setName(reaction->getId());

    } else  /* invalid reaction */
    {

      /* create an error message */
      std::ostringstream messagefm;
      messagefm << "reaction " << reaction->getId().c_str();
      messagefm << " is invalid. It has no kinetic law or its kinetic law has no math element";
      m_Errors.emplace_back(messagefm.str());
    }
  }

  /* step 2. creates ode for species who are reactants/products of the model reactions 
   * */
  for (unsigned int iSpecies = 0; iSpecies < m_Model->getNumSpecies(); iSpecies++)
  {
    const Species* species = m_Model->getSpecies(iSpecies);

    std::unique_ptr<ASTNode> ode(odeFromReactions(species));
    if (ode)
    {
      m_Odes.emplace_back(std::move(ode));

      m_OdeVariables.emplace_back(new OdeModelVariable(ODE_VARIABLE));
      m_OdeVariables.back()->setName(species->getId());
    } else
    {
    /* NOTE: if no rate rule can be derived for the species, what should we do?
     * 1. set ode = 0
     * 2. consider the species as a constant parameter (we take this action)
     * 3. is it an algebraic variable?
     * */
    }
  }

}

/* *
 * If any of compartments, species, global parameters does not have a rate rule or
 * assignment rule, consider it as a constant parameter (whose value can not
 * be changed by rules)
 * NOTE: currently algebraic variables are not identified, they should be in the
 * future when we support model with algebraic rules
 * */

void SbmlOdeModel::SbmlModelParsingHelper::addRemainingParameters()
{
  double value = 0;
  bool hasValue = false;

  /* for all compartments */
  for (unsigned int iCompartment=0; iCompartment < m_Model->getNumCompartments(); ++iCompartment)
  {
    const Compartment* compartment = m_Model->getCompartment(iCompartment);
    hasValue = false;
    if (compartment->isSetSize())
    {
      hasValue = true;
      value = compartment->getSize();
    }
    addParameter(compartment->getId(), hasValue, value);
  }

  /* for all species */
  for (unsigned int iSpecies=0; iSpecies < m_Model->getNumSpecies(); ++iSpecies)
  {
    const Species* species = m_Model->getSpecies(iSpecies);
    hasValue = false;
    if ( species->isSetInitialConcentration())
    {      
      hasValue= true;
      value = species->getInitialConcentration();
    } else //there should be 'initial amount'
    {
      const Compartment* compartment = m_Model->getCompartment(species->getCompartment());
      if (compartment 
          && compartment->isSetSize()
          && compartment->getSize() > 0 )
      {
        hasValue = true;
        value = species->getInitialAmount() / compartment->getSize();
      }
    }

    addParameter(species->getId(), hasValue, value);
  }

  /* for all parameters */
  for (unsigned int iParameter=0; iParameter < m_Model->getNumParameters(); ++iParameter)
  {
    const Parameter* parameter = m_Model->getParameter(iParameter);
    if (parameter->isSetValue())
    {
      hasValue = true;
      value = parameter->getValue();
    }
    addParameter(parameter->getId(), hasValue, value);
  }
  
}

void SbmlOdeModel::SbmlModelParsingHelper::addParameter(const std::string& name, bool hasValue, double value)
{
  int iRule{};
  OdeModelVariable* foundVariable{};

  /*  scan the ode list */
  for (auto& variable : m_OdeVariables)
  {
    if (name == variable->getName())
    {
      /* there is a rate rule for this variable */
      foundVariable = variable.get();
      break;
    }   
  }

  /*  scan the assignment list */
  if (!foundVariable)
  {
    for (auto& variable : m_AssignmentVariables )
    {
      if (name == variable->getName())
      {
        foundVariable = variable.get();
        break;
      }
    }
  }
  /* TODO: should scan the algebraic list too! */


  if (!foundVariable )
  {
    /* The variable does not have a rule, consider it as a constant variable */
    m_Parameters.emplace_back(new OdeModelVariable(PARAMETER_VARIABLE));
    m_Parameters.back()->setName(name);
    foundVariable = m_Parameters.back().get();
  }

  /* set initial assignment for the variable */
  
  for (iRule=0; iRule < m_Model->getNumInitialAssignments(); iRule++)
  {
    if (name == m_Model->getInitialAssignment(iRule)->getSymbol())
    {
      /* there is an initial assignment for the variable */
      foundVariable->setInitEquationIndex(iRule);
      break;
    }
  }

  /* set value for the variable */
  if (hasValue)
    foundVariable->setValue(value);

}

/*  Create ode for species from reactions */
std::unique_ptr<ASTNode> SbmlOdeModel::SbmlModelParsingHelper::odeFromReactions(const Species* species)
{
  std::unique_ptr<ASTNode> ode(nullptr);

  for (unsigned int iReaction=0; iReaction < m_Model->getNumReactions(); iReaction++)
  {
    const Reaction* reaction = m_Model->getReaction(iReaction);

    /* scan the reactant list */
    for (unsigned int iReactant=0; iReactant < reaction->getNumReactants(); iReactant++)
    {
      const SpeciesReference* sref = reaction->getReactant(iReactant);
      if (species->getId() == sref->getSpecies()) /* the species is a reactant of the reaction */
      {
        std::unique_ptr<ASTNode> flow(mathForSpeciesFromReaction(sref, reaction->getId().c_str(), species->getId().c_str()));
        if (flow)                               /* have math expression */
        {
          if (ode) 
          {                                     /* ode already holds math */
            std::unique_ptr<ASTNode> node(new ASTNode());
            node->setCharacter('-');
            node->addChild(ode.get());
            ode.release();
            node->addChild(flow.get());
            flow.release();
            ode = std::move(node);
          } else                                /* empty ode */
          {
            ode.reset(new ASTNode());
            ode->setCharacter('-');
            ode->addChild(flow.get());
            flow.release();
          }
        }
      }
    }

    /* scan the production list */
    for (unsigned int iProduct=0; iProduct < reaction->getNumProducts(); iProduct++)
    {
      const SpeciesReference* sref = reaction->getProduct(iProduct);
      if (species->getId() == sref->getSpecies()) /* the species is a product of the reaction */
      {
        std::unique_ptr<ASTNode> flow(mathForSpeciesFromReaction(sref, reaction->getId().c_str(), species->getId().c_str()));
        if (flow)                               /* have math expression */
        {
          if (ode) 
          {                                     /* ode already holds math */
            std::unique_ptr<ASTNode> node(new ASTNode());
            node->setCharacter('+');
            node->addChild(ode.get());
            ode.release();
            node->addChild(flow.get());
            flow.release();
            ode = std::move(node);
          } else                                /* empty ode */
          {
            ode = std::move(flow);
          }
        }
      }
    }
  }
  /*  if ode is built, devide it by compartment */
  if (ode)                                      
  {
    std::unique_ptr<ASTNode> node(new ASTNode());
    std::unique_ptr<ASTNode> compartment(new ASTNode());
    compartment->setName(species->getCompartment().c_str());

    node->setCharacter('/');
    node->addChild(ode.get());
    ode.release();
    node->addChild(compartment.get());
    compartment.release();
    ode = std::move(node);
  }

  return std::move(ode);
}

/*  create math expression for species' reaction flow */
std::unique_ptr<ASTNode> SbmlOdeModel::SbmlModelParsingHelper::mathForSpeciesFromReaction(const SpeciesReference* sref, const char* reactionId, const char* speciesId)
{
  std::unique_ptr<ASTNode> node(new ASTNode());
  std::unique_ptr<ASTNode> reaction(new ASTNode());  
  std::unique_ptr<ASTNode> stoichiometry(nullptr);


  if (m_Model->getLevel() == 2)               /* SBML level 2 */
  {
    if (sref->isSetStoichiometry())             /* there is a stoichiometry value */
    {    
      if (sref->getStoichiometry() != 1)        /* only if its value is not one */
      {                                       
        stoichiometry.reset(new ASTNode());
        stoichiometry->setValue(sref->getStoichiometry());
      } 
    } else if (sref->isSetStoichiometryMath())  /* there is a math expression for stoichiometry */
    {
      stoichiometry.reset(sref->getStoichiometryMath()->getMath()->deepCopy());
    }  /* no stoichiometry found, set default value to one, thus do nothing*/ 

  } else                                      /* SBML level 3 */
  {
    if (sref->isSetId())                        /* stoichiometry is a global parameter */
    {
      stoichiometry.reset(new ASTNode());
      stoichiometry->setName(sref->getId().c_str());
    } else if (sref->isSetStoichiometry())      /* stoichiometry is a number value */
    {
      if (sref->getStoichiometry() != 1)
      {                                       
        stoichiometry.reset(new ASTNode());
        stoichiometry->setValue(sref->getStoichiometry());
      } 
    } else                                      /* no stoichiometry is found! error! */
    {
      /* SBML level 3 does not allow a species to have no stoichiometry in a reaction
       * Violation should have been detected earlier when reading the model
       * Just in case it reaches here, we assume the stoichiometry to take default
       * value 'one' */
    }
  }
  
  if (stoichiometry)                          /* stoichiometry is found */
  {
    reaction->setName(reactionId);
    node->setCharacter('*');
    node->addChild(stoichiometry.get());
    stoichiometry.release();
    node->addChild(reaction.get());
    reaction.release();
  } else
  {
    node->setName(reactionId);
  }
 
  return std::move(node);
}

BIO_NAMESPACE_END
