#ifndef GRIT_ATTRIBUTE_MANAGER_H
#define GRIT_ATTRIBUTE_MANAGER_H

#include <grit_attribute_vector.h>
#include <grit_simplex.h>
#include <grit_simplex_set.h>

#include <util_log.h>

namespace grit
{
  namespace details
  {
    /**
     * This class implements the back-door gateway design pattern strategy for
     * the Attributes class. It allows access to the sensitive data, and it primarily
     * meant for internal use within the grit library.
     *
     * http://www.opentissue.org/mediawiki/index.php/Design_Patterns#Back-door_Gateway_Design_Pattern
     */
    template<typename types>
    class AttributeManager
    {
    public:

      typedef typename types::vector3_type         V;
      typedef typename types::real_type            T;
      typedef typename types::attributes_type      AT;
      typedef typename AT::name_iterator           name_iterator;

      typedef          Simplex0AttributeVector<V>  CT;

    public:

      /**
       * This function copies the attributes names from one Attributes object to another.
       * It also resets and creates new attributes in to_attributes.
       */
      static void copy_attribute_names( AT const & from_attributes, AT & to_attributes)
      {
        to_attributes.copy_attribute_names( from_attributes);
      }

      /**
       * This function overwrites the attributes associated with a given 0-simplex
       * from one attributes to another (used in grit_slab_decomposition.h)
       */
      static void copy_attribute_values(
                                        Simplex0   const & from             ///< simplex whose attributes are being copied
                                        , Simplex0 const & to               ///< simplex whose attributes are being overwritten
                                        , AT       const & from_attributes  ///< attributes from which values are being copied
                                        , AT             & to_attributes    ///< attributes to which values are being copied
                                        )
      {
        to_attributes.copy_attribute_values( to, from, from_attributes);
      }

      /**
       * This function overwrites the attributes associated with a given 1-simplex
       * from one attributes to another (used in grit_slab_decomposition.h)
       */
      static void copy_attribute_values(
                                        Simplex1   const & from             ///< simplex whose attributes are being copied
                                        , Simplex1 const & to               ///< simplex whose attributes are being overwritten
                                        , AT       const & from_attributes  ///< attributes from which values are being copied
                                        , AT             & to_attributes    ///< attributes to which values are being copied
                                        )
      {
        to_attributes.copy_attribute_values( to, from, from_attributes);
      }

      /**
       * This function overwrites the attributes associated with a given 2-simplex
       * from one attributes to another (used in grit_slab_decomposition.h)
       */
      static void copy_attribute_values(
                                        Simplex2   const & from             ///< simplex whose attributes are being copied
                                        , Simplex2 const & to               ///< simplex whose attributes are being overwritten
                                        , AT       const & from_attributes  ///< attributes from which values are being copied
                                        , AT             & to_attributes    ///< attributes to which values are being copied
                                        )
      {
        to_attributes.copy_attribute_values( to, from, from_attributes);
      }

    public:

      static void add_simplex0_label(
                                     Simplex0       const & s
                                     , unsigned int const & label
                                     , AT                 & attributes
                                     )
      {
        attributes.add_simplex0_label( s, label);
      }

      static void clear_simplex0_labels(
                                        Simplex0 const & s
                                        , AT           & attributes
                                        )
      {
        attributes.simplex0_labels(s).clear();
      }

      static void erase_simplex0_label(
                                       Simplex0       const & s
                                       , unsigned int const & label
                                       , AT                 & attributes
                                       )
      {
        std::vector<std::string>  const & names = attributes.simplex0_attribute_names();

        std::vector<unsigned int> const & old_labels = attributes.simplex0_labels(s);
        std::vector<unsigned int>         new_labels(0u);

        std::map< std::string, std::vector<T> > values;

        {
          std::vector<unsigned int>::const_iterator it = old_labels.begin();
          for( ; it != old_labels.end(); ++it)
          {
            unsigned int const & l = *it;

            if( l != label)
            {
              new_labels.push_back(l);

              name_iterator name_it = names.begin();
              for( ; name_it != names.end(); ++name_it)
              {
                std::string const & name = *name_it;
                T const & val = attributes.get_attribute_value( name, s, l);
                values[name].push_back(val);
              }
            }
          }
        }

        attributes.clear(s);

        {
          for( unsigned int i = 0u ; i < new_labels.size(); ++i)
          {
            unsigned int const & l = new_labels[i];
            attributes.add_simplex0_label( s, l);

            name_iterator name_it = names.begin();
            for( ; name_it != names.end(); ++name_it)
            {
              std::string const & name = *name_it;
              T const & val = values[name][i];
              attributes.set_attribute_value( name, s, l, val);
            }
          }
        }

      }

    public:

      /**
       * Used in grit_do_operations.h
       */
      static void remove(
                         SimplexSet const & S
                         , AT             & attributes
                         )
      {
        attributes.remove(S);
      }

      /**
       * Used in grit_do_operations.h
       */
      static void insert(
                         SimplexSet const & S
                         , AT             & attributes
                         )
      {
        attributes.insert(S);
      }

    public:



    };


  } // namespace details

} // namespace grit

// GRIT_ATTRIBUTE_MANAGER_H
#endif

