#ifndef AREA_COMPUTE_PERIMETER_H
#define AREA_COMPUTE_PERIMETER_H

#include <util_is_finite.h>

#include <glue_tuple.h>

#include <vector>
#include <cassert>
#include <cmath>

namespace area
{
  
  inline double compute_perimeter(
                                     std::vector<glue::Tuple> const & edges
                                     , std::vector<double>  const & x        // x-component of nodal positions
                                     , std::vector<double>  const & y        // y-component of nodal positions
                                     )
  {
    using std::sqrt;

    double L = 0.0;

    unsigned int const N = edges.size();

    for(unsigned int e=0u; e<N;++e)
    {
      unsigned int const i = edges[e].i;
      unsigned int const j = edges[e].j;

      assert( i!=j || !"compute_perimeter(): i and j was the same");

      double const xi = x[i];
      double const xj = x[j];
      double const yi = y[i];
      double const yj = y[j];

      double const Le = sqrt(   (xi-xj)*(xi-xj) + (yi-yj)*(yi-yj));

      assert( is_finite(Le) || !"compute_perimeter(): Le was not finite");
      assert( Le>0         || !"compute_perimeter(): Le was non positive");

      L += Le;
    }

    return L;
  }

}// end of namespace area

// AREA_COMPUTE_PERIMETER_H
#endif
