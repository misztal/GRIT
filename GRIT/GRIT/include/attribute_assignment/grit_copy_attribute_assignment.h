#ifndef GRIT_COPY_ATTRIBUTE_ASSIGNMENT_H
#define GRIT_COPY_ATTRIBUTE_ASSIGNMENT_H

#include <grit_interface_attribute_assignment.h>
#include <grit_interface_mesh.h>

#include <grit_simplex.h>
#include <grit_simplex_set.h>

#include <map>
#include <vector>
#include <string>

namespace grit
{
  namespace details
  {
    /**
     * This AttributeAssignment object copies the attribute values
     * for newly inserted edges and triangles (Simplex1 and Simplex2
     * objects) from corresponding, removed edges and triangles.
     * It is also used as a base class for EdgeCollapseAA, EdgeSplitAA,
     * MergeAA and VertexSplitAA.
     */
    template< typename types>
    class CopyAttributeAssignment
    : public InterfaceAttributeAssignment<types>
    {
    protected:

      typedef          InterfaceAttributeAssignment<types> base_class;

      typedef          std::set<unsigned int>              label_set;
      typedef typename label_set::const_iterator           label_iterator;

      typedef typename types::real_type                    T;
      typedef typename types::attributes_type              AT;
      typedef typename types::attribute_manager_type       AMT;
      typedef typename types::param_type                   PT;

      typedef typename AT::name_iterator                   name_iterator;
      typedef typename AT::name_vector                     name_vector;

      typedef typename base_class::simplex1_lut_type       simplex1_lut_type;
      typedef typename base_class::simplex2_lut_type       simplex2_lut_type;

    public:

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
        copy_simplex1_attributes( new_simplices, attributes, simplex1_lut);
        copy_simplex2_attributes( new_simplices, attributes, simplex2_lut);
      }

    protected:

      /**
       * This function assigns to each simplex1 from new_simplices set attribute
       * values equal to the attribute values of corresponding simplices, specified
       * by the look-up table simplex1_lut. If a corresponding simplex1 does not
       * exist in simplex1_lut, or an attribute value for it is unspecified, then
       * the new simplex1 is not assigned any attributes, which could lead
       * the following parts of the program to crash.
       */
      void copy_simplex1_attributes(
                                    SimplexSet          const & new_simplices
                                    , AT                      & attributes
                                    , simplex1_lut_type const & simplex1_lut
                                    ) const
      {
        SimplexSet::simplex1_const_iterator it = new_simplices.begin1();

        for( ; it!=new_simplices.end1(); ++it)
        {
          Simplex1      const & s               = *it;
          name_vector   const & attribute_names = attributes.simplex1_attribute_names();
          name_iterator         name_it         = attribute_names.begin();

          for( ; name_it != attribute_names.end(); ++name_it)
          {
            std::string const & name = *name_it;

            if( simplex1_lut.find(s) != simplex1_lut.end())
            {
              Simplex1  const & parent = simplex1_lut.at(s);

              if( attributes.exist_attribute_value( name, parent))
              {
                T const value = attributes.get_attribute_value( name, parent );
                attributes.set_attribute_value( name, s, value);
              }
            }
          }
        }
      }

    protected:

      /**
       * This function assigns to each simplex2 from new_simplices set attribute
       * values equal to the attribute values of corresponding simplices, specified
       * by the look-up table simplex2_lut. If a corresponding simplex2 does not
       * exist in simplex2_lut, or an attribute value for it is unspecified, then
       * the new simplex2 is not assigned any attributes, which could lead
       * the following parts of the program to crash.
       */
      void copy_simplex2_attributes(
                                    SimplexSet          const & new_simplices
                                    , AT                      & attributes
                                    , simplex2_lut_type const & simplex2_lut
                                    ) const
      {
        SimplexSet::simplex2_const_iterator it = new_simplices.begin2();

        for( ; it != new_simplices.end2(); ++it)
        {
          Simplex2      const & s               = *it;
          name_vector   const & attribute_names = attributes.simplex2_attribute_names();
          name_iterator         name_it         = attribute_names.begin();

          for( ; name_it != attribute_names.end(); ++name_it)
          {
            std::string const & name = *name_it;

            if( simplex2_lut.find(s) != simplex2_lut.end())
            {
              Simplex2 const & parent = simplex2_lut.at(s);

              if( attributes.exist_attribute_value( name, parent))
              {
                T const value = attributes.get_attribute_value( name, parent );
                attributes.set_attribute_value( name, s, value);
              }
            }
          }
        }
      }

    protected:

      /**
       * This helper method is not used in CopyAttributeAssignment, but is used
       * in several implementations of it.
       * Returns the set of nonambient labels among 2-simplices in @param simplex_set.
       */
      label_set compute_all_nonambient_labels(
                                              SimplexSet      const & simplex_set
                                              , InterfaceMesh const & mesh
                                              , Parameters    const & parameters
                                              ) const
      {
        label_set result;

        SimplexSet::simplex2_const_iterator it = simplex_set.begin2();

        for( ; it!=simplex_set.end2(); ++it)
        {
          Simplex2     const & t     = *it;
          unsigned int const   label = mesh.label(t);

          if( parameters.use_ambient() && (label==parameters.ambient_label()))
          {
            continue;
          }

          result.insert( mesh.label(t));
        }

        return result;
      }

    protected:

      /**
       * Deletes the records corresponding to labels in @param labels
       * for 0-simplex @param s in @param attributes.
       */
      void delete_simplex0_labels(
                                  Simplex0    const & s
                                  , label_set const & labels
                                  , AT              & attributes
                                  ) const
      {
        label_iterator it = labels.begin();

        for( ; it!=labels.end(); ++it)
        {
          unsigned int  const label   = *it;

          AMT::erase_simplex0_label( s, label, attributes);
        }
      }
    };

  } // end namespace details
} // end namespace grit

// GRIT_COPY_ATTRIBUTE_ASSIGNMENT_H
#endif
