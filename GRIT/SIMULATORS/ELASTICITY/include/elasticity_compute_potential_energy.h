#ifndef ELASTICITY_COMPUTE_POTENTIAL_ENERGY_H
#define ELASTICITY_COMPUTE_POTENTIAL_ENERGY_H

#include <vector>

namespace elasticity
{
  
  inline double compute_potential_energy(
                                         double const & g         
                                         , std::vector<double> const & px
                                         , std::vector<double> const & py
                                         , std::vector<double> const & M
                                         )
  {
    double Epot = 0.0;

    unsigned int V = px.size();

    for(unsigned int v = 0u; v < V; ++v)
    {
      Epot += g*M[v]*py[v];
    }
    
    return Epot;
  }

}// end of namespace elasticity

// ELASTICITY_COMPUTE_POTENTIAL_ENERGY_H
#endif
