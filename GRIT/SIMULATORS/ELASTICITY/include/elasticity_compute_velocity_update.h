#ifndef ELASTICITY_COMPUTE_VELOCITY_UPDATE_H
#define ELASTICITY_COMPUTE_VELOCITY_UPDATE_H

#include <util_is_finite.h>

#include <vector>

namespace elasticity
{
  inline void compute_velocity_update(
                                      double              const & dt
                                      , std::vector<double> const & M
                                      , std::vector<double> const & vx
                                      , std::vector<double> const & vy
                                      , std::vector<double> const & Fx
                                      , std::vector<double> const & Fy
                                      , std::vector<double>       & vx_new
                                      , std::vector<double>       & vy_new
                                      )
  {
    unsigned int const V = vx.size();

    for(unsigned int v = 0u; v < V; ++v)
    {
      assert( is_finite(Fx[v]) || !"compute_velocity_update(): Fx[v] was not finite");
      assert( is_finite(Fy[v]) || !"compute_velocity_update(): Fy[v] was not finite");
      assert( is_finite(M[v])  || !"compute_velocity_update(): M[v] was not finite");
      assert( M[v]>0          || !"compute_velocity_update(): M[v] was non positive");

      vx_new[v] = vx[v] + dt*Fx[v]/M[v];
      vy_new[v] = vy[v] + dt*Fy[v]/M[v];

      assert( is_finite(vx_new[v]) || !"compute_velocity_update(): vx_new[v] was not finite");
      assert( is_finite(vy_new[v]) || !"compute_velocity_update(): vy_new[v] was not finite");
    }
  }
  
}// end of namespace elasticity

// ELASTICITY_COMPUTE_VELOCITY_UPDATE_H
#endif
