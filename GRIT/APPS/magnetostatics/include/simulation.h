#ifndef SIMULATION_H
#define SIMULATION_H

#include <magnetism_compute_static_potential.h>
#include <magnetism_constant_dipole_field.h>
#include <magnetism_circular_dipole_field.h>

#include <grit.h>
#include <glue.h>

#include <util_log.h>
#include <util_config_file.h>


inline void setup_simulation_fields( grit::engine2d_type &  engine )
{
  engine.attributes().create_attribute( "phi",   2u );
  engine.attributes().create_attribute( "q",     2u );

  // 2016-01-12 Marek: temporary, just to test if Simplex1Attributes is supported correctly
  engine.attributes().create_attribute( "sizing_field", 1u);

  glue::clear_attribute( engine, "phi",  0.0, glue::FACE_ATTRIBUTE() );
  glue::clear_attribute( engine, "q",    0.0, glue::FACE_ATTRIBUTE() );

  // 2016-01-12 Marek: temporary, just to test if Simplex1Attributes is supported correctly
  glue::clear_attribute( engine, "sizing_field", 0.0, glue::EDGE_ATTRIBUTE() );
}




inline void parse_dipole_fields(util::ConfigFile const & settings, std::map<unsigned int, magnetism::DipoleField *> & fields)
{
  util::Log log;
  
  fields.clear();
  
  std::vector<std::string> field_names = settings.get_values("dipole_fields");
  
  if (field_names.empty())
  {
    log << "No dipole field names was specified" << util::Log::newline();
    // exit with big error
  }
  
  
  std::vector<std::string>::const_iterator field_name = field_names.begin();
  std::vector<std::string>::const_iterator end = field_names.end();
  
  for(;field_name != end;++field_name)
  {
    std::vector<std::string> field_params = settings.get_values( *field_name );
    
    if (field_params[0].compare("constant")==0)
    {
      unsigned int const field_label = util::to_value<unsigned int>( field_params[1] );
      double       const mx          = util::to_value<double>(       field_params[2] );
      double       const my          = util::to_value<double>(       field_params[3] );
      
      magnetism::ConstantDipoleField * field = new magnetism::ConstantDipoleField(mx, my);   // 2017-05-16 Kenny code review: Maybe use unique_ptr pointers or deal with dynamic memory allocation otherwise?
      fields[field_label] = field;
      
    }
    else if (field_params[0].compare("circular")==0)
    {
      unsigned int const field_label = util::to_value<unsigned int>( field_params[1] );
      double       const mx          = util::to_value<double>(       field_params[2] );
      double       const my          = util::to_value<double>(       field_params[3] );
      double       const cx          = util::to_value<double>(       field_params[4] );
      double       const cy          = util::to_value<double>(       field_params[5] );
      
      magnetism::CircularDipoleField * field = new magnetism::CircularDipoleField(mx, my, cx, cy);  // 2017-05-16 Kenny code review: Maybe use unique_ptr pointers or deal with dynamic memory allocation otherwise?
      fields[field_label] = field;
      
    }
    else
    {
      log << "Unknown field type: " << field_params[0] << util::Log::newline();
      // exit with big error
    }
    
  }
}





inline void do_simulation_step(
                               grit::engine2d_type          & engine
                               , util::ConfigFile      const & settings
                               )
{
  util::Log log;

  bool         const matlab_write                = util::to_value<bool>(settings.get_value("matlab_write","true"));
  bool         const matlab_append               = util::to_value<bool>(settings.get_value("matlab_append","false"));
  std::string  const matlab_filename             = settings.get_value("matlab_filename","magnetostatics.m");

  std::string  const output_path               = settings.get_value("output_path","");

  unsigned int const preconditioner_choice       = util::to_value<unsigned int>(settings.get_value("preconditioner_choice","1"));
  unsigned int const solver_choice               = util::to_value<unsigned int>(settings.get_value("solver_choice","3"));
  bool         const solver_use_host             = util::to_value<bool>(settings.get_value("solver_use_host","true"));
  unsigned int const solver_max_iterations       = util::to_value<unsigned int>(settings.get_value("solver_max_iterations","400"));
  double       const solver_relative_tolerance   = util::to_value<double>(settings.get_value("solver_relative_tolerance","10e-15"));
  double       const solver_absolute_tolerance   = util::to_value<double>(settings.get_value("solver_absolute_tolerance","10e-10"));

  double       const Mx                          = util::to_value<double>(settings.get_value("mx","1"));
  double       const My                          = util::to_value<double>(settings.get_value("my","0"));
  bool         const order_2nd                   = util::to_value<bool>(settings.get_value("order_2nd","true"));

  unsigned int const magnet_label                       = util::to_value<unsigned int>(settings.get_value("magnet_label","1"));
  std::vector<unsigned int>  magnet_labels;


  // Read the magnet labels and populate the magnet_labels vector
  std::vector<std::string> tmp_labels = settings.get_values("magnet_labels");

  for (std::vector<std::string>::const_iterator it = tmp_labels.begin(); it != tmp_labels.end(); it++)
  {
      magnet_labels.push_back(util::to_value<unsigned int>(*it));
  }

  log << "Magnet labels: ";
  for (std::vector<unsigned int>::const_iterator it = magnet_labels.begin(); it != magnet_labels.end(); it++)
  {
      log << *it << " ";
  }
  log << util::Log::newline();

  
  std::map<unsigned int, magnetism::DipoleField *> fields;
  parse_dipole_fields(settings, fields);
  

  std::vector<glue::Triplet> adjacent;
  std::vector<unsigned int>  labels;
  std::vector<double> x;
  std::vector<double> y;
  std::vector<double> q;      // Desired quality measure
  std::vector<double> phi;    // Resulting potential

  glue::get_labels(engine, labels);

  glue::Phase const domain = glue::make_phase(engine);

  glue::make_adjacency(engine, domain, adjacent );
  glue::get_sub_range_current( engine, domain, x, y);
  glue::get_sub_range(engine, domain, "phi",   phi,  glue::FACE_ATTRIBUTE()   );
  glue::get_sub_range(engine, domain, "q",     q,    glue::FACE_ATTRIBUTE()   );

  magnetism::compute_static_potential(
                                      labels
                                      , magnet_label
                                      , magnet_labels
                                      , domain.m_triangles
                                      , adjacent
                                      , order_2nd
                                      , x
                                      , y
                                      , Mx
                                      , My
                                      , phi
                                      , q
                                      , solver_use_host
                                      , solver_max_iterations
                                      , solver_relative_tolerance
                                      , solver_absolute_tolerance
                                      , preconditioner_choice
                                      , solver_choice
                                      , matlab_write
                                      , matlab_append
                                      , output_path + "/"+ matlab_filename
                                      );

  glue::set_sub_range(engine, domain, "phi",  phi,  glue::FACE_ATTRIBUTE()   );
  glue::set_sub_range(engine, domain, "q",    q,  glue::FACE_ATTRIBUTE()   );

  // How do we tell grit engine to use q as a sizing field for refinement/coarsening?

}

// SIMULATION_H
#endif
