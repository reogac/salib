/**
 @file EventExecution.h
 @brief This class represent a pending event execution
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#ifndef  EventExecution_INC
#define  EventExecution_INC

class EventExecution
{
  public:
    EventExecution(const int eventId, const int numAssigments); 
    ~EventExecution();

    void setTime(const double time);
    double getTime() const;

    void setEventId(const int id);
    int getEventId() const;

    void setPriority(const double priority);
    bool hasPriority() const;
    double getPriority() const;

    double getValue(const int index) const;
    void setValue(const int index, const double value);
    const double* getValues() const;
    
    bool operator> (const EventExecution& other) const;
  protected:
    EventExecution(const EventExecution& other) = delete;
    EventExecution& operator= (const EventExecution& other) = delete;

    int m_EventId;                               /* event identifier */
    int m_NumAssignments;
    double m_Time;                               /* time when should this event be excecuted */
    double m_Priority;                           /* priority of the event */
    bool m_HasPriority;                          /* does priority_ value have a meaning */
    double* m_AssignmentValues;                  /* RHS evaluated results of the event assignments */
};
#endif   /* ----- #ifndef EventExecution_INC  ----- */

