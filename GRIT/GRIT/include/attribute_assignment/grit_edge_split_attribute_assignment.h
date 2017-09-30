#ifndef GRIT_EDGE_SPLIT_ATTRIBUTE_ASSIGNMENT_H
#define GRIT_EDGE_SPLIT_ATTRIBUTE_ASSIGNMENT_H

#include <grit_interface_attribute_assignment.h>
#include <grit_interface_mesh.h>

#include <grit_simplex.h>
#include <grit_simplex_set.h>

#include <attribute_assignment/grit_copy_attribute_assignment.h>

#include <utilities/grit_extract_simplices.h>
#include <utilities/grit_compute_midpoint.h>

#include <set>
#include <map>
#include <vector>

namespace grit
{
  namespace details
  {
    /**
     * This is the AttributeAssignment object designed to use with EdgeSplitOperation
     * (see mesh_operations/grit_edge_split_operation.h).
     * This takes care of assigning labels and interpolating attributes into the newly
     * created vertex.
     * The attributes for new 1- and 2-simplices are copied using the look-up tables.
     */
    template< typename types>
    class EdgeSplitAttributeAssignment
    : public CopyAttributeAssignment<types>
    {
    protected:

      typedef          CopyAttributeAssignment<types> base_class;

      typedef typename types::math_types              MT;
      typedef typename types::vector3_type            V;
      typedef typename types::real_type               T;
      typedef typename V::value_traits                VT;

      typedef typename types::attributes_type         AT;
      typedef typename types::attribute_manager_type  AMT;
      typedef typename types::param_type              PT;

      typedef typename AT::name_vector                name_vector;
      typedef typename AT::name_iterator              name_iterator;

      typedef typename base_class::simplex1_lut_type  simplex1_lut_type;
      typedef typename base_class::simplex2_lut_type  simplex2_lut_type;
      typedef typename base_class::label_set          label_set;
      typedef typename base_class::label_iterator     label_iterator;

    public:

      /**
       * Input:
       * @param new_simplices   the set of new simplices created by the EdgeSplitOperation.
       *                        Should contain 1 0-simplex (newly created vertex),
       *                        2+K 1-simplices (see below) and 2*K 2-simplices (newly
       *                        inserted triangles).
       * @param old_simplices   the set of simplices removed by the EdgeCollapseOperation.
       *                        Should contain no 0-simplices, 1 1-simplex (the split edge)
       *                        and K=(1 or 2) 2-simplices (1 if splitting boundary edge).
       */
      void operator()(
                      SimplexSet          const & new_simplices
                      , SimplexSet        const & old_simplices
                      , InterfaceMesh     const & mesh
                      , PT                const & parameters
                      , AT                      & attributes
                      , simplex1_lut_type const & simplex1_lut
                      , simplex2_lut_type const & simplex2_lut
                      ) const
      {
        assert( new_simplices.size(0u)==1u || !"EdgeSplitAttributeAssignment() incorrect input");
        assert( old_simplices.size(1u)==1u || !"EdgeSplitAttributeAssignment() incorrect input");
        assert( old_simplices.size(2u)<=2u || !"EdgeSplitAttributeAssignment() incorrect input");

        assert( new_simplices.size(1u)-old_simplices.size(2u) == 2u
               || !"EdgeSplitAttributeAssignment(): incorrect input");

        assert( new_simplices.size(2u) == 2u*old_simplices.size(2u)
               || !"EdgeSplitAttributeAssignment(): incorrect input");

        //--- Edge which became split.
        Simplex1 const & edge   = *old_simplices.begin1();
        //--- New vertex at the midpoint of the edge.
        Simplex0 const & vertex = *new_simplices.begin0();

        SimplexSet const C = mesh.closure(edge);
        SimplexSet const S = mesh.star(edge);

        //--- Find all labels assigned to the newly created node.
        label_set labels = base_class::compute_all_nonambient_labels( S, mesh, parameters);

        //--- Find the vertices of edge.
        Simplex0 v1, v2;
        extract_simplices(v1, v2, C);

        //--- Assigning the attributes for vertex
        attributes.set_current_value( vertex, compute_midpoint<MT>(v1, v2, attributes.current()));

        Simplex0 const & idx0 = edge.get_simplex0_idx0();
        Simplex0 const & idx1 = edge.get_simplex0_idx1();

        //--- To make sure that simplex0_labels exists also for ambient vertices.
        AMT::clear_simplex0_labels(vertex, attributes);

        name_vector    const & attribute_names = attributes.simplex0_attribute_names();
        label_iterator         it              = labels.begin();

        for( ; it!=labels.end(); ++it)
        {
          unsigned int const label = *it;

          AMT::add_simplex0_label( vertex, label, attributes);

          name_iterator name_it = attribute_names.begin();

          for ( ; name_it!=attribute_names.end(); ++name_it)
          {
            std::string const & name = *name_it;

            T const val0  = attributes.get_attribute_value( name, idx0, label);
            T const val1  = attributes.get_attribute_value( name, idx1, label);
            T const value = VT::half() * ( val0 + val1 );

            attributes.set_attribute_value( *name_it, vertex, label, value);
          }
        }

        //--- Copy attributes for Simplex1 and Simplex2 objects
        base_class::copy_simplex1_attributes( new_simplices, attributes, simplex1_lut);
        base_class::copy_simplex2_attributes( new_simplices, attributes, simplex2_lut);
      }

    };

  } // end namespace details
} // end namespace grit

// GRIT_EDGE_SPLIT_ATTRIBUTE_ASSIGNMENT_H
#endif
