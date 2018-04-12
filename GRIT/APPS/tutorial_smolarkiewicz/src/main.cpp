#include <configuration.h>

#include <grit.h>
#include <glue.h>
#include <util.h>

grit::engine2d_type       engine;
grit::param_type          parameters;

util::ConfigFile      settings;
util::Log             logging;

std::string const newline = util::Log::newline();
std::string const tab     = util::Log::tab();

//--- Decide upon some names for creating user-custom attribute fields,
//--- they can be anything you want as long as they are unique.
std::string const refinement_attribute_name = "my_refinement_values";
std::string const coarsening_attribute_name = "my_coarsening_values";

bool reverse_flow = false;


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

  glue::resize_domain( engine );

  std::vector<double> px;
  std::vector<double> py;

  glue::get_sub_range_current(engine, phase, px, py );

  std::vector<double> px_new;
  std::vector<double> py_new;

  double const pi = M_PI;

  unsigned int const N = px.size();

  px_new.resize(N);      ///< new x-coordinate
  py_new.resize(N);      ///< new x-coordinate

  double const dt   = 0.0025;
  double const M    = 50;  ///< number of sub-steps
  double const dt_M = dt/M;

  for( unsigned int m = 0; m < M; ++m)
  {
    for( unsigned int n = 0; n < N; ++n)
    {
      double const x = px[n];
      double const y = py[n];

      double u =  -sin(4.0 * pi * (x + 0.5 )) * sin( 4.0*pi*(y + 0.5 ));
      double v =  -cos(4.0 * pi * (x + 0.5 )) * cos( 4.0*pi*(y + 0.5 ));

      if( reverse_flow )
      {
        px_new[n] = px[n] - u*dt_M;
        py_new[n] = py[n] - v*dt_M;

        px[n] = px_new[n];
        py[n] = py_new[n];
      }
      else
      {
        px_new[n] = px[n] + u*dt_M;
        py_new[n] = py[n] + v*dt_M;

        px[n] = px_new[n];
        py[n] = py_new[n];
      }
    }
  }

  glue::set_sub_range_target(engine, phase, px_new, py_new);
}


void update_sizing_fields()
{
  grit::InterfaceMesh const & mesh = engine.mesh();

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

    grit::SimplexSet const interface_ = grit::filter(all, grit::IsInterface(mesh));
    grit::SimplexSet const S          = mesh.star(interface_);
    grit::SimplexSet const C          = mesh.closure(S);
    grit::SimplexSet const SC         = mesh.star(C);
    grit::SimplexSet const CSC        = mesh.closure(SC);

    glue::Phase      const zone2  = glue::make_phase(engine, CSC  );
    glue::Phase      const zone1  = glue::make_phase(engine, C    );

    {
      std::vector<double> const refinement_values( zone2.m_edges.size(), refinement_value/3.0);
      std::vector<double> const coarsening_values( zone2.m_edges.size(), coarsening_value/3.0);

      glue::set_sub_range( engine, zone2, refinement_attribute_name, refinement_values, glue::EDGE_ATTRIBUTE());
      glue::set_sub_range( engine, zone2, coarsening_attribute_name, coarsening_values, glue::EDGE_ATTRIBUTE());
    }

    {
      std::vector<double> const refinement_values( zone1.m_edges.size(), refinement_value/10.0);
      std::vector<double> const coarsening_values( zone1.m_edges.size(), coarsening_value/10.0);

      glue::set_sub_range( engine, zone1, refinement_attribute_name, refinement_values, glue::EDGE_ATTRIBUTE());
      glue::set_sub_range( engine, zone1, coarsening_attribute_name, coarsening_values, glue::EDGE_ATTRIBUTE());
    }
  }

  {
    //--- Extract the contact surface
    grit::SimplexSet const interface_ = grit::filter(all, grit::IsDimension(mesh, 1u)
                                                    && grit::IsInterface(mesh));

    glue::Phase const interface_line = glue::make_phase(engine, mesh.closure(interface_));

    std::vector<double> const refinement_values( interface_line.m_edges.size(), refinement_value/20.);
    std::vector<double> const coarsening_values( interface_line.m_edges.size(), coarsening_value/20.);

    glue::set_sub_range( engine, interface_line, refinement_attribute_name, refinement_values, glue::EDGE_ATTRIBUTE());
    glue::set_sub_range( engine, interface_line, coarsening_attribute_name, coarsening_values, glue::EDGE_ATTRIBUTE());
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

  bool const use_sizing_fields = util::to_value<bool>(settings.get_value("use_sizing_fields","true"));

  if( use_sizing_fields)
  {
    create_sizing_fields();
  }

  write_svg_files(output_path, 0u);
  logging << tab << "Wrote svg file for frame 0" << newline;

  for (unsigned int i = 1u; i <= max_steps; ++i)
  {
    if( i==(max_steps/2) ) reverse_flow = true;

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
