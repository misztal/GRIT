#ifndef GLUE_PHASE_H
#define GLUE_PHASE_H

#include <glue_triplet.h>
#include <glue_tuple.h>

#include <grit.h>

#include <vector>
#include <map>
#include <cassert>

namespace glue
{

  /**
   * A phase class.
   * This object class represents triangles, edges and vertices of a phase
   * in GRIT. It has additional information that allow one to map from a local
   * vertex index space to the global GRIT index space.
   *
   * The index spaces are perhaps better illustrated by a small example. Say
   * one has the triangle mesh with 6 vertices labeled V = [ 1 2 3 4 5 6];
   *
   *  T = [ 1 2 3;  1 3 4; 4 3 5; 3 2 5; 2 6 5  ];
   *
   * For completeness (but not important for phase representation) we assume the coordinates
   *
   *  x = [ 0 2 1 0 2 4];
   *  y = [ 0 0 1 2 2 1];
   *
   * One may inspect our little example using Matlab and writing triplot(T,x,y).
   *
   * Assume the phase labels of T are given as
   *
   * P = [2 2 1 1 -1]
   *
   * label -1 is often reserved to mean the ambient phase (empty void space). We now make a phase for phase label 1
   *
   * glue::Phase A = glue::make_phase( my_engine, 1 )
   *
   * According to our Matlab example mesh data phase A contains the following information
   *
   * A.m_label     == 1
   * A.m_vertices  == { 2, 3, 4, 5 }                        % Four vertices
   * A.m_indices   == { {2, 0}, {3, 1}, {4, 2 }, {5, 3 } }
   * A.m_edges     == [ 0 1; 0 3; 1 2; 1 3; 2 3 ]           % Five edges
   * A.m_triangles == [ 0 3 1; 1 3 2 ]                      % Two triangles
   *
   * Conversion from local to global index space are important for updating
   * attribute vector storing vertex information (@see glue::get_sub_range()
   * and glue::set_sub_range() ) or when accumulating data (@see example code
   * in glue::Neighbors).
   */
  class Phase
  {
  public:

    std::vector<unsigned int>            m_labels;        ///< The corresponding grit phase label of this object.

    std::vector<grit::Simplex0>          m_vertices;     ///< Local to global indices
    std::map<unsigned int, unsigned int> m_indices;      ///< Global to local indices.
    std::vector<Tuple>                   m_edges;        ///< All edges of object using local indices
    std::vector<Triplet>                 m_triangles;    ///< All triangles of object using local indices

  public:

    Phase()
    : m_labels()
    {}

    Phase(Phase const & A)
    {
      *this = A;
    }

    virtual ~Phase(){}

    Phase const & operator=(Phase const & A)
    {
      if(this != &A)
      {
        this->m_labels    = A.m_labels;
        this->m_vertices  = A.m_vertices;
        this->m_indices   = A.m_indices;
        this->m_edges     = A.m_edges;
        this->m_triangles = A.m_triangles;
      }
      return *this;
    }

  public:

    unsigned int get_local_index( grit::Simplex0 const & global) const
    {
      return m_indices.at( global.get_idx0() ); //C++11 feature
    }

    unsigned int get_local_index( unsigned int const & global) const
    {
      return m_indices.at( global ); //C++11 feature
    }

  };

}// end of namespace glue

// GLUE_PHASE_H
#endif
