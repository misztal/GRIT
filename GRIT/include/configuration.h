#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#if (_MSC_VER >= 1200)
# pragma once
# pragma warning(default: 56 61 62 191 263 264 265 287 289 296 347 529 686)
# pragma warning(disable: 503)
#endif

#ifdef WIN32
#  define WIN32_LEAN_AND_MEAN
#  define _USE_MATH_DEFINES
#  define NOMINMAX
#  include <windows.h>
#  undef WIN32_LEAN_AND_MEAN
#  undef NOMINMAX
#endif

// CONFIGURATION H
#endif
