#ifndef GLUE_MAKE_ADJACENCY
#define GLUE_MAKE_ADJACENCY

#include <glue_triplet.h>
#include <glue_phase.h>

#include <grit.h>

#include <vector>
#include <map>
#include <cassert>

namespace glue
{
  namespace details
  {

    inline bool is_valid_adjacent(std::vector< unsigned int > const & A, unsigned int const & N)
    {
      unsigned int const UNDEFINED = glue::Triplet::UNDEFINED();

      unsigned int const & a = A[0u];
      unsigned int const & b = A[1u];
      unsigned int const & c = A[2u];

      if(a==UNDEFINED && b==UNDEFINED && c==UNDEFINED)
      {
        return false;
      }

      if (a!=UNDEFINED)
      {
        if (a>=N)
          return false;

        if(a==b)
          return false;

        if(a==c)
          return false;
      }

      if (b!=UNDEFINED)
      {
        if (b>=N)
          return false;

        if(b==a)
          return false;

        if(b==c)
          return false;
      }

      if (c!=UNDEFINED)
      {
        if (c>=N)
          return false;

        if(c==a)
          return false;

        if(c==b)
          return false;
      }

      return true;
    }

  }// end namespace details

  inline void make_adjacency(
                             grit::engine2d_type   const & engine
                             , glue::Phase          const & phase
                             , std::vector<Triplet>       & adjacency
                             )

  {
    typedef std::map< grit::Simplex2, unsigned int>   index_map;
    typedef index_map::const_iterator                  index_iterator;

    adjacency.clear();

    unsigned int const UNDEFINED = glue::Triplet::UNDEFINED();
    unsigned int const N         = phase.m_triangles.size();

    adjacency.resize(N);

    //--- Build a triangle index map ---------------------------------------
    index_map  triangle_indices;
    {
      unsigned int count = 0u;

      for(unsigned int t = 0; t < N; ++t)
      {
        unsigned int    const & i  = phase.m_triangles[t].i;
        unsigned int    const & j  = phase.m_triangles[t].j;
        unsigned int    const & k  = phase.m_triangles[t].k;

        grit::Simplex0 const & v0 = phase.m_vertices[i];
        grit::Simplex0 const & v1 = phase.m_vertices[j];
        grit::Simplex0 const & v2 = phase.m_vertices[k];

        grit::Simplex2 const & s = grit::make_simplex2(v0, v1, v2);

        triangle_indices[s] = count;

        ++count;
      }
    }

    //--- Loop over triangles and find their adjacent neighbors --------------
    {
      std::vector< grit::Simplex1 > edges(3u);
      std::vector< unsigned int >    adjacent(3u);

      for(unsigned int t = 0u; t < N; ++t)
      {
        unsigned int    const & i  = phase.m_triangles[t].i;
        unsigned int    const & j  = phase.m_triangles[t].j;
        unsigned int    const & k  = phase.m_triangles[t].k;

        grit::Simplex0 const & v0 = phase.m_vertices[i];
        grit::Simplex0 const & v1 = phase.m_vertices[j];
        grit::Simplex0 const & v2 = phase.m_vertices[k];

        grit::Simplex2 const & s = grit::make_simplex2(v0, v1, v2);

        edges[0u] = grit::Simplex1(s.get_simplex0_idx1(), s.get_simplex0_idx2());
        edges[1u] = grit::Simplex1(s.get_simplex0_idx2(), s.get_simplex0_idx0());
        edges[2u] = grit::Simplex1(s.get_simplex0_idx0(), s.get_simplex0_idx1());

        for (unsigned int i = 0u; i < 3u; ++i)
        {
          grit::SimplexSet const star = engine.mesh().star( edges[i] );

          if (star.size(2u) == 1u)
          {
            adjacent[i] = UNDEFINED;
          }
          else
          {
            grit::Simplex2 A;
            grit::Simplex2 B;
            grit::extract_simplices(A,B, star);

            if( s == A)
            {
              // B must be adjacent, assign the index of B if B is in the domain
              index_iterator lookup = triangle_indices.find( B );

              adjacent[i] = lookup == triangle_indices.end() ? UNDEFINED : lookup->second ;
            }
            else
            {
              // B==s and A must be adjacent, assign the index of A if A is in the domain
              index_iterator lookup = triangle_indices.find( A );

              adjacent[i] = lookup == triangle_indices.end() ? UNDEFINED : lookup->second ;
            }
          }
        }

        assert( details::is_valid_adjacent(adjacent, N) || !"make_adjacent(): internal error ");

        adjacency[t] = make_triplet( adjacent[0u], adjacent[1u], adjacent[2u] );
      }
    }

  }

}// end namespace glue

// GLUE_MAKE_ADJACENCY
#endif
