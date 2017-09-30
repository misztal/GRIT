#ifndef GRIT_HAS_EAR_H
#define GRIT_HAS_EAR_H

#include <grit_simplex.h>
#include <grit_interface_mesh.h>

namespace grit
{

  /**
   * Tests if any triangles of the mesh is an ear (has two open boundary
   * edges). This is often the cases for "corners" of rectangular
   * domains.
   */
  inline bool has_ear(InterfaceMesh & mesh)
  {
    SimplexSet const all = mesh.get_all_simplices();

    SimplexSet::simplex2_const_iterator begin = all.begin2();
    SimplexSet::simplex2_const_iterator end   = all.end2();

    for (SimplexSet::simplex2_const_iterator s2 = begin; s2 != end; ++s2)
    {
      if (mesh.is_ear(*s2))
        return true;
    }

    return false;
  }

} // namespace grit

// GRIT_HAS_EAR_H
#endif