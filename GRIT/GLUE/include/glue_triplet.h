#ifndef GLUE_TRIPLET_H
#define GLUE_TRIPLET_H

#include <vector>
#include <limits>

namespace glue
{

  /**
   * A triplet class.
   * This class is used to represent triangles by storing the three
   * vertex indices defining the corners of the triangle. The global
   * vertex  indices i, j and k are locally 0, 1 and 2 vertex indices. It
   * is often assumed when vertices are mapped to their coordinates that
   * the index ordering are such that one can assume a positive orientation
   * (ccw-order) of the vertices.
   */
  class Triplet
  {
  public:

	static unsigned int UNDEFINED() { return std::numeric_limits<unsigned int>::max(); }

  public:


    unsigned int i;
    unsigned int j;
    unsigned int k;

  public:

    Triplet()
    : i( UNDEFINED() )
    , j( UNDEFINED() )
    , k( UNDEFINED() )
    {}

  public:

    bool has_member(unsigned int const & n) const
    {
      return (i == n || j == n || k == n);
    }

    std::vector< unsigned int > convert_to_vector() const
    {
      std::vector< unsigned int > indices;

      indices.resize(3);
      indices[0] = i;
      indices[1] = j;
      indices[2] = k;

      return indices;
    }

    unsigned int operator[](unsigned int const & idx) const
    {
      if(idx==0u)
        return i;
      if(idx==1u)
        return j;
      if(idx==2u)
        return k;
      return UNDEFINED();
    }

    unsigned int operator()(unsigned int const & idx) const
    {
      return (*this)[idx];
    }

  };

  inline unsigned int get_local_index(Triplet const & T, unsigned int idx)
  {
    if( T.i == idx )
      return 0u;

    if( T.j == idx )
      return 1u;

    if( T.k == idx )
      return 2u;

    return Triplet::UNDEFINED();
  }


  inline Triplet make_triplet(unsigned int const & i, unsigned int const & j, unsigned int const & k)
  {
    Triplet triplet;

    triplet.i = i;
    triplet.j = j;
    triplet.k = k;

    return triplet;
  }

}// namespace glue

// GLUE_TRIPLET_H
#endif
