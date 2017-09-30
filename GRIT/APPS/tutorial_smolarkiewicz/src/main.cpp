#include <grit.h>
#include <glue.h>
#include <util.h>

grit::engine2d_type  engine;
grit::param_type     parameters;

util::ConfigFile      settings;
util::Log             logging;

std::string const newline = util::Log::newline();
std::string const tab     = util::Log::tab();


void write_svg_files(
                     std::string const & output_path
                     , unsigned int const & frame_number
                     )
{
  std::string const filename = output_path + util::generate_filename("/tutorial", frame_number, "svg");

  glue::svg_draw(filename, engine, parameters);

  logging << tab << tab << "write_svg_files() Done writting  " << filename << newline;
}

void do_simulation_step()
{
  using std::fabs;
  using std::sin;
  using std::cos;

  unsigned int const object_label    = util::to_value<unsigned int>(settings.get_value("object_label","1"));

  glue::Phase const phase    = glue::make_phase(engine, object_label);

  std::vector<double> px;
  std::vector<double> py;

  glue::get_sub_range_current(engine, phase, px, py );

  std::vector<double> px_new;
  std::vector<double> py_new;

  double const pi = M_PI;

  unsigned int const N = px.size();

  px_new.resize(N);      ///< new x-coordinate
  py_new.resize(N);      ///< new x-coordinate

  double const dt = 0.01;

  for( unsigned int n = 0; n < N; ++n)
  {
    double const x = px[n];
    double const y = py[n];

    double u = - sin(4.0 * pi * (x + 0.5 )) * sin( 4.0*pi*(y + 0.5 ));
    double v =  -cos(4.0 * pi * (x + 0.5 )) * cos( 4.0*pi*(y + 0.5 ));

    px_new[n] = px[n] + u*dt;
    py_new[n] = py[n] + v*dt;
  }

  glue::set_sub_range_target(engine, phase, px_new, py_new);
}

int main()
{
  if(!settings.load("tutorial_smolarkiewicz.cfg"))
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

  write_svg_files(output_path, 0u);
  logging << tab << "Wrote svg file for frame 0" << newline;

  for (unsigned int i = 1u; i <= max_steps; ++i)
  {
    do_simulation_step();
    logging << tab << "Simulation step " << i << " done..." << newline;

    engine.update(parameters);
    logging << tab << "Updated the mesh" << newline;

    write_svg_files(output_path, i);
    logging << tab << "Wrote svg file for frame " << i << newline;
  }

  logging << "Done" << newline;

  return 0;
}
