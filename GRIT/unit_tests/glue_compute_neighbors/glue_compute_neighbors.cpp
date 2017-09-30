#include <glue_tuple.h>
#include <glue_compute_neighbors.h>

#include <vector>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/test_tools.hpp>


BOOST_AUTO_TEST_SUITE(comppute_neighbors);

BOOST_AUTO_TEST_CASE(test1)
{
  std::vector<glue::Tuple> edges;

  edges.push_back( glue::make_tuple(0u, 1u) );
  edges.push_back( glue::make_tuple(0u, 2u) );
  edges.push_back( glue::make_tuple(1u, 2u) );

  unsigned int const V = 3u;

  glue::Neighbors info;

  glue::compute_neighbors(edges, V, info);

  for (unsigned int i =0u; i< info.m_V2T.size(); ++i)
  {
    std::cout << "( " << info.m_V2T[i].first << " , " <<  info.m_V2T[i].second << " ) : ";
  }
  std::cout << std::endl;
  for (unsigned int i =0u; i< info.m_offset.size(); ++i)
  {
    std::cout << info.m_offset[i] << " : ";
  }
  std::cout << std::endl;

  BOOST_CHECK_EQUAL(info.m_V2T.size(), 2*edges.size());
  BOOST_CHECK_EQUAL(info.m_offset.size(), V+1u);

  BOOST_CHECK_EQUAL(info.m_V2T[0].first, 0u);  BOOST_CHECK_EQUAL(info.m_V2T[0].second, 0u);
  BOOST_CHECK_EQUAL(info.m_V2T[1].first, 0u);  BOOST_CHECK_EQUAL(info.m_V2T[1].second, 1u);
  BOOST_CHECK_EQUAL(info.m_V2T[2].first, 1u);  BOOST_CHECK_EQUAL(info.m_V2T[2].second, 0u);
  BOOST_CHECK_EQUAL(info.m_V2T[3].first, 1u);  BOOST_CHECK_EQUAL(info.m_V2T[3].second, 2u);
  BOOST_CHECK_EQUAL(info.m_V2T[4].first, 2u);  BOOST_CHECK_EQUAL(info.m_V2T[4].second, 1u);
  BOOST_CHECK_EQUAL(info.m_V2T[5].first, 2u);  BOOST_CHECK_EQUAL(info.m_V2T[5].second, 2u);

  BOOST_CHECK_EQUAL(info.m_offset[0], 0u);
  BOOST_CHECK_EQUAL(info.m_offset[1], 2u);
  BOOST_CHECK_EQUAL(info.m_offset[2], 4u);
  BOOST_CHECK_EQUAL(info.m_offset[3], 6u);
}


BOOST_AUTO_TEST_SUITE_END();
