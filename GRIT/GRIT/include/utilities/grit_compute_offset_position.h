#ifndef GRIT_COMPUTE_OFFSET_POSITION_H
#define GRIT_COMPUTE_OFFSET_POSITION_H

#include <grit_simplex.h>
#include <grit_interface_mesh.h>
#include <grit_logic_expressions.h>
#include <grit_simplex_set.h>
#include <grit_simplex_set_functions.h>

#include <utilities/grit_compute_normal.h>

namespace grit
{
  namespace details
  {
    /**
     * This function computes the projection of an interface vertex v onto
     * the offset of the boundary of the phase specified by label. The offset
     * is in the specified distance from the boundary, which is positive in
     * the outwards direction, and negative in the inwards direction.
     */
    template<
             typename   MT
             , typename AT ///< attribute manager types
             >
    inline typename MT::vector3_type compute_offset_position(
                                                             Simplex0                 const & v
                                                             , InterfaceMesh          const & mesh
                                                             , AT                     const & attributes
                                                             , unsigned int           const & label
                                                             , typename MT::real_type const & distance
                                                             )
    {
      typedef typename MT::vector3_type V;
      typedef typename MT::real_type    T;
      typedef typename V::value_traits  VT;

      assert( IsInterface(mesh)(v) || !"compute_offset_position(): Internal error A");

      SimplexSet const Sv = mesh.star(v);

      SimplexSet const I = filter(
                                  Sv
                                  , IsDimension( mesh, 1u)
                                  && InPhase( mesh, label)
                                  && IsInterface(mesh)
                                  );

      assert( I.size(1u)==2u || !"compute_offset_position(): Internal error B");

      std::vector<V> normals;

      SimplexSet::simplex1_const_iterator it = I.begin1();
      for( ; it != I.end1(); ++it)
      {
        Simplex1 const & s = *it;
        V const n = compute_normal<MT>( s, mesh, attributes, label);
        normals.push_back(n);
      }

      V const   n   = MT::normalize(normals[0u]+normals[1u]);
      V const & pos = attributes.get_current_value(v);

      T const   c   = MT::dot( n, normals[0u]);
      assert( c>VT::zero() || !"compute_offset_position(): Interal error C");

      return pos + (distance/c)*n;
    }

  } // namespace details
} // namespace grit

// GRIT_COMPUTE_OFFSET_POSITION
#endif
