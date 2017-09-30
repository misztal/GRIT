#ifndef EROSION_COMPUTE_GROWTH_H
#define EROSION_COMPUTE_GROWTH_H

#include <erosion_core_object.h>

#include <vector>
#include <cmath>

namespace erosion
{

  namespace details
  {
    inline void compute_growth_velocity(
                                        CoreObject            const & core
                                        , std::vector<double>       & v_x
                                        , std::vector<double>       & v_y
                                        )
    {
      using std::max;

      unsigned int const N = core.m_interface_x.size();

      v_x.resize( N, 0.0 );
      v_y.resize( N, 0.0 );

      for( unsigned int v=0u; v<N; ++v)
      {
        bool   const strict_convex  = core.m_interface_vertex_strict_convex[v];
        double const interior_angle = core.m_interface_vertex_theta[v];
        double const n_x            = core.m_interface_vertex_normals_x[v];
        double const n_y            = core.m_interface_vertex_normals_y[v];

        if( strict_convex)
          continue;

        if( interior_angle < PI() )
          continue;

        double const speed = max(0.01, interior_angle );

        v_x[v] = - speed * n_x;
        v_y[v] = - speed * n_y;
      }
    }

    inline void update_coordinates(
                                   std::vector<double>          & x
                                   , std::vector<double>        & y
                                   , std::vector<double>  const & v_x
                                   , std::vector<double>  const & v_y
                                   , double               const & dt
                                   )
    {
      unsigned int const N = x.size();
      for( unsigned int n=0u; n<N; ++n)
      {
        x[n] +=  dt * v_x[n];
        y[n] +=  dt * v_y[n];
      }
    }

    inline void project_onto_core(
                                  std::vector<double>         & x
                                  , std::vector<double>       & y
                                  , double              const & core_radius
                                  , double              const & core_x
                                  , double              const & core_y
                                  )
    {
      using std::sqrt;

      unsigned int const N = x.size();

      double const core_radius_sqr = core_radius*core_radius;

      for( unsigned int n=0; n<N; ++n)
      {
        double x_n = x[n];
        double y_n = y[n];

        double distance_sqr = ((x_n - core_x) *(x_n - core_x))  +  ((y_n - core_y)*(y_n - core_y));

        if(distance_sqr > core_radius_sqr )
        {
          double const down_scale = sqrt(core_radius_sqr/distance_sqr);

          x_n =  (x_n - core_x) * down_scale  + core_x;
          y_n =  (y_n - core_y) * down_scale  + core_y;
        }

        x[n] = x_n;
        y[n] = y_n;
      }
    }

  } // end namespace details


  inline void compute_growth(
                             CoreObject       & core
                             , double   const & mu
                             , double   const & core_radius
                             , double   const & core_x
                             , double   const & core_y
                             )
  {
    assert(mu          >= 0.0 || !"compute_growth(): flow magnitude/strength must be non-negative" );
    assert(core_radius >  0.0 || !"compute_growth(): core  radius must be positive"                );

    std::vector<double> vx;
    std::vector<double> vy;

    details::compute_growth_velocity(
                                     core
                                     , vx
                                     , vy
                                     );

    details::update_coordinates(
                                core.m_interface_x
                                , core.m_interface_y
                                , vx
                                , vy
                                , mu
                                );

    details::project_onto_core(
                               core.m_interface_x
                               , core.m_interface_y
                               , core_radius
                               , core_x
                               , core_y
                               );


  }

} // end namespace erosion

// EROSION_COMPUTE_GROWTH_H
#endif
