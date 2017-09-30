#include <util_find_index.h>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/test_tools.hpp>



BOOST_AUTO_TEST_SUITE(find_index);

BOOST_AUTO_TEST_CASE(test)
{

  std::vector<int> array;

  array.push_back( 10 );
  array.push_back( 20 );
  array.push_back( 30 );

  unsigned int i0 = util::find_index(10, array );
  unsigned int i1 = util::find_index(20, array );
  unsigned int i2 = util::find_index(30, array );
  unsigned int i3 = util::find_index(40, array );


  BOOST_CHECK_EQUAL(i0, 0u);
  BOOST_CHECK_EQUAL(i1, 1u);
  BOOST_CHECK_EQUAL(i2, 2u);
  BOOST_CHECK_EQUAL(i3, 0xFFFFFFFF);
}

BOOST_AUTO_TEST_SUITE_END();
