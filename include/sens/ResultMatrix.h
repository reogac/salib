/**
 @file ResultMatrix.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */


#ifndef  ResultMatrix_INC
#define  ResultMatrix_INC

#include "Matrix.h"

BIO_NAMESPACE_BEGIN

#define SIM_SUCCESS 1
#define SIM_FAILURE 0

class ResultMatrix : public DMatrix
{
  public:
    ResultMatrix(const int nouts, const int nsamples);
    ~ResultMatrix();
    int* getLabels() { return m_Labels; }

    std::unique_ptr<ResultMatrix> subMatrix(const int startRow, const int nrows);
  protected:
    int* m_Labels;
  private:
    ResultMatrix();
    ResultMatrix(const ResultMatrix& other);
    ResultMatrix& operator=(const ResultMatrix& other);
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef ResultMatrix_INC  ----- */


