#ifndef UTIL_IS_FINITE_H
#define UTIL_IS_FINITE_H

#ifdef WIN32
#include <float.h>
#else
#include <cmath>
#endif

namespace util
{
  
#ifdef WIN32

#define is_finite(val) (_finite(val)!=0)  ///< Is finite number test
#else
#if (__APPLE__)
#define is_finite(val) std::isfinite(val)   ///<Is finite number test
#else
#define is_finite(val) (finite(val)!=0)  ///< Is finite number test
#endif
#endif


} // namespace tiny

//UTIL_IS_FINITE_H
#endif


