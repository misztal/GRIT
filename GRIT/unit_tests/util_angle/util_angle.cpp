#include <OpenTissue/core/math/math_vector3.h>
#include <util_angle.h>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/test_tools.hpp>

typedef double                        T;
typedef OpenTissue::math::Vector3<T>  V;
typedef V::value_traits               VT;


BOOST_AUTO_TEST_SUITE(util_angle);

BOOST_AUTO_TEST_CASE(test1)
{

  V const v0 = V(0.0, 1.0, 0.0);
  V const v1 = V(0.0, 0.0, 0.0);
  V const v2 = V(1.0, 0.0, 0.0);

  T const angle = util::angle(v0,v1,v2);

  BOOST_CHECK_CLOSE(angle, VT::pi_half(), 0.01 );
}

BOOST_AUTO_TEST_CASE(test2)
{

  V const v0 = V(0.0, 1.0, 0.0);
  V const v1 = V(0.0, 0.0, 0.0);
  V const v2 = V(1.0, 1.0, 0.0);

  T const angle = util::angle(v0,v1,v2);

  BOOST_CHECK_CLOSE(angle, VT::pi_quarter(), 0.01 );
}


BOOST_AUTO_TEST_SUITE_END();
