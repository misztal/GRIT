#ifndef UTIL_INDEX_HELPER_H
#define UTIL_INDEX_HELPER_H

namespace util
{

  inline unsigned int to_one_based(unsigned int const & index) {  return index + 1u; }

  inline unsigned int to_zero_based(unsigned int const & index) {  return index - 1u; }

  
} // namespace util

// UTIL_INDEX_HELPER_H
#endif
