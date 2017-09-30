#ifndef AREA_COMPUTE_PERIMETER_GRADIENT_H
#define AREA_COMPUTE_PERIMETER_GRADIENT_H

#include <util_is_finite.h>

#include <glue_tuple.h>

#include <vector>
#include <cassert>
#include <cmath>

namespace area
{
  
  inline void compute_perimeter_gradient(
                                     std::vector<glue::Tuple> const & edges
                                     , std::vector<double>  const & x        // x-component of nodal positions
                                     , std::vector<double>  const & y        // y-component of nodal positions
                                     , std::vector<double>        & nabla_x  // x-component of area gradient
                                     , std::vector<double>        & nabla_y  // y-component of area gradient
                                     )
  {
    using std::sqrt;

    unsigned int const N = edges.size();

    nabla_x.resize( x.size() );
    nabla_y.resize( y.size() );

    std::fill(nabla_x.begin(), nabla_x.end(), 0.0);
    std::fill(nabla_y.begin(), nabla_y.end(), 0.0);

    for(unsigned int e=0u; e<N;++e)
    {
      unsigned int const i = edges[e].i;
      unsigned int const j = edges[e].j;

      assert( i!=j || !"compute_perimeter_gradient(): i and j was the same");

      double const xi = x[i];
      double const xj = x[j];
      double const yi = y[i];
      double const yj = y[j];

      double const Le = sqrt(   (xi-xj)*(xi-xj) + (yi-yj)*(yi-yj));

      assert( is_finite(Le) || !"compute_perimeter_gradient(): Le was not finite");
      assert( Le>0          || !"compute_perimeter_gradient(): Le was non positive");

      double const nabla_ix = (xi-xj)/Le;
      double const nabla_iy = (yi-yj)/Le;
      double const nabla_jx = (xj-xi)/Le;
      double const nabla_jy = (yj-yi)/Le;

      nabla_x[i] += nabla_ix;
      nabla_x[j] += nabla_jx;
      nabla_y[i] += nabla_iy;
      nabla_y[j] += nabla_jy;
    }
  }

}// end of namespace area

// AREA_COMPUTE_PERIMETER_GRADIENT_H
#endif
