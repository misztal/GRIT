#ifndef ELASTICITY_COMPUTE_LUMPED_MASSES_H
#define ELASTICITY_COMPUTE_LUMPED_MASSES_H

#include <glue_neighbors.h>

#include <vector>

namespace elasticity
{

  inline void compute_lumped_masses(
                                    glue::Neighbors           const & neighbors
                                    , unsigned int        const & V
                                    , std::vector<double> const & Ae
                                    , double              const & rho
                                    , std::vector<double>       & M
                                    )
  {
    M.resize(V);

    std::fill( M.begin(), M.end(), 0.0);

    for(unsigned int v = 0u; v < V; ++v)
    {
      unsigned int const start = neighbors.m_offset[v   ];
      unsigned int const end   = neighbors.m_offset[v+1u];
      for(unsigned int e = start; e<end; ++e)
      {
        unsigned int const idx     = neighbors.m_V2T[e].second;
        
        M[v]   +=  rho*Ae[idx]/3.0;
      }
    }
  }

}// end of namespace elasticity

// ELASTICITY_COMPUTE_LUMPED_MASSES_H
#endif
