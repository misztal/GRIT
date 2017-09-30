#ifndef UTIL_COLLAPSE_TIME_H
#define UTIL_COLLAPSE_TIME_H

#include <util_is_finite.h>
#include <util_is_number.h>

#include <cassert>
#include <cmath>   // needed for std::fabs & std::sqrt


// 2015-11-01 Kenny: Missing unit-test?


namespace util
{
  /// This function computes the time t when a triangle with vertices specified by v1, v2 and v3
  /// and the velocities at the nodes u1, u2 and u3 becomes degenerate, i.e. the value of t for
  /// which triangle v1+u1*t, v2+u2*t, v3+u3*t has 0-area.
  /// Negative result indicates the triangle will not collapse for the current values of u1, u2 and u3.
  template <typename V>
  inline typename V::value_type collapse_time(
                                       V const & v1
                                       , V const & v2
                                       , V const & v3
                                       , V const & u1
                                       , V const & u2
                                       , V const & u3
                                       )
  {
    using std::fabs;
    using std::sqrt;

    typedef typename V::value_type    T;
    typedef typename V::value_traits  VT;

    assert( is_finite(v1[0u]) || !"collapse_time(): internal error");
    assert( is_finite(v1[1u]) || !"collapse_time(): internal error");
    assert( is_finite(v2[0u]) || !"collapse_time(): internal error");
    assert( is_finite(v2[1u]) || !"collapse_time(): internal error");
    assert( is_finite(v3[0u]) || !"collapse_time(): internal error");
    assert( is_finite(v3[1u]) || !"collapse_time(): internal error");

    assert( is_number(u1[0u]) || !"collapse_time(): internal error");
    assert( is_number(u1[1u]) || !"collapse_time(): internal error");
    assert( is_number(u2[0u]) || !"collapse_time(): internal error");
    assert( is_number(u2[1u]) || !"collapse_time(): internal error");
    assert( is_number(u3[0u]) || !"collapse_time(): internal error");
    assert( is_number(u3[1u]) || !"collapse_time(): internal error");

    V const e12 = v2 - v1;
    V const e13 = v3 - v1;
    V const u12 = u2 - u1;
    V const u13 = u3 - u1;

    T const a = u12[0u]*u13[1u] - u12[1u]*u13[0u];
    T const b = u12[0u]*e13[1u] + e12[0u]*u13[1u] - u12[1u]*e13[0u] - e12[1u]*u13[0u];
    T const c = e12[0u]*e13[1u] - e12[1u]*e13[0u];

    if (fabs(a) <= VT::zero())
    {
      if (fabs(b) <= VT::zero())
      {
        return -VT::one(); //--- the current velocity field is area preserving
      }
      else
      {

        T const result = -c/b;

        assert( is_finite(result) || !"collapse_time(): internal error");
        assert( is_number(result) || !"collapse_time(): internal error");

        return result;
      }
    }
    else
    {
      T const delta = b*b - VT::four()*a*c;

      if (delta <= VT::zero())
      {
        return -VT::one(); //---: the triangle won't collapse with current velocity field
      }
      else
      {
        T const root = (-b - sqrt(delta))/(VT::two()*a);
        assert( is_finite(root) || !"collapse_time(): internal error");
        assert( is_number(root) || !"collapse_time(): internal error");

        if (root >= VT::zero())
          return root;

        T const result = (-b + sqrt(delta))/(VT::two()*a);

        assert( is_finite(result) || !"collapse_time(): internal error");
        assert( is_number(result) || !"collapse_time(): internal error");

        return result;
      }
    }
  }

}// end namespace util

// UTIL_COLLAPSE_TIME_H
#endif

