#ifndef UTIL_PRECONDITIONERS_H
#define UTIL_PRECONDITIONERS_H

#include <util_coo_matrix.h>

#include <cassert>

namespace util
{

  template<typename T>
  inline void make_identity_preconditioner(
                                             COOMatrix<T> const & A
                                           , COOMatrix<T> & P
                                           )
  {
    assert(A.rows() == A.cols() || !"make_identity_preconditioner(): Matrix must be square");

    size_t const N = A.cols();

    P = make_identity<T>(N);
  }

  /**
   * Returns inverse diagonal of the input matrix to create a jacobi
   * preconditioner
   *
   * @param A       The matrix that needs the preconditioner
   *
   * @param P        Return argument for where the inverse diagonal is returned.
   *
   */
  template<typename T>
  inline void make_jacobi_preconditioner(
                                         COOMatrix<T> const & A
                                         , COOMatrix<T> & P
                                         )
  {
    assert(A.rows() == A.cols() || !"make_jacobi_preconditioner(): Matrix must be square");

    unsigned int const K                   = A.rows();
    bool         const verbose             = false;
    bool         const guard_against_zeros = true;

    P.clear();
    P.init(K, K, 1u);

    for (unsigned int i =0u; i< A.nzeros(); ++i)
    {
      unsigned int const & r = A.row_indices()[i];
      unsigned int const & c = A.column_indices()[i];
      T            const & v = A.values()[i];

      if(r==c)
      {
        P.insert(c, r, v, guard_against_zeros, verbose);
      }
    }

  }


}// end namespace util

// UTIL_PRECONDITIONERS_H
#endif
