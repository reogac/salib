/**
 @file KineticParameter.h
 @brief Header file for KineticParameter class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#ifndef  KineticParameter_INC
#define  KineticParameter_INC

#include "OdeModelVariable.h"

BIO_NAMESPACE_BEGIN

class KineticParameter : public OdeModelVariable
{
  public:
    KineticParameter(const std::string reaction, const std::string name);
    ~KineticParameter();

    std::string getReaction() const;

    std::string getLocalName() const;
  protected:
    std::string m_Reaction; // reaction name
    std::string m_LocalName; // local name
  private:
    static std::string local2Global(const std::string reaction, const std::string name); //convert local name to global name

};
BIO_NAMESPACE_END

#endif   /* ----- #ifndef KineticParameter_INC  ----- */


