/**
 @file SobolSaltelli.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  SobolSaltelli_INC
#define  SobolSaltelli_INC

#include "SALessSimple.h"

BIO_NAMESPACE_BEGIN

/**
 * @brief A class to implement the variance based (Sobol) global sensitivity analysis.
 * The implementation include three sentitivity indice estimators, including
 * \cite Saltelli2002 (sobol2002),\cite{Sobol’2007957} (sobol2007) and
 * \cite{Saltelli2010259} (sobol2010).
 *
 * The sampling procedures for the three estimators are identical, as well as
 * the equation for the calculation of the first order sensitivity indices.
 * However the equation for the calculation of the total sensitivity indices are
 * slightly different.
 *
 * 
 */
class SobolSaltelli : public SALessSimple
{
  public:
    typedef enum 
    {
      SOBOL2002 = 0,
      SOBOL2007,
      SOBOL2010
    } SobolEstimator_t;

    SobolSaltelli();
  /**
   * @brief Sets the number of samples
   * */
    void setN(const int n);
    void setEstimator(const SobolEstimator_t type);
  private:
    int getNumSens() const override;
    void doSA() override;
    
    int N_;
    SobolEstimator_t estimator_;
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef SobolSaltelli_INC  ----- */

