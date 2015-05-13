/**
  @file normal.cpp
  @brief 
  @author Thai Quang Tung (tungtq), tungtq@gmail.com
  */
#include "normal.h"

# include <cstdlib>
# include <iostream>
# include <iomanip>
# include <cmath>
# include <ctime>
# include <cstring>

double r8poly_value ( int n, double a[], double x )

  //****************************************************************************80
  //
  //  Purpose:
  //
  //    R8POLY_VALUE evaluates a double precision polynomial.
  //
  //  Discussion:
  //
  //    For sanity's sake, the value of N indicates the NUMBER of
  //    coefficients, or more precisely, the ORDER of the polynomial,
  //    rather than the DEGREE of the polynomial.  The two quantities
  //    differ by 1, but cause a great deal of confusion.
  //
  //    Given N and A, the form of the polynomial is:
  //
  //      p(x) = a[0] + a[1] * x + ... + a[n-2] * x^(n-2) + a[n-1] * x^(n-1)
  //
  //  Licensing:
  //
  //    This code is distributed under the GNU LGPL license.
  //
  //  Modified:
  //
  //    13 August 2004
  //
  //  Author:
  //
  //    John Burkardt
  //
  //  Parameters:
  //
  //    Input, int N, the order of the polynomial.
  //
  //    Input, double A[N], the coefficients of the polynomial.
  //    A[0] is the constant term.
  //
  //    Input, double X, the point at which the polynomial is to be evaluated.
  //
  //    Output, double R8POLY_VALUE, the value of the polynomial at X.
  //
{
  int i;
  double value;

  value = 0.0;

  for ( i = n-1; 0 <= i; i-- )
  {
    value = value * x + a[i];
  }

  return value;
}

double normal_01_cdf ( double x )

  //****************************************************************************80
  //
  //  Purpose:
  //
  //    NORMAL_01_CDF evaluates the Normal 01 CDF.
  //
  //  Licensing:
  //
  //    This code is distributed under the GNU LGPL license.
  //
  //  Modified:
  //
  //    10 February 1999
  //
  //  Author:
  //
  //    John Burkardt
  //
  //  Reference:
  //
  //    A G Adams,
  //    Areas Under the Normal Curve,
  //    Algorithm 39,
  //    Computer j.,
  //    Volume 12, pages 197-198, 1969.
  //
  //  Parameters:
  //
  //    Input, double X, the argument of the CDF.
  //
  //    Output, double CDF, the value of the CDF.
  //
{
  double a1 = 0.398942280444;
  double a2 = 0.399903438504;
  double a3 = 5.75885480458;
  double a4 = 29.8213557808;
  double a5 = 2.62433121679;
  double a6 = 48.6959930692;
  double a7 = 5.92885724438;
  double b0 = 0.398942280385;
  double b1 = 3.8052E-08;
  double b2 = 1.00000615302;
  double b3 = 3.98064794E-04;
  double b4 = 1.98615381364;
  double b5 = 0.151679116635;
  double b6 = 5.29330324926;
  double b7 = 4.8385912808;
  double b8 = 15.1508972451;
  double b9 = 0.742380924027;
  double b10 = 30.789933034;
  double b11 = 3.99019417011;
  double cdf;
  double q;
  double y;
  //
  //  |X| <= 1.28.
  //
  if ( fabs ( x ) <= 1.28 )
  {
    y = 0.5 * x * x;

    q = 0.5 - fabs ( x ) * ( a1 - a2 * y / ( y + a3 - a4 / ( y + a5
            + a6 / ( y + a7 ) ) ) );
    //
    //  1.28 < |X| <= 12.7
    //
  }
  else if ( fabs ( x ) <= 12.7 )
  {
    y = 0.5 * x * x;

    q = exp ( - y ) * b0 / ( fabs ( x ) - b1
        + b2 / ( fabs ( x ) + b3
          + b4 / ( fabs ( x ) - b5
            + b6 / ( fabs ( x ) + b7
              - b8 / ( fabs ( x ) + b9
                + b10 / ( fabs ( x ) + b11 ) ) ) ) ) );
    //
    //  12.7 < |X|
    //
  }
  else
  {
    q = 0.0;
  }
  //
  //  Take account of negative X.
  //
  if ( x < 0.0 )
  {
    cdf = q;
  }
  else
  {
    cdf = 1.0 - q;
  }

  return cdf;
}
//****************************************************************************80

