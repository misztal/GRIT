#ifndef GLUE_COMPUTE_BOUNDING_BOX_H
#define GLUE_COMPUTE_BOUNDING_BOX_H

#include <glue_phase.h>

#include <grit.h>

#include <limits>

namespace glue
{
  template< class VT >
  inline void compute_bounding_box(
                                   grit::engine2d_type const & engine
                                   , glue::Phase       const & object
                                   , VT                      & p_min
                                   , VT                      & p_max
                                   )
  {
    typedef typename grit::engine2d_type::MT::vector3_type  V;
    typedef typename grit::engine2d_type::MT::real_type     T;

    std::vector< grit::Simplex0 > const & vertices = object.m_vertices;

    unsigned int const n_verts = vertices.size();

    T const t_min   = std::numeric_limits<T>::min();
    T const t_max   = std::numeric_limits<T>::max();

    p_min[0u] = t_max;
    p_min[1u] = t_max;
    p_min[2u] = 0.;

    p_max[0u] = t_min;
    p_max[1u] = t_min;
    p_max[2u] = 0.;

    for( unsigned int i=0u; i<n_verts; ++i )
    {
      grit::Simplex0 const & s   = vertices[i];
      V              const & pos = engine.attributes().get_current_value( s );

      if( pos[0u] < p_min[0u] ) p_min[0u] = pos[0u];
      if( pos[1u] < p_min[1u] ) p_min[1u] = pos[1u];

      if( pos[0u] > p_max[0u] ) p_max[0u] = pos[0u];
      if( pos[1u] > p_max[1u] ) p_max[1u] = pos[1u];
    }
  }

} // glue

// GLUE_COMPUTE_BOUNDING_BOX_H
#endif
