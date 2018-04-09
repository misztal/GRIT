#include <configuration.h>

#include <grit.h>
#include <util.h>
#include <glue.h>


grit::engine2d_type  engine;
grit::param_type     parameters;

util::ConfigFile      settings;
util::Log             logging;

//--- Decide upon some names for creating user-custom attribute fields,
//--- they can be anything you want as long as they are unique.
std::string const refinement_attribute_name = "my_refinement_values";
std::string const coarsening_attribute_name = "my_coarsening_values";

std::string const newline = util::Log::newline();
std::string const tab     = util::Log::tab();

void write_svg_files(
                     std::string const & output_path
                     , unsigned int const & frame_number
                     )
{
  std::string const filename = output_path + util::generate_filename("/tutorial_contact", frame_number, "svg");

  glue::svg_draw(filename, engine, parameters);

  logging << tab << tab << "write_svg_files() Done writting  " << filename << newline;
}

void do_simulation_step()
{
  double       const move_x              = util::to_value<double>(settings.get_value("move_x","0.01"));
  double       const move_y              = util::to_value<double>(settings.get_value("move_y","0.01"));
  unsigned int const north_object_label  = util::to_value<unsigned int>(settings.get_value("north_object_label","1"));
  unsigned int const south_object_label  = util::to_value<unsigned int>(settings.get_value("south_object_label","2"));

  glue::Phase  const north_phase         = glue::make_phase(engine, north_object_label);
  glue::Phase  const south_phase         = glue::make_phase(engine, south_object_label);

  std::vector<double> north_px;
  std::vector<double> north_py;
  std::vector<double> south_px;
  std::vector<double> south_py;

  glue::get_sub_range_current(engine, north_phase, north_px, north_py);
  glue::get_sub_range_current(engine, south_phase, south_px, south_py);

  std::vector<double> north_px_new (north_px.begin(), north_px.end());
  std::vector<double> north_py_new (north_py.begin(), north_py.end());
  std::vector<double> south_px_new (south_px.begin(), south_px.end());
  std::vector<double> south_py_new (south_py.begin(), south_py.end());

  for (unsigned int i = 0u; i != north_py_new.size(); ++i)
  {
    north_px_new[i] += move_x;
    north_py_new[i] += move_y;
  }

  for (unsigned int i = 0; i != south_py_new.size(); ++i)
  {
    south_px_new[i] -= move_x;
    south_py_new[i] -= move_y;
  }

  glue::set_sub_range_target(engine, north_phase, north_px_new, north_py_new);
  glue::set_sub_range_target(engine, south_phase, south_px_new, south_py_new);
}

// 2016-07-25 Marek: I think this is how the sizing fields should be used.
//                   Using sparse sizing fields can't be implemented optimally,
//                   it will invariably lead to some unexpected behavior ("spilling of the sizing field").
//                   Should we remove support for sparse sizing fields?
//                   They introduce unnecessary overhead IMHO.
// 2016-07-25 Marek: TODO: check how the sizing fields propagate if the number of scheduler iterations is higher than 1.
void update_sizing_fields()
{
  grit::InterfaceMesh const & mesh = engine.mesh();

  unsigned int const north_object_label  = util::to_value<unsigned int>(settings.get_value("north_object_label","1"));
  unsigned int const south_object_label  = util::to_value<unsigned int>(settings.get_value("south_object_label","2"));

  double const refinement_value = util::to_value<double>(settings.get_value("custom_refinement_value","0.5"));
  double const coarsening_value = util::to_value<double>(settings.get_value("custom_coarsening_value","0.1"));;

  glue::clear_attribute( engine
                        , refinement_attribute_name
                        , refinement_value
                        , glue::EDGE_ATTRIBUTE()
                        );

  glue::clear_attribute( engine
                        , coarsening_attribute_name
                        , coarsening_value
                        , glue::EDGE_ATTRIBUTE()
                        );

  grit::SimplexSet const all = engine.mesh().get_all_simplices();

  {
    grit::SimplexSet const objects = grit::filter(all, grit::InPhase(mesh, north_object_label)
                                                    || grit::InPhase(mesh, south_object_label));

    glue::Phase const phase = glue::make_phase(engine, objects);

    std::vector<double> const refinement_values( phase.m_edges.size(), 0.025);
    std::vector<double> const coarsening_values( phase.m_edges.size(), 0.01);

    glue::set_sub_range( engine, phase, refinement_attribute_name, refinement_values, glue::EDGE_ATTRIBUTE());
    glue::set_sub_range( engine, phase, coarsening_attribute_name, coarsening_values, glue::EDGE_ATTRIBUTE());
  }

  {
    //--- Extract the contact surface
    grit::SimplexSet const contact = grit::filter(all, grit::IsDimension(mesh, 1u)
                                                    && grit::InPhase(mesh, north_object_label)
                                                    && grit::InPhase(mesh, south_object_label));

    glue::Phase const contact_line = glue::make_phase(engine, mesh.closure(contact));

    std::vector<double> const refinement_values( contact_line.m_edges.size(), 0.0125);
    std::vector<double> const coarsening_values( contact_line.m_edges.size(), 0.005);

    glue::set_sub_range( engine, contact_line, refinement_attribute_name, refinement_values, glue::EDGE_ATTRIBUTE());
    glue::set_sub_range( engine, contact_line, coarsening_attribute_name, coarsening_values, glue::EDGE_ATTRIBUTE());
  }
}

