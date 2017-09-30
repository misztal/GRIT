#ifndef ELASTICITY_COMPUTE_DAMPING_FORCES_H
#define ELASTICITY_COMPUTE_DAMPING_FORCES_H

#include <vector>

namespace elasticity
{

  inline void compute_damping_forces(
                                     std::vector<double> const & M
                                     , double              const & c
                                     , std::vector<double> const & vx
                                     , std::vector<double> const & vy
                                     , std::vector<double>       & Fx  // x component of gravity forces
                                     , std::vector<double>       & Fy  // y component of gravity forces
  )
  {
    unsigned int const V = M.size();

    Fx.resize(V);
    Fy.resize(V);

    std::fill( Fx.begin(), Fx.end(), 0.0);
    std::fill( Fy.begin(), Fy.end(), 0.0);

    for(unsigned int v = 0u; v < V; ++v)
    {
      Fx[v]  =  - c*M[v]*vx[v];
      Fy[v]  =  - c*M[v]*vy[v];
    }
  }

}// end of namespace elasticity

// ELASTICITY_COMPUTE_DAMPING_FORCES_H
#endif
