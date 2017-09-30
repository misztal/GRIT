#ifndef GLUE_VECTOR_ASSEMBLY_H
#define GLUE_VECTOR_ASSEMBLY_H

#include <util_log.h>
#include <util_is_finite.h>
#include <util_is_number.h>
#include <util_tensors.h>

#include <glue_tuple.h>
#include <glue_compute_neighbors.h>

#include <cassert>
#include <vector>
#include <map>

namespace glue
{

  /**
   *
   * @param Ae   The element-wise data to be assembled. 
   */
  template <typename T>
  inline std::vector<T> vector_assembly(
                                          std::vector<Tuple> const & edges
                                        , unsigned int const & V
                                        , std::vector<util::Tensor1> const & Ae
                                        , bool const & interlaced = true
                                        )
  {
    assert(Ae.size() == edges.size() || !"vector_assembly(): edges and Ae must have same length (one vector per edge)");

    std::vector<T> x;

    glue::Neighbors neighbors;
    glue::compute_neighbors(edges, V, neighbors);

    unsigned int const nvars = 2u*V;

    x.resize(nvars);

    for (unsigned int r = 0u; r < V; ++r)
    {
      unsigned int const start = neighbors.m_offset[r];
      unsigned int const end   = neighbors.m_offset[r + 1u];

      unsigned int const r_x  = interlaced ? 2*r  + 0u : r + 0u;
      unsigned int const r_y  = interlaced ? 2*r  + 1u : r + V;

      x[ r_x ] = 0;
      x[ r_y ] = 0;

      for (unsigned int e = start; e<end; ++e)
      {
        unsigned int const   idx  = neighbors.m_V2T[e].second;
        util::Tensor1 const & E = Ae[ idx];

        x[ r_x ] += E.m_0;
        x[ r_y ] += E.m_1;
      }
    }
    
    return x;
  }


  template <typename T>
  inline std::vector<T> vector_assembly(
                                        std::vector<Tuple> const & edges
                                        , unsigned int const & V
                                        , std::vector<util::Block2x1Tensor1> const & Ae
                                        , bool const & interlaced = true
                                        )
  {
    assert(Ae.size() == edges.size() || !"vector_assembly(): edges and Ae must have same length (one vector per edge)");

    std::vector<T> x;

    glue::Neighbors neighbors;
    glue::compute_neighbors(edges, V, neighbors);

    unsigned int const nvars = 2u*V;

    x.resize(nvars);

    for (unsigned int r = 0u; r < V; ++r)
    {
      unsigned int const start = neighbors.m_offset[r];
      unsigned int const end   = neighbors.m_offset[r + 1u];

      unsigned int const r_x  = interlaced ? 2*r  + 0u : r + 0u;
      unsigned int const r_y  = interlaced ? 2*r  + 1u : r + V;

      x[ r_x ] = 0;
      x[ r_y ] = 0;

      for (unsigned int e = start; e<end; ++e)
      {
        unsigned int const   idx  = neighbors.m_V2T[e].second;
        glue::Tuple  const & edge = edges[idx];

        util::Tensor1 const & E = Ae[idx].m_block[get_local_index(edge, r)];

        x[ r_x ] += E.m_0;
        x[ r_y ] += E.m_1;
      }
    }
    
    return x;
  }

}//namespace glue

// GLUE_VECTOR_ASSEMBLY_H
#endif
