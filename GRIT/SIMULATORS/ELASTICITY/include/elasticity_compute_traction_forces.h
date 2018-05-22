#ifndef ELASTICITY_COMPUTE_TRACTION_FORCES_H
#define ELASTICITY_COMPUTE_TRACTION_FORCES_H

#include <elasticity_traction.h>

#include <util_is_finite.h>

#include <glue_triplet.h>
#include <glue_compute_neighbors.h>

#include <vector>
#include <cassert>

namespace elasticity
{

  inline void compute_traction_forces(
                                      glue::Neighbors       const & neighbors
                                      , Traction            const & traction
                                      , std::vector<double> const & px        // current x-component of nodal positions
                                      , std::vector<double> const & py        // current y-component of nodal positions
                                      , std::vector<double>       & Fx        // x-component of traction nodal forces
                                      , std::vector<double>       & Fy        // y-component of traction nodal forces
  )
  {
    unsigned int const S =  traction.m_surface.size();  // Number of edges with traction on
    unsigned int const V  = px.size();                  // Number of vertices

    Fx.resize(V);
    Fy.resize(V);

    std::fill( Fx.begin(),   Fx.end(),   0.0);
    std::fill( Fy.begin(),   Fy.end(),   0.0);

    std::vector< double >  Fxe( 2u*S ); // x-component of traction force per edge element
    std::vector< double >  Fye( 2u*S ); // y-component of traction force per edge element

    std::fill( Fxe.begin(),  Fxe.end(),  0.0);
    std::fill( Fye.begin(),  Fye.end(),  0.0);

    for(unsigned s = 0u; s < S; ++s)
    {
      unsigned int const i = traction.m_surface[s].i;
      unsigned int const j = traction.m_surface[s].j;

      assert( i!=j || !"compute_traction_forces(): i and j was the same");

      double const px_i = px[i];
      double const py_i = py[i];
      double const px_j = px[j];
      double const py_j = py[j];

      double const ex = px_j- px_i;
      double const ey = py_j- py_i;

      assert( is_finite(ex) || !"compute_traction_forces(): ex was not finite");
      assert( is_finite(ey) || !"compute_traction_forces(): ey was not finite");

      // edge length
      double const l = sqrt( ex*ex + ey*ey  );

      assert( is_finite(l) || !"compute_traction_forces(): l was not finite");
      assert( l>0         || !"compute_traction_forces(): l was non positive");

      // traction forces
      double const ftx_i =  traction.m_tx[s]*l/2.0;
      double const ftx_j =  traction.m_tx[s]*l/2.0;
      double const fty_i =  traction.m_ty[s]*l/2.0;
      double const fty_j =  traction.m_ty[s]*l/2.0;

      assert( is_finite(ftx_i) || !"compute_traction_forces(): ftx_i was not finite");
      assert( is_finite(ftx_j) || !"compute_traction_forces(): ftx_j was not finite");
      assert( is_finite(fty_i) || !"compute_traction_forces(): fty_i was not finite");
      assert( is_finite(fty_j) || !"compute_traction_forces(): fty_j was not finite");

      Fxe[s*2  ] = ftx_i;
      Fye[s*2  ] = fty_i;
      Fxe[s*2+1] = ftx_j;
      Fye[s*2+1] = fty_j;
    }

    for(unsigned int v = 0u; v < V; ++v)
    {
      unsigned int const start = neighbors.m_offset[v   ];
      unsigned int const end   = neighbors.m_offset[v+1u];

      for(unsigned int e = start; e<end; ++e)
      {
        unsigned int const idx     = neighbors.m_V2T[e].second;
        unsigned int const local_v = get_local_index( traction.m_surface[idx], v);
        
        Fx[v]  +=  Fxe[2*idx + local_v];
        Fy[v]  +=  Fye[2*idx + local_v];
      }
    }
  }

}// end of namespace elasticity

// ELASTICITY_COMPUTE_TRACTION_FORCES_H
#endif