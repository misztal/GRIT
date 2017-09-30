#ifndef UTIL_INTEGER_H
#define UTIL_INTEGER_H

namespace util
{

  /**
   * Integer Type Wrapper.
   * This type wrapper is sometimes necessary when storing data
   * in STL containers to avoid STL code optimizations specific
   * for integer type.
   */
  struct Integer
  {
    int value;

    Integer()
    : value(-1)
    { }

  };

}//end of namespace util

// UTIL_INTEGER_H
#endif
