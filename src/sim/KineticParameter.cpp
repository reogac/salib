/**
 @file KineticParameter.cpp
 @brief Implementation for KineticParameter class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "KineticParameter.h"

BIO_NAMESPACE_BEGIN

KineticParameter::KineticParameter(const std::string reaction, const std::string name)
  : OdeModelVariable(PARAMETER_VARIABLE)
    , m_Reaction(reaction)
    , m_LocalName(name)
{
  m_Name = KineticParameter::local2Global(reaction, name);
}

KineticParameter::~KineticParameter()
{

}

std::string KineticParameter::getReaction() const
{
  return m_Reaction;
}

std::string KineticParameter::getLocalName() const
{
  return m_LocalName;
}

std::string KineticParameter::local2Global(const std::string reaction, const std::string name)
{
  return name + "_of_" + reaction;
}


BIO_NAMESPACE_END
