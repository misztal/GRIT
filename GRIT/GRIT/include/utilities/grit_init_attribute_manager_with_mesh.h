#ifndef GRIT_INIT_ATTRIBUTE_MANAGER_WITH_MESH_H
#define GRIT_INIT_ATTRIBUTE_MANAGER_WITH_MESH_H

#include <grit_attributes.h>
#include <grit_attribute_manager.h>
#include <grit_parameters.h>

#include <util_log.h>

namespace grit
{
  /** Initializes m_simplex0_labels based on the information provided in the @mesh and @parameters.
    *  Attribute vector m_simplex0_labels contains, for each vertex (Simplex0) a vector of unsigned int
    *  values which constitute the set of all triangle (Simplex2) labels, among triangles adjacent to given
    *  vertex. If ambient phase is specified, it is not contained in m_simplex0_labels.
    */
  template< typename types>
  inline void init_attribute_manager_with_mesh(
                              typename types::mesh_impl const & mesh
                              , details::Parameters const & parameters
                              , details::Attributes<types> & attributes
                              )
  {
    typedef details::AttributeManager<types>    attribute_manager;

    SimplexSet const all = filter( mesh.get_all_simplices(), IsDimension( mesh, 0u));

    for (SimplexSet::simplex0_const_iterator it = all.begin0(); it != all.end0(); ++it)
    {
      Simplex0 const & s = *it;

      SimplexSet const S = mesh.star(s);

      attribute_manager::clear_simplex0_labels( s, attributes);

      for (unsigned int i = 0u; i < parameters.labels().size(); ++i)
      {
        unsigned int const & label = parameters.labels()[i];
        if (parameters.use_ambient() && (label == parameters.ambient_label()))
        {
          continue;
        }

        SimplexSet const Q = filter( S, IsDimension( mesh, 2u) && InPhase( mesh, label));

        if (Q.size(2u) > 0u)
          attribute_manager::add_simplex0_label( s, label, attributes);

      }
    }
  }
} // namespace grit

// GRIT_INIT_ATTRIBUTE_MANAGER_WITH_MESH_H
#endif
