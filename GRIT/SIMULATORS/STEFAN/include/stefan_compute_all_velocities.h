#ifndef STEFAN_COMPUTE_ALL_VELOCITIES_H
#define STEFAN_COMPUTE_ALL_VELOCITIES_H

#include <stefan_domain.h>
#include <stefan_make_domain.h>

#include <grit.h>
#include <util.h>

namespace stefan
{
  namespace details
  {
    inline double compute_directional_derivative(
                                                   grit::engine2d_type       const & engine
                                                 , Domain                    const & domain
                                                 , std::vector<double>       const & field
                                                 , grit::Simplex2            const & triangle
                                                 , double                    const & n_x
                                                 , double                    const & n_y
                                                 )
    {
      using grit::Simplex0;

      using std::max;

      Simplex0 const & i0 = triangle.get_idx0();
      Simplex0 const & i1 = triangle.get_idx1();
      Simplex0 const & i2 = triangle.get_idx2();

      unsigned int const id_local_0 = domain.m_all.get_local_index(i0);
      unsigned int const id_local_1 = domain.m_all.get_local_index(i1);
      unsigned int const id_local_2 = domain.m_all.get_local_index(i2);

      unsigned int const max_id = max( id_local_0, max( id_local_1, id_local_2 ));

      assert( max_id<domain.m_x.size() || !"compute_directional_derivative() Internal error!");
      assert( max_id<domain.m_y.size() || !"compute_directional_derivative() Internal error!");
      assert( max_id<field.size()      || !"compute_directional_derivative() Internal error!");

      double x_0 = domain.m_x[id_local_0];
      double x_1 = domain.m_x[id_local_1];
      double x_2 = domain.m_x[id_local_2];

      double y_0 = domain.m_y[id_local_0];
      double y_1 = domain.m_y[id_local_1];
      double y_2 = domain.m_y[id_local_2];

      double f_0 = field[id_local_0];
      double f_1 = field[id_local_1];
      double f_2 = field[id_local_2];

	  double x_01 = x_1 - x_0;
	  double x_02 = x_2 - x_0;
	  double y_01 = y_1 - y_0;
	  double y_02 = y_2 - y_0;
	  double f_01 = f_1 - f_0;
	  double f_02 = f_2 - f_0;

      /*double const xc  = (x_0+x_1+x_2)/3.;
      double const yc  = (y_0+y_1+y_2)/3.;
      double const fc  = (f_0+f_1+f_2)/3.;

      x_0 -= xc;    x_1 -= xc;    //x_2 -= xc;
      y_0 -= yc;    y_1 -= yc;    //y_2 -= yc;
      f_0 -= fc;    f_1 -= fc;    //f_2 -= fc;*/

      double const det = x_01*y_02 - y_01*x_02;

      assert( std::fabs(det)>0. || "compute_directional_derivative() Error! encountered degenerate triangle!");

      double const grad_x = (f_01*y_02-f_02*y_01)/det;
      double const grad_y = (x_01*f_02-x_02*f_01)/det;

      return grad_x*n_x + grad_y*n_y;
    }


    inline double compute_edge_normal_velocity(
                                                 grit::engine2d_type       const & engine
                                               , Domain                    const & domain
                                               , std::vector<double>       const & temperature
                                               , double                    const   k_liquid
                                               , double                    const   k_solid
                                               , double                    const   latent_heat
                                               , grit::Simplex1            const & edge
                                               )
    {
      using grit::SimplexSet;
      using grit::Simplex2;

      SimplexSet const adjacent = engine.mesh().star(edge);

      assert( adjacent.size(2u) == 2u || !"compute_edge_normal_velocity() Internal error!" );

      SimplexSet::simplex2_const_iterator it = adjacent.begin2();

      Simplex2 t_solid, t_liquid;

      // 2017-02-12 Marek: here the value cached in stefan::Domain should be used
      double n_x = 0.0;
      double n_y = 0.0;

      compute_edge_normal(engine, domain.m_label_liquid, edge, n_x, n_y);

      if( engine.mesh().label(*it) == domain.m_label_liquid )
      {
        t_liquid = *it; ++it;
        t_solid  = *it;
      }
      else
      {
        t_solid  = *it; ++it;
        t_liquid = *it;
      }

      assert( engine.mesh().label(t_liquid)==domain.m_label_liquid || !"compute_edge_normal_velocity() Internal error!" );
      assert( engine.mesh().label(t_solid) ==domain.m_label_solid  || !"compute_edge_normal_velocity() Internal error!" );

      double const dT_dn_liquid = compute_directional_derivative( engine, domain, temperature, t_liquid, n_x, n_y );
      double const dT_dn_solid  = compute_directional_derivative( engine, domain, temperature, t_solid,  n_x, n_y );

      double const u_n = (k_solid*dT_dn_solid - k_liquid*dT_dn_liquid)/latent_heat;

      return u_n;
    }


