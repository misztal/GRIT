#ifndef AREA_COMPUTE_MAXIMIZE_AREA_H
#define AREA_COMPUTE_MAXIMIZE_AREA_H

#include <area_compute_area_gradient.h>

#include <glue.h>

#include <cmath>

namespace area
{
  inline void compute_maximize_area(
                                     glue::Phase const & obj
                                   , std::vector<double> const & px
                                   , std::vector<double> const & py
                                   , std::vector<double>       & px_new
                                   , std::vector<double>       & py_new
                                   , unsigned int const & max_area_iterations
                                   , double const & alpha
                                   )
  {
    using std::fabs;

    assert(alpha >= 0.0        || !"compute_maximization(): area gradient step must be non-negative"     );

    unsigned int const N = px.size();

    px_new = px;
    py_new = py;

    for( unsigned int i = 0; i < max_area_iterations; ++i)
    {
      std::vector<double> grad_area_x;
      std::vector<double> grad_area_y;
      area::compute_area_gradient( obj.m_triangles, px_new, py_new, grad_area_x, grad_area_y);

      for( unsigned int n = 0; n < N; ++n)
      {
        px_new[n] = px_new[n] + alpha*grad_area_x[n];
        py_new[n] = py_new[n] + alpha*grad_area_y[n];
      }
    }

  }
  
} //namespace area

// AREA_COMPUTE_MAXIMIZE_AREA_H
#endif
