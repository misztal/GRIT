#ifndef GRIT_ACCUMULATE_ATTRIBUTES_H
#define GRIT_ACCUMULATE_ATTRIBUTES_H

#include <grit_simplex.h>
#include <grit_attribute_vector.h>
#include <grit_simplex_set.h>

namespace grit
{

  template <typename T, typename F>
  inline T accumulate_attributes(
                       Simplex0AttributeVector<T> const & values
                     , SimplexSet const & A
                     , F const & functor
                     )
  {
    T value = T();

    for (SimplexSet::simplex2_const_iterator it = A.begin2(); it != A.end2(); ++it)
    {
      Simplex2 s = *it;

      T const v1 = values[s.get_simplex0_idx0()];
      T const v2 = values[s.get_simplex0_idx1()];
      T const v3 = values[s.get_simplex0_idx2()];

      value += functor(v1, v2, v3);
    }

    return value;
  }

  template <typename T>
  inline T accumulate_attributes(
                      Simplex0AttributeVector<T> const & values
                      , SimplexSet const & A
                      )
  {
    T value = T();

    for (SimplexSet::simplex0_const_iterator it = A.begin0(); it != A.end0(); ++it)
    {
      Simplex0 s = *it;

      value += values[s];
    }

    return value;
  }

} // end namespace grit

// GRIT_ACCUMULATE_ATTRIBUTES_H
#endif
