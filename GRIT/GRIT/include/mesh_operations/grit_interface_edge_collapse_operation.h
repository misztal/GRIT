#ifndef GRIT_INTERFACE_EDGE_COLLAPSE_OPERATION_H
#define GRIT_INTERFACE_EDGE_COLLAPSE_OPERATION_H

#include <grit_interface_mesh_operation.h>
#include <grit_interface_mesh.h>

#include <grit_logic_expressions.h>
#include <grit_simplex.h>
#include <grit_simplex_set.h>
#include <grit_simplex_set_functions.h>

#include <utilities/grit_extract_simplices.h>
#include <utilities/grit_find_collapsable_vertices.h>
#include <utilities/grit_is_any_inverted.h>
#include <utilities/grit_changes_phase_area.h>

#include <util_log.h>

#include <string>
#include <map>

namespace grit
{
  namespace details
  {
    /**
     * This class provides an interface for different implementations of
     * edge collapse operation, and contains shared variables and functionality
     * between these operations.
     */
    template< typename types>
    class InterfaceEdgeCollapseOperation
    : public InterfaceMeshOperation< Simplex1, types>
    {
    protected:

      typedef          InterfaceMeshOperation<
                                              Simplex1
                                              ,types
                                              >        base_class;

      typedef typename types::real_type                T;
      typedef typename types::vector3_type             V;
      typedef typename types::math_types               MT;
      typedef typename types::attributes_type          AT;
      typedef typename types::param_type               PT;

      typedef typename V::value_traits                 VT;

      typedef typename base_class::simplex1_lut_type   simplex1_lut_type;
      typedef typename base_class::simplex2_lut_type   simplex2_lut_type;

    protected:

      std::string  const & m_name;       ///< operation name used to retrieve parameters

      unsigned int const   m_label;      ///< label of the phase on which operation is performed

      PT           const & m_parameters; ///< parameters container

    public:

      InterfaceEdgeCollapseOperation(
                                     std::string    const & operation_name
                                     , unsigned int const   label
                                     , PT           const & parameters
                                     )
      : m_name(operation_name)
      , m_label(label)
      , m_parameters(parameters)
      { }

    public:

      void init(
                InterfaceMesh const & mesh
                , AT                & attributes
                )
      { }

    public:

      bool update_local_attributes(
                                   Simplex1        const & simplex
                                   , InterfaceMesh const & mesh
                                   , AT                  & attributes
                                   )
      {
        return false;
      }

    public:

      /**
       * Finds the simplices removed and created by the edge collapse operation
       *
       * @param from            a vertex to be collapsed
       * @param to              the target vertex of edge collapse
       * @param simplex         an edge (simplex1) connecting @param from and @param to
       * @param new_simplices   upon return, stores all simplices created by edge collapse
       *                        and the target vertex (whose attributes have to be updated)
       * @param old_simplices   upon return, stores all simplices removed by edge collapse
       *                        (i.e. star of @param from)
       * @param simplex1_lut    simplex1 look-up table, stores a "parent" simplex1 for
       *                        each simplex1 in @param new_simplices
       * @param simplex2_lut    simplex2 look-up table, stores a "parent" simplex2 for
       *                        each simplex2 in @param new_simplices
       */
      void collapse(
                    Simplex0            const & from
                    , Simplex0          const & to
                    , Simplex1          const & simplex
                    , InterfaceMesh           & mesh
                    , AT                const & attributes
                    , SimplexSet              & new_simplices
                    , SimplexSet              & old_simplices
                    , simplex1_lut_type       & simplex1_lut
                    , simplex2_lut_type       & simplex2_lut
                    )
      {
        assert( Simplex1(from,to)==simplex || !"InterfaceEdgeCollapseOperation: wrong input");

        SimplexSet const S  = mesh.star(simplex);
        SimplexSet const Sv = mesh.star(from);
        SimplexSet const T2 = difference(Sv, S);
        SimplexSet const T1 = difference(Sv, mesh.closure(S));

        //--- Entire star of the from vertex is removed (including from).
        old_simplices.insert(Sv);

        //--- Each triangle in the star of from that is not in the star of simplex
        //--- is reconnected to vertex to.
        SimplexSet::simplex2_const_iterator it = T2.begin2();

        for( ; it!=T2.end2(); ++it)
        {
          Simplex2 const & t = *it;

          //--- Find all vertices of t
          Simplex0 s0 = t.get_simplex0_idx0();
          Simplex0 s1 = t.get_simplex0_idx1();
          Simplex0 s2 = t.get_simplex0_idx2();

          //--- Find which one vertices of t is from, and replace it with to.
          if (s0 == from) s0 = to;
          if (s1 == from) s1 = to;
          if (s2 == from) s2 = to;

          assert( (s0==to || s1==to || s2==to) || "InterfaceEdgeCollapseOperation: Internal error");

          //--- Add updated simplex2 to new_simplices, and set t as it's parent
          //--- (for correct attribute assignment).
          Simplex2 n( s0, s1, s2);
          new_simplices.insert(n);
          simplex2_lut[n] = t;
        }

        //--- Edges have to be taken care of in a separate loop
        //--- so that we don't overwrite attributes for already
        //--- existing edges.
        SimplexSet::simplex1_const_iterator it1 = T1.begin1();

        for( ; it1!=T1.end1(); ++it1)
        {
          Simplex1 const & e = *it1;

          Simplex0 s0 = e.get_simplex0_idx0();
          Simplex0 s1 = e.get_simplex0_idx1();

          if (s0 == from) s0 = to;
          if (s1 == from) s1 = to;

          Simplex1 n( s0, s1);
          new_simplices.insert(n);
          simplex1_lut[n] = e;
        }

        //--- Add vertex to to new_simplices, for attribute re-assignment.
        new_simplices.insert(to);
      }

    }; // class InterfaceEdgeCollapseOperation

  } // end namespace details
} // end namespace grit

// GRIT_INTERFACE_EDGE_COLLAPSE_OPERATION_H
#endif

