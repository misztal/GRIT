#ifndef GLUE_SVG_DRAW_H
#define GLUE_SVG_DRAW_H

#include <grit.h>

#include <string>
#include <vector>

namespace glue
{
  // 2016-04-14 Marek: the styles and functionality are moved to grit_svg_draw.h

  /**
   *
   * @param attribute_name_x          Specifies the name of user specified attribute that
   *                                  should be used as the x-coordinate for the drawing. Default
   *                                  value is "current" which means the x-coordinate will
   *                                  be taken from the in-built current attributes.
   *
   * @param attribute_name_y          Works the same as attribute_name_x except this is for
   *                                  setting the y-coordinaets.
   */
  inline void svg_draw(
                       std::string const & filename
                       , grit::engine2d_type const & engine
                       , grit::param_type const & parameters
                       , std::string const & attribute_name_x = "current"
                       , std::string const & attribute_name_y = "current"
                       )
  {
    grit::SimplexSet const all = engine.mesh().get_all_simplices();

    grit::svg_draw(
                   filename
                   , all
                   , engine.mesh()
                   , parameters
                   , engine.attributes()
                   , attribute_name_x
                   , attribute_name_y
                   );
  }


}//end namespace glue

// GLUE_SVG_DRAW_H
#endif
