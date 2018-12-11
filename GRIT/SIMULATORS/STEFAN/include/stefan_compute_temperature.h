#ifndef STEFAN_COMPUTE_TEMPERATURE_H
#define STEFAN_COMPUTE_TEMPERATURE_H

#include <stefan_assemble_problem.h>
#include <stefan_coo_matrix.h>
#include <stefan_domain.h>

#include <grit.h>
#include <util.h>

#include <util_solve_equation.h>

namespace stefan
{
  /**
   * This fuction performs the single time step integration of the heat equation:
   *
   * dT               _
   * -- = (k_i/c_i) * V^2 T ,
   * dt
   *
   * where i = solid, liquid; subject to boundary conditions
   *
   * T(x) = T_melting   on the interface between the solid and the liquid phase
   * T(x) = T_outer     on the outer boundary of the domain
   *
   * Variable @param temperature should provide the current, vertex-based temperature
   * field in the system, and upon return, the updated field.
   *
   * The PDE is discretized using the Finite Element method and solved by implicit
   * Euler method.
   */
  template< typename T>
  inline void compute_temperature(
                                  grit::engine2d_type         const & engine
                                  , Domain                    const & domain
                                  , std::vector<unsigned int> const & labels
                                  , T                         const   c_liquid
                                  , T                         const   c_solid
                                  , T                         const   k_liquid
                                  , T                         const   k_solid
                                  , T                         const   t_outer
                                  , T                         const   t_melting
                                  , T                         const   surface_tension
                                  , T                         const   kinetic_coefficient
                                  , T                         const   dt
                                  , std::vector<T>            const & vx
                                  , std::vector<T>            const & vy
                                  , std::vector<T>                  & temperature
                                  , unsigned int              const   max_iterations        = 100u
                                  , T                         const   relative_tolerance    = 1e-7
                                  , T                         const   absolute_tolerance    = 1e-10
                                  , unsigned int              const   preconditioner_choice = util::DIAGONAL_PRECONDITIONER
                                  , unsigned int              const   solver_choice         = util::GMRES_SOLVER
                                  , bool                      const   write                 = true
                                  , bool                      const   use_host              = true
                                  )
  {
    COOMatrix<T>   A;
    std::vector<T> b;

    stefan::assemble_problem(
                               engine
                             , domain
                             , labels
                             , temperature
                             , c_liquid
                             , c_solid
                             , k_liquid
                             , k_solid
                             , t_outer
                             , t_melting
                             , surface_tension
                             , kinetic_coefficient
                             , dt
                             , vx
                             , vy
                             , A
                             , b
                             );

    std::vector<T> residuals;

    util::solve_equation(
                           A
                         , temperature
                         , b
                         , max_iterations
                         , relative_tolerance
                         , absolute_tolerance
                         , preconditioner_choice
                         , solver_choice
                         , (write ? &residuals : 0)
                         , use_host
                         );

  }

} // namespace stefan

// STEFAN_COMPUTE_TEMPERATURE_H
#endif
