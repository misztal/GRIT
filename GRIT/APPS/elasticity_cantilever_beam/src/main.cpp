#include <fps_loop.h>
#include <fixed_step_loop.h>

#include <grit.h>

#include <util.h>

grit::engine2d_type  engine;
grit::param_type     parameters;

util::ConfigFile      settings;
util::Log             logging;

std::string const newline = util::Log::newline();
std::string const tab     = util::Log::tab();


int main()
{
  if(!settings.load("elasticity_cantilever_beam.cfg"))
  {
    return 1;
  }

  std::string  const txt_filename = settings.get_value("txt_filename","rectangle_refined.txt");
  std::string  const output_path  = settings.get_value("output_path","");

  util::LogInfo::on()             = util::to_value<bool>(settings.get_value("logging","true"));
  util::LogInfo::console()        = util::to_value<bool>(settings.get_value("console","true"));
  util::LogInfo::filename()       = output_path + "/" + settings.get_value("log_file","log.txt");

  logging << "### " << util::timestamp() << newline;

  parameters = grit::make_parameters_from_config_file(settings);

  if (!grit::is_valid(parameters))
  {
    logging << "main() ERROR: Invalid parameters - check the settings files for errors." << newline;
    return 1;
  }

  grit::init_engine_with_mesh_file( util::get_data_file_path(txt_filename), parameters, engine);

  if( settings.get_value("loop").compare("fps")==0)
  {
    fps_loop(engine, parameters, settings);
  }
  else if( settings.get_value("loop").compare("fixed")==0)
  {
    fixed_step_loop(engine, parameters, settings);
  }
  else
  {
    logging << "Unknown simulation loop type " << settings.get_value("loop") << newline;
    return 1;
  }

  logging << "Simulation loop done" << newline;
  
  return 0;
}

