#ifndef SIMULATION_H
#define SIMULATION_H

#include <sizing_fields.h>
#include <sub_range.h>

#include <stefan_compute_temperature.h>

#include <grit.h>
#include <glue.h>

#include <util_log.h>
#include <util_config_file.h>

void setup_simulation_fields(
                             grit::engine2d_type       & engine
                             , stefan::Domain    const & domain
                             , double            const & core_x
                             , double            const & core_y
                             , double            const & core_radius
                             , double            const & t_outer
                             , double            const & t_melting
                             )
{
  engine.attributes().create_attribute( "temperature", 0u );

  glue::clear_attribute( engine, "temperature", 0.0, glue::VERTEX_ATTRIBUTE() );

  std::vector<double> const & x_coords = domain.m_x;
  std::vector<double> const & y_coords = domain.m_y;

  unsigned int const n = x_coords.size();

  std::vector<double> temperature( n, t_outer );

  // 2016-03-06 Marek: this is correct for the end simulation, but won't yield
  //                   dendrites on its own.
  for( unsigned int i=0u; i<n; ++i )
  {
    double const x = x_coords[i]-core_x;
    double const y = y_coords[i]-core_y;

    double const r = std::sqrt(x*x+y*y);

    if( r<core_radius )
    {
      temperature[i] = t_melting;
    }
    else
    {
      temperature[i] = t_melting + (r-core_radius)*(t_outer-t_melting)/0.5;
    }
  }

  details::set_sub_range( engine, domain.m_all, "temperature", temperature, glue::VERTEX_ATTRIBUTE() );
}


void init_simulation(
                       grit::engine2d_type        & engine
                     , grit::param_type           & parameters
                     , util::ConfigFile     const & settings
                     )
{
  using std::sqrt;

  util::Log log;

  std::string const & tab     = util::Log::tab();
  std::string const & newline = util::Log::newline();

  engine.update(parameters);

  log << tab << "Initialization pre-updated the mesh" << newline;

  unsigned int const label_liquid     = util::to_value<unsigned int>(settings.get_value("label_liquid","1"));
  unsigned int const label_solid      = util::to_value<unsigned int>(settings.get_value("label_solid" ,"0"));

  double       const t_outer          = util::to_value<double>(settings.get_value("temperature_outer"      , "100.0"));
  double       const t_melting        = util::to_value<double>(settings.get_value("temperature_melting"    ,"1000.0"));

  double       const refinement_value = util::to_value<double>(settings.get_value("custom_refinement_value",  "0.10"));
  double       const coarsening_value = util::to_value<double>(settings.get_value("custom_coarsening_value",  "0.02"));;

  std::string  const refinement_attribute_name = settings.get_value( "refinement_attribute_name" );
  std::string  const coarsening_attribute_name = settings.get_value( "coarsening_attribute_name" );

  stefan::Domain domain = stefan::make_domain(
                                                engine
                                              , label_liquid
                                              , label_solid
                                              , false  // We do not need normal information for initialization
                                              );

  double core_x, core_y, core_radius;

  stefan::initialize_domain( domain, core_x, core_y, core_radius );

  setup_simulation_fields( engine, domain, core_x, core_y, core_radius, t_outer, t_melting );

  setup_sizing_fields(
                        engine
                      , parameters
                      , refinement_value
                      , coarsening_value
                      , refinement_attribute_name
                      , coarsening_attribute_name
                      );

  update_sizing_fields(
                        engine
                      , refinement_value
                      , coarsening_value
                      , refinement_attribute_name
                      , coarsening_attribute_name
                      );

  details::set_sub_range_target(
                                  engine
                                , domain.m_interface
                                , domain.m_interface_x
                                , domain.m_interface_y
                                , true
                                );

  engine.update( parameters );

  log << tab << "Initialization post updated the mesh" << newline;
}

void do_simulation_step(
                          grit::engine2d_type        & engine
                        , grit::param_type     const & parameters
                        , util::ConfigFile     const & settings
                        )
{
  util::Log log;

  std::string const & newline = util::Log::newline();
  std::string const & tab     = util::Log::tab();

  log << tab << "inside do_simulation_step()" << newline;

  unsigned int   const label_liquid = util::to_value<unsigned int>(settings.get_value("label_liquid"     ,"1"     ));
  unsigned int   const label_solid  = util::to_value<unsigned int>(settings.get_value("label_solid"      ,"0"     ));

  static double  const c_liquid     = util::to_value<double>(settings.get_value("heat_capacity_liquid"   ,   "1.0"));
  static double  const c_solid      = util::to_value<double>(settings.get_value("heat_capacity_solid"    ,   "1.0"));
  static double  const k_liquid     = util::to_value<double>(settings.get_value("heat_diffusivity_liquid",   "1.0"));
  static double  const k_solid      = util::to_value<double>(settings.get_value("heat_diffusivity_solid" ,   "1.0"));
  static double  const t_outer      = util::to_value<double>(settings.get_value("temperature_outer"      , "100.0"));
  static double  const t_melting    = util::to_value<double>(settings.get_value("temperature_melting"    ,"1000.0"));
  static double  const latent_heat  = util::to_value<double>(settings.get_value("latent_heat"            ,   "1.0"));
  static double  const time_step    = util::to_value<double>(settings.get_value("time_step"              ,   "0.1"));

  static double  const surf_tension = util::to_value<double>(settings.get_value("surface_tension"        ,   "1.0"));
  static double  const kinetic_coef = util::to_value<double>(settings.get_value("kinetic_coefficient"    ,   "1.0"));

  static double  const refinement_value = util::to_value<double>(settings.get_value("custom_refinement_value",  "0.10"));
  static double  const coarsening_value = util::to_value<double>(settings.get_value("custom_coarsening_value",  "0.02"));;

  static std::string const refinement_attribute_name = settings.get_value( "refinement_attribute_name" );
  static std::string const coarsening_attribute_name = settings.get_value( "coarsening_attribute_name" );

  std::vector<unsigned int>  labels;

  std::vector<double> temperature;

  glue::get_labels( engine, labels );

  stefan::Domain domain = stefan::make_domain(
                                                engine
                                              , label_liquid
                                              , label_solid
                                              );

  details::get_sub_range( engine, domain.m_all, "temperature", temperature, glue::VERTEX_ATTRIBUTE() );

  stefan::compute_motion(
                           engine
                         , domain
                         , labels
                         , c_liquid
                         , c_solid
                         , k_liquid
                         , k_solid
                         , latent_heat
                         , t_outer
                         , t_melting
                         , time_step
                         , surf_tension
                         , kinetic_coef
                         , temperature
                         );

  update_sizing_fields(
                         engine
                       , refinement_value
                       , coarsening_value
                       , refinement_attribute_name
                       , coarsening_attribute_name
                       );

  details::set_sub_range_target(
                                  engine
                                , domain.m_interface
                                , domain.m_interface_x
                                , domain.m_interface_y
                                , true
                                );

  details::set_sub_range( engine, domain.m_all, "temperature", temperature, glue::VERTEX_ATTRIBUTE() );

  log << "Leaving do_simulation_step()" << util::Log::newline();
}


// SIMULATION_H
#endif
