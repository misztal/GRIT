#include <grit.h>
#include <glue.h>  // needed for svg_draw
#include <util.h>  // needed for util::get_data_file_path

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/test_tools.hpp>

inline void test_data(std::string const & txt_filename)
{
  grit::engine2d_type  engine;
  grit::param_type     parameters;

  std::string const full_filename =  util::get_data_file_path(txt_filename);

  BOOST_CHECK_NO_THROW( grit::init_engine_with_mesh_file(full_filename,parameters,engine) );

  {
    std::string const svg_filename = full_filename + ".svg";
    std::vector<unsigned int> labels;
    grit::compute_phase_labels(engine.mesh(), labels);
    for(unsigned int i=0u; i< labels.size();++i)
    {
      parameters.add_label(labels[i]);
    }
    glue::svg_draw(svg_filename, engine, parameters);
  }
}

BOOST_AUTO_TEST_SUITE(grit);

BOOST_AUTO_TEST_CASE(test_mesh_data_files)
{
  test_data( "circle.txt"                     );  // has an ear
  test_data( "crescent.txt"                   );  // has an ear
  test_data( "filled.txt"                     );  // has an ear
  test_data( "filled2.txt"                    );  // has an ear
  test_data( "filled3.txt"                    );  // has an ear
  test_data( "hollow_circle.txt"              );  // has an ear
  test_data( "JeppeSandbox.txt"               );  // has an ear
  test_data( "rectangle.txt"                  );  // has an ear
  test_data( "rectangle_refined.txt"          );  // has an ear
  test_data( "simple_rectangle.txt"           );  // has an ear, has triangles with negative orientation
  test_data( "smaller_circle.txt"             );  // has an ear
  test_data( "small_circle.txt"               );  // has an ear
  test_data( "small_circles.txt"              );  // has an ear
  test_data( "small_circle_larger_domain.txt" );  // has an ear
  test_data( "small_magnets.txt"              );  // has an ear
  test_data( "small_rectangle.txt"            );  // has an ear
  test_data( "small_rectangles.txt"           );  // has an ear
  test_data( "unit_coarse.txt"                );  // has an ear
  test_data( "circle_enright.txt"             );
  test_data( "horseshoe.txt"                  );
  test_data( "magnet.txt"                     );
  test_data( "refined_circle.txt"             );
  test_data( "unit_test.txt"                  );
  test_data( "zalesak.txt"                    );
  test_data( "two_rectangles.txt"             );
  test_data( "breast.msh"                     ); // has triangles with negative orientation
  test_data( "two_rectangles.msh"             );
}

BOOST_AUTO_TEST_SUITE_END();


