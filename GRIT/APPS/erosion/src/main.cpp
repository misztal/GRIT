#include <erosion.h>

#include <grit.h>
#include <glue.h>
#include <util.h>


grit::engine2d_type  engine;
grit::param_type     parameters;

util::ConfigFile      settings;
util::Log             logging;

std::string const newline = util::Log::newline();
std::string const tab     = util::Log::tab();

double core_radius = 0.0;
double core_x      = 0.0;
double core_y      = 0.0;





void write_svg_files(
                     std::string const & output_path
                     , unsigned int const & frame_number
                     )
{
  std::string const filename = output_path + util::generate_filename("/erosion", frame_number, "svg");

  glue::svg_draw(filename, engine, parameters);

  logging << tab << tab << "write_svg_files() Done writting  " << filename << newline;
}




void init_simulation()
{
  using std::sqrt;

  engine.update(parameters);

  logging << tab << "Initialization pre-updated the mesh" << newline;

  unsigned int const object_label    = util::to_value<unsigned int>(settings.get_value("object_label","1"));

  erosion::CoreObject core = erosion::make_core_object(
                                                           engine
                                                           , object_label
                                                           , false  // We do not need normal information for initialization
                                                           );

  erosion::initialize_core_geometry(
                                      core
                                      , core_radius
                                      , core_x
                                      , core_y
                                      );

  glue::set_sub_range_target(
                             engine
                             , core.m_interface
                             , core.m_interface_x
                             , core.m_interface_y
                             , true
                             );

  engine.update(parameters);

  logging << tab << "Initialization post updated the mesh" << newline;
}

void do_simulation_step(double const & core_radius)
{
  unsigned int        const object_label    = util::to_value<unsigned int>(settings.get_value("object_label","1")      );
  static double       const mu              = util::to_value<double>(      settings.get_value("mu","0.01")             );

  erosion::CoreObject core = erosion::make_core_object(engine, object_label);

/*
  erosion::compute_growth(
                            core
                            , mu
                            , core_radius
                            , core_x
                            , core_y
                            );
                            */

  erosion::compute_offsets(
                            engine
                            , core
                            , mu
                            , core_radius
                            , core_x
                            , core_y
                            );

  glue::set_sub_range_target(
                             engine
                             , core.m_interface
                             , core.m_interface_x
                             , core.m_interface_y
                             , true
                             );
}


int main()
{
  if(!settings.load("erosion.cfg"))
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
    logging << tab << "Simulating..." << newline;

    do_simulation_step(core_radius);
    logging << tab << "Simulation step " << i << " done..." << newline;

    engine.update(parameters);
    logging << tab << "Updated the mesh" << newline;

    write_svg_files(output_path, i);
    logging << tab << "Wrote svg file for frame " << i << newline;
  }

  logging << "Done" << newline;

  return 0;
}
