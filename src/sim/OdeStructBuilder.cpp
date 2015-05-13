/**
 @file OdeStructBuilder.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "OdeStructBuilder.h"

#include <iostream>

BIO_NAMESPACE_BEGIN

OdeStructBuilder::OdeStructBuilder(const Model& model)
  : m_Model(model)
{
}

std::unique_ptr<OdeVariable> OdeStructBuilder::createAssignmentVariable(const std::string& id)
{
  std::unique_ptr<OdeVariable> variable(nullptr);
  /* 
  for (unsigned int iA =0; iA<m_Model.getNumAssignments(); ++iA)
  {
    const Rule& rule = m_Model.getAssignment(iA);
    if (id == rule.getVariable())
    {
      variable.reset(new OdeVariable(ODE_ASSIGNMENT_VARIABLE));
      variable->m_EqIndex = m_Assignments.size();
      m_Assignments.emplace_back(rule.getMath()->deepCopy());
      variable->m_Name = id;
      variable->m_Index = m_Variables.size();    
      std::cout << *m_Assignments.back() << std::endl;
      break;
    }
  }
  */
  return variable;
}

std::unique_ptr<OdeVariable> OdeStructBuilder::createOdeVariable(const std::string& id)
{

  std::unique_ptr<OdeVariable> variable(nullptr);
  /*
  for (unsigned int iR =0; iR<m_Model.getNumRateRules(); ++iR)
  {
    const Rule& rule = m_Model.getRateRule(iR);
    if (id == rule.getVariable())
    {
      variable.reset(new OdeVariable(ODE_ODE_VARIABLE));
      variable->m_EqIndex = m_Odes.size();
      m_Odes.emplace_back(rule.getMath()->deepCopy());
      variable->m_Name = id;
      variable->m_Index = m_Variables.size();    
      std::cout << *m_Odes.back() << std::endl;
      break;
    }
  }
  */
  return variable;
}


std::unique_ptr<ASTNode> OdeStructBuilder::buildRateEquation(const std::string& cId)
{
  std::unique_ptr<ASTNode> ode(nullptr);
  std::unique_ptr<ASTNode> rate(nullptr);
  /*
  for (unsigned int iT = 0; iT<m_Model.getNumTransitions(); ++iT)
  {
    const Transition& t = m_Model.getTransition(iT);
    const ASTNode* tMath = t.getMath();
    const std::string& fromId = t.getFrom();
    const std::string& toId = t.getTo();
    if (cId == fromId)
    {
      if (toId.length() == 0
          || tMath->isName()
          || tMath->isNumber())
      {
        rate.reset(tMath->deepCopy());
      } else
      {
        rate.reset(ASTFactory::name(t.getId().c_str()));
      }

      if (!ode)
      {
        ode.reset(ASTFactory::node(AST_OP_NEG));
        ode->setChild(rate.release());
      } else
      {          
        std::unique_ptr<ASTNode> tmpMath(ASTFactory::node(AST_OP_MINUS));
        tmpMath->setChild(ode.release(),0);
        tmpMath->setChild(rate.release(),1);
        ode = std::move(tmpMath);
      }
    } else if (cId == toId)
    {
      if (fromId.length()==0
          || tMath->isName()
          || tMath->isNumber())
      {
        rate.reset(tMath->deepCopy());
      } else
      {
        rate.reset(ASTFactory::name(t.getId().c_str()));
      }

      if (!ode)
      {
        ode = std::move(rate); 

      } else
      {
        std::unique_ptr<ASTNode> tmpMath(ASTFactory::node(AST_OP_PLUS));
        tmpMath->setChild(ode.release(),0);
        tmpMath->setChild(rate.release(),1);
        ode = std::move(tmpMath);
      }
    }
  }
  */
  return ode;    
}


void OdeStructBuilder::setVariableValue(std::unique_ptr<OdeVariable>& variable, const bool hasValue, const double value)
{
  /*
  bool found = false;
  for (unsigned int iIA=0; iIA<m_Model.getNumInitAssignments(); ++iIA)
  {
    const Rule& rule = m_Model.getInitAssignment(iIA);
    if (rule.getVariable() == variable->m_Name)
    {
      found = true;
      variable->m_InitEqIndex = m_InitAssignments.size();
      m_InitAssignments.emplace_back(rule.getMath()->deepCopy());
      break;
    }
  }
  if (hasValue)
  {
    variable->m_Value = value;
  } else
  {
    if (!found)
    {
      m_Errors.add(
          new SimError(SIM_WARNING_VARIABLE_NO_VALUE
              , formatMessage("Neither a value nor an init assignment rule is found for '%s', set its value to zero", variable->m_Name.c_str())
              , LEVEL_WARNING));
    }
    variable->m_Value=0.0;
  }
  */
}


void OdeStructBuilder::scanAssignments()
{

}

