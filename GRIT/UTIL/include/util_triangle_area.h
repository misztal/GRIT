#ifndef UTIL_TRIANGLE_AREA_H
#define UTIL_TRIANGLE_AREA_H

#include <util_is_finite.h>
#include <util_is_number.h>

#include <cassert>
#include <cmath>   // needed for std::fabs


namespace util
{

  template <typename V>
  inline typename V::value_type signed_triangle_area(
                                                      V const & a
                                                    , V const & b
                                                    , V const & c
                                                    )
  {
    typedef typename V::value_type   T;
    typedef typename V::value_traits VT;

    assert( is_finite(a[0]) || !"signed_triangle_area(): internal error");
    assert( is_finite(a[1]) || !"signed_triangle_area(): internal error");
    assert( is_finite(b[0]) || !"signed_triangle_area(): internal error");
    assert( is_finite(b[1]) || !"signed_triangle_area(): internal error");
    assert( is_finite(c[0]) || !"signed_triangle_area(): internal error");
    assert( is_finite(c[1]) || !"signed_triangle_area(): internal error");

    assert( is_number(a[0]) || !"signed_triangle_area(): internal error");
    assert( is_number(a[1]) || !"signed_triangle_area(): internal error");
    assert( is_number(b[0]) || !"signed_triangle_area(): internal error");
    assert( is_number(b[1]) || !"signed_triangle_area(): internal error");
    assert( is_number(c[0]) || !"signed_triangle_area(): internal error");
    assert( is_number(c[1]) || !"signed_triangle_area(): internal error");

    T const result = ((b[0] - a[0]) * (c[1] - a[1]) - (c[0] - a[0]) * (b[1] - a[1])) * VT::half();

    assert( is_finite(result) || !"signed_triangle_area(): internal error");
    assert( is_number(result) || !"signed_triangle_area(): internal error");

    return result;
  }

  class SignedTriangleArea
  {
  public:

    template <typename V>
    inline typename V::value_type operator()(
                                             V const & a
                                             , V const & b
                                             , V const & c
                                             )
    {
      return signed_triangle_area(a,b,c);
    }

  };
  
  template <typename V>
  inline typename V::value_type triangle_area(
                                              V const & a
                                              , V const & b
                                              , V const & c
                                              )
  {
    using std::fabs;

    return fabs( signed_triangle_area(a,b,c));
  }


  class TriangleArea
  {
  public:

    template <typename V>
    inline typename V::value_type operator()(
                                             V const & a
                                             , V const & b
                                             , V const & c
                                             )
    {
      return triangle_area(a,b,c);
    }

  };

}//end of namespace util

// UTIL_TRIANGLE_AREA_H
#endif
