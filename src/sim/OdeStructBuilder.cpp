/**
 @file OdeStructBuilder.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "OdeStructBuilder.h"

#include <iostream>

#include <sbml/SBMLTypes.h>

BIO_NAMESPACE_BEGIN

OdeStructBuilder::OdeStructBuilder(const Model& m)
  : m_Model(m)
{
}

/* 
 * Creates OdeStruct from Sbml Model
 * 
 * The process has two main steps. First, the Sbml model is parsed, its entities
 * are stored in the OdeStructBuilder member data. Second, a OdeStruct object
 * will be construct from these internal data and return to the caller.
 *
 * 1. Reads Sbml model and parse its content
 *
 *  1.1 Collects rate rules, assignment rules (and algebraic rules) from the
 *  model.
 *    For each found rule, its variable will be created then inserted to the
 *    variable list 'm_Variables'. Its math equation will be copied and pushed to a
 *    corresponding rule list (m_Odes, m_Assignments, m_AlgEquations).
 *
 *  1.2 Builds rate rule for each species. 
 *
 *    Rate equations can be inferred from reactions that the species involves.
 *
 *    - Each local parameter of reaction will be assigned a global name in the
 *    form of "reaction's id" + "$" + "parameter's id".  
 *
 *    - As a result the kinetic math of the reaction has to be modified to the
 *    new parameter identities.
 *
 *    - Kinetic math (reaction base rate) is inserted to assignment rule list
 *    'm_Assignments'.
 *    - A new assigment variable is created bearing the reaction id as name then
 *    inserted to the assignment list 'm_Assignments'
 *
 *    - Infered rate equations for species are inserted to the rate rule list
 *    'm_Odes'. Ode variables will be created for the species and inserted to
 *    the variable list 'm_Variables'
 *
 *  1.3 Rescans all species, compartments and global parameters
 *
 *      For each of compartments, species, global parameters, if it is found in
 *      the m_Variables, updates its value (if available). Otherwise if it is
 *      not found, creates a parameter variable then inserts to m_Variables
 *
 *  1.3 Adds initial assignments
 *
 *      For each initial assignment, copies then inserts its math expressions to
 *      m_InitAssignments. Updates the its index (of m_InitAssignments) for the
 *      coresponding variable in m_Variables 
 * 
 * 2. Builds an OdeStruct object
 * 
 * */

OdeStruct* OdeStructBuilder::build()
{
  collectRules();
  buildRateRules();
  rescanEntities();
  addInitAssignments();
  return buildOdeStruct();
}

void OdeStructBuilder::addRateRule(const std::string& id, std::unique_ptr<ASTNode> eq)
{
  m_Variables.emplace_back(new OdeVariable(ODE_ODE_VARIABLE));
  OdeVariable* v = m_Variables.back().get();
  v->m_EqIndex = m_Odes.size();
  m_Odes.emplace_back(std::move(eq));
  v->m_Name = id;
  v->m_Index = m_Variables.size()-1;    
}

void OdeStructBuilder::addAssignment(const std::string& id, std::unique_ptr<ASTNode> eq)
{
  m_Variables.emplace_back(new OdeVariable(ODE_ASSIGNMENT_VARIABLE));
  OdeVariable* v = m_Variables.back().get();
  v->m_EqIndex = m_Assignments.size();
  m_Assignments.emplace_back(std::move(eq));
  v->m_Name = id;
  v->m_Index = m_Variables.size()-1;    
}


OdeVariable* OdeStructBuilder::findVariable(const std::string& id)
{
  OdeVariable* ret(nullptr);
  for (auto& v : m_Variables)
    if (v->m_Name == id)
    {
      ret = v.get();
      break;
    }
  return ret;
}

OdeVariable* OdeStructBuilder::addParameter(const std::string& id, bool search /* false */)
{
  OdeVariable* v(nullptr);
  if (search)
    v = findVariable(id);

  if (!v) //if not search, or search has no result
  {
    m_Variables.emplace_back(new OdeVariable(ODE_PARAMETER));
    v = m_Variables.back().get();
  }

  v->m_Name = id;
  v->m_Index = m_Variables.size()-1;
  return v;
}

