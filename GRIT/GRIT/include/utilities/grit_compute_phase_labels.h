#ifndef GRIT_COMPUTE_PHASE_LABELS_H
#define GRIT_COMPUTE_PHASE_LABELS_H

#include <grit_simplex.h>
#include <grit_simplex_set.h>
#include <grit_interface_mesh.h>

#include <algorithm>
#include <vector>

namespace grit
{
  /**
   * Compute Phase Labels.
   * This function finds all phase labels (ids) in a mesh and returns an
   * array with the labels sorted in increasing order.
   *
   * @param mesh    The mesh from which phase labels should be extracted
   * @param labels  Upon return this array holds all the phase labels in the mesh
   *
   */
  inline void compute_phase_labels(InterfaceMesh const & mesh, std::vector<unsigned int> & labels)
  {
    labels.clear();

    SimplexSet const all = mesh.get_all_simplices();

    SimplexSet::simplex2_const_iterator begin = all.begin2();
    SimplexSet::simplex2_const_iterator end   = all.end2();

    for (SimplexSet::simplex2_const_iterator s = begin; s != end; ++s)
    {
      unsigned int const value = mesh.label( *s );

      if (std::find(labels.begin(), labels.end(), value)==labels.end())
        labels.push_back(value);
    }

    std::sort( labels.begin(), labels.end() );
  }

} // end namespace grit

// GRIT_COMPUTE_PHASE_LABELS_H
#endif
