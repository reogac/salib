/**
 @file EventExecution.cpp
 @brief Implementation for EventExecution class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "EventExecution.h"
#include <cstdlib>

EventExecution::EventExecution(const int eventId, const int numAssignments)
  : m_EventId(eventId)
    , m_NumAssignments(numAssignments)
    , m_AssignmentValues{}
{
  if (numAssignments > 0)
    m_AssignmentValues = new double[numAssignments];
}

EventExecution::~EventExecution()
{
  delete m_AssignmentValues;
}
void EventExecution::setTime(const double time)
{
  m_Time = time;
}
double EventExecution::getTime() const
{
  return m_Time;
}

void EventExecution::setEventId(const int id)
{
  m_EventId = id;
}

int EventExecution::getEventId() const
{
  return m_EventId;
}

void EventExecution::setPriority(const double priority)
{
  m_HasPriority = true;
  m_Priority = priority;
}
bool EventExecution::hasPriority() const
{
  return m_HasPriority;
}

double EventExecution::getPriority() const
{
  return m_Priority;
}

double EventExecution::getValue(const int index) const
{
  return m_AssignmentValues[index];
}
void EventExecution::setValue(const int index, const double value)
{
  m_AssignmentValues[index] = value;
}
const double* EventExecution::getValues() const
{
  return m_AssignmentValues;
}

bool EventExecution::operator>(const EventExecution& other) const
{
  if (m_Time > other.getTime())
  {
    return true;
  } else if (m_Time < other.getTime())
  {
    return false;
  } else
  {
    return (rand() % 2 == 1); 
  }


}

