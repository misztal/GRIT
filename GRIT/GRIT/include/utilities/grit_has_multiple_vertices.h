#ifndef GRIT_HAS_MULTIPLE_VERTICES_H
#define GRIT_HAS_MULTIPLE_VERTICES_H

#include <grit_simplex_set.h>

namespace grit
{
  namespace details
  {

    template<
             typename V
             , typename AMT ///< attribute manager type
             >
    inline bool has_multiple_vertices(
                               SimplexSet               const & S
                               , AMT                    const & attributes
                               , typename V::value_type const & threshold = V::value_traits::zero()
                               )
    {
      typedef SimplexSet::simplex0_const_iterator iterator;

      iterator a   = S.begin0();
      iterator end = S.end0();

      for(; a != end; ++a)
      {

        V const & r_a = attributes.get_current_value( *a );

        iterator b = a;
        ++b;

        for(; b != end; ++b)
        {
          V const & r_b = attributes.get_current_value( *b );

          V const diff = r_a - r_b;

          if(  norm_1( diff ) < threshold )
            return true;

        }

      }

      return false;
    }

  } // namespace details

} // namesace grit

// GRIT_HAS_MULTIPLE_VERTICES_H
#endif
