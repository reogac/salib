/**
 @file ModelInput.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "ModelInput.h"
#include "common/SaException.h"

BIO_NAMESPACE_BEGIN


ModelInput::ModelInput(const std::string& name)
  : m_Name(name)
  , m_Index(-1)
  , m_Ref(0)
  , m_HasRef(false)
  , m_Count(10)
  , m_Log(false)
  , m_Dist(nullptr)
{
}

ModelInput::~ModelInput()
{
  delete m_Dist;
}
/*  
ModelInput::ModelInput(const ModelInput& other)
  : m_Name(other.m_Name)
  , m_Index(other.m_Index)
  , m_Ref(other.m_Ref)
  , m_HasRef(other.m_HasRef)
  , m_Count(other.m_Count)
  , m_Log(other.m_Log)
  , m_Dist(nullptr)
{
  if (other.m_Dist)
    m_Dist = other.m_Dist->clone();
}

ModelInput& ModelInput::operator=(const ModelInput& other)
{
  if (this != &other)
  {
    delete m_Dist;

    m_Name  = other.m_Name;
    m_Index = other.m_Index;
    m_Ref   = other.m_Ref;
    m_Count = other.m_Count;
    m_Log   = other.m_Log;

    if (other.m_Dist)
      m_Dist = other.m_Dist->clone();
  }
  return *this;
}
*/

const std::string& ModelInput::getName() const
{
  return m_Name;
}

ModelInput& ModelInput::setName(std::string name)
{
  m_Name = name;
  return *this;
}

double ModelInput::getRef() const
{
  return m_Ref;
}

ModelInput& ModelInput::setRef(const double ref)
{
  m_Ref = ref;
  return *this;
}

bool ModelInput::hasRef() const
{
  return m_HasRef;
}

int ModelInput::getCount() const
{
  return m_Count;
}

ModelInput& ModelInput::setCount(const int count)
{
  m_Count = count;
  return *this;
}

bool ModelInput::isLog() const
{
  return m_Log;
}

ModelInput& ModelInput::setLog(const bool log)
{
  m_Log = log;
  return *this;
}

const InputDist* ModelInput::getDist() const
{
  return m_Dist;
}
ModelInput& ModelInput::setUniform(const double lower, const double upper)
{
  delete m_Dist;
  m_Dist = new InputUniform(lower, upper);
  return *this;
}

ModelInput& ModelInput::setNormal(const double mean, const double std)
{
  delete m_Dist;
  m_Dist = new InputNormal(mean, std);
  return *this;
}
/*  
ModelInput& ModelInput::setDist(const InputDist& dist)
{
  delete m_Dist;
  m_Dist = dist.clone();
  return *this;
}
*/

int ModelInput::getIndex() const
{
  return m_Index;
}
ModelInput& ModelInput::setIndex(const int index)
{
  m_Index = index;
  return *this;
}


ModelInputList::ModelInputList()
{
}

ModelInputList::~ModelInputList()
{
}
/*  
void ModelInputList::add(const ModelInput& input)
{
  bool isFound = false;
  for (auto& inp : m_List)
  {
    if (inp.getName() == input.getName())
    {
      isFound =true;
      break;
    }
  }
  if (isFound)
  {
    throw InputException("Dupplicated model input name");
  }
  m_List.emplace_back(input);
}

*/

ModelInput& ModelInputList::add(const std::string& name)
{
  for (auto& inp : m_List)
  {
    if (inp->getName() == name)
    {
      throw InputException("Duplicate model input name");
    }
  }
  m_List.emplace_back(new ModelInput(name));
  return *(m_List.back().get());
}

const ModelInput* ModelInputList::get(const std::string& name) const
{
  const ModelInput* ret = nullptr;

  for (auto& inp : m_List)
  {
    if (inp->getName() == name)
    {
      ret = inp.get();
      break;
    }
  }
  return ret;
}

const ModelInput* ModelInputList::get(const int index) const
{
  return m_List[index].get();
}

int ModelInputList::size() const
{
  return m_List.size();
}

BIO_NAMESPACE_END

