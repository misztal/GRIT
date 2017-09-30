#include <OpenTissue/core/math/math_vector3.h>

#include <util_barycentric.h>
#include <util_barycentric_inside.h>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/test_tools.hpp>

typedef double                       T;
typedef OpenTissue::math::Vector3<T> V;


BOOST_AUTO_TEST_SUITE(barycentric);

BOOST_AUTO_TEST_CASE(line_inside)
{
  V x1 = V( 0.0, 0.0, 0.0);
  V x2 = V(1.0, 0.0, 0.0);
  V p  =  0.55 * x1 + 0.45 * x2;

  T w1 = 0.0;
  T w2 = 0.0;

  util::barycentric(x1, x2, p, w1, w2);

  BOOST_CHECK_CLOSE(w1, 0.55, 0.01);
  BOOST_CHECK_CLOSE(w2, 0.45, 0.01);

  BOOST_CHECK( util::barycentric_inside(w1, w2));
}


BOOST_AUTO_TEST_CASE(triangle_inside)
{
  V x1 = V(0.0, 0.0, 0.0);
  V x2 = V(1.0, 0.0, 0.0);
  V x3 = V(0.0, 1.0, 0.0);
  V p  =  0.5 * x1 + 0.25 * x2+ 0.25 * x3;

  T w1 = 0.0;
  T w2 = 0.0;
  T w3 = 0.0;

  util::barycentric(x1, x2, x3, p, w1, w2, w3);

  BOOST_CHECK_CLOSE(w1, 0.5, 0.01);
  BOOST_CHECK_CLOSE(w2, 0.25, 0.01);
  BOOST_CHECK_CLOSE(w3, 0.25, 0.01);

  BOOST_CHECK( util::barycentric_inside(w1, w2, w3));
}


BOOST_AUTO_TEST_CASE(tetrahedron_inside)
{
  V x1 = V(0.0, 0.0, 0.0);
  V x2 = V(1.0, 0.0, 0.0);
  V x3 = V(0.0, 1.0, 0.0);
  V x4 = V(0.0, 0.0, 1.0);
  V p  =  0.4 * x1 + 0.3 * x2+ 0.2 * x3 + 0.1 * x4;

  T w1 = 0.0;
  T w2 = 0.0;
  T w3 = 0.0;
  T w4 = 0.0;

  util::barycentric(x1, x2, x3, x4, p, w1, w2, w3, w4);

  BOOST_CHECK_CLOSE(w1, 0.4, 0.01);
  BOOST_CHECK_CLOSE(w2, 0.3, 0.01);
  BOOST_CHECK_CLOSE(w3, 0.2, 0.01);
  BOOST_CHECK_CLOSE(w4, 0.1, 0.01);

  BOOST_CHECK( util::barycentric_inside(w1, w2, w3, w4));
}


BOOST_AUTO_TEST_CASE(line_outside)
{
  V x1 = V(1.0, 0.0, 0.0);
  V x2 = V(2.0, 0.0, 0.0);
  V p  = -1.0 * x1 + 2.0*x2;

  T w1 = 0.0;
  T w2 = 0.0;

  util::barycentric(x1, x2, p, w1, w2);

  BOOST_CHECK_CLOSE(w1, -1.0, 0.01);
  BOOST_CHECK_CLOSE(w2,  2.0, 0.01);

  BOOST_CHECK( ! util::barycentric_inside(w1, w2));
}


BOOST_AUTO_TEST_CASE(triangle_outside)
{
  V x1 = V(0.0, 0.0, 0.0);
  V x2 = V(1.0, 0.0, 0.0);
  V x3 = V(0.0, 1.0, 0.0);
  V p  = V(1.0, 1.0, 0.0);

  T w1 = 0.0;
  T w2 = 0.0;
  T w3 = 0.0;

  util::barycentric(x1, x2, x3, p, w1, w2, w3);

  BOOST_CHECK_CLOSE(w1, -1.0, 0.01);
  BOOST_CHECK_CLOSE(w2,  1.0, 0.01);
  BOOST_CHECK_CLOSE(w3,  1.0, 0.01);

  BOOST_CHECK( ! util::barycentric_inside(w1, w2, w3));
}


BOOST_AUTO_TEST_CASE(tetrahedron_outside)
{
  V x1 = V(0.0, 0.0, 0.0);
  V x2 = V(1.0, 0.0, 0.0);
  V x3 = V(0.0, 1.0, 0.0);
  V x4 = V(0.0, 0.0, 1.0);
  V p  = V(1.0, 1.0, 1.0);;

  T w1 = 0.0;
  T w2 = 0.0;
  T w3 = 0.0;
  T w4 = 0.0;

  util::barycentric(x1, x2, x3, x4, p, w1, w2, w3, w4);

  BOOST_CHECK_CLOSE(w1, -2.0, 0.01);
  BOOST_CHECK_CLOSE(w2,  1.0, 0.01);
  BOOST_CHECK_CLOSE(w3,  1.0, 0.01);
  BOOST_CHECK_CLOSE(w4,  1.0, 0.01);

  BOOST_CHECK( ! util::barycentric_inside(w1, w2, w3, w4));
}



BOOST_AUTO_TEST_SUITE_END();
