/**
 @file CvodeSettings.h
 @brief class to keep parameter settings for cvode solver
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
    CvodeSettings* clone() const;
    
    CvodeSettings& operator= (const CvodeSettings& other);
    
    CvodeSettings& setATol(const double atol);
    double getATol() const;

    CvodeSettings& setRTol(const double rtol);
    double getRTol() const;
    
    int getMaxInternalStep() const;
    CvodeSettings& setMaxInternalStep(const int num);

    int getMaxOrder() const;
    CvodeSettings& setMaxOrder(const int order);

  protected:
    double m_ATol;
    double m_RTol;
    int m_MaxInternalStep;
    int m_MaxOrder;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef CvodeSettings_INC  ----- */


