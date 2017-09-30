#ifndef LIQUID_COMPUTE_TIME_STEP_H
#define LIQUID_COMPUTE_TIME_STEP_H

#include <liquid_element_array_info.h>
#include <liquid_compute_element_array_info.h>
#include <liquid_compute_stabilization.h>
#include <liquid_parameters.h>

#include <util_tensors.h>
#include <util_coo_matrix.h>
#include <util_solve_equation.h>
#include <util_generate_filename.h>

#include <glue_tuple.h>
#include <glue_triplet.h>
#include <glue_compute_neighbors.h>
#include <glue_matrix_assembly.h>
#include <glue_vector_assembly.h>
#include <glue_matlab_write_mesh.h>

#include <vector>

namespace liquid
{

  inline void compute_time_step(
                                Parameters const & params
                                , std::vector<glue::Triplet> const & triangles
                                , std::vector<glue::Triplet> const & adjacent
                                , std::vector<double> const & px        // Current spatial x-component of nodal positions
                                , std::vector<double> const & py        // Current spatial  y-component of nodal positions
                                , std::vector<double> const & vx        // Current x-component of nodal velocited
                                , std::vector<double> const & vy        // Current y-component of nodal positions
                                , std::vector<double> const & p         // Current center pressure values
                                , std::vector<double>       & vx_new    // New x-component of nodal velocities
                                , std::vector<double>       & vy_new    // New y-component of nodal velocities
                                , std::vector<double>       & p_new     // New center pressure values
  )
  {
    assert( is_valid(params) || !"compute_time_step(): internal error");

    using namespace util;

    ElementArrayInfo info;

    compute_element_array_info(params, triangles, adjacent, px, py, vx, vy, info );

    // Given time discretization we have
    //
    //  A = (M + delta_t D + delta_t^2 Hessian(U))
    //  b = M u + delta_t B f - delta_t nabla U
    //
    // where U is the surface tension potential

    bool const interlaced          = true;  // Controls the memory layout of the assembled matrices
    bool const gurad_against_zeros = false;
    bool const verbose             = false;

    unsigned int const T  = triangles.size();
    unsigned int const V  = px.size();

    util::COOMatrix<double> A;
    util::COOMatrix<double> D;
    util::COOMatrix<double> S;

    util::COOMatrix<double> const M = glue::matrix_assembly<double>(
                                                                    triangles
                                                                    , V
                                                                    , info.m_Me
                                                                    , interlaced
                                                                    , gurad_against_zeros
                                                                    , verbose
                                                                    );

    util::COOMatrix<double> const P = glue::matrix_assembly<double>(
                                                                    triangles
                                                                    , V
                                                                    , info.m_Pe
                                                                    , interlaced
                                                                    , gurad_against_zeros
                                                                    , verbose
                                                                    );

    if (params.use_viscosity() )
    {
      D = glue::matrix_assembly<double>(
                                        triangles
                                        , V
                                        , info.m_De
                                        , interlaced
                                        , gurad_against_zeros
                                        , verbose
                                        );

      A = util::add(M, util::mul( D, params.dt(), gurad_against_zeros, verbose) );
    }
    else
    {
      A = M;
    }


    if (params.use_stabilization())
    {
      S = compute_stabilization(params, triangles, adjacent, px, py);
    }
    else
    {
      S = util::make_zero<double>( T );
    }

    std::vector<double>     const u  = util::make_from_blocks(vx, vy, interlaced);
    std::vector<double>           b  = util::mul(M,u);

    std::vector<double> gravity;
    std::vector<double> surface_tension;

    if (params.use_gravity())
    {
      std::vector<double> const f  = util::make_from_blocks(info.m_bx, info.m_by, interlaced);

      gravity = util::mul( params.dt(), util::mul(M,f) );

      util::add_to(b, gravity);
    }

    if ( params.use_surface_tension() )
    {
      surface_tension = glue::vector_assembly<double>(
                                                      info.m_free_surface
                                                      , V
                                                      , info.m_Fs
                                                      , interlaced
                                                      );

      util::add_to(b, surface_tension);
    }

    std::vector<double>  e; e.resize( T, 0.0 );

    //
    // Resulting matrix equation to be solved
    //
    //    | A    P  |  | u |    | b |
    //    | P^T  S  |  | p |  = | e |
    //
    //         K         z    =   k
    //
    util::COOMatrix<double> const K = params.use_pressure() ? util::make_from_blocks(A, P, S)     : A;
    std::vector<double>           z = params.use_pressure() ? util::make_from_blocks(u, p, false) : u;
    std::vector<double>     const k = params.use_pressure() ? util::make_from_blocks(b, e, false) : b;

    std::vector<double> residuals;

    util::solve_equation(
                         K
                         , z
                         , k
                         , params.max_iterations()
                         , params.relative_tolerance()
                         , params.absolute_tolerance()
                         , params.precond_choice()
                         , params.solver_choice()
                         , (params.use_profiling() ? &residuals : 0)
                         , params.use_host()
                         );

    std::vector<double> u_new;

    if (params.use_pressure())
    {
      util::split_into_blocks(z, u.size(), u_new, p_new);
    }
    else
    {
      u_new = z;
    }

    util::split_into_blocks(u_new, vx_new, vy_new, interlaced);

    if(params.matlab_write())
    {
      static int count = 1;

      std::string const filename = params.output_path() + "/" + params.matlab_filename();

      std::ofstream matlab;

      if( params.matlab_append() )
      {
        matlab.open( filename.c_str() , std::ofstream::out | std::ofstream::app);

        if(count==1)
        {
          matlab << "close all;" << std::endl;
          matlab << "clear all;" << std::endl;
          matlab << "clc;" << std::endl;
        }

      }
      else
      {
        std::string const stripped         = filename.substr(0, filename.find_last_of(".") );
        std::string const unique_filename  = util::generate_filename(stripped, count, "m");

        matlab.open(unique_filename.c_str() , std::ofstream::out );

        matlab << "close all;" << std::endl;
        matlab << "clear all;" << std::endl;
        matlab << "clc;" << std::endl;
      }

      matlab << " interlaced = " << interlaced << ";" << std::endl;
      matlab << " T = " << glue::matlab_write_mesh(triangles) << ";" << std::endl;
      matlab << " M = " << util::to_string(M) << ";" << std::endl;
      matlab << " D = " << util::to_string(D) << ";" << std::endl;
      matlab << " S = " << util::to_string(S) << ";" << std::endl;
      matlab << " P = " << util::to_string(P) << ";" << std::endl;
      matlab << " A = " << util::to_string(A) << ";" << std::endl;

      matlab << util::matlab_write_vector("b", b) << std::endl;
      matlab << util::matlab_write_vector("u", u) << std::endl;
      matlab << util::matlab_write_vector("u_new", u_new) << std::endl;
      matlab << util::matlab_write_vector("gravity", gravity) << std::endl;
      matlab << util::matlab_write_vector("tension", surface_tension) << std::endl;


      matlab << " K = " << util::to_string(K) << ";" << std::endl;
      matlab << util::matlab_write_vector("z", z) << std::endl;
      matlab << util::matlab_write_vector("k", k) << std::endl;
      matlab << util::matlab_write_vector("residuals", residuals) << std::endl;


      matlab << util::matlab_write_vector("px", px) << std::endl;
      matlab << util::matlab_write_vector("py", py) << std::endl;
      matlab << util::matlab_write_vector("vx", vx) << std::endl;
      matlab << util::matlab_write_vector("vy", vy) << std::endl;
      matlab << util::matlab_write_vector("vx_new", vx_new) << std::endl;
      matlab << util::matlab_write_vector("vy_new", vy_new) << std::endl;
      matlab << util::matlab_write_vector("p", p) << std::endl;
      matlab << util::matlab_write_vector("p_new", p_new) << std::endl;

      matlab << "figure(1);" << std::endl;
      matlab << "clf" << std::endl;
      matlab << "plot(residuals);" << std::endl;
      matlab << "xlabel('iterations');" << std::endl;
      matlab << "ylabel('Residual');" << std::endl;
      matlab << "title('Convergence plot');" << std::endl;

      matlab << "figure(2);" << std::endl;
      matlab << "triplot(T,px,py,'r');" << std::endl;
      matlab << "hold on;" << std::endl;
      matlab << "quiver(px,py, vx_new, vy_new, 'b');" << std::endl;
      matlab << "xlabel('x');" << std::endl;
      matlab << "ylabel('y');" << std::endl;
      matlab << "title('Final velocity');" << std::endl;
      matlab << "hold off;" << std::endl;

      matlab << "figure(3);" << std::endl;
      matlab << "triplot(T,px,py,'r');" << std::endl;
      matlab << "hold on;" << std::endl;
      matlab << "quiver(px,py, vx, vy, 'b');" << std::endl;
      matlab << "xlabel('x');" << std::endl;
      matlab << "ylabel('y');" << std::endl;
      matlab << "title('Initial velocity');" << std::endl;
      matlab << "hold off;" << std::endl;

      if(params.use_pressure())
      {
        matlab << "figure(4);" << std::endl;
        matlab << "triplot(T,px,py,'r');" << std::endl;
        matlab << "hold on;" << std::endl;
        matlab << "cdata = p;" << std::endl;
        matlab << "caxis( [ min(cdata(:))  max(cdata(:)) ] );" << std::endl;
        matlab << "patch('Faces', T, 'Vertices', [px', py'], 'FaceColor','flat','FaceVertexCData',cdata');" << std::endl;
        matlab << "colorbar;" << std::endl;
        matlab << "xlabel('x');" << std::endl;
        matlab << "ylabel('y');" << std::endl;
        matlab << "title('Pressure Field');" << std::endl;
        matlab << "hold off;" << std::endl;
      }

      if(params.use_surface_tension())
      {
        matlab << "figure(5);" << std::endl;
        matlab << "triplot(T,px,py,'r');" << std::endl;
        matlab << "hold on;" << std::endl;
        matlab << "tension_x = tension(1:2:end-1);" << std::endl;
        matlab << "tension_y = tension(2:2:end);" << std::endl;
        matlab << "quiver(px,py, tension_x, tension_y, 'b');" << std::endl;
        matlab << "xlabel('x');" << std::endl;
        matlab << "ylabel('y');" << std::endl;
        matlab << "title('Surface Tension Forces');" << std::endl;
        matlab << "hold off;" << std::endl;
      }

      if(params.use_gravity())
      {
        matlab << "figure(6);" << std::endl;
        matlab << "triplot(T,px,py,'r');" << std::endl;
        matlab << "hold on;" << std::endl;
        matlab << "gravity_x = gravity(1:2:end-1);" << std::endl;
        matlab << "gravity_y = gravity(2:2:end);" << std::endl;
        matlab << "quiver(px,py, gravity_x, gravity_y, 'b');" << std::endl;
        matlab << "xlabel('x');" << std::endl;
        matlab << "ylabel('y');" << std::endl;
        matlab << "title('Gravity Forces');" << std::endl;
        matlab << "hold off;" << std::endl;
      }

      if(params.use_pressure())
      {
        matlab << "figure(7);" << std::endl;
        matlab << "triplot(T,px,py,'r');" << std::endl;
        matlab << "hold on;" << std::endl;
        matlab << "pressure_forces = -(P*p')';" << std::endl;
        matlab << "pressure_x = pressure_forces(1:2:end-1);" << std::endl;
        matlab << "pressure_y = pressure_forces(2:2:end);" << std::endl;
        matlab << "quiver(px,py, pressure_x, pressure_y, 'b');" << std::endl;
        matlab << "xlabel('x');" << std::endl;
        matlab << "ylabel('y');" << std::endl;
        matlab << "title('Pressure Forces');" << std::endl;
        matlab << "hold off;" << std::endl;
      }

      if(params.use_viscosity())
      {
        matlab << "figure(8);" << std::endl;
        matlab << "triplot(T,px,py,'r');" << std::endl;
        matlab << "hold on;" << std::endl;
        matlab << "viscosity_forces = (D*u_new')';" << std::endl;
        matlab << "viscosity_x = viscosity_forces(1:2:end-1);" << std::endl;
        matlab << "viscosity_y = viscosity_forces(2:2:end);" << std::endl;
        matlab << "quiver(px,py, viscosity_x, viscosity_y, 'b');" << std::endl;
        matlab << "xlabel('x');" << std::endl;
        matlab << "ylabel('y');" << std::endl;
        matlab << "title('Viscosity Forces');" << std::endl;
        matlab << "hold off;" << std::endl;
      }

      matlab.flush();
      matlab.close();

      ++count;
    }

  }

}// namespace liquid

// LIQUID_COMPUTE_TIME_STEP_H
#endif
