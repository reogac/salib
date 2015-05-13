/**
 @file Morris.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  Morris_INC
#define  Morris_INC

#include "SASimple.h"

BIO_NAMESPACE_BEGIN

class Morris : public SASimple 
{
  public: 
    /**
     * @brief Constructor
     * */
    Morris();
    /**
     * @brief Sets the number of trajectories
     *
     * @param r number of trajectories
     * */
    void setR(const int r);

    /**
     * @brief Sets the number of grid levels 
     *
     * @param p number of grid levels 
     * */
    void setP(const int p);
  private:
    int getNumSens() const override;
    int getNumSamples() const override;
    void sample() override;
    void estimate() override;

    int r_;                                     /* number of trajectories */
    int p_;                                     /* number of grid levels */
    std::unique_ptr<IMatrix> perm_;             /* array of permutation vectors used to generate Bstar matrix */
};

BIO_NAMESPACE_END

#endif   /* ----- #ifndef Morris_INC  ----- */

