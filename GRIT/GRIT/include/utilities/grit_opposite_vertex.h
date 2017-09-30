#ifndef GRIT_OPPOSITE_VERTEX_H
#define GRIT_OPPOSITE_VERTEX_H

#include <grit_interface_mesh.h>

namespace grit
{
  /// Returns the opposite vertex to @param edge in triangle @param simplex.
  inline Simplex0 opposite_vertex(
                                  InterfaceMesh const & mesh
                                  , Simplex2 const & simplex
                                  , Simplex1 const & edge
                                  )
  {
    SimplexSet B = mesh.full_boundary(simplex);
    SimplexSet Be = mesh.boundary(edge);
    SimplexSet const D  = difference(B,Be);

    assert( D.size(0u) == 1u || !"opposite_vertex(): incorrect number of vertices" );

    return *(D.begin0()); // vertex opposite to the max_edge
  }
}

// GRIT_OPPOSITE_VERTEX_H
#endif
