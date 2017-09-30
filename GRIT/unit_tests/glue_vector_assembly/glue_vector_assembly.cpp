#include <util_tensors.h>
#include <glue_tuple.h>
#include <glue_vector_assembly.h>

#include <vector>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/test_tools.hpp>


BOOST_AUTO_TEST_SUITE(vector_assembly);

BOOST_AUTO_TEST_CASE(test1)
{
  std::vector<glue::Tuple> edges;

  edges.push_back( glue::make_tuple(0u, 1u) );
  edges.push_back( glue::make_tuple(0u, 2u) );
  edges.push_back( glue::make_tuple(1u, 2u) );

  unsigned int const V = 3u;

  std::vector<util::Tensor1> Ae;

  Ae.resize(edges.size());

  Ae[0] = util::make(1.0, 0.0);
  Ae[1] = util::make(2.0, 0.0);
  Ae[2] = util::make(3.0, 0.0);

  {
    bool interlaced = false;

    std::vector<double> y = glue::vector_assembly<double>( edges, V, Ae, interlaced );

    BOOST_CHECK_CLOSE(y[0], 3.0, 0.01);
    BOOST_CHECK_CLOSE(y[1], 4.0, 0.01);
    BOOST_CHECK_CLOSE(y[2], 5.0, 0.01);
    BOOST_CHECK_CLOSE(y[3], 0.0, 0.01);
    BOOST_CHECK_CLOSE(y[4], 0.0, 0.01);
    BOOST_CHECK_CLOSE(y[5], 0.0, 0.01);
  }
  {
    bool interlaced = true;

    std::vector<double> y = glue::vector_assembly<double>( edges, V, Ae, interlaced );

    BOOST_CHECK_CLOSE(y[0], 3.0, 0.01);
    BOOST_CHECK_CLOSE(y[1], 0.0, 0.01);
    BOOST_CHECK_CLOSE(y[2], 4.0, 0.01);
    BOOST_CHECK_CLOSE(y[3], 0.0, 0.01);
    BOOST_CHECK_CLOSE(y[4], 5.0, 0.01);
    BOOST_CHECK_CLOSE(y[5], 0.0, 0.01);
  }
}

BOOST_AUTO_TEST_CASE(test2)
{
  std::vector<glue::Tuple> edges;

  edges.push_back( glue::make_tuple(0u, 1u) );
  edges.push_back( glue::make_tuple(0u, 2u) );
  edges.push_back( glue::make_tuple(1u, 2u) );

  unsigned int const V = 3u;

  std::vector<util::Block2x1Tensor1> Ae;

  Ae.resize(edges.size());

  Ae[0].m_block[0] = util::make(1.0, 0.0);
  Ae[0].m_block[1] = util::make(2.0, 0.0);
  Ae[1].m_block[0] = util::make(1.0, 0.0);
  Ae[1].m_block[1] = util::make(2.0, 0.0);
  Ae[2].m_block[0] = util::make(1.0, 0.0);
  Ae[2].m_block[1] = util::make(2.0, 0.0);

  {
    bool interlaced = false;

    std::vector<double> y = glue::vector_assembly<double>( edges, V, Ae, interlaced );


    BOOST_CHECK_CLOSE(y[0], 2.0, 0.01);
    BOOST_CHECK_CLOSE(y[1], 3.0, 0.01);
    BOOST_CHECK_CLOSE(y[2], 4.0, 0.01);
    BOOST_CHECK_CLOSE(y[3], 0.0, 0.01);
    BOOST_CHECK_CLOSE(y[4], 0.0, 0.01);
    BOOST_CHECK_CLOSE(y[5], 0.0, 0.01);
  }
  
}

BOOST_AUTO_TEST_SUITE_END();
