#ifndef GLUE_NEIGHBORS_H
#define GLUE_NEIGHBORS_H

#include <vector>

namespace glue
{
  /**
   * Neighborhood Information Class.
   * The offset array index into the V2T mapping such that all
   * triangles of vertex i is given by the index range
   *
   *    V2T[ offset(i) ],... V2T[ offset(i+1) ]
   *
   * The same data structue can be used to hold tuple
   * neighbors of a vertex.
   *
   * This type of neighborhood information is expensive to compute as
   * it involves a sorting of data. However, it is very useful in for
   * instance finite element assemblies where one wishes to iterate
   * over all element neighbors of a give vertex to accumate each elements
   * "contribution" to that vertex.
   *
   *
   * Here is a typical example usage showing how to accumate vertex data
   * from some element data.
   *
   *   unsigned int const V = ... number of vertices in GRIT ...
   *
   *
   *   glue::Phase A = glue::make_phase(my_engine, 1);
   *
   *   unsigned int const N = ... number of elements in A ...
   *
   *   std::vector<some_type> element_data( 3*N );
   *
   *    ... loop over element and for each element compute three data values ...
   *
   *   glue::Neighbors neighbors;
   *   glue::compute_neighbors(A.m_triangles,V,neighbors);
   *
   *   std::vector< some_type > accumulated_data( V );
   *
   *   for(unsigned int v = 0u; v < V; ++v)
   *   {
   *     unsigned int const start = neighbors.m_offset[v   ];
   *     unsigned int const end   = neighbors.m_offset[v+1u];
   *     for(unsigned int e = start; e<end; ++e)
   *     {
   *       unsigned int const idx     = neighbors.m_V2T[e].second;
   *       unsigned int const local_v = get_local_index( triangles[idx], v);
   *
   *       accumulated_data[v]  -=  element_data[3*idx + local_v];
   *     }
   *   }
   *
   *
   *
   * @see glue::compute_neighbors for more details.
   */
  class Neighbors
  {
  public:

    std::vector<std::pair<unsigned int, unsigned int> > m_V2T;    ///< This map contains node to triangle mapping. Node indices are stored incrementally
    std::vector<unsigned int>                           m_offset; ///< Offset(i) tells the startig index in the V2T mapping for vertex i's mapping info.

  };

}// namespace glue

// GLUE_NEIGHBORS_H
#endif
