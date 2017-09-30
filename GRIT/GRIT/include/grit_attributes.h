#ifndef GRIT_ATTRIBUTES_H
#define GRIT_ATTRIBUTES_H

#include <grit_attribute_vector.h>
#include <grit_attribute_manager.h>
#include <grit_logic_expressions.h>
#include <grit_simplex.h>
#include <grit_simplex_set.h>
#include <grit_simplex_set_functions.h>

#include <util_log.h>
#include <util_find_index.h>

#include <vector>
#include <map>
#include <string>
#include <algorithm>  // Needed for std::find
#include <stdexcept>  // Needed for std::logic_error

namespace grit
{
  namespace details
  {
    /**
     * This class is the main storage object for all the attributes used
     * in the GRIT library.
     *
     * It allows to define custom, named attributes for 0-, 1- and 2-simplices.
     * Names starting with a double underscore (i.e. "__name") are reserved
     * for internal use only.
     *
     * The private member functions are exposed through AttributeManager object
     * in grit_attribute_manager.h, however, they should only be used by GRIT
     * developers.
     *
     * Custom attributes for 0-simplices support multiple values, one per each
     * phase the 0-simplex belongs to (except for the ambient phase).
     * Custom attributes for 1- and 2-simplices a single-valued and defined also
     * on purely ambient simplices.
     */
    template<typename types>
    class Attributes
    {
    private:

      friend class AttributeManager<types>;      ///< Backdoor for GRIT framework to be able to work on attribute data when changing mesh.

    public:

      typedef typename types::vector3_type               V;
      typedef typename types::real_type                  T;
      typedef typename types::param_type                 PT;

      typedef typename V::value_traits                   VT;

      typedef          Simplex0AttributeVector<V>        CT;  ///< special attribute vector type used to store current (i.e. mesh) positions of 0-simplices;
      typedef typename types::sim0_attributes_uint_type  AI0;

      typedef typename types::sim2_attributes_real_type  AR2;
      typedef typename types::sim1_attributes_real_type  AR1;
      typedef typename types::sim0_attributes_real_type  AR0;

      typedef typename types::mesh_impl                  MI;

      typedef          std::vector<std::string>          name_vector;
      typedef          name_vector::const_iterator       name_iterator;

    protected:

      CT         m_current;          ///< current positions of the mesh's vertices
      AI0        m_simplex0_labels;  ///< identifies all labels associated with each vertex.

      std::map< std::string, AR0>  m_simplex0_real_attributes; ///< Associative map of all custom (named) Simplex0 attributes
      std::map< std::string, AR1>  m_simplex1_real_attributes; ///< Associative map of all custom (named) Simplex1 attributes
      std::map< std::string, AR2>  m_simplex2_real_attributes; ///< Associative map of all custom (named) Simplex2 attributes

      std::vector< std::string>    m_simplex0_attribute_names; ///< vector of all names of custom Simplex0 attributes
      std::vector< std::string>    m_simplex1_attribute_names; ///< vector of all names of custom Simplex1 attributes
      std::vector< std::string>    m_simplex2_attribute_names; ///< vector of all names of custom Simplex2 attributes

    public:

      std::vector< std::string> const & simplex0_attribute_names() const { return m_simplex0_attribute_names; }
      std::vector< std::string> const & simplex1_attribute_names() const { return m_simplex1_attribute_names; }
      std::vector< std::string> const & simplex2_attribute_names() const { return m_simplex2_attribute_names; }

    public:

      // 2016-01-11 Marek: there are a couple of spots left that will require some extra work to remove .current()
      //                   use - will work on it.
      CT       & current()       { return m_current; }
      CT const & current() const { return m_current; }

    public:

