#ifndef ELASTICITY_COMPUTE_SEMI_IMPLICIT_TIME_STEP_H
#define ELASTICITY_COMPUTE_SEMI_IMPLICIT_TIME_STEP_H

#include <elasticity_object.h>
#include <elasticity_dirichlet.h>
#include <elasticity_traction.h>
#include <elasticity_parameters.h>
#include <elasticity_compute_triangle_areas.h>
#include <elasticity_compute_lumped_masses.h>
#include <elasticity_compute_elastic_forces.h>
#include <elasticity_compute_traction_forces.h>
#include <elasticity_compute_damping_forces.h>
#include <elasticity_compute_gravity_forces.h>
#include <elasticity_compute_position_update.h>
#include <elasticity_compute_velocity_update.h>

#include <glue_compute_neighbors.h>

#include <util_tensors.h>

#include <cassert>
#include <cmath>
#include <vector>

namespace elasticity
{

  inline void compute_semi_implicit_time_step(
                                              double const & dt
                                              , Object           & obj      ///< The elastic object to be time integrated
                                              , Parameters const & params   ///< Parameters controling simulation
  )
  {
    using namespace util;

    unsigned int const N  = obj.m_triangles.size();           // Number of triangles
    unsigned int const V  = obj.m_px.size();                  // Number of vertices

    double const E    = params.E();     // Young modulus
    double const nu   = params.nu();    // Poisson ratio
    double const rho0 = params.rho0();  // Material space mass density
    double const g    = params.g();     // Acceleration of gravity
    double const c    = params.c();     // Viscous mass damping

    std::vector< double >  A0e(N);       // Material Triangle areas
    std::vector< double >  M(V);         // Lumped mass matrix

    std::vector< double >  Ex(V);        // x-component of elastic nodal forces
    std::vector< double >  Ey(V);        // y-component of elastic nodal forces
    std::vector< double >  Tx(V);        // x-component of traction nodal forces
    std::vector< double >  Ty(V);        // y-component of traction nodal forces
    std::vector< double >  Gx(V);        // x-component of gravity nodal forces
    std::vector< double >  Gy(V);        // y-component of gravity nodal forces
    std::vector< double >  Dx(V);        // x-component of damping nodal forces
    std::vector< double >  Dy(V);        // y-component of damping nodal forces
    std::vector< double >  Fx(V);        // x-component of total nodal forces
    std::vector< double >  Fy(V);        // y-component of total nodal forces

    glue::Neighbors neighbors;
    glue::compute_neighbors(obj.m_triangles,V,neighbors);

    compute_triangle_areas(obj.m_triangles, obj.m_px0, obj.m_py0, A0e);

    compute_lumped_masses(neighbors, V, A0e, rho0, M);

    if(params.use_dirichlet())
    {
      assert( test_dirichlet( obj.m_dirichlet, obj.m_px, obj.m_py, obj.m_vx, obj.m_vy) || !"compute_semi_implicit_time_step(): Caller must make sure Dirichlet conditions are fulfilled before calling");
    }

    if(params.use_elasticity())
    {
      compute_elastic_forces( neighbors, obj.m_triangles, obj.m_px0, obj.m_py0, obj.m_px, obj.m_py, E, nu, Ex, Ey);
    }

    if(params.use_traction())
    {
      compute_neighbors(obj.m_traction.m_surface, V, neighbors);

      compute_traction_forces( neighbors, obj.m_traction, obj.m_px, obj.m_py, Tx, Ty);
    }

    if(params.use_gravity())
    {
      compute_gravity_forces( M, g, Gx, Gy);
    }

    if(params.use_damping())
    {
      compute_damping_forces( M, c, obj.m_vx, obj.m_vy, Dx, Dy);
    }

    //--- Compute total nodal forces ---------------------------------------------
    for(unsigned int v = 0u; v < V; ++v)
    {
      Fx[v] = Ex[v] + Tx[v] + Dx[v] + Gx[v];
      Fy[v] = Ey[v] + Ty[v] + Dy[v] + Gy[v];
    }

    compute_velocity_update(dt, M, obj.m_vx, obj.m_vy, Fx, Fy, obj.m_vx_new, obj.m_vy_new);
    compute_position_update(dt, obj.m_px, obj.m_py, obj.m_vx_new, obj.m_vy_new, obj.m_px_new, obj.m_py_new);
    
    if(params.use_dirichlet())
    {
      apply_dirichlet( obj.m_dirichlet, obj.m_px_new, obj.m_py_new, obj.m_vx_new, obj.m_vy_new);
    }
  }
  
}// end of namespace elasticity

// ELASTICITY_COMPUTE_SEMI_IMPLICIT_TIME_STEP_H
#endif
