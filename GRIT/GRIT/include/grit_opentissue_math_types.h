#ifndef GRIT_OPENTISSUE_MATH_TYPES_H
#define GRIT_OPENTISSUE_MATH_TYPES_H

#include <util_is_finite.h>
#include <util_is_number.h>

#include <OpenTissue/core/containers/mesh/polymesh/polymesh.h>

namespace grit
{

  template <typename mesh_type>
  class OpenTissueMathTypes
  {
  public:

    typedef typename mesh_type::math_types::real_type    real_type;
    typedef typename mesh_type::math_types::vector3_type vector3_type;
    typedef typename vector3_type::value_traits          VT;

    static real_type length(vector3_type const & v)
    {
      return OpenTissue::math::length(v);
    }

    static real_type sqr_distance(vector3_type const & v1, vector3_type const & v2)
    {
      return OpenTissue::math::sqr_distance(v1, v2);
    }

    static vector3_type perpendicular(vector3_type const & v)
    {
      return vector3_type(-v[1u], v[0u], VT::zero());
    }

    static vector3_type normalize(vector3_type const & v)
    {
      return OpenTissue::math::unit(v);
    }

    static real_type dot(vector3_type const & v1, vector3_type const & v2)
    {
      return v1[0u]*v2[0u]+v1[1u]*v2[1u];
    }

    static real_type infinity()
    {
      return std::numeric_limits<real_type>::infinity();
    }

    static real_type zero()
    {
      return VT::zero();
    }

    static real_type pi()
    {
      return VT::pi();
    }

    static real_type deg_to_rad(real_type const t)
    {
      return t*VT::pi()/180.;
    }

  };

}//end namespace grit

// GRIT_OPENTISSUE_MATH_TYPES_H
#endif
