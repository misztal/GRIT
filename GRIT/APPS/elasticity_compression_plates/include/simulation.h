#ifndef DO_SIMULATION_STEP_H
#define DO_SIMULATION_STEP_H

#include <elasticity.h>

#include <grit.h>

#include <glue.h>

#include <util_distance_fields.h>
#include <util_log.h>


/**
 * This is a fake-collision resovling approach. No real contact forces are computed. Instead
 * one object is assumed to be free moving and one simply alters its positions and velocities
 * to avoid penetrations. The other objects are assumed never to be in contact.
 */
inline void resolve_collisions(
                               std::vector<glue::ContactPoint> & contacts
                               , std::vector<elasticity::Object> objects
                               , int const & free_object
                               , bool const & sliding
                               )
{
  using namespace elasticity;

  util::Log log;

  std::vector<glue::ContactPoint>::iterator contact = contacts.begin();
  std::vector<glue::ContactPoint>::iterator     end = contacts.end();
  for(;contact!=end; ++contact)
  {
    double const pxA =  objects[ contact->m_A ].m_px_new[ contact->m_vertex_A ];
    double const pyA =  objects[ contact->m_A ].m_py_new[ contact->m_vertex_A ];
    double const vxA =  objects[ contact->m_A ].m_vx_new[ contact->m_vertex_A ];
    double const vyA =  objects[ contact->m_A ].m_vy_new[ contact->m_vertex_A ];

    double const pxB =  objects[ contact->m_B ].m_px_new[ contact->m_vertex_B ];
    double const pyB =  objects[ contact->m_B ].m_py_new[ contact->m_vertex_B ];
    double const vxB =  objects[ contact->m_B ].m_vx_new[ contact->m_vertex_B ];
    double const vyB =  objects[ contact->m_B ].m_vy_new[ contact->m_vertex_B ];

    double const nx = contact->m_nx;
    double const ny = contact->m_ny;
    double const px = contact->m_px;
    double const py = contact->m_py;

    double const u_normal = (vxB - vxA)*nx + (vyB- vyA)*ny;

    if(u_normal<=0)
    {
      // This suggest a penetrating contact hence we
      // must back-project positions on vertices corresponding
      // to free object and set velocities of free object vertices
      // equal to plate velocities


      log << "Contact:"
      << " nx = " << nx
      << " ny = " << ny
      << " px = " << px
      << " py = " << py
      << " A = " << contact->m_A
      << " B = " << contact->m_B
      << util::Log::newline();

      log << "Pre-collision: u_n = " << u_normal
      << " pxA = " << pxA
      << " pyA = " << pyA
      << " vxA = " << vxA
      << " vyA = " << vyA
      << " pxB = " << pxB
      << " pyB = " << pyB
      << " vxB = " << vxB
      << " vyB = " << vyB
      << util::Log::newline();

      if(contact->m_A == free_object)
      {
        objects[ contact->m_A ].m_px_new[ contact->m_vertex_A ] = pxB;
        objects[ contact->m_A ].m_py_new[ contact->m_vertex_A ] = pyB;
        if(sliding)
        {
          objects[ contact->m_A ].m_vx_new[ contact->m_vertex_A ] -= u_normal*nx;
          objects[ contact->m_A ].m_vy_new[ contact->m_vertex_A ] -= u_normal*ny;
        }
        else
        {
          objects[ contact->m_A ].m_vx_new[ contact->m_vertex_A ] = vxB;
          objects[ contact->m_A ].m_vy_new[ contact->m_vertex_A ] = vyB;
        }
      }
      if(contact->m_B == free_object)
      {
        objects[ contact->m_B ].m_px_new[ contact->m_vertex_B ] = pxA;
        objects[ contact->m_B ].m_py_new[ contact->m_vertex_B ] = pyA;
        if(sliding)
        {
          objects[ contact->m_B ].m_vx_new[ contact->m_vertex_B ] += u_normal*nx;
          objects[ contact->m_B ].m_vy_new[ contact->m_vertex_B ] += u_normal*ny;
        }
        else
        {
          objects[ contact->m_B ].m_vx_new[ contact->m_vertex_B ] = vxA;
          objects[ contact->m_B ].m_vy_new[ contact->m_vertex_B ] = vyA;
        }
      }
      else
      {
        log << "Internal error : Neither"
        << contact->m_A
        << " or "
        << contact->m_B
        << " was the free object "
        << free_object
        << util::Log::newline();

      }

      double const pxA_after =  objects[ contact->m_A ].m_px_new[ contact->m_vertex_A ];
      double const pyA_after =  objects[ contact->m_A ].m_py_new[ contact->m_vertex_A ];
      double const vxA_after =  objects[ contact->m_A ].m_vx_new[ contact->m_vertex_A ];
      double const vyA_after =  objects[ contact->m_A ].m_vy_new[ contact->m_vertex_A ];

      double const pxB_after =  objects[ contact->m_B ].m_px_new[ contact->m_vertex_B ];
      double const pyB_after =  objects[ contact->m_B ].m_py_new[ contact->m_vertex_B ];
      double const vxB_after =  objects[ contact->m_B ].m_vx_new[ contact->m_vertex_B ];
      double const vyB_after =  objects[ contact->m_B ].m_vy_new[ contact->m_vertex_B ];

      double const u_normal_after = (vxB_after - vxA_after)*nx + (vyB_after- vyA_after)*ny;

      log << "Post-collision: u_n = " << u_normal_after
      << " pxA = " << pxA_after
      << " pyA = " << pyA_after
      << " vxA = " << vxA_after
      << " vyA = " << vyA_after
      << " pxB = " << pxB_after
      << " pyB = " << pyB_after
      << " vxB = " << vxB_after
      << " vyB = " << vyB_after
      << util::Log::newline();

    }
  }
}


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

