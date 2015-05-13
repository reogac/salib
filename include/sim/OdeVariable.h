/**
 @file OdeVariable.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  OdeVariable_INC
#define  OdeVariable_INC

#include "common/namespace.h"

#include <string>

BIO_NAMESPACE_BEGIN

typedef enum
{
  ODE_ODE_VARIABLE = 0,
  ODE_ASSIGNMENT_VARIABLE,
  ODE_PARAMETER,
  ODE_ALGEBRAIC_VARIABLE
} OdeVariableType_t;

class OdeVariable
{
  public:
    OdeVariable(const OdeVariableType_t type);
    int getType() const { return m_Type; }
    int getIndex() const { return m_Index; }
    int getEqIndex() const { return m_EqIndex; }
    int getInitEqIndex() const { return m_InitEqIndex; }
    bool hasInitAssignment() const  { return m_InitEqIndex>=0; }
    const std::string& getName() const  { return m_Name; }
    double getValue() const { return m_Value; }
  protected:
    int m_Type;
    int m_Index;                                /* variable index */
    int m_EqIndex;                              /* equation index */
    int m_InitEqIndex;                          /* init equation index */
    double m_Value;                             /* constant value or initialized value */
    
    std::string m_Name;                         /* an unique name */
  private:
    friend class OdeStructBuilder;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef OdeVariable_INC  ----- */


