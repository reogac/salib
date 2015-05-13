/**
 @file EventExecutionList.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  EventExecutionList_INC
#define  EventExecutionList_INC

#include <list>

#include "EventExecution.h"

class EventExecutionList
{
  public:
    EventExecutionList();
    ~EventExecutionList();

    void add(EventExecution* ee);
    EventExecution* pop();
    const EventExecution* top() const;
    int size() const;
    void removeById(const int eventId);
    double getNextExecutionTime() const;
  private:
    EventExecutionList(const EventExecutionList& other) = delete;
    EventExecutionList& operator= (EventExecutionList& other) = delete;

    void insert(EventExecution* ee, std::list<EventExecution>::iterator& first
        , std::list<EventExecution>::iterator& last);

    std::list<EventExecution*> list_;
};

#endif   /* ----- #ifndef EventExecutionList_INC  ----- */

