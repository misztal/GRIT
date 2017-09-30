#ifndef UTIL_IS_CCW_H
#define UTIL_IS_CCW_H

namespace util
{
  template <typename V>
  inline bool is_ccw(
                       V const & v0
                     , V const & v1
                     , V const & v2
                     )
  {
    //  The (double) signed area is by definition given as
    //
    //  d =(v1-v0) x (v2-v1)
    //
    //       | (v1x-v0x) |    | (v2x-v1x) |
    //  d =  | (v1y-v0y) ]  X | (v2y-v1y) |  =  (v1x-v0x)(v2y-v1y) - (v2x-v1x)(v1y-v0y)
    //  d =  v1x v2y - v0x v2y + v0x v1y - v1x v1y + v1x v1y - v2x v1y + v2x v0y - v1x v0y
    //  d =  v1x v2y - v0x v2y + v0x v1y - v2x v1y + v2x v0y - v1x v0y
    //
    //  if d>0 we have positive area and this is the same as ccw order
    //
    return (v0[0] * v1[1]) + (v1[0] * v2[1]) +
           (v2[0] * v0[1]) - (v1[1] * v2[0]) -
           (v2[1] * v0[0]) - (v0[1] * v1[0]) > 0;
  }

}//end of namespace util

// UTIL_IS_CCW_H
#endif
