#ifndef UTIL_COO_GMRES_H
#define UTIL_COO_GMRES_H

#include <util_log.h>
#include <util_coo_matrix.h>

#include <cassert>
#include <vector>
#include <cmath>

namespace util
{
  namespace details
  {

    template<typename T>
    class Array2D
    {
    public:

      unsigned int                  m_rows;
      unsigned int                  m_cols;
      std::vector< std::vector<T> > m_data;

    public:

      Array2D()
      : m_rows(0u)
      , m_cols(0u)
      , m_data(0u)
      {}

      Array2D(unsigned int const & rows, unsigned int const & cols)
      : m_rows(rows)
      , m_cols(cols)
      {
        T const zero = T(0.0);

        m_data.resize( cols );

        for(unsigned int j = 0u; j < cols; ++j)
        {
          m_data[j].resize( rows, zero );
        }
      }

    public:

      T const & operator() (unsigned int const & i,unsigned int const & j) const
      {
        assert( i < m_rows || !"Array2D::operator(): index i out of bounds");
        assert( j < m_cols || !"Array2D::operator(): index j out of bounds");

        return this->m_data[j][i];
      }

      T & operator() (unsigned int const & i,unsigned int const & j)
      {
        assert( i < m_rows || !"Array2D::operator(): index i out of bounds");
        assert( j < m_cols || !"Array2D::operator(): index j out of bounds");

        return this->m_data[j][i];
      }


      std::vector<T> const & column(unsigned int const & j) const
      {
        assert( j < m_cols || !"Array2D::operator(): index j out of bounds");

        return this->m_data[j];
      }

      std::vector<T>  & column(unsigned int const & j)
      {
        assert( j < m_cols || !"Array2D::operator(): index j out of bounds");

        return this->m_data[j];
      }

    };

    template <typename T>
    inline void apply_plane_rotation(  T & dx
                                     , T & dy
                                     , T const & cs
                                     , T const & sn
                                     )
    {
      T const temp = cs * dx + sn * dy;
      dy = -sn * dx + cs * dy;
      dx = temp;
    }

    template <typename T>
    inline void generate_plane_rotation(  T const & dx
                                        , T const & dy
                                        , T & cs
                                        , T & sn
                                        )
    {
      using std::fabs;
      using std::sqrt;

      T const zero = T(0.0);
      T const one  = T(1.0);

      if(dy == zero)
      {
        cs = one;
        sn = zero;
      }
      else if (fabs(dy) > fabs(dx))
      {
        T const tmp = dx / dy;
        sn = one / sqrt(one + tmp*tmp);
        cs = tmp*sn;
      }
      else
      {
        T const tmp = dy / dx;
        cs = one / sqrt(one + tmp*tmp);
        sn = tmp*cs;
      }
    }

