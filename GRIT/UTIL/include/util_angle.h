#ifndef UTIL_ANGLE_H
#define UTIL_ANGLE_H

#include <util_is_finite.h>
#include <util_is_number.h>

#include <cmath>
#include <cassert>

namespace util
{

  /**
   * Computes the smalles angle between the line segments (v0-v1) and (v2-v1).
   *
   * @param v0   One end-point of the first line segment.
   * @param v1   The "other" shared end-point between the first and second line segments.
   * @param v2   One end-point of the second line segment.
   *
   * @return     The angle between the specified line segments.
   */
  template <typename V>
  inline typename V::value_type angle(
                       V const & v0
                     , V const & v1
                     , V const & v2
                     )
  {
    using std::acos;
    using std::min;
    using std::max;

    typedef typename V::value_type   T;
    typedef typename V::value_traits VT;

    V const a         = v0-v1;
    V const b         = v2-v1;

    T const aa        = inner_prod(a,a);
    T const bb        = inner_prod(b,b);

    assert(aa > VT::zero() || !"angle(): zero length edge encountered");
    assert(is_finite(aa)   || !"angle(): infinity");
    assert(is_number(aa)   || !"angle(): nan");
    assert(bb > VT::zero() || !"angle(): zero length edge encountered");
    assert(is_finite(bb)   || !"angle(): infinity");
    assert(is_number(bb)   || !"angle(): nan");

    T const value = inner_prod(a,b) / ( sqrt(aa) * sqrt(bb) );
    assert(is_finite(value) || !"angle(): infinity");
    assert(is_number(value) || !"angle(): nan");

    T const clamped_value = max (   min( VT::one(), value) , -VT::one() ) ;
    assert(is_finite(clamped_value) || !"angle(): infinity");
    assert(is_number(clamped_value) || !"angle(): nan");

    return acos( clamped_value );
  }

}//end of namespace util

// UTIL_ANGLE_H
#endif
