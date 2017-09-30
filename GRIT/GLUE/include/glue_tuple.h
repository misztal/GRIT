#ifndef GLUE_TUPLE_H
#define GLUE_TUPLE_H

#include <limits>

namespace glue
{

  /**
   * A Tuple class.
   * This class is used to represent edges. The global vertex
   * indices i and j are mapped to the local indices 0 and 1.
   */
  class Tuple
  {
  public:
    
    static unsigned int UNDEFINED() { return std::numeric_limits<unsigned int>::max(); }

  public:


    unsigned int i;
    unsigned int j;

  public:

    Tuple()
    : i( UNDEFINED() )
    , j( UNDEFINED() )
    {}

    unsigned int operator[](unsigned int const & idx) const
    {
      if(idx==0u)
        return i;
      if(idx==1u)
        return j;
      return UNDEFINED();
    }

    unsigned int operator()(unsigned int const & idx) const
    {
      return (*this)[idx];
    }

  };

  inline unsigned int get_local_index(Tuple const & T, unsigned int idx)
  {
    if( T.i == idx )
      return 0u;

    if( T.j == idx )
      return 1u;

    return Tuple::UNDEFINED();
  }

  inline Tuple make_tuple(unsigned int const & i, unsigned int const & j)
  {
    Tuple tuple;

    tuple.i = i;
    tuple.j = j;

    return tuple;
  }


}// end of namespace glue

// GLUE_TUPLE_H
#endif
