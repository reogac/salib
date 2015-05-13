/**
 @file TaskDesc.h
 @brief Header file for TaskDec class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */



#ifndef  TaskDesc_INC
#define  TaskDesc_INC

#include <cstdio>


class SaConfig;
class OdeModel;

class TaskDesc
{
  public:
    TaskDesc(const OdeModel& model);
    ~TaskDesc();

    int read(const char* filename);
    

  protected:
  private:
    const OdeModel& m_Model;

};
#endif   /* ----- #ifndef TaskDesc_INC  ----- */

