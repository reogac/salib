/**
 @file TaskDesc.cpp
 @brief Implementation for TaskDesc class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#include "TaskDesc.h"

#include "../CommonDefs.h"
#include "../OdeModel.h"
#include "../SimSettings.h"
#include "../SaConfig.h"

TaskDesc::TaskDesc(const OdeModel& model)
  :m_Model(model)
{

}

TaskDesc::~TaskDesc()
{

}


int TaskDesc::read(const char* filename)
{
  return SATOOLS_SUCCESS;
}


