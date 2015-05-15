/**
 @file OdeVariable.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */
#include "OdeVariable.h"

BIO_NAMESPACE_BEGIN
OdeVariable::OdeVariable(const OdeVariableType_t type)
  : m_Type(type)
  , m_Index(-1)
  , m_EqIndex(-1)
  , m_InitEqIndex(-1)
  , m_Value(0.0)
  , m_HasValue(false)
{

}

BIO_NAMESPACE_END



