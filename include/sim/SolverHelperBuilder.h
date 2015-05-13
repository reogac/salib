/**
 @file SolverHelperBuilder.h
 @brief A helper class for building internal structures of SolverHelper
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#ifndef  SolverHelperBuilder_INC
#define  SolverHelperBuilder_INC

#include <vector>
#include <memory>

#include "ASTNode.h"
#include "OdeModel.h"

BIO_NAMESPACE_BEGIN

class SolverHelperBuilder
{
  public:
    SolverHelperBuilder(const OdeModel& model);
    void build();

    std::vector< std::unique_ptr<ASTNode> >& getRootFinders() ;
  private:
    void buildRootFinders();
    const OdeModel& model_;
    std::vector< std::unique_ptr<ASTNode> > rootFinders_;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef SolverHelperBuilder_INC  ----- */

