/**
 @file OdeModelEvent.h
 @brief this class represents model events
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#ifndef  OdeModelEvent_INC
#define  OdeModelEvent_INC


#include <memory>

#include "ASTNode.h"
#include "ASTNodeOptimizer.h"
#include "ModelBase.h"
#include "OdeModelEventAssignment.h"

BIO_NAMESPACE_BEGIN

class OdeModelEvent : public ModelComponentWithMath
{
  public:  
    OdeModelEvent();
    /* OdeModelEvent(const Event& event); */
    ~OdeModelEvent();
    OdeModelEvent(const OdeModelEvent& other);
    OdeModelEvent& operator=(const OdeModelEvent& other);
    OdeModelEvent* clone() const;

    OdeModelEvent* optimize(const ASTNodeOptimizer& optimizer);

    void index(const ASTNodeIndexer& indexer);
    void simplify(const ASTNodeOptimizer& simplifier);

    void setTrigger(const ASTNode* trigger);
    const ASTNode* getTrigger() const;

    void setDelay(const ASTNode* delay);
    void setDelay(std::unique_ptr<ASTNode> delay);
    const ASTNode* getDelay() const;

    void setPriority(const ASTNode* priority);
    const ASTNode* getPriority() const;

    int getNumAssignments() const;
    int getVariableIndex(const int assignmentId) const;
    const ASTNode* getAssignment(const int assignmentId) const;

    bool getInitValue() const;
    bool isPersist() const;
    bool isEvaluateNow() const;

  protected:

    std::unique_ptr<ASTNode> m_Trigger;
    std::unique_ptr<ASTNode> m_Delay;
    std::unique_ptr<ASTNode> m_Priority;
    std::vector< std::unique_ptr<OdeModelEventAssignment> > m_Assignments;

    bool m_InitValue;
    bool m_Persist;
    bool m_EvaluateNow;
};

BIO_NAMESPACE_END
#endif   /* ----- #ifndef OdeModelEvent_INC  ----- */

