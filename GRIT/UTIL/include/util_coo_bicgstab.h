#ifndef UTIL_COO_BICGSTAB_H
#define UTIL_COO_BICGSTAB_H

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
  inline bool bicgstab(  COOMatrix<T>     const & P
                       , COOMatrix<T>     const & A
                       , std::vector<T>         & x
                       , std::vector<T>   const & b
                       , unsigned int     const & max_iterations = 1000u
                       , T                const & tolerance      = 10e-5
                       , bool             const & verbose        = false
                       , std::vector<T>         * residual       = 0
                       )
  {
    using std::sqrt;

    typedef std::vector<T>  V;

    assert((P.rows() == A.rows() && P.cols() == A.cols())  || !"bicgstab(): incompatable dimensions");
    assert((A.rows() == A.cols() && A.rows() == b.size())  || !"bicgstab(): incompatable dimensions");
    assert(max_iterations>0u                               || !"bicgstab():  max iterations must be positive");
    assert(tolerance >= 0.0                                || !"bicgstab(): tolerance must be non-negative");

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

    std::vector<T>   s(N);
    std::vector<T>  Pp(N);
    std::vector<T> APp(N);
    std::vector<T>  Ps(N);
    std::vector<T> APs(N);

    V y = mul(A,x);              // y <- Ax
    V r = add_mul(b, -one, y);   // r <- b - A*x

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

        log << "bicgstab(): success, converged to " << sqrt(rr) << " in 0 iterations" << util::Log::newline();
      }
      return true;
    }

    V p = r;                     // p <- r
    V r_star = r;                // r_star <- r

    T r_r_star_old = dot(r_star, r);

    for (unsigned int i=1u; i<max_iterations; ++i)
    {
      Pp            = mul(P,p);                            // Mp    = M*p
      APp           = mul(A, Pp);                          // AMp   = A*Mp
      T const alpha = r_r_star_old / dot(r_star, APp);     // alpha = (r_j, r_star) / (A*M*p, r_star)
      s             = add_mul(r, -alpha, APp);             // s_j   = r_j - alpha * AMp

      T const ss = dot(s,s);
      if(ss <= sqr_tolerance)
      {
        x = add_mul(x, alpha, Pp); // x += alpha*M*p_j

        if (verbose)
        {
          util::Log log;

          log << "bicgstab(): success, converged to " << sqrt(ss) << " in " << i << " iterations" << util::Log::newline();
        }

        return true;
      }

      Ps            = mul(P,s);                           // Ms    = M*s_j
      APs           = mul(A, Ps);                         // AMs   = A*Ms
      T const omega = dot(APs, s) / dot(APs, APs);        // omega = (AMs, s) / (AMs, AMs)

      x             = add_mul(x, alpha, Pp );              // x_{j+1} = x_j + alpha*M*p_j + omega*M*s_j
      x             = add_mul(x, omega, Ps );
      r             = add_mul(s,-omega, APs);              // r_{j+1} = s_j - omega*A*M*s

      T const r_r_star_new = dot(r_star, r);

      T const beta = (r_r_star_new / r_r_star_old) * (alpha / omega);   // beta_j = (r_{j+1}, r_star) / (r_j, r_star) * (alpha/omega)

      r_r_star_old = r_r_star_new;

      p = add_mul(r,beta, p);
      p = add_mul(p,-beta*omega, APp);                       // p_{j+1} = r_{j+1} + beta*(p_j - omega*A*M*p)
    }

    if (verbose)
    {
      util::Log log;

      log << "bicg(): Failure, did not converge within max iterations = " << max_iterations << util::Log::newline();
    }

    return false;
  }

}//namespace util

// UTIL_COO_BICGSTAB_H
#endif

