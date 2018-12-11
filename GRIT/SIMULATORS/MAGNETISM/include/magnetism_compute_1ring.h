#ifndef MAGNETISM_COMPUTE_1RING_H
#define MAGNETISM_COMPUTE_1RING_H

#include <glue_triplet.h>

#include <util_log.h>

#include <util_cpp11.h>

#include <vector>
#include <list>

namespace magnetism
{

  /**
   *
   * @param index  The index of triangle for which we evaluate the neighborhood.
   * @param ring   Upon return this argument holds the 1-ring neighborhood of triangles.
   */
  inline void compute_1ring(
                            std::vector<unsigned int>    const & labels
                            , std::vector<glue::Triplet> const & triangles
                            , std::vector<glue::Triplet> const & neighbors
                            , unsigned int const               & index
                            , std::vector<unsigned int>        & ring
                            )
  {
    glue::Triplet const triangle      = triangles[index];
    unsigned int  const label         = labels[index];

    std::vector< bool > processed(triangles.size(), false);

    ring.clear();
    ring.push_back(index);
    processed[index] = true;

    std::list< unsigned int > work;

    for (unsigned int i = 0; i < 3; ++i)
    {
      unsigned int const n = neighbors[index](i);

      if ( n == glue::Triplet::UNDEFINED() )
        continue;

      if ( labels[n] != label )
        continue;

      work.push_back( n );
    }

    while (not work.empty())
    {
      unsigned int const current = work.front();

      work.pop_front();

      if (processed[current])
        continue;

      ring.push_back(current);

      processed[current] = true;

      for (unsigned int i = 0; i < 3; ++i)
      {
        unsigned int const n = neighbors[current](i);

        if ( n == glue::Triplet::UNDEFINED() )
          continue;

        if ( labels[n] != label )
          continue;

        if ( processed[n] )
          continue;

        glue::Triplet const neighbor = triangles[n];

        bool const touching = neighbor.has_member(triangle.i) || neighbor.has_member(triangle.j) || neighbor.has_member(triangle.k);
        
        if (not touching)
          continue;
        
        work.push_back( n );
        
      }//end for
      
    }//end while
  }
  
}// namespace magnetism

// MAGNETISM_COMPUTE_1RING_H
#endif
