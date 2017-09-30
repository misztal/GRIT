#ifndef GRIT_MERGE_QUALITY_MEASURE_H
#define GRIT_MERGE_QUALITY_MEASURE_H

#include <grit_interface_quality_measure.h>
#include <grit_interface_mesh.h>

#include <grit_logic_expressions.h>
#include <grit_simplex.h>
#include <grit_simplex_set.h>
#include <grit_simplex_set_functions.h>

#include <utilities/grit_extract_simplices.h>
#include <utilities/grit_opposite_vertex.h>
#include <utilities/grit_longest_edge.h>

#include <util_triangle_area.h>

#include <string>

namespace grit
{
  namespace details
  {
    /// This quality measure is used by the TriangleSplit operation, which is a part of the block
    /// taking care of the interface merging. It checks whether a triangle "sandwiched" between
    /// interface edge and vertex has height lower than the threshold specified in the config file.
    template <typename types>
    class MergeQualityMeasure
    : public InterfaceQualityMeasure<Simplex2>
    {
    protected:

      typedef typename types::attributes_type  AT;
      typedef typename types::param_type       PT;
      typedef typename types::math_types       MT;
      typedef typename types::real_type        T;
      typedef typename types::vector3_type     V;

      typedef typename V::value_traits         VT;

    protected:

      InterfaceMesh const & m_mesh;
      AT            const & m_attributes;

      T             const   m_angle_threshold; ///< Threshold on the maximum angle
      T             const   m_distance_threshold; ///< Threshold on the minimum height

    protected:

      V const & get_coordinates( Simplex0 const & s) const
      {
        return this->m_attributes.current()[ s ];
      }

    public:

      MergeQualityMeasure(
                          std::string    const & operation_name
                          , unsigned int const & label
                          , PT           const & parameters
                          , InterfaceMesh      const & mesh
                          , AT           const & attributes
                          )
      : m_mesh(mesh)
      , m_attributes(attributes)
      , m_angle_threshold(MT::deg_to_rad(parameters.get_angle_threshold(operation_name,label)))
      , m_distance_threshold(parameters.get_distance_threshold(operation_name,label))
      {}

      bool is_bad(Simplex2 const & s) const
      {
        Simplex1 edge;

        /*SimplexSet const B  = filter( m_mesh.boundary(s), 1u);
        SimplexSet const Bi = filter( m_mesh.boundary(s), IsInterface(m_mesh));

        if( Bi.size(0u) <= 1u)
        {
          return false;
        }

        longest_edge<MT>( B, m_attributes.current, edge);
        */

        // Finds the longest interface edge
        longest_edge<MT>( m_mesh, m_attributes.current(), s, edge);

        assert( IsValid(m_mesh)(edge) || !"MergeQualityMeasure::is_bad(): internal error" );

        if (!IsInterface(m_mesh)(edge)) return false;

        SimplexSet const Be = m_mesh.boundary(edge);

        Simplex0 const vo = opposite_vertex( m_mesh, s, edge); // vertex opposite to the max_edge

        if (!IsInterface(m_mesh)(vo)) return false;

        Simplex0 v1, v2;
        extract_simplices(v1, v2, Be);

        V const p1 = m_attributes.get_current_value(v1);
        V const p2 = m_attributes.get_current_value(v2);
        V const po = m_attributes.get_current_value(vo);


        SimplexSet const S = m_mesh.star(edge);

        SimplexSet::simplex2_const_iterator it = S.begin2();

        Simplex2 t = *it;

        if( t == s)
        {
          t = *(++it);
        }

        Simplex0 const w = opposite_vertex( m_mesh, t, edge);
        V const pw  = m_attributes.get_current_value(w);

        V const ea0 = pw-p1;
        V const ea1 = po-p1;

        V const eb0 = pw-p2;
        V const eb1 = po-p2;

        T const a0 = (ea0[0u]*ea1[1u]-ea0[1u]*ea1[0u]);
        T const a1 = (eb0[0u]*eb1[1u]-eb0[1u]*eb1[0u]);

        // 2016-06-01 Marek: todo: replace with signed_triangle_area
        //--- this is so that we don't insert inverted triangles
        if( a0*a1 > 0.0 || std::fabs(a0) < 1e-9 || std::fabs(a1) < 1e-9 )
        {
          return false;
        }

        V const e  = MT::normalize(p2-p1);
        V const r  = po-p1;
        V const h  = r-e*MT::dot(r,e);

        V const e1 = MT::normalize(p1-po);
        V const e2 = MT::normalize(p2-po);

        if( MT::dot(e1,e2) > std::cos( m_angle_threshold)
           && h[0u]*h[0u]+h[1u]*h[1u] > m_distance_threshold*m_distance_threshold
           )
        {
          return false;
        }

        return true;
      }
    };

  } // end namespace details
}//end namespace grit

// GRIT_MERGE_QUALITY_MEASURE
#endif