    inline void compute_vertex_velocity(
                                        grit::engine2d_type const & engine
                                        , Domain            const & domain
                                        , grit::Simplex0    const & vertex
                                        , unsigned int      const & local_id
                                        , grit::SimplexSet  const & edges
                                        , double                  & v_x
                                        , double                  & v_y
                                        , std::map<grit::Simplex1,double> const & edge_normal_velocity
                                        )
    {
      using grit::Simplex0;
      using grit::Simplex1;
      using grit::SimplexSet;

      assert( edges.size(1u)==2u || !"compute_vertex_velocity(): internal error");

      SimplexSet::simplex1_const_iterator edge = edges.begin1();

      Simplex1 const edge_a = *edge++;
      Simplex1 const edge_b = *edge++;

      SimplexSet const Ca = engine.mesh().closure( edge_a );
      SimplexSet const Cb = engine.mesh().closure( edge_b );

      Simplex0 v_a, v_b;

      {
        SimplexSet::simplex0_const_iterator vert = Ca.begin0();

        if( vert->get_idx0() != vertex )
        {
          v_a = *vert;
        }
        else
        {
          ++vert;
          v_a = *vert;
        }
      }

      {
        SimplexSet::simplex0_const_iterator vert = Cb.begin0();

        if( vert->get_idx0() != vertex )
        {
          v_b = *vert;
        }
        else
        {
          ++vert;
          v_b = *vert;
        }
      }

      double const l_a = util::edge_length( engine.attributes().get_current_value(vertex), engine.attributes().get_current_value(v_a) );
      double const l_b = util::edge_length( engine.attributes().get_current_value(vertex), engine.attributes().get_current_value(v_b) );

      // 2017-02-12 Marek: here the value cached in stefan::Domain should be used
      double na_x = 0.0;
      double na_y = 0.0;

      compute_edge_normal(engine, domain.m_label_liquid, edge_a, na_x, na_y);

      //double const hat_na_x =  na_y;
      //double const hat_na_y = -na_x;

      // 2017-02-12 Marek: here the value cached in stefan::Domain should be used
      double nb_x = 0.0;
      double nb_y = 0.0;

      compute_edge_normal(engine, domain.m_label_liquid, edge_b, nb_x, nb_y);

      //double const hat_nb_x =  nb_y;
      //double const hat_nb_y = -nb_x;

      double const un_a = edge_normal_velocity.at(edge_a);
      double const un_b = edge_normal_velocity.at(edge_b);

      //double const p_x  = domain.m_interface_x[local_id];
      //double const p_y  = domain.m_interface_y[local_id];

      //double const pa_x = p_x + un_a * na_x;
      //double const pa_y = p_y + un_a * na_y;

      //double const pb_x = p_x + un_b * nb_x;
      //double const pb_y = p_y + un_b * nb_y;

      //double const b_x  = pb_x - pa_x;
      //double const b_y  = pb_y - pa_y;

      // 2017-02-12 Marek: try to think of some more elegant way of finding the intersection.
      //double const det   = -hat_na_x*hat_nb_y + hat_na_y*hat_nb_x;
      //double const det_a =      -b_x*hat_nb_y +      b_y*hat_nb_x;
      //double const det_b =      -b_x*hat_na_y +      b_y*hat_na_x;

      v_x = (un_a*na_x*l_a+un_b*nb_x*l_b)/(l_a+l_b);
      v_y = (un_a*na_y*l_a+un_b*nb_y*l_b)/(l_a+l_b);

	  //v_x = -(na_x*l_a+nb_x*l_b)/(l_a+l_b);
	  //v_y = -(na_y*l_a+nb_y*l_b)/(l_a+l_b);

      /*if( std::abs(det) > 1e-2)
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
      }*/

      /*double const v_sq = v_x*v_x+v_y*v_y;

      if( v_sq>10.0)
      {
        double const v = std::sqrt(v_sq);
        v_x /= 0.1*v;
        v_y /= 0.1*v;
      }*/

    }


    inline void compute_all_velocities(
                                       grit::engine2d_type         const & engine
                                       , Domain                    const & domain
                                       , std::vector<double>       const & temperature
                                       , double                    const   k_liquid
                                       , double                    const   k_solid
                                       , double                    const   latent_heat
                                       , std::vector<double>             & v_x
                                       , std::vector<double>             & v_y
                                       )
    {
      using grit::Simplex0;
      using grit::Simplex1;
      using grit::SimplexSet;

      using glue::Tuple;

      glue::Phase           const & interface = domain.m_interface;
      std::vector<Simplex0> const & vertices  = interface.m_vertices;
      std::vector<Tuple>    const & edges     = interface.m_edges;

      std::map<Simplex1,double> edge_normal_velocity;

      unsigned int const n_v = vertices.size();
      unsigned int const n_e = edges.size();

      v_x.resize( n_v, 0.0 );
      v_y.resize( n_v, 0.0 );

      //--- Compute edge normal velocities
      for( unsigned int i=0u; i<n_e; ++i )
      {
        Tuple    const & e   = edges[i];

        Simplex0 const & s0a = vertices[e.i];
        Simplex0 const & s0b = vertices[e.j];

        Simplex1 const s( s0a, s0b );

        edge_normal_velocity[s] = compute_edge_normal_velocity(
                                                                 engine
                                                               , domain
                                                               , temperature
                                                               , k_liquid
                                                               , k_solid
                                                               , latent_heat
                                                               , s
                                                               );
      }

      //--- Compute vertex velocities based on edge normal velocities
      for( unsigned int i=0u; i<n_v; ++i)
      {
        Simplex0     const s        = vertices[i];
        unsigned int const local_id = interface.m_indices.at(s.get_idx0());

        SimplexSet adjacent = engine.mesh().star(s);
        SimplexSet edges    = grit::compute_interface(engine.mesh(), adjacent);

        assert( edges.size(1u)>=2u || !"compute_vertex_normals() Internal error!");

        if( edges.size(1u)==2u)
        {
          compute_vertex_velocity(
                                    engine
                                  , domain
                                  , s
                                  , local_id
                                  , edges
                                  , v_x[local_id]
                                  , v_y[local_id]
                                  , edge_normal_velocity
                                  );
        }
        else
        {
          //--- Non manifold configuration detected, do nothing.
          continue;
        }
      }
    }

  } // namespace details

} // namespace stefan

// STEFAN_COMPUTE_ALL_VELOCITIES_H
#endif
