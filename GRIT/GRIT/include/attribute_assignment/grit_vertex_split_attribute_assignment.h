#ifndef GRIT_VERTEX_SPLIT_ATTRIBUTE_ASSIGNMENT
#define GRIT_VERTEX_SPLIT_ATTRIBUTE_ASSIGNMENT

#include <attribute_assignment/grit_copy_attribute_assignment.h>

#include <utilities/grit_compute_midpoint.h>
#include <utilities/grit_extract_simplices.h>
#include <utilities/grit_compute_offset_position.h>

#include <util_log.h>

namespace grit
{
  namespace details
  {
    template< typename types>
    class VertexSplitAttributeAssignment
    : public CopyAttributeAssignment<types>
    {
    protected:

      typedef          CopyAttributeAssignment<types> base_class;

      typedef typename types::real_type               T;
      typedef typename types::vector3_type            V;
      typedef typename types::math_types              MT;
      typedef typename types::attributes_type         AT;
      typedef typename types::attribute_manager_type  AMT;
      typedef typename types::param_type              PT;

      typedef typename V::value_traits                VT;

      typedef typename AT::name_vector                name_vector;
      typedef typename AT::name_iterator              name_iterator;

      typedef typename base_class::simplex1_lut_type  simplex1_lut_type;
      typedef typename base_class::simplex2_lut_type  simplex2_lut_type;

    public:

      /**
       * Input:
       * @param new_simplices  A set of simplices created by VertexSplitOperation.
       *                       Should consist of 2 0-simplices (both the old and the newly
       *                       created vertex), (N+2) 1-simplices and (N+2) 2-simplices.
       * @param old_simplices  A set of simplices removed by VertexSplitOperation.
       *                       Should consist of 0 0-simplices, N 2-simplices (all triangles
       *                       in the star of the split vertex, which are in phase undergoing
       *                       splitting), (N-1) 1-simplices (non-interface edges in phase
       *                       undergoing splitting.
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
        assert( parameters.use_ambient()   || !"VertexSplitAttributeAssignment() use_ambient has to be true VertexSplit to work");
        assert( old_simplices.size(0u)==0u || !"VertexSplitAttributeAssignment() incorrect input");
        assert( new_simplices.size(0u)==2u || !"VertexSplitAttributeAssignment() incorrect input");

        assert( old_simplices.size(2u)-old_simplices.size(1u) == 1u
               || !"VertexSplitAttributeAssignment() incorrect input");

        assert( new_simplices.size(1u) == new_simplices.size(2u)
               || !"VertexSplitAttributeAssignment() incorrect input");

        assert( new_simplices.size(2u)-old_simplices.size(2u) == 2u
               || !"VertexSplitAttributeAssignment() incorrect input");

        //--- All triangles in old_simplices belong to a single phase.
        Simplex2     const & t     = *(old_simplices.begin2());
        unsigned int const   label = mesh.label(t);

        //--- Identify which vertex in new_simplices is the newly inserted one,
        //--- and which one existed before the operation.
        Simplex0 v_old, v_new;
        identify_vertices( new_simplices, t, v_old, v_new, mesh);

        //--- Compute the offset position for v_new.
        T const & distance     = -parameters.get_strength( "vertex_split", label);
        V const   inside_point = compute_offset_position<MT>( v_old, mesh, attributes, label, distance);

        attributes.set_current_value( v_new, inside_point );

        AMT::add_simplex0_label( v_new, label, attributes);

        name_vector const & att_names = attributes.simplex0_attribute_names();
        for( name_iterator it = att_names.begin(); it != att_names.end(); ++it)
        {
          std::string const & name = *it;
          T const & val = attributes.get_attribute_value( name, v_old, label);
          attributes.set_attribute_value( name, v_new, label, val);
        }

        AMT::erase_simplex0_label( v_old, label, attributes);

        //--- Copy attributes for Simplex1 and Simplex2 objects
        base_class::copy_simplex1_attributes( new_simplices, attributes, simplex1_lut);
        base_class::copy_simplex2_attributes( new_simplices, attributes, simplex2_lut);
      }

    protected:

      void identify_vertices(
                             SimplexSet      const & new_simplices
                             , Simplex2      const & triangle
                             , Simplex0            & v_old
                             , Simplex0            & v_new
                             , InterfaceMesh const & mesh
                             ) const
      {
        Simplex0 v1, v2;
        extract_simplices( v1, v2, new_simplices);

        SimplexSet const C = mesh.closure(triangle);

        if(C.contains(v1))
        {
          v_new = v2;
          v_old = v1;
        }
        else
        {
          v_new = v1;
          v_old = v2;
        }
      }

    };
  } // namespace details

} // namespace grit

// GRIT_VERTEX_SPLIT_ATTRIBUTE_ASSIGNMENT_H
#endif