double normal_01_cdf_inv ( double p )

  //****************************************************************************80
  //
  //  Purpose:
  //
  //    NORMAL_01_CDF_INV inverts the standard normal CDF.
  //
  //  Discussion:
  //
  //    The result is accurate to about 1 part in 10**16.
  //
  //  Licensing:
  //
  //    This code is distributed under the GNU LGPL license.
  //
  //  Modified:
  //
  //    27 December 2004
  //
  //  Author:
  //
  //    Original FORTRAN77 version by Michael Wichura.
  //    C++ version by John Burkardt.
  //
  //  Reference:
  //
  //    Michael Wichura,
  //    The Percentage Points of the Normal Distribution,
  //    Algorithm AS 241,
  //    Applied Statistics,
  //    Volume 37, Number 3, pages 477-484, 1988.
  //
  //  Parameters:
  //
  //    Input, double P, the value of the cumulative probability
  //    densitity function.  0 < P < 1.  If P is outside this range, an
  //    "infinite" value is returned.
  //
  //    Output, double NORMAL_01_CDF_INV, the normal deviate value
  //    with the property that the probability of a standard normal deviate being
  //    less than or equal to this value is P.
  //
{
  double a[8] = {
    3.3871328727963666080,     1.3314166789178437745e+2,
    1.9715909503065514427e+3,  1.3731693765509461125e+4,
    4.5921953931549871457e+4,  6.7265770927008700853e+4,
    3.3430575583588128105e+4,  2.5090809287301226727e+3 };
  double b[8] = {
    1.0,                       4.2313330701600911252e+1,
    6.8718700749205790830e+2,  5.3941960214247511077e+3,
    2.1213794301586595867e+4,  3.9307895800092710610e+4,
    2.8729085735721942674e+4,  5.2264952788528545610e+3 };
  double c[8] = {
    1.42343711074968357734,     4.63033784615654529590,
    5.76949722146069140550,     3.64784832476320460504,
    1.27045825245236838258,     2.41780725177450611770e-1,
    2.27238449892691845833e-2,  7.74545014278341407640e-4 };
  double const1 = 0.180625;
  double const2 = 1.6;
  double d[8] = {
    1.0,                        2.05319162663775882187,
    1.67638483018380384940,     6.89767334985100004550e-1,
    1.48103976427480074590e-1,  1.51986665636164571966e-2,
    5.47593808499534494600e-4,  1.05075007164441684324e-9 };
  double e[8] = {
    6.65790464350110377720,     5.46378491116411436990,
    1.78482653991729133580,     2.96560571828504891230e-1,
    2.65321895265761230930e-2,  1.24266094738807843860e-3,
    2.71155556874348757815e-5,  2.01033439929228813265e-7 };
  double f[8] = {
    1.0,                        5.99832206555887937690e-1,
    1.36929880922735805310e-1,  1.48753612908506148525e-2,
    7.86869131145613259100e-4,  1.84631831751005468180e-5,
    1.42151175831644588870e-7,  2.04426310338993978564e-15 };
  double q;
  double r;
  double split1 = 0.425;
  double split2 = 5.0;
  double value;

  if ( p <= 0.0 )
  {
    value = -HUGE_VAL;
    return value;
  }

  if ( 1.0 <= p )
  {
    value = HUGE_VAL;
    return value;
  }

  q = p - 0.5;

  if ( fabs ( q ) <= split1 )
  {
    r = const1 - q * q;
    value = q * r8poly_value ( 8, a, r ) / r8poly_value ( 8, b, r );
  }
  else
  {
    if ( q < 0.0 )
    {
      r = p;
    }
    else
    {
      r = 1.0 - p;
    }

    if ( r <= 0.0 )
    {
      value = HUGE_VAL;
    }
    else
    {
      r = sqrt ( - log ( r ) );

      if ( r <= split2 )
      {
        r = r - const2;
        value = r8poly_value ( 8, c, r ) / r8poly_value ( 8, d, r );
      }
      else
      {
        r = r - split2;
        value = r8poly_value ( 8, e, r ) / r8poly_value ( 8, f, r );
      }
    }

    if ( q < 0.0 )
    {
      value = - value;
    }

  }

  return value;
}
//****************************************************************************80

double truncated_normal_ab_cdf_inv ( double cdf, double mu, double s, double a, 
    double b )

  //****************************************************************************80
  //
  //  Purpose:
  //
  //    TRUNCATED_NORMAL_AB_CDF_INV inverts the truncated Normal CDF.
  //
  //  Licensing:
  //
  //    This code is distributed under the GNU LGPL license.
  //
  //  Modified:
  //
  //    14 August 2013
  //
  //  Author:
  //
  //    John Burkardt
  //
  //  Parameters:
  //
  //    Input, double CDF, the value of the CDF.
  //    0.0 <= CDF <= 1.0.
  //
  //    Input, double MU, S, the mean and standard deviation of the
  //    parent Normal distribution.
  //
  //    Input, double A, B, the lower and upper truncation limits.
  //
  //    Output, double TRUNCATED_NORMAL_AB_CDF_INV, the corresponding argument.
  //
{
  double alpha;
  double alpha_cdf;
  double beta;
  double beta_cdf;
  double x;
  double xi;
  double xi_cdf;

  if ( cdf < 0.0 || 1.0 < cdf )
  {
    std::cerr << "\n";
    std::cerr << "TRUNCATED_NORMAL_AB_CDF_INV - Fatal error!\n";
    std::cerr << "  CDF < 0 or 1 < CDF.\n";
    exit ( 1 );
  }

  alpha = ( a - mu ) / s;
  beta = ( b - mu ) / s;

  alpha_cdf = normal_01_cdf ( alpha );
  beta_cdf = normal_01_cdf ( beta );

  xi_cdf = ( beta_cdf - alpha_cdf ) * cdf + alpha_cdf;
  xi = normal_01_cdf_inv ( xi_cdf );

  x = mu + s * xi;

  return x;
}

