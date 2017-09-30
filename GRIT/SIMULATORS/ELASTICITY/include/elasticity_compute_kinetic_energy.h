#ifndef ELASTICITY_COMPUTE_KINETIC_ENERGY_H
#define ELASTICITY_COMPUTE_KINETIC_ENERGY_H

#include <vector>

namespace elasticity
{
  
  inline double compute_kinetic_energy(
                                       std::vector<double> const & vx
                                       , std::vector<double> const & vy
                                       , std::vector<double> const & M
                                       )
  {
    double Ekin = 0.0;

    unsigned int V = vx.size();

    for(unsigned int v = 0u; v < V; ++v)
    {
      Ekin += 0.5*(vx[v]*vx[v] + vy[v]*vy[v])*M[v];
    }
    
    return Ekin;
  }

}// end of namespace elasticity

// ELASTICITY_COMPUTE_KINETIC_ENERGY_H
#endif
