#ifndef ELASTICITY_COMPUTE_GRAVITY_FORCES_H
#define ELASTICITY_COMPUTE_GRAVITY_FORCES_H

#include <vector>

namespace elasticity
{

  inline void compute_gravity_forces(
                                     std::vector<double> const & M
                                     , double              const & g
                                     , std::vector<double>       & Fx  // x component of gravity
                                     , std::vector<double>       & Fy  // y component of gravity
  )
  {
    unsigned int const V = M.size();

    Fx.resize(V);
    Fy.resize(V);

    std::fill( Fx.begin(), Fx.end(), 0.0);
    std::fill( Fy.begin(), Fy.end(), 0.0);

    for(unsigned int v = 0u; v < V; ++v)
    {
      Fx[v] = 0.0;
      Fy[v] =  -M[v]*g;
    }
  }

}// end of namespace elasticity

// ELASTICITY_COMPUTE_GRAVITY_FORCES_H
#endif
