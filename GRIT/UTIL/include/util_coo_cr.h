#ifndef UTIL_COO_CR_H
#define UTIL_COO_CR_H

#include <util_log.h>
#include <util_coo_matrix.h>

#include <cassert>
#include <vector>
#include <cmath>

namespace util
{

  /**
   * Implementation based on https://en.wikipedia.org/wiki/Conjugate_residual_method
   */
  template<typename T>
  inline bool cr(   COOMatrix<T>     const & P
                  , COOMatrix<T>     const & A
                  , std::vector<T>         & x
                  , std::vector<T>   const & b
                  , unsigned int     const & max_iterations = 1000u
                  , unsigned int     const & restart        = 50u
                  , T                const & tolerance      = 10e-5
                  , bool             const & verbose        = false
                  , std::vector<T>         * residual       = 0
                  )
  {
    using std::sqrt;

    typedef  std::vector<T>  V;

    assert((P.rows() == A.rows() && P.cols() == A.cols())  || !"cr(): incompatable dimensions");
    assert((A.rows() == A.cols() && A.rows() == b.size())  || !"cr(): incompatable dimensions");
    assert(max_iterations>0u                               || !"cr():  max iterations must be positive");
    assert(restart>=0u                                     || !"cr(): restart must be non-negative");
    assert(tolerance >= 0.0                                || !"cr(): tolerance must be non-negative");

    T const zero          = T(0.0);
    T const one           = T(1.0);
    T const sqr_tolerance = tolerance*tolerance;

    unsigned int const N = A.cols();

    if(residual)
    {
      (*residual).reserve(max_iterations);
    }

    if(x.size() != N)
    {
      x.resize(N);
      std::fill(x.begin(), x.end(), zero);
    }

    V Ax  = mul(A,x);             // Ax  <- A*x
    V r   = add_mul(b,-one, Ax);  // r   <- b - A*x
    V p   = mul(P,r);             // p   <- P*r
    V Ap  = mul(A,p);             // Ap  <- A*p
    V PAp = mul(P,Ap);            // PAp <- P*A*p
    V Ar  = mul(A,r);             // Ar  <- A*r
    T rAr = dot(r,Ar);            // rAr <- r^T A r

    for(unsigned int i = 0u;i < max_iterations; ++i)
    {

      T const alpha =  rAr / dot(Ap, PAp);         // alpha <- r^T A *r / (A*p)^T((P * A*p)

      x  = add_mul(x, alpha, p);                   // x <- x + alpha * p

      if( ((i % restart) ==0u ) && (i > 0u) )
      {
        Ax = mul(A,x);                             // Ax <- A*x
        r  = add_mul(b,-one, Ax);                  // r <- b - A*x
      }
      else
      {
        r  = add_mul(r,-alpha, PAp);               // r <- r - alpha * (P*A*p)
      }

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

          log << "cr(): success, converged to " << sqrt(rr) << " in " << i << " iterations" << util::Log::newline();
        }
        return true;
      }

      Ar  = mul(A,r);                  // Ar <- A*r

      T const rAr_old = rAr;

      rAr = dot(r,Ar);                 // rAr = r^T A r

      T const beta = rAr / rAr_old;    // beta <- (r_{k+1} A^T  r_{k+1}) / (r_{k}^T A  r_{k})

      p   = add_mul(r,beta, p);         // p   <- r + beta*p
      Ap  = add_mul(Ar,beta, Ap);       // Ap  <- Ar + beta*Ap
      PAp = mul(P, Ap);                 // PAp <- P*A*p
    }

    if (verbose)
    {
      util::Log log;

      log << "cr(): Failure, did not converge within max iterations = " << max_iterations << util::Log::newline();
    }

    return false;
  }

}//namespace util

// UTIL_COO_CR_H
#endif

