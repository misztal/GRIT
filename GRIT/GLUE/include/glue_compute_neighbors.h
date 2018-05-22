#ifndef GLUE_COMPUTE_NEIGHBORS_H
#define GLUE_COMPUTE_NEIGHBORS_H

#include <glue_tuple.h>
#include <glue_triplet.h>
#include <glue_neighbors.h>

#include <util_log.h>

#include <vector>
#include <algorithm>

namespace glue
{

  namespace details
  {

    inline void print_V2T(Neighbors const & info)
    {
      util::Log log;

      log << "print V2T" << util::Log::newline();
      for ( size_t idx = 0u; idx < info.m_V2T.size(); ++idx)
      {
        log << "V2T[" << idx  << "] = (" << info.m_V2T[ idx ].first << "," << info.m_V2T[ idx ].second << ")" << util::Log::newline();
      }
    }

    inline void print_offset(Neighbors const & info)
    {
      util::Log log;

      log << "print offset" << util::Log::newline();
      for ( size_t idx = 0u; idx < info.m_offset.size(); ++idx)
      {
        log << "offset[" << idx  << "] = (" << info.m_offset[ idx ] << ")" << util::Log::newline();
      }
    }

    inline void scan_for_start_offset(
                                      unsigned int const & V
                                      , Neighbors  & info
                                      )
    {
      unsigned int const UNASSIGNED = 0xFFFFFFFFu;
      unsigned int const N          = info.m_V2T.size();

      info.m_offset.resize( V + 1u);

      std::fill(info.m_offset.begin(), info.m_offset.end(), UNASSIGNED);

      info.m_offset[ info.m_V2T[ 0 ].first ] = 0;
      info.m_offset[ V ]                     = N;

      for(unsigned int n = 1u; n < N; ++n)
      {
        unsigned int const cur  = info.m_V2T[ n ].first;
        unsigned int const prev = info.m_V2T[ n - 1u ].first;

        if (prev != cur)
        {
          info.m_offset[cur] = n;
        }
      }
    }

    inline void skip_isolated_nodes(
                                    unsigned int const & V
                                    , Neighbors  & info
                                    )
    {
      unsigned int const UNASSIGNED = 0xFFFFFFFFu;

      for(unsigned int v = 1u; v < V+1u; ++v)
      {
        unsigned int const cur = info.m_offset[v];

        if (cur == UNASSIGNED )
          continue;

        unsigned int scan = v-1u;
        unsigned int left = info.m_offset[scan];

        while(left == UNASSIGNED )
        {
          info.m_offset[scan] = cur;

          if(scan==0) // nothing more left to scan
            break;

          --scan;
          left =  info.m_offset[scan];
        }

      }
    }

  }// end namespace details

