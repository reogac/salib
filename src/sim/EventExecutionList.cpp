/**
 @file EventExecutionList.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "EventExecutionList.h"

EventExecutionList::EventExecutionList()
{
}

EventExecutionList::~EventExecutionList()
{
  for (auto& ee : list_)
    delete ee;
}

void EventExecutionList::add(EventExecution* ee)
{
  /* TODO: binary search  */
  if (!ee)
    return;
  if (list_.size() == 0 )
  {
    list_.emplace_back(ee);
    return;
  }
  
  auto it = list_.begin();
  while (it!=list_.end())
  {
    if ((*ee) > *(*it))
    {
      list_.insert(it, ee);
      break;
    }
    ++it;
  }

  if (it == list_.end())
    list_.emplace_back(ee);

}

void EventExecutionList::insert(EventExecution* ee
    , std::list<EventExecution>::iterator& first
    , std::list<EventExecution>::iterator& last)
{

}

const EventExecution* EventExecutionList::top() const
{
  if (list_.size()>0)
    return list_.back();
  return nullptr;
}

EventExecution* EventExecutionList::pop()
{
  if (list_.size() > 0)
  {
    EventExecution* ret = list_.back();
    list_.pop_back();
    return ret;
  }

  return nullptr;
}

int EventExecutionList::size() const
{
  return list_.size();
}

void EventExecutionList::removeById(const int eventId)
{
  for (auto it = list_.begin(); it != list_.end();)
  {
    if ((*it)->getEventId() == eventId)
      it = list_.erase(it);
    else
      ++it;
  }
}
double EventExecutionList::getNextExecutionTime() const
{
  double ret = -1;
  if (list_.size()>0)
    ret = list_.back()->getTime();
  return ret;
}
