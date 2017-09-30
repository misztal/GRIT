#include <glue.h>
#include <util.h>   // needed for util::get_data_file_path
#include <grit.h>

#include <vector>
#include <string>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/test_tools.hpp>


//
// Test mesh, A's are phae 1, all other triangles are phase 0
//
//16 *---*---*---* 19
//   |\  |  /|  /|
//   | \ | / | / |
//   |  \|/  |/  |
//12 *---*---*---* 15
//   |  /|a /| a/|
//   | / | / | / |
//   |/  |/ a|/ a|
// 8 *---*---*---* 11
//   |  /|a /|a /|
//   | / | / | / |
//   |/  |/ a|/ a|
// 4 *---*---*---* 7
//   |  /|  /|\  |
//   | / | / | \ |
//   |/  |/  |  \|
//   *---*---*---*
//   0   1   2   3


BOOST_AUTO_TEST_SUITE(glue);

BOOST_AUTO_TEST_CASE(make_adjacent_test)
{
  grit::engine2d_type  engine;
  grit::param_type     parameters;

  std::string txt_filename = "unit_test.txt";

  grit::init_engine_with_mesh_file( util::get_data_file_path(txt_filename),parameters, engine);

  std::vector<grit::Simplex2> known_domain;

  known_domain.resize(8u);



  glue::Phase const phase = glue::make_phase(engine, 1u );

  std::vector<glue::Triplet> adjacent;

  glue::make_adjacency( engine, phase, adjacent );


  BOOST_CHECK_EQUAL( adjacent.size(), 8u );


}


BOOST_AUTO_TEST_SUITE_END();
