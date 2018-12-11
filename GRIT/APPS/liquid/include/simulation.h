#ifndef SIMULATION_H
#define SIMULATION_H

#include <liquid.h>

#include <grit.h>
#include <glue.h>

#include <util_log.h>


inline void setup_simulation_fields( grit::engine2d_type &  engine )
{
  engine.attributes().create_attribute( "vx",  0u );
  engine.attributes().create_attribute( "vy",  0u );
  engine.attributes().create_attribute( "p",   2u );

  glue::clear_attribute( engine, "vx", 0.0, glue::VERTEX_ATTRIBUTE() );
  glue::clear_attribute( engine, "vy", 0.0, glue::VERTEX_ATTRIBUTE() );
  glue::clear_attribute( engine, "p",  0.0, glue::FACE_ATTRIBUTE() );
}

inline void do_simulation_step(
                               grit::engine2d_type          & engine
                              , unsigned int           const & liquid_label
                              , liquid::Parameters     const & params
                              )
{
  util::Log log;

  std::vector<glue::Triplet> adjacent;
  std::vector<double> x;
  std::vector<double> y;
  std::vector<double> vx;
  std::vector<double> vy;
  std::vector<double> p;

  glue::Phase const domain = glue::make_phase(engine, liquid_label);

  glue::make_adjacency(engine, domain, adjacent );

  glue::get_sub_range_current( engine, domain, x, y);

  glue::get_sub_range(engine, domain, "vx",  vx, glue::VERTEX_ATTRIBUTE() );
  glue::get_sub_range(engine, domain, "vy",  vy, glue::VERTEX_ATTRIBUTE() );
  glue::get_sub_range(engine, domain, "p",   p,  glue::FACE_ATTRIBUTE()   );

  std::vector<double> vx_new = vx;
  std::vector<double> vy_new = vy;
  std::vector<double> p_new  = p;

  liquid::compute_time_step(
                            params
                            , domain.m_triangles
                            , adjacent
                            , x
                            , y
                            , vx
                            , vy
                            , p
                            , vx_new
                            , vy_new
                            , p_new
                            );

  glue::set_sub_range(engine, domain, "vx", vx_new, glue::VERTEX_ATTRIBUTE() );
  glue::set_sub_range(engine, domain, "vy", vy_new, glue::VERTEX_ATTRIBUTE() );
  glue::set_sub_range(engine, domain, "p",  p_new,  glue::FACE_ATTRIBUTE()   );

  std::vector<double> x_new = util::add_mul(x, params.dt(), vx);
  std::vector<double> y_new = util::add_mul(y, params.dt(), vy);

  glue::set_sub_range_target(engine, domain, x_new, y_new);
}

// SIMULATION_H
#endif