      /**
       *
       * @param attribute_name         The end-users name for the attribute.
       * @param dimension              The dimension of the simplex type that the attribute
       *                               should be created for. Current legal values are 0 (a
       *                               vertex attribute) and 2 (a "triangle" attribute).
       */
      void create_attribute(std::string const & attribute_name, unsigned int const dimension)
      {
        if (exist_attribute(attribute_name, dimension))
        {
          util::Log logging;

          logging << "Internal error: Simplex" << dimension << "attribute " << attribute_name  << " already exist?" << util::Log::newline();

          throw std::logic_error("Trying to create same attribute multiple times");
        }

        if (dimension == 0u)
        {
          m_simplex0_attribute_names.push_back(attribute_name);
          m_simplex0_real_attributes[attribute_name].clear();

          return;
        }

        if (dimension == 1u)
        {
          m_simplex1_attribute_names.push_back(attribute_name);
          m_simplex1_real_attributes[attribute_name].clear();

          return;
        }

        if (dimension == 2u)
        {
          m_simplex2_attribute_names.push_back(attribute_name);
          m_simplex2_real_attributes[attribute_name].clear();

          return;
        }
      }

    public:

      /**
       * This function tests if a specified named attribute of given dimension
       * exist in the attribute manager.
       */
      bool exist_attribute(std::string const & attribute_name, unsigned int const & dimension) const
      {
        if (dimension == 0u)
        {
          std::vector<std::string>::const_iterator lookup = find(
                                                                 m_simplex0_attribute_names.begin()
                                                                 , m_simplex0_attribute_names.end()
                                                                 , attribute_name);

          if(lookup == m_simplex0_attribute_names.end() )
            return false;

          return true;

        }

        if (dimension == 1u)
        {
          std::vector<std::string>::const_iterator lookup = find(
                                                                 m_simplex1_attribute_names.begin()
                                                                 , m_simplex1_attribute_names.end()
                                                                 , attribute_name);

          if(lookup == m_simplex1_attribute_names.end() )
            return false;

          return true;
        }

        if (dimension == 2u)
        {
          std::vector<std::string>::const_iterator lookup = find(
                                                                 m_simplex2_attribute_names.begin()
                                                                 , m_simplex2_attribute_names.end()
                                                                 , attribute_name);

          if(lookup == m_simplex2_attribute_names.end() )
            return false;

          return true;
        }

        return false;
      }

    public:

      /**
       * This function tests if the value for a specified named attribute
       * exist in the attribute manager, for a given simplex.
       */
      bool exist_attribute_value(std::string const & attribute_name, Simplex1 const & simplex) const
      {
        if( exist_attribute( attribute_name, 1u))
        {
          return m_simplex1_real_attributes.at(attribute_name).has_value(simplex);
        }

        return false;
      }

      /**
       * This function tests if the value for a specified named attribute
       * exist in the attribute manager, for a given simplex.
       */
      bool exist_attribute_value(std::string const & attribute_name, Simplex2 const & simplex) const
      {
        if( exist_attribute( attribute_name, 2u))
        {
          return m_simplex2_real_attributes.at(attribute_name).has_value(simplex);
        }

        return false;
      }

    public:

      /**
       * This function returns the current position of a specified 0-simplex
       */
      V const & get_current_value( Simplex0 const & s) const
      {
        return m_current[s];
      }

      /**
       * This function sets the current position of a specified 0-simplex to
       * a given vector value. This *moves* the vertex without ensuring
       * validity of the resulting mesh - handle with care.
       */
      void set_current_value(
                             Simplex0 const & s
                             , V      const & value
                             )
      {
        m_current[s] = value;
      }

    public:

      /**
       * This function returns the target position of 0-simplex,
       * associated with specified label (different phases can
       * impose different target positions on a single vertex).
       * If the target position of a given simplex does not exist
       * the function returns the current value.
       *
       * @param label       label identifying the phase
       */
      V get_target_value(
                         Simplex0       const & s
                         , unsigned int const & label
                         ) const
      {
        std::string const name_x("__target_x");
        std::string const name_y("__target_y");

        if( !exist_attribute( "__target_x", 0u) || !exist_attribute( "__target_y", 0u))
        {
          util::Log logging;
          logging << "get_target_value() Warning! target vector was not defined / using current value" << util::Log::newline();
          return m_current[s];
        }

        std::vector< unsigned int> const & labels = m_simplex0_labels[s];

        unsigned int const idx = util::find_index( label, labels);
        if( idx > labels.size())
        {
          util::Log logging;
          logging << "get_target_value() Warning! label not found / using current value" << util::Log::newline();
          return m_current[s];
        }

        if( idx < m_simplex0_real_attributes.at(name_x)[s].size()
         && idx < m_simplex0_real_attributes.at(name_y)[s].size())
        {
          V result(VT::zero());
          result[0u] = m_simplex0_real_attributes.at(name_x)[s][idx];
          result[1u] = m_simplex0_real_attributes.at(name_y)[s][idx];
          return result;
        }

        util::Log logging;
        logging << "get_target_value() Warning! mismatch between the number of labels and attribute size / using current value" << util::Log::newline();

        return m_current[s];
      }


