/**
 @file CvodeSettings.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  CvodeSettings_INC
#define  CvodeSettings_INC

#include "SolverSettings.h"

BIO_NAMESPACE_BEGIN

class CvodeSettings : public SolverSettings
{
  public:
    CvodeSettings();
    CvodeSettings(const CvodeSettings& other);
    CvodeSettings& operator= (const CvodeSettings& other);
    CvodeSettings* clone() const override;

    void setAError(const double error) { m_AError = error; }    
    double getAError() const  { return m_AError;}
    void setRError(const double error) { m_RError = error; }
    double getRError() const { return m_RError; }
  private:
    double m_AError;
    double m_RError;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef CvodeSettings_INC  ----- */

