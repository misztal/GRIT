#ifndef UTIL_DETERMINANT_H
#define UTIL_DETERMINANT_H

namespace util
{

  /**
   *  Compute Determinant.
   *  Given matrix the
   *
   *         | a b c |
   *     A = | d e f |
   *         | g h i |
   *
   *  This function computes
   *
   *   det(A) = a(ei-hf) - b(di-gf) + c(dh-ge)
   *
   */
  template <typename T>
  inline T determinant(
                         T const & a
                       , T const & b
                       , T const & c
                       , T const & d
                       , T const & e
                       , T const & f
                       , T const & g
                       , T const & h
                       , T const & i
                       )
  {
    return a*e*i + b*f*g + c*d*h - c*e*g - b*d*i - a*f*h;
  }

}//end of namespace util

// UTIL_DETERMINANT_H
#endif
