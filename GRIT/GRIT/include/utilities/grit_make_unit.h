#ifndef GRIT_MAKE_UNIT_H
#define GRIT_MAKE_UNIT_H

#include <grit_engine_2d.h>
#include <grit_attribute_vector.h>

#include <cmath>

namespace grit
{

  /**
   * This function makes sure that the coordinaets fits exactly within a unit
   * box computational domain. That is after invocaiton one has x in [0..1] and y in [0..1].
   */
  template <typename types>
  inline void make_unit( details::Engine2D<types> & engine)
  {
    using std::min;
    using std::max;

    typedef typename types::real_type    T;
    typedef typename types::vector3_type V;

    typedef typename Simplex0AttributeVector<V>::iterator iterator;

    iterator begin = engine.attributes().current().begin();
    iterator end   = engine.attributes().current().end();

    T min_x = (*begin)[0];
    T max_x = (*begin)[0];
    T min_y = (*begin)[1];
    T max_y = (*begin)[1];

    for(iterator it = begin; it != end; ++it)
    {
      V const & p = (*it);

      T const x = p[0];
      T const y = p[1];

      min_x = min(x, min_x);
      max_x = max(x, max_x);
      min_y = min(y, min_y);
      max_y = max(y, max_y);
    }

    T const scale = max((max_x-min_x), (max_y-min_y));

    for(iterator it = begin; it != end; ++it)
    {
      V & p = (*it);

      T const x = p[0];
      T const y = p[1];

      p[0] = (x - min_x)/scale;
      p[1] = (y - min_y)/scale;
    }
  }

} // end namespace grit

// GRIT_MAKE_UNIT_H
#endif
