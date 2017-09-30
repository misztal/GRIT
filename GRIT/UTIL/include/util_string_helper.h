#ifndef UTIL_STRING_HELPER_H
#define UTIL_STRING_HELPER_H

#include <string>

namespace util
{
  template<typename T>
  T to_value(std::string const & txt);
  
  template<typename T>
  std::string to_string(T const & value);
  
  
} // namespace util

// UTIL_STRING_HELPER_H
#endif
