#ifndef ELASTICITY_COMPUTE_STRAIN_ENERGY_H
#define ELASTICITY_COMPUTE_STRAIN_ENERGY_H

#include <util_is_finite.h>
#include <util_tensors.h>

#include <glue_triplet.h>

#include <vector>
#include <cassert>

namespace elasticity
{

  inline double compute_strain_energy(
                                      std::vector<glue::Triplet> const & triangles
                                      , std::vector<double>  const & px0       // Material x-component of nodal positions
                                      , std::vector<double>  const & py0       // Material y-component of nodal positions
                                      , std::vector<double>  const & px        // current x-component of nodal positions
                                      , std::vector<double>  const & py        // current y-component of nodal positions
                                      , double               const & E
                                      , double               const & nu
                                      )
  {
    using namespace util;

    double Estrain = 0.0;

    unsigned int const N  = triangles.size();           // Number of triangles

    //--- Convert E and nu into Lamé coefficients --------------------------------
    double const lambda = (E*nu) / ( (1 + nu)*(1 - 2*nu) );
    double const mu     =  E     / (        2*(1+nu)     );

    assert( is_finite(lambda) || !"compute_strain_energy(): lambda was not finite");
    assert( is_finite(mu)     || !"compute_strain_energy(): mu was not finite");

    for(unsigned int e=0u; e<N;++e)
    {
      unsigned int const i = triangles[e].i;
      unsigned int const j = triangles[e].j;
      unsigned int const k = triangles[e].k;

      assert( i!=j || !"compute_strain_energy(): i and j was the same");
      assert( i!=k || !"compute_strain_energy(): i and k was the same");
      assert( j!=k || !"compute_strain_energy(): j and k was the same");

      double const px0i = px0[i];
      double const px0j = px0[j];
      double const px0k = px0[k];

      double const py0i = py0[i];
      double const py0j = py0[j];
      double const py0k = py0[k];

      double const pxi = px[i];
      double const pxj = px[j];
      double const pxk = px[k];

      double const pyi = py[i];
      double const pyj = py[j];
      double const pyk = py[k];

      Tensor2 D0;
      D0.m_00 = px0j-px0i;  D0.m_01 = px0k-px0i;
      D0.m_10 = py0j-py0i;  D0.m_11 = py0k-py0i;

      assert( is_finite(D0.m_00) || !"compute_strain_energy(): D0.m_00 was not finite");
      assert( is_finite(D0.m_01) || !"compute_strain_energy(): D0.m_01 was not finite");
      assert( is_finite(D0.m_10) || !"compute_strain_energy(): D0.m_10 was not finite");
      assert( is_finite(D0.m_11) || !"compute_strain_energy(): D0.m_11 was not finite");

      Tensor2 D;
      D.m_00 = pxj-pxi;  D.m_01 = pxk-pxi;
      D.m_10 = pyj-pyi;  D.m_11 = pyk-pyi;

      assert( is_finite(D.m_00) || !"compute_strain_energy(): D.m_00 was not finite");
      assert( is_finite(D.m_01) || !"compute_strain_energy(): D.m_01 was not finite");
      assert( is_finite(D.m_10) || !"compute_strain_energy(): D.m_10 was not finite");
      assert( is_finite(D.m_11) || !"compute_strain_energy(): D.m_11 was not finite");

      // Compute deformation gradient
      //
      //   F = \frac{\partial x }{ \partial X0} = nabla_X0 x
      //
      Tensor2 const F = mul(D, inverse(D0) );

      //Compute strain tensor
      //
      //  G = 1/2(F^T F - I)
      Tensor2 const G = mul(0.5,  sub(mul(trans(F),F), identity() ) );

      // St Venant Kirchoff Strain Energy
      //
      // W =lambda/2 tr(G)^2 + mu tr(G^T G)
      //
      double const trG  = trace(G);
      double const trGG = concat(G,G);
      double const A0e  = det(D0) / 2.0;
      
      Estrain +=  (lambda*trG*trG*0.5 + mu*trGG)*A0e;
    }
    
    return Estrain;
  }

}// end of namespace elasticity

// ELASTICITY_COMPUTE_STRAIN_ENERGY_H
#endif