#ifndef STEFAN_COMPUTE_MOTION_H
#define STEFAN_COMPUTE_MOTION_H

#include <stefan_compute_all_velocities.h>
#include <stefan_compute_temperature.h>
#include <stefan_domain.h>
#include <stefan_make_domain.h>

#include <vector>
#include <cmath>

namespace stefan
{
  namespace details
  {

    inline void compute_vertex_coordinates(
                                            std::vector<double>        & x
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

  } // end namespace details

  inline void compute_motion(
                             grit::engine2d_type         const & engine
                             , Domain                          & domain
                             , std::vector<unsigned int> const & labels
                             , double                    const   c_liquid
                             , double                    const   c_solid
                             , double                    const   k_liquid
                             , double                    const   k_solid
                             , double                    const   latent_heat
                             , double                    const   t_outer
                             , double                    const   t_melting
                             , double                    const   dt
                             , double                    const   surf_tension
                             , double                    const   kinetic_coef
                             , std::vector<double>             & temperature
                             )
  {
    std::vector<double> vx;
    std::vector<double> vy;

    details::compute_all_velocities(
                                    engine
                                    , domain
                                    , temperature
                                    , k_liquid
                                    , k_solid
                                    , latent_heat
                                    , vx
                                    , vy
                                    );

    compute_temperature(
                        engine
                        , domain
                        , labels
                        , c_liquid
                        , c_solid
                        , k_liquid
                        , k_solid
                        , t_outer
                        , t_melting
                        , surf_tension
                        , kinetic_coef
                        , dt
                        , vx
                        , vy
                        , temperature
                        );

    details::compute_vertex_coordinates(
                                        domain.m_interface_x
                                        , domain.m_interface_y
                                        , vx
                                        , vy
                                        , dt
                                        );
  }

} // end namespace stefan

// STEFAN_COMPUTE_MOTION_H
#endif
