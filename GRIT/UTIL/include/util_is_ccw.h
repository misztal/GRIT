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
    // Transform to barycentric system of coordinates
	V const vc = (v0 + v1 + v2) / 3.;

	V const w0 = v0 - vc;
	V const w1 = v1 - vc;
	V const w2 = v2 - vc;

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
    return (w0[0] * w1[1]) + (w1[0] * w2[1]) +
           (w2[0] * w0[1]) - (w1[1] * w2[0]) -
           (w2[1] * w0[0]) - (w0[1] * w1[0]) > 0;
  }

}//end of namespace util

// UTIL_IS_CCW_H
#endif
