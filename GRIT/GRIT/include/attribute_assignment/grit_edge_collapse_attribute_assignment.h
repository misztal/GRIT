#ifndef GRIT_EDGE_COLLAPSE_ATTRIBUTE_ASSIGNMENT_H
#define GRIT_EDGE_COLLAPSE_ATTRIBUTE_ASSIGNMENT_H

#include <grit_interface_attribute_assignment.h>
#include <grit_interface_mesh.h>
#include <grit_simplex_set.h>

#include <attribute_assignment/grit_copy_attribute_assignment.h>

#include <utilities/grit_extract_simplices.h>

#include <util_barycentric.h>
#include <util_log.h>

#include <algorithm>
#include <map>
#include <cassert>

namespace grit
{
  namespace details
  {
    /**
     * This is the AttributeAssignment object designed to use with the implementations
     * of InterfaceEdgeCollapseOperation (see mesh_operations/grit_interface_edge_collapse_operation.h).
     * This takes care of re-assigning labels and attributes to the target vertex of the edge collapse
     * operation (i.e. the vertex collapsed onto).
     * The attributes for new 1- and 2-simplices are copied using the look-up tables.
     */
    template< typename types>
    class EdgeCollapseAttributeAssignment
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

      typedef typename AT::name_iterator              name_iterator;
      typedef typename AT::name_vector                name_vector;

      typedef typename base_class::simplex1_lut_type  simplex1_lut_type;
      typedef typename base_class::simplex2_lut_type  simplex2_lut_type;
      typedef typename base_class::label_set          label_set;
      typedef typename base_class::label_iterator     label_iterator;

    public:

      /**
       * Input:
       * @param new_simplices   the set of new simplices created by the EdgeCollapseOperation
       *                        (see also InterfaceEdgeCollapseOperation::collapse()).
       *                        Should contain 1 0-simplex (the vertex which was collapsed onto),
       *                        N 1-simplices (newly inserted edges) and N+1 2-simplices (newly
       *                        inserted triangles).
       * @param old_simplices   the set of simplices removed by the EdgeCollapseOperation.
       *                        Should contain 1 0-simplex (the collapsed vertex), N+3 1-simplices
       *                        and N+3 2-simplices (the full star of the collapsed vertex).
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
        assert( ( new_simplices.size(0u)==1u && old_simplices.size(0u)==1u )
               || !"EdgeCollapseAttributeAssignment() incorrect input");

        assert( old_simplices.size(2u)-new_simplices.size(2u) == 2u
               || !"EdgeCollapseAttributeAssignment() incorrect input");

        assert( old_simplices.size(1u)-new_simplices.size(1u) == 3u
               || !"EdgeCollapseAttributeAssignment() incorrect input");

        assert( old_simplices.size(2u)==old_simplices.size(1u)
               || !"EdgeCollapseAttributeAssignment() incorrect input");

        //--- The (previously existing) target vertex, for which we have to
        //--- update the attributes.
        Simplex0 const & vn = *(new_simplices.begin0());
        //--- The collapsed (removed) vertex.
        Simplex0 const & vo = *(old_simplices.begin0());

        //--- Edge collapse can produce destruction or merging of phases.
        //--- In such case, we need to remove or add new phases to the
        //--- target vertex. In case of phase self-collisions, we interpolate
        //--- vertex attributes between the collapsed and the target vertex.
        label_set labels_new, labels_rem, labels_int;

        compute_label_sets( vn, vo, old_simplices, mesh, parameters
                           , labels_new, labels_rem, labels_int);

        name_vector const & attribute_names = attributes.simplex0_attribute_names();

        //--- Here we remove destroyed labels (has to go first, become configuration exist,
        //--- where labels_new and labels_rem contain the same label).
        base_class::delete_simplex0_labels( vn, labels_rem, attributes);

        //--- Here we add new labels and copy the corresponding attribute values from vo
        {
          label_iterator it = labels_new.begin();

          for( ; it!=labels_new.end(); ++it)
          {
            unsigned int  const label   = *it;
            name_iterator       name_it = attribute_names.begin();

            AMT::add_simplex0_label( vn, label, attributes);

            for( ; name_it!=attribute_names.end(); ++name_it)
            {
              std::string const & name = *name_it;
              T           const   val  = attributes.get_attribute_value( name, vo, label);

              attributes.set_attribute_value( name, vn, label, val);
            }
          }
        }

        //--- Here we deal with self-collision (we interpolate between the old and new value).
        {
          label_iterator it = labels_int.begin();

          for( ; it != labels_int.end(); ++it)
          {
            unsigned int  const & label   = *it;
            name_iterator         name_it = attribute_names.end();

            for( ; name_it != attribute_names.end(); ++name_it)
            {
              std::string const & name = *name_it;
              T           const   valn = attributes.get_attribute_value( name, vn, label);
              T           const   valo = attributes.get_attribute_value( name, vo, label);

              attributes.set_attribute_value( name, vn, label, VT::half()*(valo+valn));
            }
          }
        }

        //--- Copy attributes for Simplex1 and Simplex2 objects
        base_class::copy_simplex1_attributes( new_simplices, attributes, simplex1_lut);
        base_class::copy_simplex2_attributes( new_simplices, attributes, simplex2_lut);
      }

    protected:

      void compute_label_sets(
                              Simplex0        const & vn
                              , Simplex0      const & vo
                              , SimplexSet    const & old_simplices
                              , InterfaceMesh const & mesh
                              , Parameters    const & parameters
                              , label_set           & labels_new
                              , label_set           & labels_rem
                              , label_set           & labels_int
                              ) const
      {
        //--- old_simplices is the star of vo.
        SimplexSet const Sn = mesh.star(vn);
        SimplexSet const X  = intersection(Sn,old_simplices);
        SimplexSet const Dn = difference(Sn,X);
        SimplexSet const Do = difference(old_simplices,X);

        label_set  const ln = base_class::compute_all_nonambient_labels( Dn, mesh, parameters);
        label_set  const lo = base_class::compute_all_nonambient_labels( Do, mesh, parameters);
        label_set  const lx = base_class::compute_all_nonambient_labels( X , mesh, parameters);

        std::set_difference( lo.begin(), lo.end(), ln.begin(), ln.end()
                            , std::inserter( labels_new, labels_new.begin()));

        std::set_difference( lx.begin(), lx.end(), ln.begin(), ln.end()
                            , std::inserter( labels_rem, labels_rem.begin()));

        std::set_intersection( lo.begin(), lo.end(), ln.begin(), ln.end()
                              , std::inserter( labels_int, labels_int.begin()));
      }

    };
  } // end namespace details
} // end namespace grit

// GRIT_EDGE_COLLAPSE_ATTRIBUTE_ASSIGNMENT_H
#endif
