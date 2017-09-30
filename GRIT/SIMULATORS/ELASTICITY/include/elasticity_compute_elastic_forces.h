#ifndef ELASTICITY_COMPUTE_ELASTIC_FORCES_H
#define ELASTICITY_COMPUTE_ELASTIC_FORCES_H

#include <util_is_finite.h>
#include <util_tensors.h>

#include <glue_triplet.h>
#include <glue_compute_neighbors.h>

#include <vector>
#include <cassert>

namespace elasticity
{

  inline void compute_elastic_forces(
                                     glue::Neighbors const & neighbors
                                     , std::vector<glue::Triplet> const & triangles
                                     , std::vector<double>  const & px0       // Material x-component of nodal positions
                                     , std::vector<double>  const & py0       // Material y-component of nodal positions
                                     , std::vector<double>  const & px        // current x-component of nodal positions
                                     , std::vector<double>  const & py        // current y-component of nodal positions
                                     , double               const & E
                                     , double               const & nu
                                     , std::vector<double>        & Fx        // x-component of elastic nodal forces
                                     , std::vector<double>        & Fy        // y-component of elastic nodal forces
  )
  {
    using namespace util;

    unsigned int const N  = triangles.size();           // Number of triangles
    unsigned int const V  = px.size();                  // Number of vertices

    //--- Convert E and nu into Lamé coefficients --------------------------------
    double const lambda = (E*nu) / ( (1 + nu)*(1 - 2*nu) );
    double const mu     =  E     / (        2*(1+nu)     );

    assert( is_finite(lambda) || !"compute_elastic_forces(): lambda was not finite");
    assert( is_finite(mu)     || !"compute_elastic_forces(): mu was not finite");

    Fx.resize(V);
    Fy.resize(V);

    std::fill( Fx.begin(),   Fx.end(),   0.0);
    std::fill( Fy.begin(),   Fy.end(),   0.0);

    std::vector< double >  Fxe( 3u*N );  // x-component of elastic forces per triangle element
    std::vector< double >  Fye( 3u*N );  // y-component of elastic forces per triangle element

    std::fill( Fxe.begin(),  Fxe.end(),  0.0);
    std::fill( Fye.begin(),  Fye.end(),  0.0);

    for(unsigned int e=0u; e<N;++e)
    {
      unsigned int const i = triangles[e].i;
      unsigned int const j = triangles[e].j;
      unsigned int const k = triangles[e].k;

      assert( i!=j || !"compute_elastic_forces(): i and j was the same");
      assert( i!=k || !"compute_elastic_forces(): i and k was the same");
      assert( j!=k || !"compute_elastic_forces(): j and k was the same");

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

      assert( is_finite(D0.m_00) || !"compute_elastic_forces(): D0.m_00 was not finite");
      assert( is_finite(D0.m_01) || !"compute_elastic_forces(): D0.m_01 was not finite");
      assert( is_finite(D0.m_10) || !"compute_elastic_forces(): D0.m_10 was not finite");
      assert( is_finite(D0.m_11) || !"compute_elastic_forces(): D0.m_11 was not finite");

      Tensor2 D;
      D.m_00 = pxj-pxi;  D.m_01 = pxk-pxi;
      D.m_10 = pyj-pyi;  D.m_11 = pyk-pyi;

      assert( is_finite(D.m_00) || !"compute_elastic_forces(): D.m_00 was not finite");
      assert( is_finite(D.m_01) || !"compute_elastic_forces(): D.m_01 was not finite");
      assert( is_finite(D.m_10) || !"compute_elastic_forces(): D.m_10 was not finite");
      assert( is_finite(D.m_11) || !"compute_elastic_forces(): D.m_11 was not finite");

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

      assert( fabs(det_F)>0 || !"compute_elastic_forces(): Ill-posed deformed flat triangle found");

      //Convert to Cauchy stress tensor
      //
      //      j = det(F)
      //  sigma =  F S F^T / j
      //
      Tensor2 const sigma = mul( (1.0/det_F) , mul( mul( F, S  ) , trans(F) ) );

      // Compute elastic forces
      //
      //  The elastic force from e'th triangle on i'th node is given by
      //
      //      f_i^e = int_{A^e} sigma nabla w_i^e dA
      //
      //  where w_i^e is the local shape function of node i'th wrt. triangle e
      //
      //     nabla w_i^e  =  nabla A(j,k,x) / A^e  =    (1/2) hat(x_k - x_j) / A^e
      //
      //  Putting it together
      //
      //      f_i^e = int_{A^e} sigma nabla w_i^e dA
      //            = A^e sigma^e (1/2) hat(x_k - x_j) / A^e
      //            = sigma^e hat(x_k - x_j) / 2
      //
      double const ex_i = ( pxk - pxj );
      double const ey_i = ( pyk - pyj );
      double const ex_j = ( pxi - pxk );
      double const ey_j = ( pyi - pyk );
      double const ex_k = ( pxj - pxi );
      double const ey_k = ( pyj - pyi );

      assert( is_finite(ex_i) || !"compute_elastic_forces(): ex_i was not finite");
      assert( is_finite(ex_j) || !"compute_elastic_forces(): ex_j was not finite");
      assert( is_finite(ex_k) || !"compute_elastic_forces(): ex_k was not finite");
      assert( is_finite(ey_i) || !"compute_elastic_forces(): ey_i was not finite");
      assert( is_finite(ey_j) || !"compute_elastic_forces(): ey_j was not finite");
      assert( is_finite(ey_k) || !"compute_elastic_forces(): ey_k was not finite");

      double const hat_ex_i = - ey_i;
      double const hat_ey_i =   ex_i;
      double const hat_ex_j = - ey_j;
      double const hat_ey_j =   ex_j;
      double const hat_ex_k = - ey_k;
      double const hat_ey_k =   ex_k;

      assert( is_finite(hat_ex_i) || !"compute_elastic_forces(): hat_ex_i was not finite");
      assert( is_finite(hat_ex_j) || !"compute_elastic_forces(): hat_ex_j was not finite");
      assert( is_finite(hat_ex_k) || !"compute_elastic_forces(): hat_ex_k was not finite");
      assert( is_finite(hat_ey_i) || !"compute_elastic_forces(): hat_ey_i was not finite");
      assert( is_finite(hat_ey_j) || !"compute_elastic_forces(): hat_ey_j was not finite");
      assert( is_finite(hat_ey_k) || !"compute_elastic_forces(): hat_ey_k was not finite");

      double const fex_i =  ( sigma.m_00 * hat_ex_i + sigma.m_01 * hat_ey_i ) / 2.0;
      double const fey_i =  ( sigma.m_10 * hat_ex_i + sigma.m_11 * hat_ey_i ) / 2.0;

      double const fex_j =  ( sigma.m_00 * hat_ex_j + sigma.m_01 * hat_ey_j ) / 2.0;
      double const fey_j =  ( sigma.m_10 * hat_ex_j + sigma.m_11 * hat_ey_j ) / 2.0;

      double const fex_k =  ( sigma.m_00 * hat_ex_k + sigma.m_01 * hat_ey_k ) / 2.0;
      double const fey_k =  ( sigma.m_10 * hat_ex_k + sigma.m_11 * hat_ey_k ) / 2.0;

      assert( is_finite(fex_i) || !"compute_elastic_forces(): fex_i was not finite");
      assert( is_finite(fex_j) || !"compute_elastic_forces(): fex_j was not finite");
      assert( is_finite(fex_k) || !"compute_elastic_forces(): fex_k was not finite");
      assert( is_finite(fey_i) || !"compute_elastic_forces(): fey_i was not finite");
      assert( is_finite(fey_j) || !"compute_elastic_forces(): fey_j was not finite");
      assert( is_finite(fey_k) || !"compute_elastic_forces(): fey_k was not finite");

      Fxe[e*3u+0u] = fex_i;
      Fye[e*3u+0u] = fey_i;

      Fxe[e*3u+1u] = fex_j;
      Fye[e*3u+1u] = fey_j;

      Fxe[e*3u+2u] = fex_k;
      Fye[e*3u+2u] = fey_k;
    }
    
    for(unsigned int v = 0u; v < V; ++v)
    {
      unsigned int const start = neighbors.m_offset[v   ];
      unsigned int const end   = neighbors.m_offset[v+1u];
      for(unsigned int e = start; e<end; ++e)
      {
        unsigned int const idx     = neighbors.m_V2T[e].second;
        unsigned int const local_v = get_local_index( triangles[idx], v);
        
        Fx[v]  -=  Fxe[3*idx + local_v];
        Fy[v]  -=  Fye[3*idx + local_v];
      }
    }
  }

}// end of namespace elasticity

// ELASTICITY_COMPUTE_ELASTIC_FORCES_H
#endif