#ifndef STEFAN_ASSEMBLE_PROBLEM_H
#define STEFAN_ASSEMBLE_PROBLEM_H

#include <stefan_coo_matrix.h>

#include <util_triangle_area.h>

#include <grit.h>

#include <vector>

namespace stefan
{
  template< typename T >
  inline void make_gibbs_boundary_condition(
                                            unsigned int     const & global_idx
                                            , Domain         const & domain
                                            , T              const & surface_tension
                                            , T              const & kinetic_coefficient
                                            , std::vector<T> const & vx
                                            , std::vector<T> const & vy
                                            , COOMatrix<T>         & A
                                            , std::vector<T>       & b
                                            )
  {
    A.insert( global_idx, global_idx, 1. );

    unsigned int const local_idx = domain.m_interface.m_indices.at(global_idx);

    double const v_x = vx[local_idx];
    double const v_y = vy[local_idx];

    double const n_x = domain.m_interface_vertex_normals_x[local_idx];
    double const n_y = domain.m_interface_vertex_normals_y[local_idx];

    double const k   = domain.m_curvature[local_idx];

    double const t_b = -surface_tension*k-kinetic_coefficient*(n_x*v_x+n_y*v_y);

    b[ global_idx ] += t_b;
  }

  template< typename T >
  inline void assemble_problem(
                                 grit::engine2d_type       const & engine
                               , Domain                    const & domain
                               , std::vector<unsigned int> const & labels
                               , std::vector<T>            const & temperature
                               , T                         const & c_liquid
                               , T                         const & c_solid
                               , T                         const & k_liquid
                               , T                         const & k_solid
                               , T                         const & t_outer
                               , T                         const & t_melting
                               , T                         const & surface_tension
                               , T                         const & kinetic_coefficient
                               , T                         const & dt
                               , std::vector<T>            const & vx
                               , std::vector<T>            const & vy
                               , COOMatrix<T>                    & A
                               , std::vector<T>                  & b
                               )
  {
    std::vector<glue::Triplet>  const & triangles = domain.m_all.m_triangles;
    std::vector<grit::Simplex0> const & vertices  = domain.m_all.m_vertices;

    unsigned int const N_v = temperature.size();
    unsigned int const N_t = triangles.size();

    double const alpha_solid  = dt*k_solid/c_solid;
    double const alpha_liquid = dt*k_liquid/c_liquid;

    A.clear();
    // 2017-02-19 Marek: it seems like automatic resizing causes the program to crash.
    A.init( N_v, N_v, 7u );

    b.resize( N_v, 0. );

    util::Log log;

    for( unsigned int i=0u; i<N_t; ++i)
    {
      glue::Triplet const & tri  = triangles[i];

      unsigned int  const   idx0 = tri.i;
      unsigned int  const   idx1 = tri.j;
      unsigned int  const   idx2 = tri.k;

      double const x0 = domain.m_x[idx0];
      double const x1 = domain.m_x[idx1];
      double const x2 = domain.m_x[idx2];

      double const y0 = domain.m_y[idx0];
      double const y1 = domain.m_y[idx1];
      double const y2 = domain.m_y[idx2];

      double const T0 = temperature[idx0];
      double const T1 = temperature[idx1];
      double const T2 = temperature[idx2];

      double const area = std::fabs((x1-x0)*(y2-y0)-(x2-x0)*(y1-y0))/2.;

      double const M_ii = area/6.;
      double const M_ij = area/12.;

      //--- Edge lengths (opposite to vertices 0, 1, 2)
      double const sqd0 = (x2-x1)*(x2-x1)+(y2-y1)*(y2-y1);
      double const sqd1 = (x0-x2)*(x0-x2)+(y0-y2)*(y0-y2);
      double const sqd2 = (x1-x0)*(x1-x0)+(y1-y0)*(y1-y0);

      double const d0   = std::sqrt(sqd0);
      double const d1   = std::sqrt(sqd1);
      double const d2   = std::sqrt(sqd2);

      //--- The heights
      double const h0   = 2.*area/d0;
      double const h1   = 2.*area/d1;
      double const h2   = 2.*area/d2;

      double const e0_x = (x2-x1)/d0;
      double const e1_x = (x0-x2)/d1;
      double const e2_x = (x1-x0)/d2;

      double const e0_y = (y2-y1)/d0;
      double const e1_y = (y0-y2)/d1;
      double const e2_y = (y1-y0)/d2;

      double const cos0 = e2_x*e1_x + e2_y*e1_y;
      double const cos1 = e0_x*e2_x + e0_y*e2_y;
      double const cos2 = e1_x*e0_x + e1_y*e0_y;

      double const alpha = (labels[i] == domain.m_label_liquid)?(alpha_liquid):(alpha_solid);

      double const L00  = area*alpha/(h0*h0);
      double const L11  = area*alpha/(h1*h1);
      double const L22  = area*alpha/(h2*h2);

      double const L01  = area*alpha*cos2/(h0*h1);
      double const L02  = area*alpha*cos1/(h0*h2);
      double const L12  = area*alpha*cos0/(h1*h2);

      // 2017-03-06 Marek: some redundancy here, each time a vertex is visited a boundary condition is constructed.
      //                   No effect on simulation, though - effectively just scaling.
      {
        if( grit::IsInterface(engine.mesh())(vertices[idx0]))
        {
          //make_gibbs_boundary_condition( idx0, domain, surface_tension, kinetic_coefficient, vx, vy, A, b );

          // 2017-03-06 Marek: this should be used for standard solidification,
          //                   but won't yield dendrites on its own.
		  A.insert(idx0, idx0, 1.);
          b[idx0] += t_melting;
        }
        // 2017-03-13 Marek: switching this off is equivalent to setting 0 von Neumann conditions.
        //                   TODO: try to make the b.c.'s configurable
        else if( grit::IsBoundary(engine.mesh())(vertices[idx0]))
        {
          A.insert( idx0, idx0, 1. );
          b[idx0] += t_outer;
        }
        else
        {
          A.insert( idx0, idx0, M_ii+L00 );
          A.insert( idx0, idx1, M_ij+L01 );
          A.insert( idx0, idx2, M_ij+L02 );

          b[idx0] += M_ii*T0 + M_ij*T1 + M_ij*T2;
        }
      }

      {
        if( grit::IsInterface(engine.mesh())(vertices[idx1]))
        {
          //make_gibbs_boundary_condition( idx1, domain, surface_tension, kinetic_coefficient, vx, vy, A, b );

          // 2017-03-06 Marek: this should be used for standard solidification,
          //                   but won't yield dendrites on its own.
          A.insert( idx1, idx1, 1. );
          b[idx1] += t_melting;
        }
        else if( grit::IsBoundary(engine.mesh())(vertices[idx1]))
        {
          A.insert( idx1, idx1, 1. );
          b[idx1] += t_outer;
        }
        else
        {
          A.insert( idx1, idx0, M_ij+L01 );
          A.insert( idx1, idx1, M_ii+L11 );
          A.insert( idx1, idx2, M_ij+L12 );

          b[idx1] += M_ij*T0 + M_ii*T1 + M_ij*T2;
        }
      }

      {
        if( grit::IsInterface(engine.mesh())(vertices[idx2]))
        {
          //make_gibbs_boundary_condition( idx2, domain, surface_tension, kinetic_coefficient, vx, vy, A, b );

          // 2017-03-06 Marek: this should be used for standard solidification,
          //                   but won't yield dendrites on its own.
          A.insert( idx2, idx2, 1. );
          b[idx2] += t_melting;
        }
        else if( grit::IsBoundary(engine.mesh())(vertices[idx2]))
        {
          A.insert( idx2, idx2, 1. );
          b[idx2] += t_outer;
        }
        else
        {
          A.insert( idx2, idx0, M_ij+L02 );
          A.insert( idx2, idx1, M_ij+L12 );
          A.insert( idx2, idx2, M_ii+L22 );

          b[idx2] += M_ij*T0 + M_ij*T1 + M_ii*T2;
        }
      }

    }
  }

} // namespace stefan

// STEFAN_ASSEMBLE_PROBLEM_H
#endif
