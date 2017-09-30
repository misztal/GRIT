#ifndef UTIL_MAXMIN_EDGE_RATIO_H
#define UTIL_MAXMIN_EDGE_RATIO_H

#include <util_edge_length.h>
#include <util_sort.h>
#include <util_is_number.h>
#include <util_is_finite.h>

#include <cassert>

namespace util
{

  template <typename V>
  inline typename V::value_type maxmin_edge_ratio(
                                                  V const & a
                                                  , V const & b
                                                  , V const & c
                                                  )
  {
    typedef typename V::value_type   T;
    typedef typename V::value_traits VT;

    T const l1 = edge_length(a,b);
    T const l2 = edge_length(b,c);
    T const l3 = edge_length(c,a);

    assert(l1 > VT::zero() || !"maxmin_edge_ratio(): internal error");
    assert(l2 > VT::zero() || !"maxmin_edge_ratio(): internal error");
    assert(l3 > VT::zero() || !"maxmin_edge_ratio(): internal error");
    assert( is_number(l1)  || !"maxmin_edge_ratio(): internal error");
    assert( is_finite(l1)  || !"maxmin_edge_ratio(): internal error");
    assert( is_number(l2)  || !"maxmin_edge_ratio(): internal error");
    assert( is_finite(l2)  || !"maxmin_edge_ratio(): internal error");
    assert( is_number(l3)  || !"maxmin_edge_ratio(): internal error");
    assert( is_finite(l3)  || !"maxmin_edge_ratio(): internal error");

    T lmin = VT::zero();
    T lmed = VT::zero();
    T lmax = VT::zero();

    util::sort<T>(l1, l2, l3, lmin, lmed, lmax);

    assert(lmin > VT::zero() || !"maxmin_edge_ratio(): internal error");
    assert(lmin <= lmed      || !"maxmin_edge_ratio(): internal error");
    assert(lmed <= lmax      || !"maxmin_edge_ratio(): internal error");

    T const ratio = (lmed / lmin);

    assert( is_number(ratio)  || !"maxmin_edge_ratio(): internal error");
    assert( is_finite(ratio)  || !"maxmin_edge_ratio(): internal error");
    
    return ratio;
  }

  class MaxMinEdgeRatio
  {
  public:

    template <typename V>
    inline typename V::value_type operator()(
                                             V const & a
                                             , V const & b
                                             , V const & c
                                             )
    {
      return maxmin_edge_ratio(a,b,c);
    }

  };

}// end namespace util

// UTIL_MAXMIN_EDGE_RATIO_H
#endif
