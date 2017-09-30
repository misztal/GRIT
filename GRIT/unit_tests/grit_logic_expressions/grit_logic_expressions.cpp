#include <grit.h>
#include <util.h>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/test_tools.hpp>


template<typename E>
bool test_func( grit::LogicExpression<E> const & condition, grit::Simplex0 const & s)
{
  return condition(s);
}

template<typename E>
bool test_func( grit::LogicExpression<E> const & condition, grit::Simplex1 const & s)
{
  return condition(s);
}

template<typename E>
bool test_func( grit::LogicExpression<E> const & condition, grit::Simplex2 const & s)
{
  return condition(s);
}

BOOST_AUTO_TEST_SUITE(grit);

//
// Unit test mesh, A's are phase 1, all other triangles are phase 0
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
//


BOOST_AUTO_TEST_CASE(logic_expression_testing)
{
  grit::engine2d_type        engine;
  grit::param_type           parameters;
  std::string           const txt_filename = util::get_data_file_path("unit_test.txt");

  grit::init_engine_with_mesh_file(txt_filename,parameters,engine);

  // 2015-12-04 Kenny code review: This unit test will crash with expections... it needs some data into the mesh before it makes sense to use it...

  grit::SimplexSet const all = engine.mesh().get_all_simplices();

  grit::Simplex0 const s0  = *(all.begin0());
  grit::Simplex1 const s1  = *(all.begin1());
  grit::Simplex2 const s2  = *(all.begin2());

  BOOST_CHECK_EQUAL( test_func( grit::Bool(true) &&  grit::Bool(true), s0)    , true  );
  BOOST_CHECK_EQUAL( test_func( grit::Bool(false) ||  grit::Bool(true), s1)   , true  );
  BOOST_CHECK_EQUAL( test_func( !grit::Bool(true), s2)                         , false );

  unsigned int label = engine.mesh().label(s2);
  
  BOOST_CHECK_EQUAL( test_func( grit::InPhase(engine.mesh(),label) , s2)       , true  );
  BOOST_CHECK_EQUAL( test_func( grit::InPhase(engine.mesh(),label+1) , s2)     , false );

  BOOST_CHECK_EQUAL( test_func( grit::IsDimension(engine.mesh(), 0) , s0), true );
  BOOST_CHECK_EQUAL( test_func( grit::IsDimension(engine.mesh(), 1) , s1), true );
  BOOST_CHECK_EQUAL( test_func( grit::IsDimension(engine.mesh(), 2) , s2), true );

  BOOST_CHECK_EQUAL( test_func( grit::IsDimension(engine.mesh(), 1) , s0), false );
  BOOST_CHECK_EQUAL( test_func( grit::IsDimension(engine.mesh(), 2) , s1), false );
  BOOST_CHECK_EQUAL( test_func( grit::IsDimension(engine.mesh(), 0) , s2), false );

  BOOST_CHECK_EQUAL( test_func( grit::IsDimension(engine.mesh(), 2) , s0), false );
  BOOST_CHECK_EQUAL( test_func( grit::IsDimension(engine.mesh(), 0) , s1), false );
  BOOST_CHECK_EQUAL( test_func( grit::IsDimension(engine.mesh(), 1) , s2), false );

  BOOST_CHECK_EQUAL( test_func( grit::IsManifold(engine.mesh()) , s0), true );
  BOOST_CHECK_EQUAL( test_func( grit::IsManifold(engine.mesh()) , s1), true );
  BOOST_CHECK_EQUAL( test_func( grit::IsManifold(engine.mesh()) , s2), true );

  BOOST_CHECK_EQUAL( test_func( grit::IsValid(engine.mesh()) , s0), true);
  BOOST_CHECK_EQUAL( test_func( grit::IsValid(engine.mesh()) , s1), true);
  BOOST_CHECK_EQUAL( test_func( grit::IsValid(engine.mesh()) , s2), true);

  BOOST_CHECK_EQUAL( test_func( grit::IsBoundary(engine.mesh()) , s0), true);
  BOOST_CHECK_EQUAL( test_func( grit::IsBoundary(engine.mesh()) , s1), true);
  //test_func( grit::IsBoundary(engine.mesh()) , s2);

  BOOST_CHECK_EQUAL( test_func( grit::IsInterface(engine.mesh()) , s0), false);
  BOOST_CHECK_EQUAL( test_func( grit::IsInterface(engine.mesh()) , s1), false);
  //test_func( grit::IsInterface(mesh) , s2);

  BOOST_CHECK_EQUAL( test_func( grit::IsSubmeshBoundary(engine.mesh()) , s0), false);
  BOOST_CHECK_EQUAL( test_func( grit::IsSubmeshBoundary(engine.mesh()) , s1), false);
  BOOST_CHECK_EQUAL( test_func( grit::IsSubmeshBoundary(engine.mesh()) , s2), false);

}

BOOST_AUTO_TEST_SUITE_END();