OdeStruct* OdeStructBuilder::build()
{
  /*
  //1. Get all compartments and their odes
  m_Errors.clear();
  std::cout << "Building ode structures " << std::endl;
  for (unsigned int iC=0; iC<m_Model.getNumCompartments(); ++iC)
  {
    const Compartment& c = m_Model.getCompartment(iC);
    const std::string& cId = c.getId();
    std::cout << std::endl << "compartment " << cId << std::endl;

    //first checks if the compartment has an assignment rule
    std::unique_ptr<OdeVariable> variable(createAssignmentVariable(cId));


    if (variable)
    {
      if (buildRateEquation(cId)
          || createOdeVariable(cId))
      {
        //blatant error, a compartment can't have both assignment rule and rate equation
        m_Errors.add(
            new SimError(SIM_ERROR_CONFLICT_RULE
              , formatMessage("Compartment '%s' has both rate equation and assignment rule", cId.c_str())) );
      }
      

    } else
    {
      variable = createOdeVariable(cId);
      if (variable)
      {
        if (buildRateEquation(cId))
        {
          m_Errors.add(
                       new SimError(SIM_ERROR_CONFLICT_RULE
                                    , formatMessage("Compartment '%s' has a rate equation and \
                                    a transition", cId.c_str())) );
        } else
        {
          setVariableValue(variable, c.isSetInitValue(), c.getInitValue());
        }
      } else {
        std::unique_ptr<ASTNode> ode(buildRateEquation(cId));
        if (!ode)                                   // the compartment has no ode
        {
          m_Errors.add(
            new SimError(SIM_WARNING_COMPARTMENT_WITHOUT_RATE
              , formatMessage("Compartment '%s' has no rate equation, set rate to zero", cId.c_str())
              , LEVEL_WARNING) );
          ode.reset(ASTFactory::number(0.0));
        }

        variable.reset(new OdeVariable(ODE_ODE_VARIABLE));
        variable->m_Name = cId;
        variable->m_Index = m_Variables.size();    
        variable->m_EqIndex = m_Odes.size();
        m_Odes.emplace_back(std::move(ode));
        setVariableValue(variable, c.isSetInitValue(), c.getInitValue());
      }
    }

    m_Variables.emplace_back(std::move(variable));
  }

  //2. Get all paramters
  for (unsigned int iP = 0; iP<m_Model.getNumParameters(); ++iP)
  {
    // checks if it is an assignment variable
    const Parameter& p = m_Model.getParameter(iP);
    std::unique_ptr<OdeVariable> variable(createAssignmentVariable(p.getId()));
    if (!variable)
    {
      // otherwise, it is just a constant parameter
      variable.reset(new OdeVariable(ODE_PARAMETER));
      variable->m_Name = p.getId();
      variable->m_Index = m_Variables.size();
      setVariableValue(variable,p.isSetValue(), p.getValue());
    }
    m_Variables.emplace_back(std::move(variable));
  }
*/
  //3. scan the (init)assignment rules to find any conflict or warning
  scanAssignments(); 

  //4. Turn transition rates into assignments
  transitions2Assignments();
  //5. now move the content to the ode struct
  if (m_Errors.getNumErrors() + m_Errors.getNumFatals() == 0 )
    return buildStruct();
  else
    throw std::move(m_Errors);
}

void OdeStructBuilder::transitions2Assignments()
{
  /*
  for (unsigned int iT=0; iT<m_Model.getNumTransitions(); ++iT)
  {
    const Transition& t = m_Model.getTransition(iT);
    if ( t.getFrom().length() == 0
         || t.getTo().length() == 0
         || t.getMath()->isNumber()
         || t.getMath()->isName() )
      continue;

    std::unique_ptr<OdeVariable> variable(new OdeVariable(ODE_ASSIGNMENT_VARIABLE));
    variable->m_Name = t.getId();
    variable->m_EqIndex = m_Assignments.size();
    m_Assignments.emplace_back(t.getMath()->deepCopy());
    variable->m_Index = m_Variables.size();
    m_Variables.emplace_back(std::move(variable));
  }
  */
}

OdeStruct* OdeStructBuilder::buildStruct()
{
  std::unique_ptr<OdeStruct> odeStruct(new OdeStruct());
  /*
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
  */
  return odeStruct.release();
}

void OdeStructBuilder::printOdeStruct(const OdeStruct* model) const
{
  /*
  for (unsigned int iV=0; iV<model->m_NumVariables; ++iV)
  {
    OdeVariable* v =  model->m_Variables[iV];
    if (v->m_Type == ODE_ASSIGNMENT_VARIABLE)
    {
      std::cout << v->m_Name << " = " << *model->m_Assignments[v->m_EqIndex] << std::endl;
    } else
    if (v->m_Type == ODE_ODE_VARIABLE)
    {
        std::cout << "d(" << v->m_Name << ")/dt = " << *model->m_Odes[v->m_EqIndex] << std::endl;
        if (v->m_InitEqIndex>=0)
        {
          std::cout << v->m_Name << "(0) = " << *model->m_InitAssignments[v->m_InitEqIndex] << std::endl;
        } else
        {
          std::cout << v->m_Name << "(0) = " << v->m_Value << std::endl;
        }
    } else
    {
      std::cout << v->m_Name << " = " << v->m_Value << std::endl;
    }
  }
  */
}
BIO_NAMESPACE_END


