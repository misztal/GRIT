#ifndef STEFAN_INITIALIZE_DOMAIN_H
#define STEFAN_INITIALIZE_DOMAIN_H

#include <stefan_domain.h>

#include <cmath>
#include <vector>
#include <algorithm>

namespace stefan
{

  inline void initialize_domain(
                                  Domain & domain
                                , double & core_x
                                , double & core_y
                                , double & core_radius
                                )
  {
    using std::sqrt;
    using std::max;

    util::Log         logging;

    std::string const newline = util::Log::newline();
    std::string const tab     = util::Log::tab();

    std::vector<double> & x = domain.m_interface_x;
    std::vector<double> & y = domain.m_interface_y;

    core_radius = 0.0;
    core_x      = 0.0;
    core_y      = 0.0;

    unsigned int const N = x.size();

    {
      for( unsigned int n=0u; n<N; ++n)
      {
        double const x_n = x[n];
        double const y_n = y[n];

        core_x += x_n;
        core_y += y_n;
      }
      core_x /= N;
      core_y /= N;

      double radius_sqr = 0.0;
      for( unsigned int n=0u; n<N; ++n)
      {
        double const x_n = x[n] - core_x;
        double const y_n = y[n] - core_y;

        double distance_sqr = x_n*x_n + y_n*y_n;

        radius_sqr = max( radius_sqr, distance_sqr);
      }

      core_radius = sqrt(radius_sqr);

      logging << "initialize_domain(): core_radius = " << core_radius << newline;
      logging << "initialize_domain(): core_x      = " << core_x      << newline;
      logging << "initialize_domain(): core_y      = " << core_y      << newline;
    }

    typedef std::pair<double, unsigned int>  pair_type;
    std::vector< pair_type > reorder;

    {
      reorder.resize(N);

      for( unsigned int i=0u; i<N; ++i)
      {
        double const dx = x[i] - core_x;
        double const dy = y[i] - core_y;

        double const radius = sqrt(dx*dx+dy*dy);
        double const scale  = core_radius/radius;

        x[i] = dx*scale + core_x;
        y[i] = dy*scale + core_y;

        double const theta =  atan2(dy, dx);

        reorder[i]  =  std::make_pair( theta, i );
      }

      std::sort( reorder.begin(), reorder.end() );
    }

    for( unsigned int i=0u; i<N; ++i)
    {
      int const idx = reorder[i].second;

      double const dx = x[idx] - core_x;
      double const dy = y[idx] - core_y;

      double const theta = std::atan2(dx,dy);

      double const ct  = std::cos(10.*theta);
      double const ct2 = ct*ct;
      double const ct4 = ct2*ct2;
      double const dd = 1.-0.025*ct4*ct4;

      x[idx] = dx*dd + core_x;
      y[idx] = dy*dd + core_y;
    }

  }

} //namespace stefan

// STEFAN_INITIALIZE_DOMAIN_H
#endif
