#ifndef UTIL_AREA_EDGE_RATIO_H
#define UTIL_AREA_EDGE_RATIO_H

#include <util_edge_length.h>
#include <util_triangle_area.h>
#include <util_sort.h>
#include <util_is_number.h>
#include <util_is_finite.h>

#include <cassert>
#include <cmath>

namespace util
{


  // 2015-08-04 Kenny: Is this the correct name of this quality measure function?

  template <typename V>
  inline typename V::value_type area_edge_ratio(
                                                  V const & a
                                                  , V const & b
                                                  , V const & c
                                                  )
  {
    using std::sqrt;

    typedef typename V::value_type    T;
    typedef typename V::value_traits VT;

    static const T factor = VT::four() / sqrt(VT::three());

    T const l1 = edge_length(a,b);
    T const l2 = edge_length(b,c);
    T const l3 = edge_length(c,a);

    assert(l1 > VT::zero() || !"area_edge_ratio(): internal error");
    assert(l2 > VT::zero() || !"area_edge_ratio(): internal error");
    assert(l3 > VT::zero() || !"area_edge_ratio(): internal error");
    assert( is_number(l1)  || !"area_edge_ratio(): internal error");
    assert( is_finite(l1)  || !"area_edge_ratio(): internal error");
    assert( is_number(l2)  || !"area_edge_ratio(): internal error");
    assert( is_finite(l2)  || !"area_edge_ratio(): internal error");
    assert( is_number(l3)  || !"area_edge_ratio(): internal error");
    assert( is_finite(l3)  || !"area_edge_ratio(): internal error");


    T lmin = VT::zero();
    T lmed = VT::zero();
    T lmax = VT::zero();

    util::sort<T>(l1, l2, l3, lmin, lmed, lmax);

    assert(lmin > VT::zero() || !"area_edge_ratio(): internal error");
    assert(lmin <= lmed      || !"area_edge_ratio(): internal error");
    assert(lmed <= lmax      || !"area_edge_ratio(): internal error");

    T const area = triangle_area(a,b,c);

    assert( area > VT::zero()|| !"area_edge_ratio(): internal error");
    assert( is_number(area)  || !"area_edge_ratio(): internal error");
    assert( is_finite(area)  || !"area_edge_ratio(): internal error");


    T const ratio = (factor * area) / (lmax * lmed);

    assert( is_number(ratio)  || !"area_edge_ratio(): internal error");
    assert( is_finite(ratio)  || !"area_edge_ratio(): internal error");

    return ratio;
  }

  class AreaEdgeRatio
  {
  public:

    template <typename V>
    inline typename V::value_type operator()(
                                             V const & a
                                             , V const & b
                                             , V const & c
                                             )
    {
      return area_edge_ratio(a,b,c);
    }

  };

}// end namespace util

// UTIL_AREA_EDGE_RATIO_H
#endif
