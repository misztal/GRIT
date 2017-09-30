#ifndef UTIL_BOOLEAN_H
#define UTIL_BOOLEAN_H

namespace util
{

  /**
   * Integer Type Wrapper.
   * This type wrapper is sometimes necessary when storing data
   * in STL containers to avoid STL code optimizations specific
   * for bool type.
   */
  struct Boolean
  {
    bool value;

    Boolean()
    : value(false)
    { }

  };

}//end of namespace util

// UTIL_BOOLEAN_H
#endif
