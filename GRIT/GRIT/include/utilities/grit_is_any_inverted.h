#ifndef GRIT_IS_ANY_INVERTED_H
#define GRIT_IS_ANY_INVERTED_H

#include <grit_simplex_set.h>
#include <grit_simplex.h>

#include <util_triangle_area.h>

#include <vector>

namespace grit
{
  namespace details
  {
    /**
     * Returns true if the (signed) area of any triangle in simplex set S is
     * less than threshold, otherwise returns false.
     */
    template<
             typename V
             , typename AT ///< attribute manager types
             >
    inline bool is_any_inverted(
                                SimplexSet               const & S
                                , AT                     const & attributes
                                , typename V::value_type const & threshold = V::value_traits::zero()
                                )
    {
      typedef typename V::value_type T;

      SimplexSet::simplex2_const_iterator it = S.begin2();

      for (; it != S.end2(); ++it)
      {
        Simplex2 const & s2 = *it;

        std::vector<V> vertices(3u);
        for (unsigned int i = 0u; i < 3u; ++i)
        {
          Simplex0 const & s0 = s2.get_simplex_from_local_index(i);
          vertices[i] = attributes.get_current_value( s0 );
        }

        T const area = util::signed_triangle_area(
                                                  vertices[0u]
                                                  , vertices[1u]
                                                  , vertices[2u]
                                                  );

        if (area <= threshold)
        {
          return true;
        }
      }

      return false;
    } // is_any_inverted

    /**
     * Returns true if moving simplex to a position specified by new_pos
     * reduces the (signed) area of any triangle in simplex set S below
     * threshold, otherwise returns false.
     */
    template<
             typename V
             , typename AMT ///< attribute manager types
             >
    inline bool is_any_inverted(
                                Simplex0     const & simplex
                                , SimplexSet const & S
                                , AMT        const & attributes
                                , V          const & new_pos
                                , typename V::value_type const & threshold = V::value_traits::zero()
                                )
    {
      typedef typename V::value_type T;

      SimplexSet::simplex2_const_iterator it = S.begin2();

      for( ; it != S.end2(); ++it)
      {
        Simplex2 const & s2 = *it;

        unsigned int const id_current = s2.get_local_index( simplex );

        assert( id_current < 3u || !"is_any_inverted() ERROR simplex not found in indices vector.");

        std::vector<V> vertices(3u);

        for (unsigned int i = 0u; i < 3u; ++i)
        {
          Simplex0 const & s0 = s2.get_simplex_from_local_index(i);
          vertices[i] = attributes.get_current_value( s0 );
        }

        T const area = util::signed_triangle_area(
                                                  new_pos
                                                  , vertices[(id_current+1u)%3u]
                                                  , vertices[(id_current+2u)%3u]
                                                  );

        if (area <= threshold)
        {
          return true;
        }
      }

      return false;
    } // is_any_inverted

  } // namespace details

} // namesace grit

// GRIT_IS_ANY_INVERTED_H
#endif
