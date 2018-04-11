#ifndef GLUE_RESIZE_DOMAIN_H
#define GLUE_RESIZE_DOMAIN_H

#include <glue_compute_bounding_box.h>
#include <glue_make_phase.h>
#include <glue_phase.h>

#include <grit.h>

namespace glue
{
  inline bool resize_domain(
                            grit::engine2d_type       & engine
                            , double            const   min_distance = 0.1
                            , double            const   scale        = 1.1
                            )
  {
    using namespace grit;
  
    typedef typename engine2d_type::MT::vector3_type V;

    SimplexSet const all         = engine.mesh().get_all_simplices();
    Phase      const phase_all   = make_phase(engine, all);

    SimplexSet const iface       = filter( all, IsInterface(engine.mesh()) );
    Phase      const phase_iface = make_phase(engine, iface);
    
    V pi_min, pi_max;

    compute_bounding_box( engine, phase_iface, pi_min, pi_max );

    V p_min, p_max;

    compute_bounding_box( engine, phase_all, p_min, p_max );

    V const center = (p_min+p_max)/2.;
    V const d      = p_max-p_min;

    if( 
       (pi_min[0u]-p_min[0u] < min_distance*d[0u]) ||
       (pi_min[1u]-p_min[1u] < min_distance*d[1u]) ||
       (p_max[0u]-pi_max[0u] < min_distance*d[0u]) ||
       (p_max[1u]-pi_max[1u] < min_distance*d[1u])
      )
    {
      SimplexSet bnd = filter( all, IsDimension(engine.mesh(), 0u) && IsBoundary(engine.mesh()));

      SimplexSet::simplex0_const_iterator it = bnd.begin0();

      for( ; it!=bnd.end0(); ++it )
      {
        //--- This can invert triangles, for better balance use larger min_distance and smaller scale
        Simplex0 const & s = *it; 
          
        V const & pos = engine.attributes().get_current_value(s);
        V const   pos_new = center + scale*(pos-center);
        
        engine.attributes().set_current_value(s, pos_new);
      }
    
      return true;
    }

    return false;
  }

} // namespace glue

// GLUE_RESIZE_DOMAIN_H
#endif
