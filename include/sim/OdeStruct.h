/**
 @file OdeStruct.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  OdeStruct_INC
#define  OdeStruct_INC

#include "OdeVariable.h"
#include <sbml/math/ASTNode.h>

BIO_NAMESPACE_BEGIN

class OdeStruct
{
  public:
    OdeStruct();
    virtual ~OdeStruct();

    unsigned int getNumVariables() const { return m_NumVariables; }
    const OdeVariable* const* getVariables() const { return m_Variables; }
    const OdeVariable* getVariable(const int pos) const { return m_Variables[pos];}
    int getVariableIndex(const std::string& name) const;

    unsigned int getNumOdes() const { return m_NumOdes; }
    const ASTNode* const* getOdes() const { return m_Odes; }
    const ASTNode* getOde(const int pos) const { return m_Odes[pos]; }

    unsigned int getNumAssignments() const { return m_NumAssignments; }
    const ASTNode* const* getAssignments() const { return m_Assignments; }
    const ASTNode* getAssignment(const int pos) const { return m_Assignments[pos]; }

    unsigned int getNumInitAssignments() const { return m_NumInitAssignments; }
    const ASTNode* const* getInitAssignments() const { return m_InitAssignments; }
    const ASTNode* getInitAssignment(const int pos) const { return m_InitAssignments[pos]; }
   
    double getInitialTime() const { return m_T0;}

    void index();
    void index(ASTNode* math);

  private:
    friend class OdeStructBuilder;

    class EventAssignment {
      public:
        EventAssignment(const int varId, const ASTNode* assignment);
        ~EventAssignment();
      private:
        ASTNode* assignment_;
        int variableId_;
        friend class OdeStructBuilder;
    };

    class EventStruct
    {
      public:  
        EventStruct();
        ~EventStruct();
      private:
        ASTNode* m_Trigger;
        ASTNode* m_Delay;
        ASTNode* m_Priority;
        EventAssignment** m_Assignments;
        unsigned int m_NumAssignments;

        bool m_InitValue;
        bool m_Persist;
        bool m_EvaluateNow;
        friend class OdeStructBuilder;
    };

    OdeVariable** m_Variables;
    unsigned int m_NumVariables;  

    ASTNode** m_Odes;
    unsigned int m_NumOdes;
    
    ASTNode** m_Assignments;
    unsigned int m_NumAssignments;


    ASTNode** m_AlgEquations;
    unsigned int m_NumAlgEquations;

    unsigned int m_NumInitAssignments;
    ASTNode** m_InitAssignments;

    double m_T0;

    };

BIO_NAMESPACE_END



#endif   /* ----- #ifndef OdeStruct_INC  ----- */

