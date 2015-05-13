/**
 @file MathPrinter.h
 @brief A helper class for convert math expression to string
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#ifndef  MathPrinter_INC
#define  MathPrinter_INC

#include <string>
#include "common/namespace.h"
#include "ASTNode.h"

BIO_NAMESPACE_BEGIN
class MathPrinter
{
  public:
    std::string print(const ASTNode* math) const;
};
BIO_NAMESPACE_END
#endif   /* ----- #ifndef MathPrinter_INC  ----- */