void create_sizing_fields()
{
  //--- Now create the actual edge attribute fields that will control maximum
  //--- edge length before doing a refinement and the lowest edge length
  //--- before doing coarsening.
  engine.attributes().create_attribute( refinement_attribute_name, 1u);
  engine.attributes().create_attribute( coarsening_attribute_name, 1u);

  //--- Fill in some sensible initial values for the refinement and coarsening
  //--- edge length values. Make sure coarsening is lower than refinement.
  double const refinement_value = util::to_value<double>(settings.get_value("custom_refinement_value","0.1"));
  double const coarsening_value = util::to_value<double>(settings.get_value("custom_coarsening_value","0.02"));;

  glue::clear_attribute( engine
                        , refinement_attribute_name
                        , refinement_value
                        , glue::EDGE_ATTRIBUTE()
                        );

  glue::clear_attribute( engine
                        , coarsening_attribute_name
                        , coarsening_value
                        , glue::EDGE_ATTRIBUTE()
                        );

  //--- Next connect the user defined edge attribute fields to the actual
  //--- mesh algorithm operations that do refinement and coarsening.
  parameters.set_lower_threshold_attribute( "refinement",           refinement_attribute_name);
  parameters.set_lower_threshold_attribute( "interface_refinement", refinement_attribute_name);
  parameters.set_upper_threshold_attribute( "coarsening",           coarsening_attribute_name);
  parameters.set_upper_threshold_attribute( "interface_coarsening", coarsening_attribute_name);
}

int main()
{
  if(!settings.load("tutorial_contact.cfg"))
  {
    return 1;
  }

  std::string  const txt_filename = settings.get_value("txt_filename",  "two_rectangles.txt");
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

  grit::init_engine_with_mesh_file( util::get_data_file_path(txt_filename),parameters,engine);

  bool const use_sizing_fields = util::to_value<bool>(settings.get_value("use_sizing_fields","true"));

  if( use_sizing_fields)
  {
    create_sizing_fields();
  }

  write_svg_files(output_path, 0u);
  logging << tab << "Wrote svg file for frame 0" << newline;

  for (unsigned int i = 1u; i <= max_steps; ++i)
  {
    do_simulation_step();
    logging << tab << "Simulation step " << i << " done..." << newline;

	if( use_sizing_fields )
	{
      update_sizing_fields();
      logging << tab << "Updated sizing fields" << newline;
	}

    engine.update(parameters);
    logging << tab << "Updated the mesh" << newline;

    write_svg_files(output_path, i);
    logging << tab << "Wrote svg file for frame " << i << newline;
  }

  logging << "Done" << newline;

  return 0;
}
