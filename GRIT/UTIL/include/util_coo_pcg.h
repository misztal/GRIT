#ifndef UTIL_COO_PCG_H
#define UTIL_COO_PCG_H

#include <util_log.h>
#include <util_coo_matrix.h>

#include <cassert>
#include <vector>
#include <cmath>

namespace util
{

  /**
   * Solve linear system using preconditioned conjugate gradient method.
   *
   * @param P    The Preconditioner to use
   * @param A    The system (coefficient) matrix.
   * @param x    Upon  return holds the approximate solution to A x = b.
   * @param b    The right hand side vector
   *
   * @return     If method converged then return value is true otherwise it is false.
   */
  template<typename T>
  inline bool pcg(  COOMatrix<T>     const & P
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

    assert((P.rows() == A.rows() && P.cols() == A.cols()) || !"pcg(): incompatable dimensions");
    assert((A.rows() == A.cols() && A.rows() == b.size()) || !"pcg(): incompatable dimensions");
    assert(max_iterations>0u                              || !"pcg():  max iterations must be positive");
    assert(restart>=0u                                    || !"pcg(): restart must be non-negative");
    assert(tolerance >= 0.0                               || !"pcg(): tolerance must be non-negative");

    T const zero          = T(0.0);
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

    V y   = mul(A,x);    // y   = A x
    V r   = sub(b,y);    // r   = b - A x

    // 2017-02-19 Marek: adding a check if residual is under tolerance
    T const rr = dot(r,r);

    if(rr <= sqr_tolerance)
    {
      if (verbose)
      {
        util::Log log;

        log << "pcg(): success, converged to " << sqrt(rr) << " in 0 iterations" << util::Log::newline();
      }
      return true;
    }

    V z   = mul(P,r);    // z   = P r
    V p   = z;           // p   = z
    T rPr = dot(r,z);    // rPr = r^T P r

    for(unsigned int i = 0u;i < max_iterations; ++i)
    {
      y = mul(A,p);

      T const pAp    = dot(y,p);    // pAp   = p^T A p
      T const alpha = rPr / pAp;    // alpha = r^T P r /  p^T A p

      x = add_mul(x,alpha,p);       // x     = x + alpha * p

      if(i % restart == 0)
      {
        y = mul(A,x);
        r = sub(b,y);
      }
      else
      {
        r = add_mul(r,-alpha,y);    //r      = r - alpha * A * p
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

          log << "pcg(): success, converged to " << sqrt(rr) << " in " << i << " iterations" << util::Log::newline();
        }
        return true;
      }

      T const rPr_old = rPr;

      z   = mul(P,r);             // z       = P r
      rPr = dot(r,z);             // rz      = r^T P r

      T const beta = rPr / rPr_old;

      p = add_mul(z,beta,p);      // p       = z + beta * p
    }

    if (verbose)
    {
      util::Log log;

      log << "pcg(): Failure, did not converge within max iterations = " << max_iterations << util::Log::newline();
    }

    return false;

  }

}// end namespace util

// UTIL_COO_PCG_H
#endif
