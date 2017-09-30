#ifndef GRIT_COMPUTE_BOUNDING_BOX_H
#define GRIT_COMPUTE_BOUNDING_BOX_H

#include <grit_simplex.h>
#include <grit_attribute_vector.h>
#include <grit_simplex_set.h>

namespace grit
{

  template <typename MT>
  inline void compute_bounding_box(
                              Simplex0AttributeVector<typename MT::vector3_type> & coords
                            , SimplexSet const & A
                            , typename MT::vector3_type & min
                            , typename MT::vector3_type & max
                            )
  {
    typedef  typename MT::vector3_type V;

    min = V(-MT::infinity(), -MT::infinity(), MT::zero());
    min = V( MT::infinity(),  MT::infinity(), MT::zero());

    for (SimplexSet::simplex0_const_iterator it = A.begin0(); it != A.end0(); ++it)
    {
      Simplex0 s = *it;


      V const & p = coords[s];

      min[0] = p[0] < min[0] ? p[0] : min[0];
      min[1] = p[1] < min[1] ? p[1] : min[1];
      min[2] = p[2] < min[2] ? p[2] : min[2];

      max[0] = p[0] > max[0] ? p[0] : max[0];
      max[1] = p[1] > max[1] ? p[1] : max[1];
      max[2] = p[2] > max[2] ? p[2] : max[2];
    }
  }

} // end namespace grit

// GRIT_COMPUTE_BOUNDING_BOX_H
#endif
