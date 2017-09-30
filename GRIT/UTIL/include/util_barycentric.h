#ifndef UTIL_BARYCENTRIC_H
#define UTIL_BARYCENTRIC_H

#include <util_is_number.h>
#include <util_is_finite.h>

#include <OpenTissue/core/math/math_matrix3x3.h>
#include <OpenTissue/core/math/math_vector3.h>

#include <cmath>
#include <cassert>

namespace util
{
  /*
   * Compute Barycentric Coordinates.
   * This method computes the barycentric coodinates for a point p of an edge
   * given by the points x1 and x2.
   *
   * The barycentric coordinates w1 and w2 are defined such
   * that p' = w1*x1 + w2*x2, is the point on the line closest to p.
   *
   * if 0 <= w1,w2 <= 1 then the point lies inside or on the perimeter of the triangle.
   *
   * @warning  This method uses a geometric approach to compute the barycentric coordinates.
   *
   * @param x1    The first point of the edge.
   * @param x2    The second point of the edge.
   * @param p     The point for which the barycentric coordinates should be computed.
   * @param w1    Upon return this parameter contains the value of the first barycentric coordinate.
   * @param w2    Upon return this parameter contains the value of the second barycentric coordinate.
   */
  template<typename V>
  inline void barycentric(
                          V const & x1
                          , V const & x2
                          , V const & p
                          , typename V::value_type & w1
                          , typename V::value_type & w2
                          )
  {
    using std::sqrt;
    using namespace OpenTissue::math;

    typedef typename V::value_type      T;
    typedef typename V::value_traits    VT;

    V const u  = x2-x1;
    T const uu = inner_prod(u,u);

    assert( is_number(uu)   || !"barycentric(): NaN encountered");
    assert( uu > VT::zero() || !"barycentric(): Degenerate edge encountered");

    // Project p onto edge running from x1 to x2.
    V const q  = (inner_prod(u, p-x1)/ uu )*u + x1;

    V const a  = q - x2;
    V const b  = q - x1;

    T const aa =  inner_prod(a,a) ;
    T const bb =  inner_prod(b,b) ;

    assert( is_number(aa) || !"barycentric(): NaN encountered");
    assert( is_number(bb) || !"barycentric(): NaN encountered");

    w1 = ( inner_prod(u,a) <= VT::zero() ) ? sqrt( aa  / uu ) : - sqrt( aa  / uu );
    w2 = ( inner_prod(u,b) >= VT::zero() ) ? sqrt( bb  / uu ) : - sqrt( bb  / uu );

    assert( is_number(w1) || !"barycentric(): NaN encountered");
    assert( is_number(w2) || !"barycentric(): NaN encountered");
  }

  /*
   * Compute Barycentric Coordinates.
   * This method computes the barycentric coodinates for a point p of a triangle
   * given by the points x1,x2, and x3 (in counter clockwise order).
   *
   * The barycentric coordinates w1,w2, and w3 are defined such
   * that p' = w1*x1 + w2*x2 + w3*x3, is the point in plane of the
   * triangle closest to p.
   *
   * if 0 <= w1,w2,w3 <= 1 then the point lies inside or on the perimeter of the triangle.
   *
   * @warning  This method uses a geometric approach to compute the barycentric coordinates.
   *
   * @param x1    The first point of the triangle.
   * @param x2    The second point of the triangle.
   * @param x3    The third point of the triangle.
   * @param p     The point for which the barycentric coordinates should be computed.
   * @param w1    Upon return this parameter contains the value of the first barycentric coordinate.
   * @param w2    Upon return this parameter contains the value of the second barycentric coordinate.
   * @param w3    Upon return this parameter contains the value of the third barycentric coordinate.
   */
  template<typename V>
  inline void barycentric(
                           V const & x1
                           , V const & x2
                           , V const & x3
                           , V const & p
                           , typename V::value_type & w1
                           , typename V::value_type & w2
                           , typename V::value_type & w3
                           )
  {
    using std::max;
    using std::sqrt;
    using std::fabs;
    using namespace OpenTissue::math;

    typedef typename V::value_type                  T;
    typedef typename V::value_traits                VT;

    V const bc = (x1+x2+x3)/3.;
    V const v1 = x1-bc;
    V const v2 = x2-bc;
    V const v3 = x3-bc;

    T const l1 = length(v1);
    T const l2 = length(v2);
    T const l3 = length(v3);

    assert( is_number(l1)    || !"barycentric(): NaN encountered");
    assert( is_number(l2)    || !"barycentric(): NaN encountered");
    assert( is_number(l3)    || !"barycentric(): NaN encountered");
    assert( is_finite(l1)    || !"barycentric(): Inf encountered");
    assert( is_finite(l2)    || !"barycentric(): Inf encountered");
    assert( is_finite(l3)    || !"barycentric(): Inf encountered");
    assert( l1 >= VT::zero() || !"barycentric(): norm was negative");
    assert( l2 >= VT::zero() || !"barycentric(): norm was negative");
    assert( l3 >= VT::zero() || !"barycentric(): norm was negative");

    T const r  = max( max( l1, l2), l3);

    assert( r > VT::zero() || !"barycentric(): norm was non-positive");

    V const y1 = v1/r;
    V const y2 = v2/r;
    V const y3 = v3/r;

    V const pp = (p-bc)/r;

    // 2016-05-26 Marek: adding pivoting
    // 2015-05-23 Marek: this is an alternative way of computing barycentric coordinates,
    //                   but I'm not sure if it's better.

    V const b1 = y2 - y1;
    V const b2 = y3 - y1;
    V const b3 = cross( unit(b1), unit(b2));

    V const n  = unit(b3);

    T const a1 = dot( n, cross( y3-y2, pp-y2));
    T const a2 = dot( n, cross( y1-y3, pp-y3));
    T const a3 = dot( n, cross( y2-y1, pp-y1));

    T const a  = dot( n, cross( y2-y1, y3-y1));

    assert( a!=VT::zero() || !"barycentric(): division by 0");

    T const ab1 = fabs(a1);
    T const ab2 = fabs(a2);
    T const ab3 = fabs(a3);

    if( ab1 <= ab2 && ab1 <= ab3)
    {
      w2 = a2/a;
      w3 = a3/a;
      w1 = VT::one()-w2-w3;
    }
    else if( ab2 <= ab1 && ab2 <= ab3)
    {
      w1 = a1/a;
      w3 = a3/a;
      w2 = VT::one()-w1-w3;
    }
    else
    {
      w1 = a1/a;
      w2 = a2/a;
      w3 = VT::one()-w1-w2;
    }

    assert( is_number(w1) || !"barycentric(): NaN encountered");
    assert( is_number(w2) || !"barycentric(): NaN encountered");
    assert( is_number(w3) || !"barycentric(): NaN encountered");
  }

