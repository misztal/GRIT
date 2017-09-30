#ifndef GRIT_RELABEL_PHASE_H
#define GRIT_RELABEL_PHASE_H

#include <grit_interface_mesh.h>
#include <grit_simplex.h>
#include <grit_simplex_set.h>

namespace grit
{

  /**
   * Change Phase Labels.
   * This function finds all given phase labels (ids) in a mesh and change
   * their value to the specified label.
   *
   * @param mesh    The mesh from which phase labels should be extracted
   * @param old_labels  Correspond to "input_labels" in cfg-files
   * @param new_labels  Correspond to "labels" in cfg-files
   *
   */
  inline void relabel_phase(
                            InterfaceMesh & mesh
                            , std::vector<unsigned int> const & old_labels
                            , std::vector<unsigned int> const & new_labels
                            )
  {
    SimplexSet const all = mesh.get_all_simplices();

    SimplexSet::simplex2_const_iterator begin = all.begin2();
    SimplexSet::simplex2_const_iterator end   = all.end2();

    unsigned int const n_old = old_labels.size();
    unsigned int const n_new = new_labels.size();

    assert(n_old == n_new || !"relabel_phase() error, old_labels.size() != new_labels.size(). Check input_labels and labels params in cfg file.");

    for (SimplexSet::simplex2_const_iterator it = begin; it != end; ++it)
    {
      Simplex2 const & t = *it;

      unsigned int const value = mesh.label(t);

      for( unsigned int i = 0u; i < n_old; ++i)
      {
        if( value == old_labels[i])
        {
          mesh.label(t) = new_labels[i];
        }
      }
    }
  }

} // end namespace grit

// GRIT_RELABEL_PHASE_H
#endif
