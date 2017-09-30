#ifndef GRIT_COMPUTE_MIDPOINT
#define GRIT_COMPUTE_MIDPOINT

namespace grit
{
  template <typename MT>
  inline typename MT::vector3_type compute_midpoint(
                                             Simplex0 const & A
                                             ,Simplex0 const & B
                                             , Simplex0AttributeVector<typename MT::vector3_type> const & coords
                                             )
  {
    typedef typename MT::vector3_type V;

    V const & av = coords[A];
    V const & bv = coords[B];

    V cv = V(0.5 * (av[0] + bv[0]), 0.5 * (av[1] + bv[1]), 0);

    return cv;
  }

  template <typename MT>
  inline typename MT::vector3_type compute_midpoint(
                                               Simplex0 const & A
                                             , Simplex0 const & B
                                             , Simplex0 const & C
                                             , Simplex0AttributeVector<typename MT::vector3_type> const & coords
                                             )
  {
    typedef typename MT::vector3_type V;

    V const & av = coords[A];
    V const & bv = coords[B];
    V const & cv = coords[C];

    V v = V( (av[0] + bv[0] + cv[0]) / 3.0, (av[1] + bv[1] + cv[1]) / 3.0, 0);

    return v;
  }

}// end namespace grit

// GRIT_COMPUTE_MIDPOINT
#endif
