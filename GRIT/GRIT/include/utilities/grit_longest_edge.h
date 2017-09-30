#ifndef GRIT_LONGEST_EDGE_H
#define GRIT_LONGEST_EDGE_H

#include <grit_interface_mesh.h>
#include <grit_attribute_vector.h>

#include <util_edge_length.h>

namespace grit
{
  /// Return the length of the longest edge in SimplexSet @param S,
  /// on return @param max_edge is the longest edge handle.
  template <typename MT>
  inline typename MT::real_type longest_edge(
                               SimplexSet const & S
                               , Simplex0AttributeVector<typename MT::vector3_type> const & current
                               , Simplex1 & max_edge
                               )
  {
    typedef typename MT::vector3_type V;
    typedef typename MT::real_type    T;
    typedef typename V::value_traits  VT;

    T max_sqr_length = VT::zero();

    for (SimplexSet::simplex1_const_iterator it = S.begin1(); it != S.end1(); ++it)
    {
      Simplex1 const & e = *it;

      Simplex0 const & a = e.get_simplex0_idx0();
      Simplex0 const & b = e.get_simplex0_idx1();

      V const r = current[a]-current[b];

      T const d = r[0u]*r[0u]+r[1u]*r[1u];

      if (d > max_sqr_length)
      {
        max_sqr_length = d;
        max_edge = e;
      }
    }

    return max_sqr_length;
  }

  /// Return the length of the longest edge in triangle @param simplex,
  /// on return @param max_edge is the longest edge handle.
  template <typename MT>
  inline typename MT::real_type longest_edge(
                               InterfaceMesh const & mesh
                               , Simplex0AttributeVector<typename MT::vector3_type> const & current
                               , Simplex2 const & simplex
                               , Simplex1 & max_edge
                               )
  {
    SimplexSet const B = mesh.full_boundary(simplex);

    return longest_edge<MT>(B, current, max_edge);
  }
} // end namespace grit

// GRIT_LONGEST_EDGE_H
#endif