inline elasticity::Object make_elasticity_object(
                                                 grit::engine2d_type          &  engine
                                                 , unsigned int           const & label
                                                 )
{
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

  return obj;
}

inline void do_simulation_step(
                               grit::engine2d_type          &  engine
                               , elasticity::Parameters const & params
                               )
{
  util::Log log;

  std::vector<elasticity::Object> objects(4);

  //unsigned int ambient_label = 0u;     // We have no need for ambient, but we want to make sure we can use object labels for indexing into the objects array.
  unsigned int upper_label   = 1u;
  unsigned int lower_label   = 2u;
  unsigned int object_label  = 3u;

  //objects[0] = make_elasticity_object(engine, ambient_label);
  objects[1] = make_elasticity_object(engine, upper_label);
  objects[2] = make_elasticity_object(engine, lower_label);
  objects[3] = make_elasticity_object(engine, object_label);

  //elasticity::Object & ambient = objects[ambient_label];
  elasticity::Object & upper   = objects[upper_label];
  elasticity::Object & lower   = objects[lower_label];
  elasticity::Object & obj     = objects[object_label];

  //-- Do collision detection -------------------------------------------------
  std::vector<glue::ContactPoint> contacts;

  glue::compute_contact_points(  engine
                               , objects
                               , contacts
                               , true   // ignore_ambient flag
                               , 0u     // ambient label
                               );

  //---- Kinematically move plates -----------------------------------------
  double const min_obj_y0 = *std::min_element( obj.m_py0.begin(), obj.m_py0.end() );
  double const max_obj_y0 = *std::max_element( obj.m_py0.begin(), obj.m_py0.end() );

  double const min_lower_y = *std::min_element( lower.m_py.begin(), lower.m_py.end() );
  double const max_lower_y = *std::max_element( lower.m_py.begin(), lower.m_py.end() );

  double const min_upper_y = *std::min_element( upper.m_py.begin(), upper.m_py.end() );
  double const max_upper_y = *std::max_element( upper.m_py.begin(), upper.m_py.end() );

  log << "lower  min y = " << min_lower_y << " max y = " << max_lower_y << util::Log::newline();
  log << "object min y = " << min_obj_y0  << " max y = " << max_obj_y0  << util::Log::newline();
  log << "upper  min y = " << min_upper_y << " max y ="  << max_upper_y << util::Log::newline();

  double const y_mid          = (max_obj_y0 + min_obj_y0)*0.5;
  double const plate_distance = (max_obj_y0 - min_obj_y0)*0.75;
  double const lower_y_stop   = y_mid - plate_distance*0.5;
  double const upper_y_stop   = y_mid + plate_distance*0.5;
  double const plate_speed = 0.01;

  if (max_lower_y < lower_y_stop)
  {
    for (unsigned int i = 0u; i < lower.m_vertices.size(); ++i)
    {
      lower.m_px_new[i]  = lower.m_px[i];
      lower.m_py_new[i]  = lower.m_py[i] + plate_speed;
      lower.m_vx_new[i]  = (lower.m_px_new[i] - lower.m_px[i]) / params.dt();
      lower.m_vy_new[i]  = (lower.m_py_new[i] - lower.m_py[i]) / params.dt();
    }
  }

  if(min_upper_y > upper_y_stop)
  {
    for (unsigned int i = 0u; i < upper.m_vertices.size(); ++i)
    {
      upper.m_py_new[i]  = upper.m_px[i];
      upper.m_py_new[i]  = upper.m_py[i] - plate_speed;
      upper.m_vx_new[i]  = (upper.m_px_new[i] - upper.m_px[i]) / params.dt();
      upper.m_vy_new[i]  = (upper.m_py_new[i] - upper.m_py[i]) / params.dt();
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

  //--- Resolve collsions  ------------------------------------------
  bool const sliding_allowed = false;

  resolve_collisions(
                     contacts
                     , objects
                     , object_label
                     , sliding_allowed
                     );

  glue::set_sub_range(engine, obj, "vx", obj.m_vx_new, glue::VERTEX_ATTRIBUTE());
  glue::set_sub_range(engine, obj, "vy", obj.m_vy_new, glue::VERTEX_ATTRIBUTE());

  glue::set_sub_range_target(engine, obj,   obj.m_px_new,   obj.m_py_new  );
  glue::set_sub_range_target(engine, lower, lower.m_px_new, lower.m_py_new);
  glue::set_sub_range_target(engine, upper, upper.m_px_new, upper.m_py_new);

  if(params.write_data())
    elasticity::matlab_write_object(obj, params);
}

// DO_SIMULATION_STEP_H
#endif
