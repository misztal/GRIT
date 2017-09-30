#ifndef AREA_COMPUTE_CONSTANT_PERIMETER_H
#define AREA_COMPUTE_CONSTANT_PERIMETER_H

#include <area_compute_perimeter_gradient.h>
#include <area_compute_perimeter.h>

#include <glue.h>

#include <cmath>

namespace area
{
  inline void compute_constant_perimeter(
                                     glue::Phase const & boundary
                                   , std::vector<double> const & px
                                   , std::vector<double> const & py
                                   , std::vector<double>       & px_new
                                   , std::vector<double>       & py_new
                                   , unsigned int const & max_perimeter_iterations
                                   , double const & beta
                                   , double const & c0
                                   , double const & c_threshold
                                   )
  {
    using std::fabs;

    assert(beta >= 0.0         || !"compute_constant_perimeter(): perimeter gradient step must be non-negative");
    assert(c_threshold >= 0.0  || !"compute_constant_perimeter(): perimeter threshold must be non-negative");

    unsigned int const N = px.size();

    px_new = px;
    py_new = py;

    double const c     = compute_perimeter(boundary.m_edges, px_new, py_new);
    double const gamma = c - c0;

    if( fabs( gamma ) > c_threshold)
    {
      for( unsigned int i = 0; i < max_perimeter_iterations; ++i)
      {
        std::vector<double> grad_perimeter_x;
        std::vector<double> grad_perimeter_y;
        area::compute_perimeter_gradient( boundary.m_edges, px_new, py_new, grad_perimeter_x, grad_perimeter_y);

        for( unsigned int n = 0; n < N; ++n)
        {
          px_new[n] = px_new[n] - beta*gamma*grad_perimeter_x[n];
          py_new[n] = py_new[n] - beta*gamma*grad_perimeter_y[n];
        }
      }
    }

  }
  
} //namespace area

// AREA_COMPUTE_CONSTANT_PERIMETER_H
#endif
