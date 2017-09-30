#ifndef UTIL_FIND_INDEX_H
#define UTIL_FIND_INDEX_H

#include <vector>
#include <algorithm>  // needed for std:find
#include <limits>     // needed for std::numeric_limits
#include <cassert>

namespace util
{

  /**
   * This is a convenience wrapper function that uses STL to
   * quickly find the index of a value in a std::vector.
   *
   * @param value    The value to find the index of.
   * @param array    The vector to look for the value in.
   *
   * @return         The return value will be the index of @value in @array.
   */
  template <typename T>
  inline unsigned int find_index(
                                 T const & value
                                 , std::vector<T> const & array
                                 , bool const & nothrow_assert = true
                                 )
  {
    typedef unsigned int index_type;

    typename std::vector<T>::const_iterator p = find( array.begin(), array.end(), value);

    index_type const index = p - array.begin();

    if(index >= array.size())
    {
      assert( nothrow_assert || !"find_index(): value not in array");

      return std::numeric_limits<index_type>::max();
    }

    return index;
  }

}//end of namespace util

// UTIL_FIND_INDEX_H
#endif
