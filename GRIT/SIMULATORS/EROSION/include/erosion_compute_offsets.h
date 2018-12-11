#ifndef EROSION_COMPUTE_OFFSETS_H
#define EROSION_COMPUTE_OFFSETS_H

#include <erosion_core_object.h>
#include <erosion_compute_growth.h>

#include <vector>
#include <cmath>

namespace erosion
{
  namespace details
  {
    inline double compute_edge_offset(
                                      grit::engine2d_type const & engine
                                      , CoreObject        const & core
                                      , grit::Simplex1    const & edge
                                      )
    {
      using grit::Simplex0;
      using grit::SimplexSet;
      using std::min;
      using std::max;

      SimplexSet bnd = engine.mesh().boundary(edge);

      assert( bnd.size(0u)==2u || !"compute_edge_offset(): internal error");

      SimplexSet::simplex0_const_iterator it = bnd.begin0();

      Simplex0 const node_a = *it++;
      Simplex0 const node_b = *it++;

      unsigned int const idx_local_a = core.m_interface.m_indices.at(node_a.get_idx0());
      unsigned int const idx_local_b = core.m_interface.m_indices.at(node_b.get_idx0());

      double const angle_a = core.m_interface_vertex_theta[idx_local_a];
      double const angle_b = core.m_interface_vertex_theta[idx_local_b];

      double const t = std::fabs(angle_a-angle_b);

      return -min( max(sqrt(t), 0.001), 1.);
    }


    inline void compute_vertex_velocity(
                                        grit::engine2d_type const & engine
                                        , CoreObject        const & core
                                        , grit::Simplex0    const & vertex
                                        , unsigned int      const & local_id
                                        , grit::SimplexSet  const & edges
                                        , double                  & v_x
                                        , double                  & v_y
                                        )
    {
      using grit::Simplex0;
      using grit::Simplex1;
      using grit::SimplexSet;

      assert( edges.size(1u)==2u || !"compute_vertex_velocity(): internal error");

      SimplexSet::simplex1_const_iterator edge = edges.begin1();

      Simplex1 const edge_a = *edge++;
      Simplex1 const edge_b = *edge++;

      double na_x = 0.0;
      double na_y = 0.0;

      compute_edge_normal(engine, core.m_label, edge_a, na_x, na_y);

      double const hat_na_x =  na_y;
      double const hat_na_y = -na_x;

      double nb_x = 0.0;
      double nb_y = 0.0;

      compute_edge_normal(engine, core.m_label, edge_b, nb_x, nb_y);

      double const hat_nb_x =  nb_y;
      double const hat_nb_y = -nb_x;

      double const offset_a = compute_edge_offset( engine, core, edge_a);
      double const offset_b = compute_edge_offset( engine, core, edge_b);

      double const p_x  = core.m_interface_x[local_id];
      double const p_y  = core.m_interface_y[local_id];

      double const pa_x = p_x + offset_a * na_x;
      double const pa_y = p_y + offset_a * na_y;

      double const pb_x = p_x + offset_b * nb_x;
      double const pb_y = p_y + offset_b * nb_y;

      double const b_x  = pb_x - pa_x;
      double const b_y  = pb_y - pa_y;

      double const det   = -hat_na_x*hat_nb_y + hat_nb_x*hat_na_y;
      double const det_a = -b_x*hat_nb_y + b_y*hat_nb_x;
      double const det_b =  b_y*hat_na_x - b_x*hat_na_y;

      if( std::abs(det) > 5e-2)
      {
        double const t_a = det_a/det;
        double const t_b = det_b/det;

        double const tgt_x = ((pa_x + t_a*hat_na_x) + (pb_x + t_b*hat_nb_x))/2.;
        double const tgt_y = ((pa_y + t_a*hat_na_y) + (pb_y + t_b*hat_nb_y))/2.;

        v_x = tgt_x - p_x;
        v_y = tgt_y - p_y;
      }
      else
      {
        v_x = (pa_x + pb_x)/2. - p_x;
        v_y = (pa_y + pb_y)/2. - p_y;
      }

      double const v_sq = v_x*v_x+v_y*v_y;

      if( v_sq>5.0)
      {
        double const v = std::sqrt(v_sq);
        v_x /= v;
        v_y /= v;
      }

    }


    inline void compute_all_velocities(
                                       grit::engine2d_type   const & engine
                                       , CoreObject          const & core
                                       , std::vector<double>       & v_x
                                       , std::vector<double>       & v_y
                                       )
    {
      using grit::Simplex0;
      using grit::SimplexSet;

      glue::Phase           const & interface = core.m_interface;
      std::vector<Simplex0> const & vertices  = interface.m_vertices;

      unsigned int const N = vertices.size();

      v_x.resize( N, 0.0 );
      v_y.resize( N, 0.0 );

      for( unsigned int i=0u; i<N; ++i)
      {
        Simplex0     const s        = vertices[i];
        unsigned int const local_id = interface.m_indices.at(s.get_idx0());

        SimplexSet adjacent = engine.mesh().star(s);
        SimplexSet edges    = grit::compute_interface(engine.mesh(), adjacent);

        assert( edges.size(1)>=2u || !"compute_vertex_normals(): intenral error");

        if( edges.size(1)==2u)
        {
          compute_vertex_velocity(
                                  engine
                                  , core
                                  , s
                                  , local_id
                                  , edges
                                  , v_x[local_id]
                                  , v_y[local_id]
                                  );
        }
        else
        {
          //--- Non manifold configuration detected, do nothing.
        }
      }
    }


    inline void compute_vertex_coordinates(
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

  } // end namespace details

  inline void compute_offsets(
                              grit::engine2d_type const & engine
                              , CoreObject              & core
                              , double            const & mu
                              , double            const & core_radius
                              , double            const & core_x
                              , double            const & core_y
                              )
  {
    assert(mu          >= 0.0 || !"compute_offsets(): flow magnitude/strength must be non-negative" );
    assert(core_radius >  0.0 || !"compute_offsets(): core  radius must be positive"                );

    std::vector<double> vx;
    std::vector<double> vy;

    details::compute_all_velocities(
                                    engine
                                    , core
                                    , vx
                                    , vy
                                    );

    details::compute_vertex_coordinates(
                                        core.m_interface_x
                                        , core.m_interface_y
                                        , vx
                                        , vy
                                        , mu
                                        );

    /*details::project_onto_core(
                               core.m_interface_x
                               , core.m_interface_y
                               , core_radius
                               , core_x
                               , core_y
                               );*/

  }

} // end namespace erosion

// EROSION_COMPUTE_OFFSETS_H
#endif
