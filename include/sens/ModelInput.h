/**
 @file ModelInput.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  ModelInput_INC
#define  ModelInput_INC

#include <memory>
#include <string>
#include <vector>

#include "common/namespace.h"
#include "InputDist.h"

BIO_NAMESPACE_BEGIN

class ModelInput
{
  
  public:
    ModelInput(const std::string& name = std::string(""));
    virtual ~ModelInput();

    const std::string& getName() const;
    ModelInput& setName(std::string name);

    double getRef() const;
    ModelInput& setRef(const double ref);
    bool hasRef() const;

    int getCount() const;
    ModelInput& setCount(const int count);

    bool isLog() const;
    ModelInput& setLog(const bool log=true);

    const InputDist* getDist() const;
    ModelInput& setUniform(const double lower, const double upper);
    ModelInput& setNormal(const double mean, const double std);

    int getIndex() const;
    ModelInput& setIndex(const int index);

  protected:
    std::string m_Name;
    int m_Index;
    double m_Ref;
    bool   m_HasRef;
    int m_Count;
    bool   m_Log;

    InputDist* m_Dist;
  private:
    ModelInput(const ModelInput& other) = delete;
    ModelInput& operator=(const ModelInput& other) = delete;
};

class ModelInputList
{
  public:
    ModelInputList();
    ~ModelInputList();
//    void add(const ModelInput& input);
    ModelInput& add(const std::string& name);
    const ModelInput* get(const std::string& name) const;
    const ModelInput* get(const int index) const;
    int size() const;
  protected:
    std::vector<std::unique_ptr<ModelInput> > m_List;
    
};
BIO_NAMESPACE_END

#endif   /* ----- #ifndef ModelInput_INC  ----- */