void OdeStructBuilder::collectRules()
{
  for (unsigned int iRule = 0; iRule < m_Model.getNumRules(); iRule++) 
  {
    const Rule* rule = m_Model.getRule(iRule);
    if (!rule->isSetMath())
    {
      /* TODO:create an error */
      continue; /* move to next rule */
    }

    std::unique_ptr<ASTNode> eq(rule->getMath()->deepCopy());
    const std::string& id = rule->getVariable();

    if (rule->isRate()) /* rate rule */
    {
      addRateRule(id, std::move(eq));
    } else if (rule->isAssignment())
    {
      addAssignment(id, std::move(eq));
    } else /* algebraic rule */
    {
      /* TODO: here we create dummy algebraic variables.
       * In the future they should be identified */
    }
  }

}

void OdeStructBuilder::buildRateRules()
{
  /* step 1.  turns reaction flows (kinetic math) into assignmenet rules
   *  globalizes local kinetic parameters
   *  replaces local parameters by globalized parameters*/  
  for(unsigned int iReaction=0; iReaction< m_Model.getNumReactions(); iReaction++)
  {
    const Reaction* reaction = m_Model.getReaction(iReaction);
    
    std::unique_ptr<ASTNode> assignment(nullptr);

    if (reaction->isSetKineticLaw())
    {
      const KineticLaw* kinetic = reaction->getKineticLaw();
      if (kinetic->isSetMath()) /* the reaction is valid (has kinetic law with math) */
      {
        assignment.reset(kinetic->getMath()->deepCopy());

        for (unsigned int iParameter=0;  iParameter < kinetic->getNumParameters(); iParameter++)
        {
          const Parameter* localParameter = kinetic->getParameter(iParameter);
          std::string id = reaction->getId() + "$" + localParameter->getId();
          OdeVariable* v = addParameter(id);
          if (localParameter->isSetValue())
          {
            v->setValue(localParameter->getValue());
          } else
          {
            //TODO: Creates a warning message. Every local parameters should have a value. 
            //Otherwise it is not possible to simulate the model. 
          }

          std::unique_ptr<ASTNode> mathNode(new ASTNode(AST_NAME));
          mathNode->setName(id.c_str());
          assignment->replaceArgument(localParameter->getId(), mathNode.get());
          mathNode.release();
        }
      }
    }

    if (assignment) /* the reaction is valid */
    {
      addAssignment(reaction->getId(), std::move(assignment));
    } else  /* invalid reaction */
    {

      /* TODO: create an error message */
      //      std::ostringstream messagefm;
      //      messagefm << "reaction " << reaction->getId().c_str();
      //      messagefm << " is invalid. It has no kinetic law or its kinetic law has no math element";
      //      m_Errors.emplace_back(messagefm.str());
    }
  }

  /* step 2. creates reaction rate for species who are reactants/products of the model reactions 
   * */
  for (unsigned int iSpecies = 0; iSpecies < m_Model.getNumSpecies(); iSpecies++)
  {
    const Species* species = m_Model.getSpecies(iSpecies);

    std::unique_ptr<ASTNode> rate(rateFromReactions(species));
    if (rate)
    {
      addRateRule(species->getId(), std::move(rate));
    } else
    {
    /* TODO: if no rate rule can be derived for the species, what should we do?
     * 1. set rate = 0
     * 2. consider the species as a constant parameter (we take this action)
     * 3. is it an algebraic variable?
     * */
    }
  }


}

