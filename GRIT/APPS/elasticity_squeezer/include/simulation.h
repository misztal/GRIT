#ifndef DO_SIMULATION_STEP_H
#define DO_SIMULATION_STEP_H

#include <elasticity.h>
#include <grit.h>
#include <glue.h>

#include <util_log.h>


inline void setup_simulation_fields( grit::engine2d_type &  engine )
{
  unsigned int const VERTEX_ATTRIBUTE = 0u;

  engine.attributes().create_attribute( "px0", VERTEX_ATTRIBUTE );
  engine.attributes().create_attribute( "py0", VERTEX_ATTRIBUTE );
  engine.attributes().create_attribute( "vx",  VERTEX_ATTRIBUTE );
  engine.attributes().create_attribute( "vy",  VERTEX_ATTRIBUTE );

  glue::copy_attribute( engine, glue::CURRENT_X() ,"px0", glue::VERTEX_ATTRIBUTE() );
  glue::copy_attribute( engine, glue::CURRENT_Y() ,"py0", glue::VERTEX_ATTRIBUTE() );
  glue::clear_attribute( engine, "vx", 0.0, glue::VERTEX_ATTRIBUTE() );
  glue::clear_attribute( engine, "vy", 0.0, glue::VERTEX_ATTRIBUTE() );
}

inline void do_simulation_step(
                               grit::engine2d_type          &  engine
                              , unsigned int           const & label
                              , elasticity::Parameters const & params
                               , std::string const & scene
                              )
{
  using std::min;
  using std::max;

  util::Log log;

  static double tick = 0.0;

  elasticity::Object obj = glue::make_phase(engine, label);

  glue::get_sub_range_current( engine, obj, obj.m_px, obj.m_py);

  glue::get_sub_range(engine, obj, "px0", obj.m_px0, glue::VERTEX_ATTRIBUTE() );
  glue::get_sub_range(engine, obj, "py0", obj.m_py0, glue::VERTEX_ATTRIBUTE() );
  glue::get_sub_range(engine, obj, "vx",  obj.m_vx, glue::VERTEX_ATTRIBUTE()  );
  glue::get_sub_range(engine, obj, "vy",  obj.m_vy, glue::VERTEX_ATTRIBUTE()  );

  obj.m_px_new = obj.m_px;
  obj.m_py_new = obj.m_py;
  obj.m_vx_new = obj.m_vx;
  obj.m_vy_new = obj.m_vy;


  double const min_x0 = *std::min_element( obj.m_px0.begin(), obj.m_px0.end() );
  double const max_x0 = *std::max_element( obj.m_px0.begin(), obj.m_px0.end() );
  double const min_y0 = *std::min_element( obj.m_py0.begin(), obj.m_py0.end() );
  double const max_y0 = *std::max_element( obj.m_py0.begin(), obj.m_py0.end() );
  double const dx0    = (max_x0 - min_x0);
  double const dy0    = (max_y0 - min_y0);

  double const y_mid          = (max_y0+min_y0)/2.0;
  double const plate_distance = (max_y0-min_y0)*0.75;

  double y00 =  0.01*tick + (min_y0 - dy0);
  double y11 = -0.01*tick + (max_y0 + dy0);

  double const y_lower =  std::min(  y_mid - (plate_distance/2.0), y00 );
  double const y_upper =  std::max(  y_mid + (plate_distance/2.0), y11 );

  util::Box world(
                  min_x0 - dx0
                  , max_x0 + dx0
                  , y_lower
                  , y_upper
                  );

  for (unsigned int i = 0u; i < obj.m_vertices.size(); ++i)
  {
    if ( world(obj.m_px[i], obj.m_py[i]) < 0.0)
    {
      obj.m_vx[i] = 0.0;
      obj.m_vy[i] = 0.0;

      obj.m_px[i] = min( world.m_max_x, max( obj.m_px[i], world.m_min_x) );
      obj.m_py[i] = min( world.m_max_y, max( obj.m_py[i], world.m_min_y) );
    }
  }

  //--- Now perform the time integration ---------------------------------------
  if (params.integrator().compare("semi_implicit")==0)
  {
    elasticity::compute_semi_implicit_time_step(params.dt(), obj, params);
  }
  else if (params.integrator().compare("adaptive")==0)
  {
    elasticity::compute_adaptive_time_step(params.dt(), obj, params);
  }
  else
  {
    log << "do_simulation_step(): unknown time integrator " << params.integrator() << util::Log::newline();
  }

  glue::set_sub_range(engine, obj, "vx", obj.m_vx_new, glue::VERTEX_ATTRIBUTE());
  glue::set_sub_range(engine, obj, "vy", obj.m_vy_new, glue::VERTEX_ATTRIBUTE());

  glue::set_sub_range_target(engine, obj, obj.m_px_new, obj.m_py_new);

  if(params.write_data())
    elasticity::matlab_write_object(obj, params);

  tick += params.dt();
}

// DO_SIMULATION_STEP_H
#endif
