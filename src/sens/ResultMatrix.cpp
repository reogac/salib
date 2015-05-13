/**
 @file ResultMatrix.cpp
 @brief Implementation for ResultMatrix class
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "ResultMatrix.h"

BIO_NAMESPACE_BEGIN

ResultMatrix::ResultMatrix(const int nsamples, const int nouts)
  : DMatrix(nsamples, nouts)
{
  m_Labels = new int[nsamples];
}

ResultMatrix::ResultMatrix()
  : DMatrix()
  , m_Labels(nullptr)
{
}
ResultMatrix::~ResultMatrix()
{
  if (orig_)
    delete [] m_Labels;
}


std::unique_ptr<ResultMatrix> ResultMatrix::subMatrix(const int startRow, const int nrows)
{
  std::unique_ptr<ResultMatrix> ret(new ResultMatrix());
  DMatrix::subMatrix(*ret, startRow, nrows);
  ret->m_Labels = &m_Labels[startRow];
  return std::move(ret);
}

BIO_NAMESPACE_END

