#ifndef GLUE_COPY_ATTRIBUTE_H
#define GLUE_COPY_ATTRIBUTE_H

#include <grit.h>

#include <glue_sub_range.h>   // Needed for VERTEX_ATTRIBUTE and FACE_ATTRIBUTE tags

#include <vector>
#include <string>
#include <cassert>
#include <stdexcept>  // needed for std::logic_error and std::invalid_argument

namespace glue
{

  namespace details
  {
    /**
     *
     * @param index       The coordinate component of current that should be copied to the specified attribute.
     */
    inline void copy_attribute_from_current(
                                            grit::engine2d_type & engine
                                            , std::string const & to
                                            , unsigned int const index
                                            )
    {
      typedef typename grit::default_grit_types::real_type T;

      if(! engine.attributes().exist_attribute(to, 0u) )
      {
        util::Log logging;

        logging << "copy_attribute_from_current(): Internal error; Simplex 0 attribute " << to  << " did not exist?" << util::Log::newline();

        throw std::logic_error("trying to copy to non-existing attribute");
      }


      grit::SimplexSet const all     = engine.mesh().get_all_simplices();

      grit::SimplexSet const reduced = grit::filter(
                                                      all
                                                      , grit::IsDimension( engine.mesh(), 0u)
                                                      );

      for( grit::SimplexSet::simplex0_const_iterator it = reduced.begin0(); it != reduced.end0(); ++it)
      {
        grit::Simplex0 const s = *it;

        // 2016-01-11 Marek: Changing this to fit better with the new Attributes logic;
        //T const & value = engine.attributes().current()[s](index);
        T const & value = engine.attributes().get_current_value(s)(index);

        std::vector<unsigned int> const & labels = engine.attributes().get_simplex0_labels(s);

        for(unsigned int i = 0; i < labels.size(); ++i)
          engine.attributes().set_attribute_value(to, s, labels[i], value);

      }
    }

  } // end namespace details

  struct CURRENT_X {};
  struct CURRENT_Y {};

  /**
   * Copy attribute values from another attribute.
   *
   * @param engine         The engine instance to work on.
   * @param from           The name of the attribute to copy from
   * @param to             The name of the attribute to copy to
   */
  inline void copy_attribute(
                             grit::engine2d_type       & engine
                             , std::string          const & from
                             , std::string          const & to
                             , VERTEX_ATTRIBUTE     const & /*tag*/
  )
  {

    if(! engine.attributes().exist_attribute(from, 0u) )
    {
      util::Log logging;

      logging << "copy_attribute(): Internal error; Simplex 0 attribute " << from  << " did not exist?" << util::Log::newline();

      throw std::logic_error("trying to copy from non-existing attribute");
    }


    if(! engine.attributes().exist_attribute(to, 0u) )
    {
      util::Log logging;

      logging << "copy_attribute(): Internal error; Simplex 0 attribute " << to  << " did not exist?" << util::Log::newline();

      throw std::logic_error("trying to copy to non-existing attribute");
    }


    typedef typename grit::default_grit_types::real_type T;

    grit::SimplexSet const all     = engine.mesh().get_all_simplices();

    grit::SimplexSet const reduced = grit::filter(
                                                    all
                                                    , grit::IsDimension( engine.mesh(), 0u)
                                                    );

    for( grit::SimplexSet::simplex0_const_iterator it = reduced.begin0(); it != reduced.end0(); ++it)
    {
      grit::Simplex0 const s = *it;

      std::vector<unsigned int> const & labels = engine.attributes().get_simplex0_labels(s);

      for(unsigned int i = 0; i < labels.size(); ++i)
      {
        unsigned int const label = labels[i];

        T const & value = engine.attributes().get_attribute_value(from, s,label );

        engine.attributes().set_attribute_value(to, s,label, value);
      }
    }
  }

  /**
   * Copy x-values of current vertices to a given attribute.
   *
   * @param engine         The engine instance to work on.
   * @param from           A type tag that specify we want to copy the x-coordiante of current.
   * @param to             The name of the attribute to copy to.
   */
  inline void copy_attribute(
                             grit::engine2d_type       & engine
                             , CURRENT_X          const & from
                             , std::string        const & to
                             , VERTEX_ATTRIBUTE   const & /*tag*/
  )
  {
    details::copy_attribute_from_current(engine, to, 0u);
  }

  /**
   * Copy y-values of current vertices to a given attribute.
   *
   * @param engine         The engine instance to work on.
   * @param from           A type tag that specify we want to copy the y-coordiante of current.
   * @param to             The name of the attribute to copy to.
   */
  inline void copy_attribute(
                             grit::engine2d_type       & engine
                             , CURRENT_Y          const & from
                             , std::string        const & to
                             , VERTEX_ATTRIBUTE   const & /*tag*/
  )
  {
    details::copy_attribute_from_current(engine, to, 1u);
  }

}// end namespace glue

// GLUE_COPY_ATTRIBUTE_H
#endif
