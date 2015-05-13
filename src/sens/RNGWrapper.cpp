/**
 @file RNGWrapper.cpp
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#include "RNGWrapper.h"

#include "ModelInput.h"
#include "sobol.hpp"
#include "normal.h"
#include "SAException.h"

#include <cmath>
#include <iostream>

BIO_NAMESPACE_BEGIN

RNGWrapper::RNGWrapper()
  : urd_(0,1)
{
  /* TODO: more consideration should be taken for generating the seed number */
  std::random_device rd;
  rng_.seed(rd());
}

void RNGWrapper::rand(const int lower, const int upper, int* values, const int num)
{
  std::uniform_int_distribution<> dis(lower, upper);
  for (int i=0; i<num; ++i)
    values[i] = dis(rng_);
}

void RNGWrapper::rand(double* values, const int num)
{
  for (int i=0; i<num; ++i)
    values[i] = urd_(rng_);
}
double RNGWrapper::rand()
{
  return urd_(rng_);
}

void RNGWrapper::shuffle(int* values, const int num)
{
  std::shuffle(&values[0], &values[num-1], rng_);
}
void RNGWrapper::randomLHS(double* values, const int n)
{
  double offset = 1.0/n;
  
  values[0] = urd_(rng_)/n;
  /* make sure the first value is non-zero */
  while (values[0] <= 0.0)
    values[0] = urd_(rng_)/n;

  double l = offset;
  for (int i=1; i<n; ++i)
  {
    values[i] = l + urd_(rng_)/n;
    l += offset;
  }
  std::shuffle(&values[0], &values[n-1], rng_);
}

void RNGWrapper::centerLHS(double* values, const int n)
{
  double offset = 1.0/n;
  values[0] = 0.5*offset;
  
  for (int i=1; i<n; ++i)
  {
    values[i] = values[i-1] + offset;
  }
  std::shuffle(&values[0], &values[n-1], rng_);
}

void RNGWrapper::LHS(DMatrix& mat, const bool random /*  true */)
{
  int nRows = mat.getNumRows();
  int nCols = mat.getNumCols();
  std::unique_ptr<double[]> values(new double[nRows]);

  for (int iCol=0; iCol<nCols; ++iCol)
  {
    if (random)
    {
      randomLHS(values.get(), nRows);
    }
    else
    {
      centerLHS(values.get(), nRows);
    }

    mat.fillCol(iCol, values.get());
  }
}

void RNGWrapper::LHS(DMatrix& mat, const ModelInputList* inputs, const bool random)
{
  LHS(mat, random);
  convert(mat, inputs);
}
/*  
std::unique_ptr<DMatrix> RNGWrapper::LHS(const ModelInputList* inputs, const int n, const bool random)
{
  std::unique_ptr<DMatrix> ret(LHS(n, inputs->size(), random));
  convert(*ret, inputs);
  return std::move(ret);
}

std::unique_ptr<DMatrix> RNGWrapper::LHS(const int rows, const int cols, const bool random)
{
  std::unique_ptr<DMatrix> ret(new DMatrix(rows, cols));
  LHS(*ret, random);
  return std::move(ret);
}

std::unique_ptr<DMatrix> RNGWrapper::sobol(const int rows, const int cols)
{
  std::unique_ptr<DMatrix> ret(new DMatrix(rows, cols));

  sobol(*(ret.get()));

  return std::move(ret);
}
*/
void RNGWrapper::sobol(DMatrix& mat)
{
  int nCols = mat.getNumCols();
  if (nCols>=1111)
  {
    throw SAException(ERROR_SOBOL_EXCEEDING);
  }

  static long long int seed=7123456;
  for (int iRow = 0; iRow < mat.getNumRows(); ++iRow)
  {
    i8_sobol(nCols, &seed, mat.getRow(iRow));
  }

}

/*  
std::unique_ptr<DMatrix> RNGWrapper::sobol(const ModelInputList* inputs, const int n)
{
  std::unique_ptr<DMatrix> ret(sobol(n, inputs->size()));

  convert(*(ret.get()), inputs);

  return std::move(ret);
}
*/


void RNGWrapper::MC(DMatrix& mat)
{
  int nRows = mat.getNumRows();
  std::unique_ptr<double[]> col(new double[nRows]);
  for (int iCol=0; iCol<mat.getNumCols(); ++iCol)
  {
    this->rand(col.get(), nRows);
    mat.fillCol(iCol, col.get());
  }
}

void RNGWrapper::MC(DMatrix& mat, const ModelInputList* inputs)
{
  MC(mat);
  convert(mat, inputs);
}
void RNGWrapper::convert(DMatrix& mat, const ModelInputList* inputs)
{
  for (int iCol=0; iCol<inputs->size(); ++iCol)
  {
    const ModelInput* ip = inputs->get(iCol);
    const InputDist* dist = ip->getDist();
    if (dist)
    {
      if (dist->getType() == DIST_UNIFORM)
      {
        double lower = ((const InputUniform*) dist)->getLower();
        double upper = ((const InputUniform*) dist)->getUpper();
        if (ip->isLog())
        {
          double ll = log10(lower);
          double ldist = log10(upper) - ll;
          mat.colEval(iCol,[&ll, &ldist](double value)
              {
                return pow(10, value*ldist+ll);
              });
          //mat.u2logU(iCol, lower, upper);
        } else
        {
          double distance = upper-lower;
          //mat.u2U(iCol, lower, upper);
          mat.colEval(iCol,[&lower, &distance](double value)
              {
                return value*distance + lower;
                });
        }

      } else if (dist->getType() == DIST_NORMAL)
      {
        double mean = ((const InputNormal*) dist)->getMean();
        double std = ((const InputNormal*) dist)->getStd();
        double a = mean - 3*std;
        double b = mean + 3*std;
        if (!ip->isLog())
        {
          mat.colEval(iCol,[&mean, &std, &a, &b](double value)
              {
                return truncated_normal_ab_cdf_inv(value, mean, std, a, b);
              });
          //mat.u2logN(iCol, mean, std);
        } else
        {
          mat.colEval(iCol,[&mean, &std, &a, &b](double value)
              {
                return pow(10, truncated_normal_ab_cdf_inv(value, mean, std, a,
                      b));
              });
          //mat.u2N(iCol, mean, std);
        }
      }
    }
  }
}


std::unique_ptr<DMatrix> RNGWrapper::convertCopy(const DMatrix& mat, const ModelInputList* inputs)
{
  std::unique_ptr<DMatrix> ret(new DMatrix(mat));
  convert(*ret, inputs);
  return ret;
}
void RNGWrapper::sobol(DMatrix& mat, const ModelInputList* inputs)
{
  sobol(mat);
  convert(mat, inputs);
}

BIO_NAMESPACE_END

