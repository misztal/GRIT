#include <configuration.h>

#include <area_compute_maximize_area.h>
#include <area_compute_constant_perimeter.h>
#include <area_compute_perimeter.h>

#include <grit.h>
#include <glue_svg_draw.h>
#include <util.h>


grit::engine2d_type  engine;
grit::param_type     parameters;

util::ConfigFile      settings;
util::Log             logging;

std::string const newline = util::Log::newline();
std::string const tab     = util::Log::tab();

double c0 = 100.0;   // The desired perimeter length of the phase

void write_svg_files(
                     std::string const & output_path
                     , unsigned int const & frame_number
                     )
{
  std::string const filename = output_path + util::generate_filename("/area", frame_number, "svg");

  glue::svg_draw(filename, engine, parameters);

  logging << tab << tab << "write_svg_files() Done writting  " << filename << newline;
}

void init_simulation()
{
  unsigned int const object_label    = util::to_value<unsigned int>(settings.get_value("object_label","1"));

  glue::Phase const boundary = glue::make_boundary_of_phase(engine, object_label);

  std::vector<double> px;
  std::vector<double> py;

  glue::get_sub_range_current(engine, boundary, px, py );

  c0 = area::compute_perimeter(boundary.m_edges, px, py);
}

void do_simulation_step_area()
{
  unsigned int const object_label    = util::to_value<unsigned int>(settings.get_value("object_label","1"));

  glue::Phase const phase    = glue::make_phase(engine, object_label);

  std::vector<double> px;
  std::vector<double> py;

  glue::get_sub_range_current(engine, phase, px, py );

  std::vector<double> px_new;
  std::vector<double> py_new;

  static unsigned int const max_area_iterations      = util::to_value<unsigned int>(settings.get_value("max_area_iterations"));
  static double       const alpha                    = util::to_value<double>(settings.get_value("alpha"));

  area::compute_maximize_area(
                             phase
                             , px, py
                             , px_new, py_new
                             , max_area_iterations
                             , alpha
                             );


  glue::set_sub_range_target(engine, phase, px_new, py_new);
}

void do_simulation_step_boundary()
{
  unsigned int const object_label    = util::to_value<unsigned int>(settings.get_value("object_label","1"));

  glue::Phase const boundary = glue::make_boundary_of_phase(engine, object_label);

  std::vector<double> px;
  std::vector<double> py;

  glue::get_sub_range_current(engine, boundary, px, py );

  std::vector<double> px_new;
  std::vector<double> py_new;

  static unsigned int const max_perimeter_iterations = util::to_value<unsigned int>(settings.get_value("max_perimeter_iterations"));
  static double       const beta                     = util::to_value<double>(settings.get_value("beta"));
  static double       const c_threshold              = util::to_value<double>(settings.get_value("c_threshold"));

  area::compute_constant_perimeter(
                                   boundary
                                   , px, py
                                   , px_new, py_new
                                   , max_perimeter_iterations
                                   , beta
                                   , c0
                                   , c_threshold
                                   );

  glue::set_sub_range_target(engine, boundary, px_new, py_new);
}


int main()
{
  if(!settings.load("area_maximization.cfg"))
  {
    return 1;
  }

  std::string  const txt_filename = settings.get_value("txt_filename",  "circle_enright.txt");
  std::string  const output_path  = settings.get_value("output_path",   ""                  );
  unsigned int const max_steps    = util::to_value<unsigned int>(settings.get_value("steps","100"));

  util::LogInfo::on()            = util::to_value<bool>(settings.get_value("logging","true"));
  util::LogInfo::console()       = util::to_value<bool>(settings.get_value("console","true"));
  util::LogInfo::filename()      = output_path + "/" + settings.get_value("log_file","log.txt");

  logging << "### " << util::timestamp() << newline;

  parameters = grit::make_parameters_from_config_file(settings);

  if (!grit::is_valid(parameters))
  {
    logging << "main() ERROR: Invalid parameters - check the settings files for errors." << newline;
    return 1;
  }

  grit::init_engine_with_mesh_file(
                                       util::get_data_file_path(txt_filename)
                                       , parameters,engine
                                       );

  init_simulation();

  write_svg_files(output_path, 0);
  logging << tab << "Wrote svg file for frame " << 0 << newline;

  for (unsigned int i = 1; i <= max_steps; ++i)
  {
    do_simulation_step_area();
    logging << tab << "Area Simulation step " << i << " done..." << newline;

    engine.update(parameters);
    logging << tab << "Updated the mesh" << newline;

    do_simulation_step_boundary();
    logging << tab << "Boundary Simulation step " << i << " done..." << newline;

    engine.update(parameters);
    logging << tab << "Updated the mesh" << newline;

    write_svg_files(output_path, i);
    logging << tab << "Wrote svg file for frame " << i << newline;
  }

  logging << "Done" << newline;

  return 0;
}
