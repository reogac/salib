/**
 @file normal.h
 @brief 
 @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */

#ifndef  normal_INC
#define  normal_INC

#ifdef __cplusplus
extern "C"
{
#endif


double normal_01_cdf ( double x );
double normal_01_cdf_inv ( double p );
double truncated_normal_ab_cdf_inv ( double cdf, double mu, double s, double a, double b);

double r8poly_value ( int n, double a[], double x );

#ifdef __cplusplus
}
#endif

#endif   /* ----- #ifndef normal_INC  ----- */
