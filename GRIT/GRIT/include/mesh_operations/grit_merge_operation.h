#ifndef GRIT_MERGE_OPERATION_H
#define GRIT_MERGE_OPERATION_H

#include <grit_interface_mesh_operation.h>
#include <grit_interface_mesh.h>

#include <grit_logic_expressions.h>
#include <grit_simplex.h>
#include <grit_simplex_set.h>
#include <grit_simplex_set_functions.h>

#include <utilities/grit_extract_simplices.h>
#include <utilities/grit_longest_edge.h>
#include <utilities/grit_opposite_vertex.h>
#include <utilities/grit_find_collapsable_vertices.h>
#include <utilities/grit_is_any_inverted.h>

#include <util_log.h>

#include <string>
#include <map>

namespace grit
{
  namespace details
  {
    ///
    template< typename types>
    class MergeOperation
    : public InterfaceMeshOperation< Simplex2, types>
    {
    protected:

      typedef typename types::real_type        T;
      typedef typename types::vector3_type     V;
      typedef typename V::value_traits         VT;

      typedef typename types::math_types       MT;

      typedef typename types::attributes_type  AMT;
      typedef typename types::param_type       PT;

    protected:

      std::string  const & m_name;                  ///< Name used to retrieve parameters.
      unsigned int const   m_label;                 ///< Label of the phase on which operation is performed.

      PT           const & m_parameters;            ///< Parameters container.

      T            const   m_distance_threshold;

      T            const   m_squared_distance;

    public:

      MergeOperation(
                    std::string    const & operation_name
                    , unsigned int const   label
                    , PT           const & parameters
                    )
      : m_name( operation_name)
      , m_label( label)
      , m_parameters( parameters)
      , m_distance_threshold(parameters.get_distance_threshold(operation_name,label))
      , m_squared_distance(m_distance_threshold*m_distance_threshold)
      { }

      void init(
                 InterfaceMesh  const & mesh
               , AMT  & attributes
               )
      { }

      bool update_local_attributes(
                                    Simplex2 const & simplex
                                  , InterfaceMesh  const & mesh
                                  , AMT & attributes
                                  )
      {
        return false;
      }

      bool plan_local_connectivity_changes(
                                           Simplex2                 const & simplex
                                           , InterfaceMesh                & mesh
                                           , AMT                    const & attributes
                                           , SimplexSet                   & new_simplices
                                           , SimplexSet                   & old_simplices
                                           , std::map<Simplex1, Simplex1> & simplex1_lut
                                           , std::map<Simplex2, Simplex2> & simplex2_lut
                                           )
      {
        Simplex1 max_edge;
        longest_edge<MT>( mesh, attributes.current(), simplex, max_edge);

        if (IsSubmeshBoundary(mesh)(max_edge)) return false;

        Simplex0 const vo = opposite_vertex( mesh, simplex, max_edge);

        if (IsSubmeshBoundary(mesh)(vo)) return false;

        SimplexSet const C = mesh.closure(max_edge);
        SimplexSet const S = mesh.star(max_edge);

        if (S.size(2u) != 2u) return false;

        Simplex0 v1, v2;
        extract_simplices(v1, v2, C);

        for( SimplexSet::simplex2_const_iterator it = S.begin2(); it != S.end2(); ++it)
        {
          Simplex2 const & t = *it;

          if (!(t == simplex))
          {
            old_simplices.insert(S);

            Simplex0 const w = opposite_vertex( mesh, t, max_edge);

            //--- we need it so we can appropriately fix the attributes for vo
            new_simplices.insert(vo);

            Simplex2 n1 = Simplex2(w, vo, v1);
            new_simplices.insert(n1);
            simplex2_lut[n1] = t;

            Simplex2 n2 = Simplex2(w, vo, v2);
            new_simplices.insert(n2);
            simplex2_lut[n2] = t;

            new_simplices.insert(Simplex1(w, vo));
            simplex1_lut[Simplex1(w, vo)] = max_edge;

            if (util::TriangleArea()(
                                 attributes.get_current_value(w)
                                 , attributes.get_current_value(vo)
                                 , attributes.get_current_value(v2)
                                 ) <= VT::zero()
               || util::TriangleArea()(
                                    attributes.get_current_value(w)
                                    , attributes.get_current_value(vo)
                                    , attributes.get_current_value(v1)
                                    ) <= VT::zero())
           {
             return false;
           }


            return true;
          }
        }

        return false;

      }

    }; // class MergeOperation
  } // end namespace details
} // end namespace grit

// GRIT_MERGE_OPERATION_H
#endif

