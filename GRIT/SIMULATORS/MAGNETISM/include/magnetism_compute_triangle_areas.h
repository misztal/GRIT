#ifndef MAGNETISM_COMPUTE_TRIANGLE_AREAS_H
#define MAGNETISM_COMPUTE_TRIANGLE_AREAS_H

#include <util_is_finite.h>

#include <glue_triplet.h>

#include <vector>
#include <cassert>

namespace magnetism
{

  template<typename T>
  inline void compute_triangle_areas(
                                     std::vector<glue::Triplet> const & triangles
                                     , std::vector<T>  const & x       // x-component of nodal positions
                                     , std::vector<T>  const & y       // y-component of nodal positions
                                     , std::vector<T>        & Ae
                                     )
  {
    unsigned int const N = triangles.size();

    Ae.resize(N);

    std::fill( Ae.begin(),  Ae.end(),  0.0);

    for(unsigned int e=0u; e<N;++e)
    {
      unsigned int const i = triangles[e].i;
      unsigned int const j = triangles[e].j;
      unsigned int const k = triangles[e].k;

      assert( i!=j || !"compute_triangle_areas(): i and j was the same");
      assert( i!=k || !"compute_triangle_areas(): i and k was the same");
      assert( j!=k || !"compute_triangle_areas(): j and k was the same");

      T const xi = x[i];
      T const xj = x[j];
      T const xk = x[k];

      T const yi = y[i];
      T const yj = y[j];
      T const yk = y[k];

      T const ax = (xj-xi);
      T const ay = (yj-yi);
      T const bx = (xk-xi);
      T const by = (yk-yi);

      assert( is_finite(ax) || !"compute_triangle_areas(): ax was not finite");
      assert( is_finite(ay) || !"compute_triangle_areas(): ay was not finite");
      assert( is_finite(bx) || !"compute_triangle_areas(): bx was not finite");
      assert( is_finite(by) || !"compute_triangle_areas(): by was not finite");

      Ae[e] = (ax*by - bx*ay)/2.0;

      assert( is_finite(Ae[e]) || !"compute_triangle_areas(): Ae[e] was not finite");
      assert( Ae[e]>0         || !"compute_triangle_areas(): Ae[e] was non positive");
    }
  }
  
}//namespace magnetism

// MAGNETISM_COMPUTE_TRIANGLE_AREAS_H
#endif
