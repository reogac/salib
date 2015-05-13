/**
 @file ModelOutput.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  ModelOutput_INC
#define  ModelOutput_INC

#include <memory>
#include <string>
#include <vector>

#include "common/namespace.h"

BIO_NAMESPACE_BEGIN

class ModelOutput
{
  public:
    ModelOutput(const std::string& name = "");
    ~ModelOutput();
    void setName(const std::string& name);
    const std::string& getName() const;

    class Features
    {
      public:
        virtual int size() const = 0;
        virtual Features* clone() = 0;
    };

    virtual int size() const;

  protected:
    std::string m_Name;
    Features* m_Features;

  private:
    ModelOutput(const ModelOutput& other) = delete;
    ModelOutput& operator=(const ModelOutput& other) = delete;
};

class ModelOutputList
{
  public:
    ModelOutputList();
    ~ModelOutputList();

    ModelOutput& add(const std::string& name);
    const ModelOutput* get(const std::string& name) const;
    const ModelOutput* get(const int index) const;
    virtual int size() const;
  protected:
    std::vector<std::unique_ptr<ModelOutput> > m_List;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef ModelOutput_INC  ----- */

