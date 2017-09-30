#ifndef GRIT_MERGE_ATTRIBUTE_ASSIGNMENT_H
#define GRIT_MERGE_ATTRIBUTE_ASSIGNMENT_H

#include <grit_interface_attribute_assignment.h>
#include <grit_interface_mesh.h>
#include <grit_simplex_set.h>

#include <attribute_assignment/grit_copy_attribute_assignment.h>

#include <utilities/grit_extract_simplices.h>

#include <util_barycentric.h>
#include <util_log.h>

#include <map>
#include <cassert>

namespace grit
{
  namespace details
  {
    /**
     * This is the AttributeAssignment object designed to use with MergeOperation
     * (see mesh_operations/grit_merge_operation.h).
     * This takes care of assigning labels and interpolating attributes at the vertex
     * where the interface topology change takes place.
     * The attributes for new 1- and 2-simplices are copied using the look-up tables.
     */
    template< typename types>
    class MergeAttributeAssignment
    : public CopyAttributeAssignment<types>
    {
    protected:

      typedef          CopyAttributeAssignment<types> base_class;

      typedef typename types::real_type               T;
      typedef typename types::vector3_type            V;
      typedef typename types::attributes_type         AT;
      typedef typename types::attribute_manager_type  AMT;
      typedef typename types::param_type              PT;

      typedef typename V::value_traits                VT;

      typedef typename AT::name_vector                name_vector;
      typedef typename AT::name_iterator              name_iterator;

      typedef typename base_class::simplex1_lut_type  simplex1_lut_type;
      typedef typename base_class::simplex2_lut_type  simplex2_lut_type;
      typedef typename base_class::label_set          label_set;
      typedef typename base_class::label_iterator     label_iterator;

    public:

      /**
       * Input:
       * @param new_simplices   The set of simplices created by MergeOperation.
       *                        Contains 1 0-simplex (the vertex opposite to the interface
       *                        edge in merged 2-simplex, whose labels have to be updated),
       *                        1 1-simplex and 2 2-simplices (created by "flipping"
       *                        the interface edge).
       * @param old_simplices   The set of simplices removed by MergeOperation.
       *                        Contains no 0-simplices, 1 1-simplex and 2 2-simplices
       *                        (the star of the interface edge which gets "flipped").
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
        assert( new_simplices.size(0u) == 1u || !"MergeAttributeAssignment() incorrect input");
        assert( new_simplices.size(1u) == 1u || !"MergeAttributeAssignment() incorrect input");
        assert( new_simplices.size(2u) == 2u || !"MergeAttributeAssignment() incorrect input");
        assert( old_simplices.size(0u) == 0u || !"MergeAttributeAssignment() incorrect input");
        assert( old_simplices.size(1u) == 1u || !"MergeAttributeAssignment() incorrect input");
        assert( old_simplices.size(2u) == 2u || !"MergeAttributeAssignment() incorrect input");

        //--- This is the vertex for which we have to set-up new attributes.
        //--- (i.e. the apex of the triangle sandwiched between the interfaces).
        Simplex0   const & v0   = *(new_simplices.begin0());
        //--- This is the edge which becomes "flipped" by MergeOperation.
        Simplex1   const & edge = *(old_simplices.begin1());
        SimplexSet const   C    = mesh.closure(edge);

        //--- Get the vertices of edge
        Simplex0 v1, v2;
        extract_simplices( v1, v2, C);

        V const & pos0 = attributes.get_current_value( v0 );
        V const & pos1 = attributes.get_current_value( v1 );
        V const & pos2 = attributes.get_current_value( v2 );

        //--- Compute barycentric weights of v0 in [v1, v2] to use in attribute
        //--- value interpolation.
        T w1, w2;
        util::barycentric( pos1, pos2, pos0, w1, w2);

        //--- Note: both 2-simplices in new_simplices are associated with
        //---       the same old 2-simplex, which we call parent.
        Simplex2      const & t               = *(new_simplices.begin2());
        Simplex2      const & parent          = simplex2_lut.at(t);
        name_vector   const & attribute_names = attributes.simplex0_attribute_names();

        //--- Find the sets of labels to add, remove and interpolate attributes for v0.
        label_set labels_new, labels_rem, labels_int;

        compute_label_sets( v0, parent, old_simplices, mesh, parameters
                           , labels_new, labels_rem, labels_int);

        //--- Here we remove destroyed labels (has to go first, become configuration exist,
        //--- where labels_new and labels_rem contain the same label).
        base_class::delete_simplex0_labels( v0, labels_rem, attributes);

        //--- Add new labels to v0 (multiphase collision occured).
        {
          label_iterator it = labels_new.begin();

          for( ; it!=labels_new.end(); ++it)
          {
            unsigned int  const label   = *it;
            name_iterator       name_it = attribute_names.begin();

            AMT::add_simplex0_label( v0, label, attributes);

            for( ; name_it != attribute_names.end(); ++name_it)
            {
              std::string const & name = *name_it;

              T const val1 = attributes.get_attribute_value( name, v1, label);
              T const val2 = attributes.get_attribute_value( name, v2, label);
              T const val  = w1 * val1 + w2 * val2;

              attributes.set_attribute_value( name, v0, label, val);
            }
          }
        }

        //--- Interpolate attributes at v0 (self-collision happened).
        {
          label_iterator it = labels_int.begin();

          for( ; it!=labels_int.end(); ++it)
          {
            unsigned int  const label   = *it;
            name_iterator       name_it = attribute_names.begin();

            for( ; name_it != attribute_names.end(); ++name_it)
            {
              std::string const & name = *name_it;

              T const val0 = attributes.get_attribute_value( name, v0, label);
              T const val1 = attributes.get_attribute_value( name, v1, label);
              T const val2 = attributes.get_attribute_value( name, v2, label);
              T const vali = w1 * val1 + w2 * val2;

              attributes.set_attribute_value( name, v0, label, VT::half()*(val0+vali));
            }
          }

        }

        //--- Copy attributes for Simplex1 and Simplex2 objects
        base_class::copy_simplex1_attributes( new_simplices, attributes, simplex1_lut);
        base_class::copy_simplex2_attributes( new_simplices, attributes, simplex2_lut);
      }

    protected:

      /**
       * MergeOperation might remove or add certains label to a vertex.
       * This function computes the label sets used in re/de-assigning the
       * labels to a vertex in attribute assignment.
       *
       * @param v0              vertex whose labels are to be assigned
       * @param parent
       * @param old_simplices   set of simplices removed by the operation
       * @param labels_new
       * @param labels_rem
       * @param labels_int
       */
      void compute_label_sets(
                              Simplex0        const & v0
                              , Simplex2      const & parent
                              , SimplexSet    const & old_simplices
                              , InterfaceMesh const & mesh
                              , Parameters    const & parameters
                              , label_set           & labels_new
                              , label_set           & labels_rem
                              , label_set           & labels_int
                              ) const
      {
        //--- Old_simplices is the star of edge.
        SimplexSet const S0 = mesh.star(v0);
        //--- We get a set of simplices in S0 which remain after the operation.
        SimplexSet const D0 = difference(S0,old_simplices);
        //--- These labels stay assigned to v0.
        label_set  const l0 = base_class::compute_all_nonambient_labels( D0, mesh, parameters);

        SimplexSet::simplex2_const_iterator it = old_simplices.begin2();

        //--- This simplex gets removed during MergeOperation.
        Simplex2   const removed = (*it==parent)?(*(++it)):(*it);

        label_set  l_excluded, l_included;

        {
          unsigned int const label = mesh.label(parent);

          if( !(parameters.use_ambient() && label==parameters.ambient_label()))
          {
            l_included.insert( label);
          }
        }

        {
          unsigned int const label = mesh.label(removed);

          if( !(parameters.use_ambient() && label==parameters.ambient_label()))
          {
            l_excluded.insert( label);
          }
        }

        std::set_difference( l_included.begin(), l_included.end(), l0.begin(), l0.end()
                            , std::inserter( labels_new, labels_new.begin()));

        std::set_difference( l_excluded.begin(), l_excluded.end(), l0.begin(), l0.end()
                            , std::inserter( labels_rem, labels_rem.begin()));

        std::set_intersection( l_included.begin(), l_included.end(), l0.begin(), l0.end()
                              , std::inserter( labels_int, labels_int.begin()));
      }
    };

  } // end namespace details
} // end namespace grit

// GRIT_MERGE_ATTRIBUTE_ASSIGNMENT_H
#endif
