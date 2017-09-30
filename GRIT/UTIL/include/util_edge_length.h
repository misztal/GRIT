#ifndef UTIL_EDGE_LENGTH_H
#define UTIL_EDGE_LENGTH_H

#include <util_is_finite.h>
#include <util_is_number.h>

#include <cassert>
#include <cmath>   // needed for std::sqrt

namespace util
{

  template <typename V>
  inline typename V::value_type squared_edge_length(
                                                      V const & a
                                                    , V const & b
                                                    )
  {
    typedef typename V::value_type   T;

    T const dx = (b[0] - a[0]);
    T const dy = (b[1] - a[1]);

    assert( is_finite(dx) || !"squared_edge_length(): internal error");
    assert( is_finite(dy) || !"squared_edge_length(): internal error");
    assert( is_number(dx) || !"squared_edge_length(): internal error");
    assert( is_number(dy) || !"squared_edge_length(): internal error");

    return  dx*dx+dy*dy;
  }

  class SquaredEdgeLength
  {
  public:

    template <typename V>
    inline typename V::value_type operator()(
                                             V const & a
                                             , V const & b
                                             )
    {
      return squared_edge_length(a,b);
    }

  };

  template <typename V>
  inline typename V::value_type edge_length(
                                              V const & a
                                              , V const & b
                                              )
  {
    using std::sqrt;

    return sqrt( squared_edge_length(a,b));
  }


  class EdgeLength
  {
  public:

    template <typename V>
    inline typename V::value_type operator()(
                                             V const & a
                                             , V const & b
                                             )
    {
      return edge_length(a,b);
    }

  };

}//end of namespace util

// UTIL_EDGE_LENGTH_H
#endif
