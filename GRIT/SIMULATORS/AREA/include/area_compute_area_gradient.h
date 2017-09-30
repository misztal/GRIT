#ifndef AREA_COMPUTE_AREA_GRADIENT_H
#define AREA_COMPUTE_AREA_GRADIENT_H

#include <util_is_finite.h>

#include <glue_triplet.h>

#include <vector>
#include <cassert>

namespace area
{
  
  inline void compute_area_gradient(
                                     std::vector<glue::Triplet> const & triangles
                                     , std::vector<double>  const & x        // x-component of nodal positions
                                     , std::vector<double>  const & y        // y-component of nodal positions
                                     , std::vector<double>        & nabla_x  // x-component of area gradient
                                     , std::vector<double>        & nabla_y  // y-component of area gradient
                                     )
  {
    unsigned int const N = triangles.size();

    nabla_x.resize( x.size() );
    nabla_y.resize( y.size() );

    std::fill(nabla_x.begin(), nabla_x.end(), 0.0);
    std::fill(nabla_y.begin(), nabla_y.end(), 0.0);

    for(unsigned int e=0u; e<N;++e)
    {
      unsigned int const i = triangles[e].i;
      unsigned int const j = triangles[e].j;
      unsigned int const k = triangles[e].k;

      assert( i!=j || !"compute_area_gradient(): i and j was the same");
      assert( i!=k || !"compute_area_gradient(): i and k was the same");
      assert( j!=k || !"compute_area_gradient(): j and k was the same");

      double const xi = x[i];
      double const xj = x[j];
      double const xk = x[k];

      double const yi = y[i];
      double const yj = y[j];
      double const yk = y[k];

      double const ax = (xj-xi);
      double const ay = (yj-yi);
      double const bx = (xk-xi);
      double const by = (yk-yi);

      assert( is_finite(ax) || !"compute_area_gradient(): ax was not finite");
      assert( is_finite(ay) || !"compute_area_gradient(): ay was not finite");
      assert( is_finite(bx) || !"compute_area_gradient(): bx was not finite");
      assert( is_finite(by) || !"compute_area_gradient(): by was not finite");

      double const Ae = (ax*by - bx*ay)/2.0;

      assert( is_finite(Ae) || !"compute_area_gradient(): Ae was not finite");
      assert( Ae>0         || !"compute_area_gradient(): Ae was non positive");

      double const hat_e_kj_x = -(yk-yj);
      double const hat_e_kj_y =  (xk-xj);
      double const hat_e_ik_x = -(yi-yk);
      double const hat_e_ik_y =  (xi-xk);
      double const hat_e_ji_x = -(yj-yi);
      double const hat_e_ji_y =  (xj-xi);

      double const nabla_ix = 0.5*hat_e_kj_x;
      double const nabla_iy = 0.5*hat_e_kj_y;
      double const nabla_jx = 0.5*hat_e_ik_x;
      double const nabla_jy = 0.5*hat_e_ik_y;
      double const nabla_kx = 0.5*hat_e_ji_x;
      double const nabla_ky = 0.5*hat_e_ji_y;

      nabla_x[i] += nabla_ix;
      nabla_x[j] += nabla_jx;
      nabla_x[k] += nabla_kx;

      nabla_y[i] += nabla_iy;
      nabla_y[j] += nabla_jy;
      nabla_y[k] += nabla_ky;
    }
  }

}// end of namespace area

// AREA_COMPUTE_AREA_GRADIENT_H
#endif
