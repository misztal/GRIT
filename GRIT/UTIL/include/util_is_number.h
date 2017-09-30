#ifndef UTIL_IS_NUMBER_H
#define UTIL_IS_NUMBER_H

#ifdef WIN32
#include <float.h>
#else
#include <cmath>
#endif

namespace util
{

#ifdef WIN32
#define is_number(val) (_isnan(val)==0)   ///< Is a number test
#else
#if (__APPLE__)
#define is_number(val) (std::isnan(val)==0)   ///< Is a number test
#else
#define is_number(val) (std::isnan(val)==0)   ///< Is a number test
#endif
#endif

} // namespace tiny

//UTIL_IS_NUMBER_H
#endif
