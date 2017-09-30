#ifndef MAGNETISM_COMPUTE_TRIANGLE_CENTERS
#define MAGNETISM_COMPUTE_TRIANGLE_CENTERS

#include <glue_triplet.h>

#include <vector>

namespace magnetism
{
  template<typename T>
  inline void compute_triangle_centers(
                                       std::vector<glue::Triplet> const & triangles
                                       , std::vector<T> const       & px
                                       , std::vector<T> const       & py
                                       , std::vector<T>             & cx
                                       , std::vector<T>             & cy
                                       )
  {
    unsigned int const N = triangles.size();   // Number of triangles

    cx.resize(N, 0.0);
    cy.resize(N, 0.0);

    std::fill( cx.begin(), cx.end(), 0.0 );
    std::fill( cy.begin(), cy.end(), 0.0 );

    for (unsigned int i = 0u; i<N; ++i)
    {
      glue::Triplet const triangle = triangles[i];
      cx[i] = (px[triangle.i] + px[triangle.j] + px[triangle.k])/3.0;
      cy[i] = (py[triangle.i] + py[triangle.j] + py[triangle.k])/3.0;
    }

  }

}//namespace magnetism


// MAGNETISM_COMPUTE_TRIANGLE_CENTERS
#endif