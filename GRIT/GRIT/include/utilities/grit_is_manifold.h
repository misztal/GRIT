#ifndef GRIT_IS_MANIFOLD_H
#define GRIT_IS_MANIFOLD_H

#include <grit_simplex.h>
#include <grit_interface_mesh.h>

namespace grit
{
  // 29-03-2015 Marek: this function checks if the mesh is valid, in the sense whether
  //                   all vertices and edges are 2-manifold (i.e. are not isolated).
  inline bool is_manifold(InterfaceMesh & mesh)
  {
    SimplexSet const all = mesh.get_all_simplices();

    SimplexSet::simplex1_const_iterator begin1 = all.begin1();
    SimplexSet::simplex1_const_iterator end1   = all.end1();

    for (SimplexSet::simplex1_const_iterator s1 = begin1; s1 != end1; ++s1)
    {
      if (!mesh.is_manifold(*s1))
        return false;
    }

    SimplexSet::simplex0_const_iterator begin0 = all.begin0();
    SimplexSet::simplex0_const_iterator end0   = all.end0();

    for (SimplexSet::simplex0_const_iterator s0 = begin0; s0 != end0; ++s0)
    {
      if (!mesh.is_manifold(*s0))
        return false;
    }

    // 2015-30-03 Kenny code review: Could it happen that we could have
    // triangles made of redundant vertices? two bad cases could
    // exist (v,v,v), (v,v,w)? I think the underlying OT mesh forbids
    // this, but from higher abstraction layer (and if we one day swap
    // OT out) we may wish to test for this?

    return true;
  }

} // namespace grit

// GRIT_IS_MANIFOLD_H
#endif