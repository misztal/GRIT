#ifndef GRIT_HAS_SIMPLE_BOUNDARY_H
#define GRIT_HAS_SIMPLE_BOUNDARY_H

#include <grit_engine_2d.h>
#include <grit_simplex_set.h>
#include <grit_simplex_set_functions.h>
#include <grit_logic_expressions.h>

#include <vector>
#include <map>

namespace grit
{

  namespace details
  {

    /**
     * This function tests if the current mesh has a simple
     * boundary. That is if boundary edges make exactly one
     * single non-intersection cycle. Hence, no open boundary
     * nor any holes inside mesh.
     *
     *
     * @note Current implemenation is not the fastest approach. Be
     *       careful or patient when testing large meshes.
     */
    template<typename types>
    inline bool has_simple_boundary( details::Engine2D<types> const & engine )
    {
      SimplexSet const B = filter( engine.mesh().get_all_simplices(), IsDimension(engine.mesh(),1) && IsBoundary(engine.mesh()) );

      typedef SimplexSet::simplex1_const_iterator iterator;

      //--- Initialize ---------------------------------------------------------
      std::map< Simplex0, bool> seen;
      std::map< Simplex1, bool> used;
      {
        iterator loop = B.begin1();
        iterator end  = B.end1();

        for ( ; loop != end; ++loop)
        {
          Simplex1 const edge = *loop;
          Simplex0 const a    = edge.get_simplex0_idx0();
          Simplex0 const b    = edge.get_simplex0_idx1();

          seen[a] = false;
          seen[b] = false;

          used[edge] = false;
        }
      }

      unsigned int const V = seen.size(); // Number of vertices
      unsigned int const E = used.size(); // Number of edges

      if(V != E)
      {
        // Boundary edges can not possible form a single simple closed loop
        return false;
      }

      //--- Test if boundary edges forms a simple closed loop ------------------
      Simplex0     const first = B.begin1()->get_simplex0_idx0();
      Simplex0           cur   = first;
      seen[first]              = true;
      unsigned int       count = 1;        // Number of vertices found on the boundary

      while (true)
      {
        iterator search = B.begin1();

        // 2015-12-26 Kenny code review: This for-loop is giving us O(n^2)
        // performance. It could be much faster if we could "follow" incidence
        // pointers in the mesh. However, on the other hand this implementation
        // assumes nothing of the mesh data structure.
        //
        // Actually, due to using std::map performance is going to be more
        // like O(n^2 log n).
        for (; search != B.end1(); ++search)
        {
          Simplex1 const edge = *search;

          if (used[edge])
            continue;

          Simplex0 const a = edge.get_simplex0_idx0();
          Simplex0 const b = edge.get_simplex0_idx1();

          if (cur == a )
          {

            if(seen[b])
            {
              bool const loop_to_first = b == first;
              bool const used_all      = count == V;

              return (used_all && loop_to_first);
            }

            used[edge] = true;
            seen[b]    = true;
            ++count;
            cur = b;

            break;
          }

          if (cur == b )
          {
            if(seen[a])
            {
              bool const loop_to_first = a == first;
              bool const used_all      = count == V;

              return (used_all && loop_to_first);
            }

            used[edge] = true;
            seen[a]    = true;
            ++count;
            cur = a;
            break;
          }

        }

        if(search == B.end1())
        {
          // We found open boundary issue with mesh
          return false;
        }

      }

      return true;
    }

  } // namespace details

} // namesace grit

// GRIT_HAS_SIMPLE_BOUNDARY_H
#endif
