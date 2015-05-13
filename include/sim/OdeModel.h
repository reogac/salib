/**
 @file OdeModel.h
 @brief Header file for OdeModel class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#ifndef  OdeModel_INC
#define  OdeModel_INC

#include <memory>
#include <vector>
#include <string>
#include <set>
#include <cstdio>

#include <sbml/SBMLTypes.h>

#include "ModelBase.h"
#include "OdeModelEvent.h"
#include "OdeModelVariable.h"
#include "OdeModelEquation.h"

BIO_NAMESPACE_BEGIN

class OdeModel : public ModelBase
{
  public:
    OdeModel();
    virtual ~OdeModel();

    OdeModel(const OdeModel& other);
    OdeModel& operator=(const OdeModel& other);

    OdeModel* clone() const;

    void load(const char* filename);

    void print() const;

    int getNumOdes() const;
    int getNumVariables() const;
    int getNumAssignments() const;
    int getNumAlgebraicEquations() const;
    int getNumInitAssignments() const;
    int getNumEvents() const;
    
    const OdeModelEquation* getOde(const int index) const;
    const OdeModelEquation* getAssignment(const int index) const;
    const OdeModelEquation* getAlgebraicEquation(const int index) const;
    const OdeModelEquation* getInitAssignment(const int index) const;
    const OdeModelEvent* getEvent(const int index) const;
    const OdeModelVariable* getVariable(const int index) const;
    int getVariableIndex(const std::string& name) const;

    const std::vector<std::unique_ptr<OdeModelEquation> >& 
      getOdes() const;
    const std::vector<std::unique_ptr<OdeModelEquation> >& 
      getAssignments() const;
    const std::vector<std::unique_ptr<OdeModelEquation> >& 
      getInitAssignments() const;
    const std::vector<std::unique_ptr<OdeModelEquation> >& 
      getAlgebraticEquations() const;
    const std::vector< std::unique_ptr<OdeModelVariable> >& 
      getVariables() const;
    const std::vector<std::unique_ptr<OdeModelEvent> >&
      getEvents() const;

    OdeModel* optimize(const std::set<int>& varIndexes) const;
    void simplify(const std::set<int>& varIndexes);
    void index();
    void index(ASTNode* node) const;

  protected:

    std::vector< std::unique_ptr<OdeModelVariable> >  m_Variables;

    std::vector< std::unique_ptr<OdeModelEquation> > m_Odes;
    std::vector< std::unique_ptr<OdeModelEquation> > m_InitAssignments;
    std::vector< std::unique_ptr<OdeModelEquation> > m_Assignments;
    std::vector< std::unique_ptr<OdeModelEquation> > m_AlgebraicEquations;
    std::vector< std::unique_ptr<OdeModelEvent> > m_Events;

  private:
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef OdeModel_INC  ----- */