std::unique_ptr<ASTNode> 
  OdeStructBuilder::mathForSpeciesFromReaction( const SpeciesReference* sref
                                                , const char* reactionId
                                                , const char* speciesId)
{
  std::unique_ptr<ASTNode> node(new ASTNode());
  std::unique_ptr<ASTNode> reaction(new ASTNode());  
  std::unique_ptr<ASTNode> stoichiometry(nullptr);


  if (m_Model.getLevel() == 2)               /* SBML level 2 */
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
      /* TODO:
       * SBML level 3 does not allow a species to have no stoichiometry in a reaction
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
 
  return node;
}

std::unique_ptr<ASTNode> OdeStructBuilder::rateFromReactions(const Species* species)
{
  std::unique_ptr<ASTNode> rate(nullptr);

  for (unsigned int iReaction=0; iReaction < m_Model.getNumReactions(); iReaction++)
  {
    const Reaction* reaction = m_Model.getReaction(iReaction);

    /* scan the reactant list */
    for (unsigned int iReactant=0; iReactant < reaction->getNumReactants(); iReactant++)
    {
      const SpeciesReference* sref = reaction->getReactant(iReactant);
      if (species->getId() == sref->getSpecies()) /* the species is a reactant of the reaction */
      {
        std::unique_ptr<ASTNode> flow(mathForSpeciesFromReaction(sref, reaction->getId().c_str(), species->getId().c_str()));
        if (flow)                               /* have math expression */
        {
          if (rate) 
          {                                     /* ode already holds math */
            std::unique_ptr<ASTNode> node(new ASTNode());
            node->setCharacter('-');
            node->addChild(rate.get());
            rate.release();
            node->addChild(flow.get());
            flow.release();
            rate = std::move(node);
          } else                                /* empty ode */
          {
            rate.reset(new ASTNode());
            rate->setCharacter('-');
            rate->addChild(flow.get());
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
          if (rate) 
          {                                     /* ode already holds math */
            std::unique_ptr<ASTNode> node(new ASTNode());
            node->setCharacter('+');
            node->addChild(rate.get());
            rate.release();
            node->addChild(flow.get());
            flow.release();
            rate = std::move(node);
          } else                                /* empty ode */
          {
            rate = std::move(flow);
          }
        }
      }
    }
  }
  /*  if ode is built, devide it by compartment */
  if (rate)                                      
  {
    std::unique_ptr<ASTNode> node(new ASTNode());
    std::unique_ptr<ASTNode> compartment(new ASTNode());
    compartment->setName(species->getCompartment().c_str());

    node->setCharacter('/');
    node->addChild(rate.get());
    rate.release();
    node->addChild(compartment.get());
    compartment.release();
    rate = std::move(node);
  }

  return std::move(rate);
}

void OdeStructBuilder::rescanEntities()
{

  /* for all compartments */
  for (unsigned int iCompartment=0; iCompartment < m_Model.getNumCompartments(); ++iCompartment)
  {
    const Compartment* compartment = m_Model.getCompartment(iCompartment);
    OdeVariable* v = addParameter(compartment->getId(), true);
    if (compartment->isSetSize())
    {
      v->setValue(compartment->getSize());
    }
  }

  /* for all species */
  for (unsigned int iSpecies=0; iSpecies < m_Model.getNumSpecies(); ++iSpecies)
  {
    const Species* species = m_Model.getSpecies(iSpecies);
    OdeVariable* v = addParameter(species->getId(), true);
    if ( species->isSetInitialConcentration())
    {      
      v->setValue(species->getInitialConcentration());
    } else //there should be 'initial amount'
    {
      const Compartment* compartment = m_Model.getCompartment(species->getCompartment());
      if (compartment 
          && compartment->isSetSize()
          && compartment->getSize() > 0 )
      {
        v->setValue(species->getInitialAmount() / compartment->getSize());
      }
    }
  }

  /* for all parameters */
  for (unsigned int iParameter=0; iParameter < m_Model.getNumParameters(); ++iParameter)
  {
    const Parameter* parameter = m_Model.getParameter(iParameter);

    OdeVariable* v = addParameter(parameter->getId(), true);
    if (parameter->isSetValue())
    {
      v->setValue( parameter->getValue());
    }
  }
}

void OdeStructBuilder::addInitAssignments()
{
  for (int iEq=0; iEq < m_Model.getNumInitialAssignments(); iEq++)
  {
    const InitialAssignment* ia = m_Model.getInitialAssignment(iEq);
    const std::string id = ia->getSymbol();
    OdeVariable* v = findVariable(id);
    if (v)
    {
      v->m_InitEqIndex = m_InitAssignments.size();
      m_InitAssignments.emplace_back(ia->getMath()->deepCopy());
    } else
    {
      //TODO: there should be an warning, invalid variable of the initial
      //assignment
    }
  }
}

