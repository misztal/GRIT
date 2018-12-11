#ifndef EROSION_INITIALIZE_CORE_GEOMETRY_H
#define EROSION_INITIALIZE_CORE_GEOMETRY_H

#include <erosion_core_object.h>

#include <cmath>
#include <vector>
#include <algorithm>

namespace erosion
{

  inline void initialize_core_geometry(
                                       CoreObject & core
                                       , double   & core_radius
                                       , double   & core_x
                                       , double   & core_y
                                       )
  {
    using std::sqrt;
    using std::max;

    util::Log         logging;

    std::string const newline = util::Log::newline();
    std::string const tab     = util::Log::tab();

    std::vector<double> & x = core.m_interface_x;
    std::vector<double> & y = core.m_interface_y;

    unsigned int const N = x.size();

    {
      core_x = 0.0;
      core_y = 0.0;
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

      logging << "initialize_core_geometry(): core_radius = " << core_radius << newline;
      logging << "initialize_core_geometry(): core_x      = " << core_x << newline;
      logging << "initialize_core_geometry(): core_y      = " << core_y << newline;
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

    /*for( unsigned int i=0u; i<N; ++i)
    {
      int const idx = reorder[i].second;

      if((i%40u)!=0u)
        continue;

      double const dx = x[idx] - core_x;
      double const dy = y[idx] - core_y;

      x[idx] = dx*0.95 + core_x;
      y[idx] = dy*0.95 + core_y;
    }*/

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

} //namespace erosion

// EROSION_INITIALIZE_CORE_GEOMETRY_H
#endif
