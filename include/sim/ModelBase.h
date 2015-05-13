/**
 @file ModelBase.h
 @brief Base class for model components 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#ifndef  ModelBase_INC
#define  ModelBase_INC

#include "ASTNodeOptimizer.h"
#include "common/namespace.h"

BIO_NAMESPACE_BEGIN

class ModelBase
{
  public:
    ModelBase() {};
    virtual ~ModelBase(){};
    virtual ModelBase* clone() const = 0;
};

class ModelComponentWithMath : public ModelBase
{
  public:
    ModelComponentWithMath() : ModelBase() {};
    virtual void index(const ASTNodeIndexer& indexer) = 0;
    virtual void simplify(const ASTNodeOptimizer& simplifier) = 0;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef ModelBase_INC  ----- */

