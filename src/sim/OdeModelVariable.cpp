/**
 @file OdeModelVariable.cpp
 @brief Implementation for OdeModelVariable class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "OdeModelVariable.h"

#include <cstdlib>

BIO_NAMESPACE_BEGIN

OdeModelVariable::OdeModelVariable(int type /* UNKNOWN_VARIABLE */ )
  : ModelBase()
    , m_Type(type)
    , m_Name("")
    , m_Index(-1)
    , m_Value(0)
    , m_HasValue(false)
    , m_EqIndex(-1) 
    , m_InitEqIndex(-1)
{
}

OdeModelVariable::OdeModelVariable(const OdeModelVariable& other)
  : ModelBase(other)
{
  m_Type = other.getType();
  m_Name = other.getName();
  if (other.hasValue())
  {
    m_HasValue = true;
    m_Value = other.getValue();
  }

  m_Index = other.getIndex();
  m_EqIndex = other.getEquationIndex();
  m_InitEqIndex = other.getInitEquationIndex();
}

OdeModelVariable& OdeModelVariable::operator=(const OdeModelVariable& other)
{
  if (this != &other)
  {
    this->ModelBase::operator=(other);

    m_Type = other.getType();
    m_Name = other.getName();
    if (other.hasValue())
    {
      m_HasValue = true;
      m_Value = other.getValue();
    }
    m_Index = other.getIndex();
    m_EqIndex = other.getEquationIndex();
    m_InitEqIndex = other.getInitEquationIndex();  
  }
  return *this;
}

OdeModelVariable* OdeModelVariable::clone() const
{
  return new OdeModelVariable(*this);
}

OdeModelVariable::~OdeModelVariable()
{
}

void OdeModelVariable::setType(const int type)
{
  m_Type = type;
}


int OdeModelVariable::getType() const
{
  return m_Type;
}


void OdeModelVariable::setName(const std::string& name)
{
  m_Name = name;
}


const std::string& OdeModelVariable::getName() const
{
  return m_Name;
}

bool OdeModelVariable::hasValue() const
{
  return m_HasValue;
}
void OdeModelVariable::setValue(const double value)
{
  m_Value = value;
  m_HasValue = true;
}

double OdeModelVariable::getValue() const
{
  return m_Value;
}

void OdeModelVariable::setIndex(const int index)
{
  m_Index = index;
}

int OdeModelVariable::getIndex() const
{
  return m_Index;
}

void OdeModelVariable::setEquationIndex(const int index)
{
  m_EqIndex = index;
}

int OdeModelVariable::getEquationIndex() const
{
  return m_EqIndex;
}

void OdeModelVariable::setInitEquationIndex(const int index)
{
  m_InitEqIndex = index;
}

int OdeModelVariable::getInitEquationIndex() const
{
  return m_InitEqIndex;
}
BIO_NAMESPACE_END

