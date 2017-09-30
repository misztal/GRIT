#ifndef GRIT_COMPUTE_NORMAL_H
#define GRIT_COMPUTE_NORMAL_H

#include <grit_simplex.h>
#include <grit_logic_expressions.h>
#include <grit_interface_mesh.h>
#include <grit_simplex_set.h>
#include <grit_simplex_set_functions.h>

#include <utilities/grit_extract_simplices.h>

namespace grit
{
  /**
   * Computes the normal vector to an interface edge s, pointing outwards
   * with respect to phase specified by label. If s does not lie on the interface
   * or if it is not adjacent to a triangle with specified label, returns 0-vector.
   */
  namespace details
  {
     template<
             typename   MT
             , typename AT ///< attribute manager types
             >
    inline typename MT::vector3_type compute_normal(
                                                    Simplex1        const & s
                                                    , InterfaceMesh const & mesh
                                                    , AT            const & attributes
                                                    , unsigned int  const & label
                                                    )
    {
      typedef typename MT::vector3_type V;
      typedef typename V::value_traits  VT;

      assert( IsInterface(mesh)(s) || !"compute_normal(): Internal error A");

      SimplexSet const Vs = filter( mesh.closure(s), IsDimension( mesh, 0u));

      Simplex0 v1, v2;
      extract_simplices( v1, v2, Vs);

      V const e = MT::normalize( attributes.get_current_value(v2) - attributes.get_current_value(v1));

      V const result( -e[1u], e[0u], VT::zero());

      SimplexSet const T = filter( mesh.star(s), IsDimension( mesh, 2u)&&InPhase( mesh, label));

      assert( T.size(2u)==1u || !"compute_normal(): Internal error B");

      SimplexSet const C = mesh.closure(T);
      SimplexSet const X = difference(C,Vs);

      assert( X.size(0u)==1u || !"compute_normal(): Internal error C");

      Simplex0 const vo = *(X.begin0());

      V const r = attributes.get_current_value(vo) - attributes.get_current_value(v1);

      if( MT::dot( r, result) < VT::zero())
        return result;
      else
        return -result;
    }

  } // namespace details
} // namespace grit

// GRIT_COMPUTE_NORMAL_H
#endif
