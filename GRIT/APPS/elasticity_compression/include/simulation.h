#ifndef DO_SIMULATION_STEP_H
#define DO_SIMULATION_STEP_H

#include <elasticity.h>
#include <grit.h>
#include <glue.h>

#include <util_distance_fields.h>
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
  util::Log log;

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
  double const dx0    = (max_x0 - min_x0)/1000.0;
  double const dy0    = (max_y0 - min_y0)/1000.0;

  util::Box left_region(
                        min_x0 - dx0
                        , min_x0 + dx0
                        , min_y0 - dy0
                        , max_y0 + dy0
                        );
  util::Box right_region(
                         max_x0 - dx0
                         , max_x0 + dx0
                         , min_y0 - dy0
                         , max_y0 + dy0
                         );

  obj.m_dirichlet = elasticity::make_dirichlet( (left_region + right_region), obj.m_triangles, obj.m_px0, obj.m_py0);

  // Now we change the position of the Dirichlet conditions such that the object is initially stretched.
  double const cx0 = (min_x0+max_x0)/2.0;
  double const cy0 = (min_y0+max_y0)/2.0;
  for (unsigned int i = 0u; i < obj.m_dirichlet.m_nodes.size(); ++i)
  {
    obj.m_dirichlet.m_x[i] =  0.9*(obj.m_dirichlet.m_x[i]-cx0) + cx0;
    obj.m_dirichlet.m_y[i] =  0.9*(obj.m_dirichlet.m_y[i]-cy0) + cy0;
  }

  //--- Make sure Dirichlet conditions are initially fulfilled -----------------
  elasticity::apply_dirichlet(obj.m_dirichlet, obj.m_px, obj.m_py, obj.m_vx, obj.m_vy);


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
}

// DO_SIMULATION_STEP_H
#endif