    template <typename T>
    inline void plane_rotation( Array2D<T>     & H
                              , std::vector<T> & cs
                              , std::vector<T> & sn
                              , std::vector<T> & s
                              , unsigned int const & i
                              )
    {
      for (unsigned int k = 0u; k < i; ++k)
      {
        apply_plane_rotation( H(k,i), H(k+1u,i), cs[k], sn[k]);
      }

      generate_plane_rotation( H(i,i), H(i+1u,i), cs[i], sn[i] );

      apply_plane_rotation( H(i,i), H(i+1u,i), cs[i], sn[i] );
      apply_plane_rotation(   s[i],   s[i+1u], cs[i], sn[i] );
    }


  }// end of namespace details

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
  inline bool gmres(  COOMatrix<T>     const & P
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

    typedef  details::Array2D<T>  Array2D;
    typedef  std::vector<T>       Array1D;

    assert((P.rows() == A.rows() && P.cols() == A.cols())  || !"gmres(): incompatable dimensions");
    assert((A.rows() == A.cols() && A.rows() == b.size())  || !"gmres(): incompatable dimensions");
    assert(max_iterations>0u                               || !"gmres(): max iterations must be positive");
    assert(restart>=0u                                     || !"gmres(): restart must be non-negative");
    assert(tolerance >= 0.0                                || !"gmres(): tolerance must be non-negative");

    T const zero          = T(0.0);
    T const one           = T(1.0);
    T const sqr_tolerance = tolerance*tolerance;

    unsigned int const N = A.cols();
    unsigned int const R = restart;

    if(residual)
    {
      (*residual).reserve(max_iterations);
    }

    if(x.size() != N)
    {
      x.resize(N);
      std::fill(x.begin(), x.end(), zero);
    }

    Array1D w(N);           // Current Arnoldi vector
    Array1D r(N);           // Residual vector
    Array2D V(   N, R+1u);  // Arnoldi matrix
    Array2D H(R+1u, R   );  // Hessenberg matrix
    Array1D s(R+1u);
    Array1D cs(R);
    Array1D sn(R);

    // 2017-02-19 Marek: needs guards against zeros in the denominator.
    for( unsigned int iter = 0u; iter < max_iterations; )
    {
      w            = mul(A,x);                       // w    = A*x
      r            = add_mul(w, -one, b );           // r    = A*x - b= w - b
      w            = mul(P, r);                      // V(0) = P*r
      T const beta = sqrt(dot(w,w));                 // beta = norm(V(0))
      w            = mul( -one/beta, w);             // V(0) = -V(0)/beta
      V.column(0u) = w;

      std::fill(s.begin(), s.end(), zero);
      s[0] = beta;                                     // s = [beta, 0,..., 0]^T

      T const ss = fabs(s[0]);

      // 2015-05-08 Marek: added this output for the testing phase.
      //                   It should make debugging easier.
      if (verbose)
      {
        util::Log log;
        log << "iteration #" << iter << " , residual = " << sqrt(ss) << util::Log::newline();
      }

      if(residual)
      {
        (*residual).push_back( ss );
      }

      if(ss <= sqr_tolerance)
      {
        if (verbose)
        {
          util::Log log;

          // 2015-05-08 Marek: replaced ss with sqrt(ss)
          log << "gmres(): success, converged to "
          << sqrt(ss)
          << " in "
          << iter
          << " iterations"
          << util::Log::newline();
        }
        return true;
      }

      int i = -1;

      do
      {
        ++i;
        ++iter;

        w = mul(A,w);
        w = mul(P,w);                                  // V(i+1) = P*A*V(i)

        for (unsigned int k = 0u; k <= i; ++k)
        {
          H(k, i) = dot(w, V.column(k));                //  H(k,i) = <V(i+1), V(k)>
          w       = add_mul(w, -H(k, i), V.column(k) ); //  V(i+1) = V(i+1) - H(k, i) * V(k)
        }

        H(i+1u,i)      = sqrt(dot(w,w));
        w              = mul( one/ H(i+1u,i), w);       // V(i+1) = V(i+1) / H(i+1, i)
        V.column(i+1u) = w;

        details::plane_rotation( H, cs, sn, s, i);

        T const ss = fabs(s[i+1u]);

        if(residual)
        {
          (*residual).push_back( ss );
        }

        if(ss <= sqr_tolerance)
          break;
      }
      while( (i + 1) < R && iter < max_iterations );

      //--- Solve upper triangular system in place -----------------------------
      for (int j = i; j >= 0; --j)
      {
        s[j] /= H(j,j);

        // s(0:j) = s(0:j) - s[j] H(0:j,j)

        for (int k = j-1; k >= 0; --k)
        {
          s[k] -= H(k,j) * s[j];
        }
      }

      //--- Update the solution ------------------------------------------------
      for (int j = 0; j <= i; ++j)
      {
        x = add_mul(x, s[j], V.column(j));   // x = x + s[j] * V(j)
      }
    }

    if (verbose)
    {
      util::Log log;

      log << "gmres(): Failure, did not converge within max iterations = "
          << max_iterations
          << util::Log::newline();
    }

    return false;
  }

}//namespace util

// UTIL_COO_GMRES_H
#endif
