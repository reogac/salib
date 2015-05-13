/**
 @file RNGWrapper.h
 @brief Random number generator wrapper
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  RNGWrapper_INC
#define  RNGWrapper_INC

#include <algorithm>
#include <memory>
#include<random>

#include "Matrix.h"

BIO_NAMESPACE_BEGIN

class ModelInputList;

class RNGWrapper
{
  public:
    RNGWrapper();
    /**
     * @brief Generates a vector of real (double) numbers equally
     * distributed on (0,1).
     *
     * The (0,1) range is divided into equal subranges, on each range a number
     * is generated randomly with uniform distribution.
     * This utility method is used for Latin Hypercube sampling.
     *
     * @param values pointer to the vector that stores generated numbers.
     * @param num the number of generated numbers.
     **/
    void randomLHS(double* values, const int n);

    /**
     * @brief Generates a vector of real (double) numbers equally
     * distributed on (0,1). Each value is the center of its range.
     *
     * The (0,1) range is divided into equal subranges, center values of them
     * make up to the output vector @param values.
     * This utility method is used for Latin hypercube sampling.
     *
     * @param values pointer to the vector that stores generated numbers
     * @param num the number of generated numbers.
     **/
    void centerLHS(double* values, const int n);

    /**
     * @brief Samples the matrix @param mat using Latin Hypercube sampling
     * method.
     *
     *
     * */
    void LHS(DMatrix& mat, const bool random = true);
    void LHS(DMatrix& mat, const ModelInputList* inputs, const bool random = true);
//    std::unique_ptr<DMatrix> LHS(const ModelInputList* inputs, const int n, const bool random = true);
//    std::unique_ptr<DMatrix> LHS(const int rows, const int cols, const bool random = true);

    void sobol(DMatrix& mat);
    void sobol(DMatrix& mat, const ModelInputList* inputs);
    void MC(DMatrix& mat);
    void MC(DMatrix& mat, const ModelInputList* inputs);
//    std::unique_ptr<DMatrix> sobol(const int rows, const int cols);
//    std::unique_ptr<DMatrix> sobol(const ModelInputList* inputs, const int n);

    /**
     * @brief Generates a vector of uniform distributed integer numbers
     *
     * @param lower lower bound of the uniform distribution
     * @param upper upper bound of the uniform distribution
     * @param values pointer to the vector that stores generated numbers
     * @param num the number of generated numbers
     * */
    void rand(const int lower, const int upper, int* values, const int num);

    /**
     * @brief Generates a vector of [0,1) uniform distributed real (double) numbers
     *
     * @param values pointer to the vector that stores generated numbers
     * @param num the number of generated numbers
     * */
    void rand(double* values, const int num);

    /**
     * @brief generate a random number uniformly distribute in [0,1)
     *
     * @return the generate real number
     */
    double rand();
    /** 
     * @brief shuffle a vector of size 'num'
     */
    void shuffle(int* values, const int num);

    /* Convert a matrix generated with lhs/sobol to target distributions*/
    void convert(DMatrix& mat, const ModelInputList* inputs);
    std::unique_ptr<DMatrix> convertCopy(const DMatrix& mat, const ModelInputList* inputs);
  private:
    RNGWrapper(const RNGWrapper& other) = delete;
    RNGWrapper& operator=(const RNGWrapper& other) = delete;
    std::mt19937_64 rng_;
    std::uniform_real_distribution<> urd_;

};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef RNGWrapper_INC  ----- */

