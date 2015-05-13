/**
 @file MathPrinter.cpp
 @brief Implementation for MathPrinter class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "MathPrinter.h"
#include <sbml/SBMLTypes.h>


BIO_NAMESPACE_BEGIN

std::string MathPrinter::print(const ASTNode* math) const
{
  char *tempstring = SBML_formulaToString(math);
  std::string ret(tempstring);
  free(tempstring);
  return ret;
}

BIO_NAMESPACE_END