      /**
       * This function sets the target position of a 0-simplex,
       * associated with specified label (different phases can
       * impose different target positions on a single vertex).
       * If the target attribute vector has not been initialized,
       * it is created.
       *
       * @param label       label identifying the phase
       * @param value       the new target position
       */
      void set_target_value(
                            Simplex0       const & s
                            , unsigned int const & label
                            , V            const & value
                            )
      {
        std::string const name_x("__target_x");
        std::string const name_y("__target_y");

        if( !exist_attribute( name_x, 0u))
        {
          create_attribute( name_x, 0u);
        }

        if( !exist_attribute( name_y, 0u))
        {
          create_attribute( name_y, 0u);
        }

        set_attribute_value(name_x, s, label, value[0u]);
        set_attribute_value(name_y, s, label, value[1u]);
      }

    public:

      /**
       * This function returns the value of a custom (named) attribute at a given 0-simplex,
       * associated with specified phase.
       *
       * @param label       label identifying the phase
       */
      T const & get_attribute_value(
                                    std::string    const & name
                                    , Simplex0     const & s
                                    , unsigned int const & label
                                    ) const
      {
        if (!exist_attribute( name, 0u))
        {
          util::Log logging;
          logging << "get_attribute_value() Error! Attribute " << name << " does not exist!" << util::Log::newline();
          throw std::logic_error("Trying to get attribute value from an undefined attribute");
        }

        std::vector< unsigned int> const & labels = m_simplex0_labels[s];

        unsigned int const idx = util::find_index( label, labels);

        if( idx > labels.size())
        {
          util::Log logging;
          logging << "get_attribute_value() Error! Label " << label << " does not exist for simplex s" << util::Log::newline();
          throw std::logic_error("Label does not exist for simplex s");
        }

        return m_simplex0_real_attributes.at(name)[s][idx];
      }

      /**
       * This function sets the value of a custom (named) attribute at a given 0-simplex,
       * associated with specified phase.
       *
       * @param label       label identifying the phase
       * @param att_value   the new value of the attribute.
       */
      void set_attribute_value(
                               std::string    const & name
                               , Simplex0     const & s
                               , unsigned int const & label
                               , T            const & att_value
                               )
      {
        if (!exist_attribute( name, 0u))
        {
          util::Log logging;
          logging << "set_attribute_value() Error! Attribute " << name << " does not exist!" << util::Log::newline();
          throw std::logic_error("Trying to set attribute value in an undefined attribute");
        }

        std::vector< unsigned int> const & labels = m_simplex0_labels[s];

        if (m_simplex0_real_attributes[name][s].size() < labels.size())
        {
          m_simplex0_real_attributes[name][s].resize(labels.size());
        }

        unsigned int const idx = util::find_index( label, labels );

        if( idx > labels.size())
        {
          util::Log logging;
          logging << "set_attribute_value() Error! Label " << label << " does not exist for simplex s" << util::Log::newline();
          throw std::logic_error("Trying to set set target value to an undefined label at s");
        }

        m_simplex0_real_attributes[name][s][idx] = att_value;
      }

    public:

      /**
       * This function returns the value of a custom (named) attribute at a given 1-simplex.
       */
      T const & get_attribute_value(
                                    std::string    const & name
                                    , Simplex1     const & s
                                    ) const
      {
        if (!exist_attribute(name, 1u))
        {
          util::Log logging;

          logging << "Internal error: Simplex1 attribute " << name  << " does not exist?" << util::Log::newline();

          throw std::logic_error("Trying to access an attribute value from an undefined attribute");
        }

        return m_simplex1_real_attributes.at(name)[s];
      }

      /**
       * This function sets the value of a custom (named) attribute at a given 1-simplex.
       */
      void set_attribute_value(
                               std::string    const & name
                               , Simplex1     const & s
                               , T            const & att_value
                               )
      {
        if (!exist_attribute(name, 1u))
        {
          util::Log logging;

          logging << "Internal error: Simplex1 attribute " << name  << " does not exist?" << util::Log::newline();

          throw std::logic_error("Trying to set an attribute value in an undefined attribute");
        }

        m_simplex1_real_attributes[name][s] = att_value;
      }


