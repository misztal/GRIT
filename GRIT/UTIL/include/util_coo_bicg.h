#ifndef UTIL_COO_BICG_H
#define UTIL_COO_BICG_H

#include <util_log.h>
#include <util_coo_matrix.h>

#include <vector>
#include <cassert>
#include <cmath>

namespace util
{

  template< typename T >
  inline bool bicg(  COOMatrix<T>   const & A
                   , COOMatrix<T>   const & At
                   , std::vector<T>       & x
                   , std::vector<T> const & b
                   , COOMatrix<T>   const & M
                   , COOMatrix<T>   const & Mt
                   , unsigned int   const & max_iterations = 1000u
                   , T              const & tolerance      = 10e-3
                   , bool           const & verbose        = false
                   , std::vector<T>       * residual       = 0
                   )
  {
    using std::sqrt;

    assert((M.rows()  == A.rows()  && M.cols()  == A.cols() )   || !"bicg(): incompatable dimensions"       );
    assert((A.rows()  == A.cols()  && A.rows()  == b.size() )   || !"bicg(): incompatable dimensions"       );
    assert((Mt.rows() == At.rows() && Mt.cols() == At.cols())   || !"bicg(): incompatable dimensions"       );
    assert( At.rows() == At.cols()                              || !"bicg(): incompatable dimensions"       );
    assert( tolerance>= 0.0                                     || !"bicg(): tolerance must be non-negative");

    T const zero          = T(0.0);
    T const one           = T(1.0);
    T const sqr_tolerance = tolerance*tolerance;

    unsigned int const N = A.rows();

    if(residual)
    {
      (*residual).reserve(max_iterations);
    }

    if(x.size() != N)
    {
      x.resize(N);
      std::fill(x.begin(), x.end(), zero);
    }

    std::vector<T> y(N);
    std::vector<T> p(N);
    std::vector<T> p_star(N);
    std::vector<T> q(N);
    std::vector<T> q_star(N);
    std::vector<T> r(N);
    std::vector<T> r_star(N);
    std::vector<T> z(N);
    std::vector<T> z_star(N);

    y = mul(A, x);              // y = Ax
    r = add_mul(b, - one, y );  // r = b - A*x

    T const rr = dot(r,r);

    if(residual)
    {
      (*residual).push_back( sqrt(rr) );
    }

    if(rr <= sqr_tolerance)
    {
      if (verbose)
      {
        util::Log log;

        log << "bicg(): success, converged to " << sqrt(rr) << " in 0 iterations" << util::Log::newline();
      }
      return true;
    }

    r_star = r;                 // r_star = r
    z      = mul(M, r);         // z      = M r
    z_star = mul(Mt, r_star);   // z_star = Mt r_star
    T rho  = dot(z, r_star);    // rho    = z * r_star
    p      = z;                 // p      = z
    p_star = z_star;            // p_star = r

    for (unsigned int i=1u; i<max_iterations; ++i)
    {
      q             = mul( A, p     );                   // q = A p
      q_star        = mul(At, p_star);                   // q_star = At p_star
      T const alpha = rho / dot(p_star, q);              // alpha = (rho) / (p_star, q)
      x             = add_mul(x,  alpha, p);             // x += alpha*p
      r             = add_mul(r, -alpha, q);             // r -= alpha*q
      r_star        = add_mul(r_star, -alpha, q_star);   // r_star -= alpha*q_star
      T const rr    = dot(r,r);

      if(residual)
      {
        (*residual).push_back( sqrt(rr) );
      }

      if(rr <= sqr_tolerance)
      {
        if (verbose)
        {
          util::Log log;

          log << "bicg(): success, converged to " << sqrt(rr) << " in "<< i << " iterations" << util::Log::newline();
        }
        return true;
      }

      z                = mul(M, r);                      // z      = M r
      z_star           = mul(Mt, r_star);                // z_star = Mt r_star
      T const prev_rho = rho;
      rho              = dot(z, r_star);                 // rho = (z,r_star)

      if(rho == zero)
      {
        if (verbose)
        {
          util::Log log;

          log << "bicg(): Failure, rho-factor was zero" << util::Log::newline();
        }
        return false;
      }

      T const beta = rho/prev_rho;
      p            = add_mul(z, beta, p);               // p = beta*p + z
      p_star       = add_mul(z_star, beta, p_star);     // p_star = beta*p_star + z_star
    }

    if (verbose)
    {
      util::Log log;

      log << "bicg(): Failure, did not converge within max iterations = " << max_iterations << util::Log::newline();
    }

    return false;
  }

}//namespace util

// UTIL_COO_BICG_H
#endif