  /**
   * Compute Neighbors.
   * This function computes neighborhood lookup information. This is
   * perhaps better illustrated by a small example. Say one has the
   * triangle mesh with 6 vertices labeled V = [ 1 2 3 4 5 6];
   *
   *  T = [ 1 2 3;  1 3 4; 4 3 5; 3 2 5; 2 6 5  ];
   *
   * For completeness (but not important for finding neighbors) we assume the coordinates
   *
   *  x = [ 0 2 1 0 2 4];
   *  y = [ 0 0 1 2 2 1];
   *
   * One may inspect our little example using Matlab and writing triplot(T,x,y).
   *
   * The neighborhood information of the vertices are given as arrays of triagnle indexes (into the T-array).
   *
   *   N1 = [ 1 2 ];    % Triangle neighbors of vertex 1
   *   N2 = [ 1 4 5 ];  % Triangle neighbors of vertex 2
   *   N3 = [ 1 2 3 4]; % Triangle neighbors of vertex 3
   *   N4 = [ 2 3 ];    % Triangle neighbors of vertex 4
   *   N5 = [ 3 4 5 ];  % Triangle neighbors of vertex 5
   *   N6 = [ 5 ];      % Triangle neighbors of vertex 6
   *
   * This function essentially computes the Ni's above and stored this information
   * compactly into the info argument.
   *
   * @param triangles      An array of all triangles in the mesh
   * @param V              The number of vertices in the mesh (assumed labelled from 0 to V-1)
   * @param info           Upon return this argument holds the computed information. @See glue::Neighbors for details.
   */
  inline void compute_neighbors(
                                std::vector<Triplet> const & triangles
                                , unsigned int const & V
                                , Neighbors & info
                                )
  {
    unsigned int const T          = triangles.size();
    unsigned int const N          = T*3u;

    info.m_V2T.resize( N );

    for ( size_t idx = 0u; idx < T; ++idx)
    {
      unsigned int const i  = triangles[idx].i;
      unsigned int const j  = triangles[idx].j;
      unsigned int const k  = triangles[idx].k;

      info.m_V2T[ idx*3     ].first = i;
      info.m_V2T[ idx*3 + 1 ].first = j;
      info.m_V2T[ idx*3 + 2 ].first = k;

      info.m_V2T[ idx*3     ].second = idx;
      info.m_V2T[ idx*3 + 1 ].second = idx;
      info.m_V2T[ idx*3 + 2 ].second = idx;
    }

    std::sort( info.m_V2T.begin(), info.m_V2T.end() );

    details::scan_for_start_offset(V,info);

    details::skip_isolated_nodes(V,info);
  }

  /**
   * Compute Neighbors.
   * This function computes neighborhood lookup information. This is
   * perhaps better illustrated by a small example. Say one has the
   * triangle mesh with 6 vertices labeled V = [ 1 2 3 4 5 6];
   *
   *  T = [ 1 2 3;  1 3 4; 4 3 5; 3 2 5; 2 6 5  ];
   * And wish to find neighbors of the edges given by
   *
   *  E = [ 1 3; 3 5]
   *
   * For completeness (but not important for finding neighbors) we assume the coordinates
   *
   *  x = [ 0 2 1 0 2 4];
   *  y = [ 0 0 1 2 2 1];
   *
   * One may inspect our little example using Matlab and writing triplot(T,x,y).
   *
   * The neighborhood information of the vertices are given as arrays of edge indexes (into the E-array).
   *
   *   N1 = [ 1 ];     % Edge neighbors of vertex 1
   *   N2 = [  ];      % Edge neighbors of vertex 2
   *   N3 = [ 1 2 ];   % Edge neighbors of vertex 3
   *   N4 = [ ];       % Edge neighbors of vertex 4
   *   N5 = [ 2 ];     % Edge neighbors of vertex 5
   *   N6 = [  ];      % Edge neighbors of vertex 6
   *
   * This function essentially computes the Ni's above and stored this information
   * compactly into the info argument.
   *
   * @param edges         An array of the set of edges in the mesh one wish to work with (need not be all edges in the mesh but could be a subset)
   * @param V             The number of vertices in the mesh (assumed labelled from 0 to V-1)
   * @param info          Upon return this argument holds the computed information. @See glue::Neighbors for details.
   */
  inline void compute_neighbors(
                                std::vector<Tuple> const & edges
                                , unsigned int const & V
                                , Neighbors & info
                                )
  {
    unsigned int const T          = edges.size();

    unsigned int const N          = T*2u;

    info.m_V2T.resize( N );

    for ( size_t idx = 0u; idx < T; ++idx)
    {
      unsigned int const i  = edges[idx].i;
      unsigned int const j  = edges[idx].j;
      
      info.m_V2T[ idx*2     ].first = i; 
      info.m_V2T[ idx*2 + 1 ].first = j;
      
      info.m_V2T[ idx*2     ].second = idx;
      info.m_V2T[ idx*2 + 1 ].second = idx;
    }
    
    std::sort( info.m_V2T.begin(), info.m_V2T.end() );
    
    details::scan_for_start_offset(V,info);
    
    details::skip_isolated_nodes(V,info);
  }
  
}// namespace glue

// GLUE_COMPUTE_NEIGHBORS_H
#endif