    public:

      /**
       * This function returns the value of a custom (named) attribute at a given 2-simplex.
       */
      T const & get_attribute_value(
                                    std::string    const & name
                                    , Simplex2     const & s
                                    ) const
      {
        if (!exist_attribute(name, 2u))
        {
          util::Log logging;

          logging << "Internal error: Simplex2 attribute " << name  << " does not exist?" << util::Log::newline();

          throw std::logic_error("Trying to access an attribute value from an undefined attribute");
        }

        return m_simplex2_real_attributes.at(name)[s];
      }

      /**
       * This function sets the value of a custom (named) attribute at a given 2-simplex.
       */
      void set_attribute_value(
                               std::string    const & name
                               , Simplex2     const & s
                               , T            const & att_value
                               )
      {
        if (!exist_attribute(name, 2u))
        {
          util::Log logging;

          logging << "Internal error: Simplex2 attribute " << name  << " does not exist?" << util::Log::newline();

          throw std::logic_error("Trying to set an attribute value in an undefined attribute");
        }

        m_simplex2_real_attributes[name][s] = att_value;
      }


    public:

      // 2016-01-11 Marek: Maybe this one should stay public, some GLUE functions use it.
      /**
       * This function returns the vector of all phase labels adjacent at a specified 0-simplex.
       */
      std::vector< unsigned int> const & get_simplex0_labels( Simplex0 const & s) const
      {
        return m_simplex0_labels[s];
      }

    private:

      std::vector< unsigned int> & simplex0_labels( Simplex0 const & s)
      {
        return m_simplex0_labels[s];
      }


    private:

      void add_simplex0_label(
                              Simplex0     const & s
                              , unsigned int const & label
                              )
      {
        m_simplex0_labels[s].push_back(label);
      }

    private:

      void copy_attribute_names( Attributes<types> const & from_attributes)
      {
        m_simplex0_attribute_names.clear();
        m_simplex0_real_attributes.clear();

        name_iterator name_it = from_attributes.simplex0_attribute_names().begin();
        for ( ; name_it != from_attributes.simplex0_attribute_names().end(); ++name_it)
        {
          create_attribute( *name_it, 0u);
        }

        m_simplex1_attribute_names.clear();
        m_simplex1_real_attributes.clear();

        name_it = from_attributes.simplex1_attribute_names().begin();
        for ( ; name_it != from_attributes.simplex1_attribute_names().end(); ++name_it)
        {
          create_attribute( *name_it, 1u);
        }

        m_simplex2_attribute_names.clear();
        m_simplex2_real_attributes.clear();

        name_it = from_attributes.simplex2_attribute_names().begin();
        for ( ; name_it != from_attributes.simplex2_attribute_names().end(); ++name_it)
        {
          create_attribute( *name_it, 2u);
        }
      }

    private:

      void copy_attribute_values(
                                 Simplex0 const & to
                                 , Simplex0 const & from
                                 , Attributes<types> const & from_attributes
                                 )
      {
        m_current[to] = from_attributes.m_current[from];

        std::vector<unsigned int> const & from_labels = from_attributes.m_simplex0_labels[from];
        std::vector<unsigned int>       & to_labels   = m_simplex0_labels[to];

        name_vector               const & from_names  = from_attributes.simplex0_attribute_names();

        if (from_labels.size() > 0u)
        {
          to_labels.clear();
          to_labels.insert( to_labels.begin(), from_labels.begin(), from_labels.end());

          for (unsigned int i = 0u; i < to_labels.size(); ++i)
          {
            unsigned int const label = to_labels[i];

            name_iterator name_it = from_names.begin();

            for ( ; name_it != from_names.end(); ++name_it)
            {
              std::string const & name = *name_it;

              if( exist_attribute( name, 0u))
              {
                T const val = from_attributes.get_attribute_value( name, from, label);

                set_attribute_value( name, to, label, val);
              }
            }
          }
        }
        else
        {
          //--- Labels have to be initialized for every vertex, needed to avoid SegFaults
          m_simplex0_labels[to].clear();
        }
      }


