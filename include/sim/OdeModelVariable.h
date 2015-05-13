/**
 @file OdeModelVariable.h
 @brief Header file for OdeModelVariable class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  OdeModelVariable_INC
#define  OdeModelVariable_INC

#include <string>

#include "ModelBase.h"

BIO_NAMESPACE_BEGIN

typedef enum
{
  UNKNOWN_VARIABLE =0, // unknown variable
  ODE_VARIABLE, //left hand side of a rate equation
  ASSIGNMENT_VARIABLE, //left hand side of an assignment
  ALGEBRAIC_VARIABLE, //variable from algebraic equations
  PARAMETER_VARIABLE, //model parameters
} VariableType_t;


class OdeModelVariable : public ModelBase
{
  public:
    OdeModelVariable(int type = UNKNOWN_VARIABLE);
    OdeModelVariable(const OdeModelVariable& other);
    OdeModelVariable& operator=(const OdeModelVariable& other);
    virtual ~OdeModelVariable();

    OdeModelVariable* clone() const;

    void setType(const int);
    int getType() const;

    void setName(const std::string& name);
    const std::string& getName() const;

    bool hasValue() const;
    void setValue(const double value);
    double getValue() const;

    void setIndex(const int index);
    int getIndex() const;

    void setEquationIndex(const int index);
    int getEquationIndex() const;

    void setInitEquationIndex(const int index);
    int getInitEquationIndex() const;

  protected:

    int m_Type; // type
    std::string m_Name; // a global unique name
    int m_Index; //variable index in the model
    double m_Value; // value
    bool m_HasValue; //has value/initial value?
    int m_EqIndex; //rate equation or assignment index
    int m_InitEqIndex; //initial equation
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef OdeModelVariable_INC  ----- */

