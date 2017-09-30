#ifndef ELASTICITY_MATLAB_WRITE_STRESS_AND_STRAIN_H
#define ELASTICITY_MATLAB_WRITE_STRESS_AND_STRAIN_H

#include <glue_triplet.h>

#include <util_is_finite.h>
#include <util_tensors.h>

#include <sstream>
#include <string>
#include <vector>
#include <cassert>

namespace elasticity
{

  inline std::string matlab_write_stress_and_strain(
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

    std::stringstream output;

    unsigned int const N  = triangles.size();           // Number of triangles

    //--- Convert E and nu into Lamé coefficients --------------------------------
    double const lambda = (E*nu) / ( (1 + nu)*(1 - 2*nu) );
    double const mu     =  E     / (        2*(1+nu)     );

    assert( is_finite(lambda) || !"matlab_write_stress_and_strain(): lambda was not finite");
    assert( is_finite(mu)     || !"matlab_write_stress_and_strain(): mu was not finite");

    output << "[";

    for(unsigned int e=0u; e<N;++e)
    {
      unsigned int const i = triangles[e].i;
      unsigned int const j = triangles[e].j;
      unsigned int const k = triangles[e].k;

      assert( i!=j || !"matlab_write_stress_and_strain(): i and j was the same");
      assert( i!=k || !"matlab_write_stress_and_strain(): i and k was the same");
      assert( j!=k || !"matlab_write_stress_and_strain(): j and k was the same");

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

      assert( is_finite(D0.m_00) || !"matlab_write_stress_and_strain(): D0.m_00 was not finite");
      assert( is_finite(D0.m_01) || !"matlab_write_stress_and_strain(): D0.m_01 was not finite");
      assert( is_finite(D0.m_10) || !"matlab_write_stress_and_strain(): D0.m_10 was not finite");
      assert( is_finite(D0.m_11) || !"matlab_write_stress_and_strain(): D0.m_11 was not finite");

      Tensor2 D;
      D.m_00 = pxj-pxi;  D.m_01 = pxk-pxi;
      D.m_10 = pyj-pyi;  D.m_11 = pyk-pyi;

      assert( is_finite(D.m_00) || !"matlab_write_stress_and_strain(): D.m_00 was not finite");
      assert( is_finite(D.m_01) || !"matlab_write_stress_and_strain(): D.m_01 was not finite");
      assert( is_finite(D.m_10) || !"matlab_write_stress_and_strain(): D.m_10 was not finite");
      assert( is_finite(D.m_11) || !"matlab_write_stress_and_strain(): D.m_11 was not finite");

      // Compute deformation gradient
      //
      //   F = \frac{\partial x }{ \partial X0} = nabla_X0 x
      //
      Tensor2 const F = mul(D, inverse(D0) );

      //Compute strain tensor
      //
      //  E = 1/2(F^T F - I)
      Tensor2 const E = mul(0.5,  sub(mul(trans(F),F), identity() ) );

      //Compute 2nd Piola Kirchoff stress tensor
      //
      //  s = lambda tr(E) I + 2 mu E
      //
      Tensor2 const S = add( mul( lambda*trace(E) ,identity() ) , mul( 2.0*mu , E ) );

      double const det_F = det(F);

      assert( fabs(det_F)>0 || !"matlab_write_stress_and_strain(): Ill-posed deformed flat triangle found");

      //Convert to Cauchy stress tensor
      //
      //      j = det(F)
      //  sigma =  F S F^T / j
      //
      Tensor2 const sigma = mul( (1.0/det_F) , mul( mul( F, S  ) , trans(F) ) );
      
      output << " " << E.m_00 << " " << E.m_01 << " " << E.m_11;
      output << " " << S.m_00 << " " << S.m_01 << " " << S.m_11;
      output << " " << sigma.m_00 << " " << sigma.m_01 << " " << sigma.m_11;
      output << ";" << std::endl;
    }
    
    output << "]";
    output.flush();
    
    return output.str();
  }

}// end of namespace elasticity

// ELASTICITY_MATLAB_WRITE_STRESS_AND_STRAIN_H
#endif