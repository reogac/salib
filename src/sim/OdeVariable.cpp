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
{

}

BIO_NAMESPACE_END



