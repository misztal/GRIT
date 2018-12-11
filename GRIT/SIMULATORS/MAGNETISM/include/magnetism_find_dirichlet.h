#ifndef MAGNETISM_FIND_DIRICHLET_H
#define MAGNETISM_FIND_DIRICHLET_H

#include <glue_triplet.h>

#include <algorithm>
#include <vector>

namespace magnetism
{

  /**
   * Try and find the lowest left most triangle in domain to put dirichlet condition on
   */
  template<typename T>
  inline unsigned int find_dirichlet(
                                     std::vector<glue::Triplet> const & triangles
                                     , std::vector<T> const & x
                                     , std::vector<T> const & y
                                     )
  {
    using std::min;

    T x_min = min( x[triangles[0].i], min( x[triangles[0].j], x[triangles[0].k] ) );
    T y_min = min( y[triangles[0].i], min( x[triangles[0].j], x[triangles[0].k] ) );

    unsigned int dirichlet = 0u;

    for(unsigned int r =1u; r < triangles.size(); ++r)
    {
      T const x_tst = min( x[triangles[r].i], min( x[triangles[r].j], x[triangles[r].k] ) );
      T const y_tst = min( y[triangles[r].i], min( x[triangles[r].j], x[triangles[r].k] ) );

      if( x_tst < x_min )
      {
        dirichlet = r;
      }
      else if( x_tst == x_min && y_tst < y_min )
      {
        dirichlet = r;
      }
    }

    return dirichlet;
  }
  
}//namespace magnetism

// MAGNETISM_FIND_DIRICHLET_H
#endif
