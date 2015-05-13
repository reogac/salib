/**
 @file OdeModelEquation.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  OdeModelEquation_INC
#define  OdeModelEquation_INC

#include "ASTNode.h"
#include "ASTNodeOptimizer.h"
#include "ModelBase.h"

BIO_NAMESPACE_BEGIN

typedef enum {
  ODE_EQUATION = 0,
  ASSIGNMENT_EQUATION,
  ALGEBRAIC_EQUATION,
  INIT_ASSIGNMENT_EQUATION,
  EVENT_ASSIGNMENT_EQUATION
} EquationType;

class OdeModelEquation : public ModelComponentWithMath
{
  public:
    OdeModelEquation(const EquationType type = ODE_EQUATION);
    ~OdeModelEquation();
  
    OdeModelEquation(const OdeModelEquation& other);
    OdeModelEquation& operator=(const OdeModelEquation& other);
    OdeModelEquation* clone() const;

    OdeModelEquation* optimize(const ASTNodeOptimizer& optimizer) const;

    void index(const ASTNodeIndexer& indexer);
    void simplify(const ASTNodeOptimizer& simplifier);

    void setVariable(const int id);
    int getVariable() const;
    void setMath(ASTNode* math);
    const ASTNode* getMath() const;
  protected:

    int m_Type;
    int m_Variable;
    ASTNode* m_Math;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef OdeModelEquation_INC  ----- */
