#ifndef GLUE_CLEAR_ATTRIBUTE_H
#define GLUE_CLEAR_ATTRIBUTE_H

#include <grit.h>

#include <glue_sub_range.h>   // Needed for VERTEX_ATTRIBUTE and FACE_ATTRIBUTE tags

#include <util_log.h>

#include <vector>
#include <string>
#include <stdexcept>   // needed for std::invalid_argument, std::logic_error

namespace glue
{

  inline void clear_attribute(
                              grit::engine2d_type      & engine
                              , std::string         const & name
                              , double              const & value
                              , VERTEX_ATTRIBUTE    const & /*tag*/
  )
  {
    if(! engine.attributes().exist_attribute(name, 0u) )
    {
      util::Log logging;

      logging << "clear_attribute(): Internal error; Simplex 0 attribute " << name  << " did not exist?" << util::Log::newline();

      throw std::logic_error("trying to clear non-existing attribute");
    }

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
        engine.attributes().set_attribute_value( name, s, labels[i], value);
    }
  }

  inline void clear_attribute(
                              grit::engine2d_type      & engine
                              , std::string         const & name
                              , double              const & value
                              , EDGE_ATTRIBUTE      const & /*tag*/
  )
  {
    if(! engine.attributes().exist_attribute(name, 1u) )
    {
      util::Log logging;

      logging << "clear_attribute(): Internal error; Simplex 1 attribute " << name  << " did not exist?" << util::Log::newline();

      throw std::logic_error("trying to clear non-existing attribute");
    }


    grit::SimplexSet const all     = engine.mesh().get_all_simplices();

    grit::SimplexSet const reduced = grit::filter(
                                                    all
                                                    , grit::IsDimension( engine.mesh(), 1u)
                                                    );

    for( grit::SimplexSet::simplex1_const_iterator it = reduced.begin1(); it != reduced.end1(); ++it)
    {
      grit::Simplex1 const s = *it;

      engine.attributes().set_attribute_value( name, s, value);
    }

  }

  inline void clear_attribute(
                              grit::engine2d_type      & engine
                              , std::string         const & name
                              , double              const & value
                              , FACE_ATTRIBUTE      const & /*tag*/
  )
  {
    if(! engine.attributes().exist_attribute(name, 2u) )
    {
      util::Log logging;

      logging << "clear_attribute(): Internal error; Simplex 2 attribute " << name  << " did not exist?" << util::Log::newline();

      throw std::logic_error("trying to clear non-existing attribute");
    }


    grit::SimplexSet const all     = engine.mesh().get_all_simplices();

    grit::SimplexSet const reduced = grit::filter(
                                                    all
                                                    , grit::IsDimension( engine.mesh(), 2u)
                                                    );

    for( grit::SimplexSet::simplex2_const_iterator it = reduced.begin2(); it != reduced.end2(); ++it)
    {
      grit::Simplex2 const s = *it;

      engine.attributes().set_attribute_value( name, s, value);
    }

  }

}// end namesapce glue

// GLUE_CLEAR_ATTRIBUTE_H
#endif