OdeStruct* OdeStructBuilder::buildOdeStruct()
{

  std::unique_ptr<OdeStruct> odeStruct(new OdeStruct());

  odeStruct->m_NumOdes = m_Odes.size();
  if (m_Odes.size()>0)
  {
    std::unique_ptr<ASTNode*[]> odes(new ASTNode*[m_Odes.size()]);
    for (unsigned int iO =0; iO<m_Odes.size(); ++iO)
    {
      odes[iO] = m_Odes[iO].release();
    }
    odeStruct->m_Odes = odes.release();
  }

  odeStruct->m_NumAssignments = m_Assignments.size();
  if (m_Assignments.size() > 0)
  {
    std::unique_ptr<ASTNode*[]> assignments(new ASTNode*[m_Assignments.size()]);
    for (unsigned int iA =0; iA<m_Assignments.size(); ++iA)
    {
      assignments[iA] = m_Assignments[iA].release();
    }
    odeStruct->m_Assignments = assignments.release();
  }

  odeStruct->m_NumInitAssignments = m_InitAssignments.size();
  if (m_InitAssignments.size() > 0)
  {
    std::unique_ptr<ASTNode*[]> initAssignments(new ASTNode*[m_InitAssignments.size()]);
    for (unsigned int iIA=0; iIA<m_InitAssignments.size(); ++iIA)
    {
      initAssignments[iIA] = m_InitAssignments[iIA].release();
    }
    odeStruct->m_InitAssignments = initAssignments.release();
  }
  
  odeStruct->m_NumVariables = m_Variables.size();
  if (m_Variables.size() >0)
  {
    std::unique_ptr<OdeVariable*[]> variables(new OdeVariable*[m_Variables.size()]);
    int k=0;
    for (unsigned int iV = 0; iV<m_Variables.size(); ++iV)
    {
      if (m_Variables[iV]->m_Type == ODE_ODE_VARIABLE)
      {
        variables[k] = m_Variables[iV].get();
        variables[k]->m_Index = k++;
      }
    }

    for (unsigned int iV = 0; iV<m_Variables.size(); ++iV)
    {
      if (m_Variables[iV]->m_Type == ODE_ASSIGNMENT_VARIABLE)
      {
        variables[k] = m_Variables[iV].get();
        variables[k]->m_Index = k++;
      }  
    }

    for (unsigned int iV = 0; iV<m_Variables.size(); ++iV)
    {
      if (m_Variables[iV]->m_Type == ODE_PARAMETER)
      {
        variables[k] = m_Variables[iV].get();
        variables[k]->m_Index = k++;
      }  
      m_Variables[iV].release();
    }
    odeStruct->m_Variables = variables.release();
  }

  m_Variables.clear();
  m_Odes.clear();
  m_Assignments.clear();
  m_InitAssignments.clear();
  odeStruct->index();
  printOdeStruct(odeStruct.get());
  return odeStruct.release();
}

void OdeStructBuilder::printOdeStruct(const OdeStruct* model) const
{
  for (unsigned int iV=0; iV<model->m_NumVariables; ++iV)
  {
    OdeVariable* v =  model->m_Variables[iV];
    if (v->m_Type == ODE_ASSIGNMENT_VARIABLE)
    {
      char* tmp = SBML_formulaToString(model->m_Assignments[v->m_EqIndex]);
      std::cout << v->m_Name << " = " << tmp << std::endl;
      free(tmp);

    } else
    if (v->m_Type == ODE_ODE_VARIABLE)
    {
      char* tmp = SBML_formulaToString(model->m_Odes[v->m_EqIndex]);
      std::cout << "d(" << v->m_Name << ")/dt = " << tmp << std::endl;
      free(tmp);
      if (v->m_InitEqIndex>=0)
      {
        tmp = SBML_formulaToString(model->m_InitAssignments[v->m_InitEqIndex]); 
        std::cout << v->m_Name << "(0) = " << tmp << std::endl;
        } else
        {
          std::cout << v->m_Name << "(0) = " << v->m_Value << std::endl;
        }
    } else
    {
      std::cout << v->m_Name << " = " << v->m_Value << std::endl;
    }
  }
}
BIO_NAMESPACE_END


