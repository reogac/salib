/**
 @file ModelOutput.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */
#include "ModelOutput.h"
#include "common/SaException.h"

BIO_NAMESPACE_BEGIN

ModelOutput::ModelOutput(const std::string& name /* "" */)
  : m_Name(name)
  , m_Features(nullptr)
{
}

ModelOutput::~ModelOutput()
{
  delete m_Features;
}
/* 
ModelOutput::ModelOutput(const ModelOutput& other)
  : m_Name(other.m_Name)
{
  if (other.m_Features)
    m_Features = other.m_Features->clone();
}

ModelOutput& ModelOutput::operator=(const ModelOutput& other)
{
  if (this != &other)
  {    
    delete m_Features;
    m_Name = other.m_Name;
    if (other.m_Features)
      m_Features = other.m_Features->clone();
  }
  return *this;
}
*/

void ModelOutput::setName(const std::string& name)
{
  m_Name = name;
}

const std::string& ModelOutput::getName() const
{
  return m_Name;
}

int ModelOutput::size() const
{
  int ret = 1;
  if (m_Features)
    ret = m_Features->size();
  return ret;
}

ModelOutputList::ModelOutputList()
{
}

ModelOutputList::~ModelOutputList()
{
}

ModelOutput& ModelOutputList::add(const std::string& name)
{
  bool isFound(false);
  for (auto & op : m_List)
  {
    if (op->getName() == name)
    {
      throw InputException("Dupplicated model output name");
    }
  }
  m_List.emplace_back(new ModelOutput(name));
  return *(m_List.back().get());
}


const ModelOutput* ModelOutputList::get(const std::string& name) const
{
  const ModelOutput* ret = nullptr;

  for (auto& op : m_List)
  {
    if (op->getName() == name)
    {
      ret = op.get();
      break;
    }
  }
  return ret;
}


const ModelOutput* ModelOutputList::get(const int index) const
{
  return m_List[index].get();
}

int ModelOutputList::size() const
{
  int ret = 0;
  for (auto& item : m_List)
    ret += item->size();

  return ret;  
}

BIO_NAMESPACE_END

