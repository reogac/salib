/**
 @file TestFrameworks.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  TestFrameworks_INC
#define  TestFrameworks_INC

#include <memory>

#include "sens/ModelInput.h"
#include "sens/ModelEvaluator.h"

class TestFramework
{
  public:   
    static TestFramework* instance(const std::string& name);
    static TestFramework* Gfunction(const double a[8]);
    static TestFramework* Ishigami(const double a, const double b);
  
    std::shared_ptr<reo::ModelEvaluator> createModel(void* userdata);

    const reo::ModelInputList* getInputList() const { return &m_InputList;}
    int getNumOutputs() const { return m_NumOutputs; };
  protected:
    TestFramework();
    virtual void init() = 0;

    virtual reo::ModelEvaluator* createModel_()  = 0;

    reo::ModelInputList m_InputList;
    int m_NumOutputs;
};

class PortfolioFrm : public TestFramework
{
  public:
    PortfolioFrm();

  protected:
    class TestModel : public reo::ModelEvaluator
    {
      public:
        TestModel();
        int solve(const double* inputs, double* outputs) const;
      private:
        static constexpr double Cs = 300;
        static constexpr double Ct = 500;
        static constexpr double Cj = 100;
    };

    void init();
    TestModel* createModel_();

};

class GfunctionModelFrm : public TestFramework
{
  public:
    GfunctionModelFrm(const double a[8]);
  protected:
    class TestModel : public reo::ModelEvaluator
    {
      public:
        TestModel(const double a[8]);
        int solve(const double* inputs, double* outputs) const;
      private:
        double a_[8];
    };

    void init();
    TestModel* createModel_();
  private:
    double a_[8];
};

class IshigamiFrm : public TestFramework
{
  public:
    IshigamiFrm(const double a, const double b);
  protected:
    class TestModel : public reo::ModelEvaluator
    {
      public:
        TestModel(const double a, const double b);
        int solve(const double* inputs, double* outputs) const;
      private:
      double a_;
      double b_;
    };

    void init();
    TestModel* createModel_();
  private:
    double a_;
    double b_;
};



#endif   /* ----- #ifndef TestFrameworks_INC  ----- */