      void copy_attribute_values(
                                 Simplex1 const & to
                                 , Simplex1 const & from
                                 , Attributes<types> const & from_attributes
                                 )
      {
        name_iterator name_it = from_attributes.simplex1_attribute_names().begin();
        for ( ; name_it != from_attributes.simplex1_attribute_names().end(); ++name_it)
        {
          std::string const & name = *name_it;
          if( exist_attribute( name, 1u))
          {
            if( from_attributes.exist_attribute_value( name, from))
            {
              T const val = from_attributes.get_attribute_value( name, from);
              set_attribute_value( name, to, val);
            }
          }
        }
      }

      void copy_attribute_values(
                                 Simplex2 const & to
                                 , Simplex2 const & from
                                 , Attributes<types> const & from_attributes
                                 )
      {
        name_iterator name_it = from_attributes.simplex2_attribute_names().begin();
        for ( ; name_it != from_attributes.simplex2_attribute_names().end(); ++name_it)
        {
          std::string const & name = *name_it;
          if( exist_attribute( name, 2u))
          {
            if( from_attributes.exist_attribute_value( name, from))
            {
              T const val = from_attributes.get_attribute_value( name, from);
              set_attribute_value( name, to, val);
            }
          }
        }
      }

    private:

      void clear( Simplex0 const s)
      {
        m_simplex0_labels[s].clear();

        name_iterator name_it = m_simplex0_attribute_names.begin();
        for( ; name_it != m_simplex0_attribute_names.end(); ++name_it)
        {
          std::string const & name = *name_it;
          m_simplex0_real_attributes[name][s].clear();
        }
      }

      void remove( SimplexSet const & S)
      {
        for( SimplexSet::simplex0_const_iterator it = S.begin0(); it != S.end0(); ++it)
        {
          Simplex0 const & s = *it;
          // 2016-01-18 Marek: this is unnecessary and sometimes gets in the way
          //m_current[s] = V( VT::zero());
          m_simplex0_labels[s].clear();

          name_iterator name_it = m_simplex0_attribute_names.begin();
          for ( ; name_it != m_simplex0_attribute_names.end(); ++name_it)
          {
            std::string const & name = *name_it;
            m_simplex0_real_attributes[name][s].clear();
          }
        }

        for( SimplexSet::simplex1_const_iterator it = S.begin1(); it != S.end1(); ++it)
        {
          Simplex1 const & s = *it;

          name_iterator name_it = m_simplex1_attribute_names.begin();
          for ( ; name_it != m_simplex1_attribute_names.end(); ++name_it)
          {
            std::string const & name = *name_it;
            m_simplex1_real_attributes[name].erase(s);
          }
        }

        for( SimplexSet::simplex2_const_iterator it = S.begin2(); it != S.end2(); ++it)
        {
          Simplex2 const & s = *it;

          name_iterator name_it = m_simplex2_attribute_names.begin();
          for ( ; name_it != m_simplex2_attribute_names.end(); ++name_it)
          {
            std::string const & name = *name_it;
            m_simplex2_real_attributes[name].erase(s);
          }
        }
      }

    private:

      void insert( SimplexSet const & S)
      {
        for( SimplexSet::simplex0_const_iterator it = S.begin0(); it != S.end0(); ++it)
        {
          //--- Touch each attribute vector so they're initialized
          //--- Attribute assignment takes care of adding labels etc.
          Simplex0 const & s = *it;

          m_current[s] += V( VT::zero());

          // 2016-01-18 Marek: this is so that we don't kill the attributes associated with
          //                   vertices passed through new_simplices (VertexSplit does that),
          //                   if they already exist. I guess it's not a very pretty solution,
          //                   and we should use parent_lut for Simplex0 too.
          if( m_simplex0_labels[s].size() == 0u)
          {
            name_iterator name_it = m_simplex0_attribute_names.begin();
            for ( ; name_it != m_simplex0_attribute_names.end(); ++name_it)
            {
              std::string const & name = *name_it;
              m_simplex0_real_attributes[name][s].clear();
            }
          }
        }
      }

    }; // class Attributes

  } // end namespace details
} // end namespace grit

// GRIT_ATTRIBUTES_H
#endif
