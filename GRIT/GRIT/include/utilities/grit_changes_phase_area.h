#ifndef GRIT_CHANGES_PHASE_AREA_H
#define GRIT_CHANGES_PHASE_AREA_H

#include <grit_simplex.h>
#include <grit_simplex_set.h>
#include <grit_interface_mesh.h>

#include <util_triangle_area.h>

#include <cmath>
#include <map>

namespace grit
{
  namespace details
  {
    /**
     * Returns true if collapsing vertex v towards vo (connected by an edge)
     * changes the area of any phase among triangles in simplex set S by
     * more than threshold. Returns false otherwise.
     */
    template<
             typename V
             , typename AT ///< attribute manager types
             >
    bool changes_phase_area(
                            Simplex0     const & v
                            , Simplex0   const & vo
                            , SimplexSet const & S
                            , AT         const & attributes
                            , InterfaceMesh const & mesh
                            , typename V::value_type const & threshold = V::value_traits::zero())
    {
      typedef typename V::value_type  T;

      std::map< unsigned int, T> old_areas, new_areas;

      {
        SimplexSet::simplex2_const_iterator it = S.begin2();
        for( ; it != S.end2(); ++it)
        {
          Simplex2 const & s2 = *it;

          unsigned int const l          = mesh.label(s2);
          unsigned int const id_current = s2.get_local_index( v );

          assert( id_current < 3u || !"changes_phase_area() ERROR simplex not found in indices vector.");

          std::vector<V> vertices(3u);

          for (unsigned int i = 0u; i < 3u; ++i)
          {
            Simplex0 const & s0 = s2.get_simplex_from_local_index(i);
            vertices[i] = attributes.get_current_value( s0 );
          }

          T const old_area = util::signed_triangle_area(
                                                        vertices[0u]
                                                        , vertices[1u]
                                                        , vertices[2u]
                                                        );

          V const & new_pos = attributes.get_current_value( vo );

          T const new_area = util::signed_triangle_area(
                                                        new_pos
                                                        , vertices[(id_current+1u)%3u]
                                                        , vertices[(id_current+2u)%3u]
                                                        );

          old_areas[l] += old_area;
          new_areas[l] += new_area;
        }
      }

      {
        typename std::map< unsigned int, T>::const_iterator it = old_areas.begin();
        for( ; it != old_areas.end(); ++it)
        {
          T const diff = std::abs( it->second - new_areas.at(it->first));

          if( diff > threshold)
          {
            return true;
          }
        }
      }

      return false;
    }
  } // namespace details
} // namespace grit

// GRIT_CHANGES_PHASE_AREA_H
#endif