  /*
   * Compute Barycentric Coordinates.
   * This method computes the barycentric coodinates for a point p of a tetrahedron
   * given by the points x1,x2,x3, x4 (in right-hand order).
   *
   * @warning  This method uses a geometric approach to compute the barycentric coordinates.
   *
   * @param x1    The first point of the triangle.
   * @param x2    The second point of the triangle.
   * @param x3    The third point of the triangle.
   * @param x4    The fourth point of the triangle.
   * @param p     The point for which the barycentric coordinates should be computed.
   * @param w1    Upon return this parameter contains the value of the first barycentric coordinate.
   * @param w2    Upon return this parameter contains the value of the second barycentric coordinate.
   * @param w3    Upon return this parameter contains the value of the third barycentric coordinate.
   * @param w4    Upon return this parameter contains the value of the fourth barycentric coordinate.
   */
  template<typename V>
  inline void barycentric(
                           V const & x1
                           , V const & x2
                           , V const & x3
                           , V const & x4
                           , V const & p
                           , typename V::value_type & w1
                           , typename V::value_type & w2
                           , typename V::value_type & w3
                           , typename V::value_type & w4
                           )
  {
    using std::fabs;
    using namespace OpenTissue::math;

    typedef typename V::value_type                  T;
    typedef typename V::value_traits                VT;
    typedef          OpenTissue::math::Matrix3x3<T> M;

    V const b1 = x2 - x1;
    V const b2 = x3 - x1;
    V const b3 = x4 - x1;

    // check if basis forms a right hand coordsys, if not exchange two base vectors
    if(inner_prod(b3, cross(b1, b2)) > 0)
    {
      M const basis = M(  b1(0), b2(0), b3(0)
                        , b1(1), b2(1), b3(1)
                        , b1(2), b2(2), b3(2));

      // q is the point p transformed to the isoparametric system
      V const q = inverse(basis)*(p - x1);

      w1 = VT::one() - q[0] - q[1] - q[2];
      w2 = q[0];
      w3 = q[1];
      w4 = q[2];

    }
    else
    {
      M const  basis = M(  b1(0), b3(0), b2(0)
                         , b1(1), b3(1), b2(1)
                         , b1(2), b3(2), b2(2));

      // q is the point p transformed to the isoparametric system
      V const q = inverse(basis)*(p - x1);

      w1 = VT::one() - q[0] - q[1] - q[2];
      w2 = q[0];
      w3 = q[2];// we alter the sequence of barycentric coords to match altered basis
      w4 = q[1];
    }

    assert( is_number(w1) || !"barycentric(): NaN encountered");
    assert( is_number(w2) || !"barycentric(): NaN encountered");
    assert( is_number(w3) || !"barycentric(): NaN encountered");
    assert( is_number(w4) || !"barycentric(): NaN encountered");
  }

}//namespace util

// UTIL_BARYCENTRIC_H
#endif
