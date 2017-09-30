#ifndef UTIL_SOLVE_EQUATION_H
#define UTIL_SOLVE_EQUATION_H

#include <vector>

#include <util_solve_equation_choices.h>

#ifdef CUDA_FOUND
#include <util_solve_equation_cusp.h>
#else
#include <util_coo_matrix.h>
#include <util_coo_preconditioners.h>
#include <util_coo_pcg.h>
#include <util_coo_cr.h>
#include <util_coo_bicg.h>
#include <util_coo_bicgstab.h>
#include <util_coo_gmres.h>
#endif

namespace util
{

#ifdef CUDA_FOUND

  template<typename T>
  inline void solve_equation(
                             COOMatrix<T>      const & A
                             , std::vector<T>        & x
                             , std::vector<T>  const & b
                             , unsigned int    const & max_iterations
                             , T               const & relative_tolerance
                             , T               const & absolute_tolerance
                             , unsigned int    const & preconditioner_choice
                             , unsigned int    const & solver_choice
                             , std::vector<T>        * residuals
                             , bool            const & use_host
                             )
  {
    if(use_host)
    {
      solve_equation_host(
                          A.cols()
                          , A.nzeros()
                          , A.row_indices()
                          , A.column_indices()
                          , A.values()
                          , x
                          , b
                          , max_iterations
                          , relative_tolerance
                          , absolute_tolerance
                          , preconditioner_choice
                          , solver_choice
                          , residuals
                          );
    }
    else
    {
      solve_equation_device(
                            A.cols()
                            , A.nzeros()
                            , A.row_indices()
                            , A.column_indices()
                            , A.values()
                            , x
                            , b
                            , max_iterations
                            , relative_tolerance
                            , absolute_tolerance
                            , preconditioner_choice
                            , solver_choice
                            , residuals
                            );
    }
  }

#else

  template<typename T>
  inline void solve_equation(
                             COOMatrix<T>       const & A
                             , std::vector<T>         & x
                             , std::vector<T>   const & b
                             , unsigned int     const & max_iterations
                             , T                const & relative_tolerance
                             , T                const & absolute_tolerance
                             , unsigned int     const & preconditioner_choice
                             , unsigned int     const & solver_choice
                             , std::vector<T>         * residuals
                             , bool             const & use_host
                             )
  {
    util::Log         logging;
    std::string const newline = util::Log::newline();

    if(!use_host)
    {
      logging << "util::solve_equation(...): Only host  when CUSP/CUDA not available, defaulting to native COOMatrix instead of CUSP" << newline;
    }

    unsigned int const restart = 10u;
    bool         const verbose = false;

    util::COOMatrix<T> M;

    switch (preconditioner_choice)
    {
      case DIAGONAL_PRECONDITIONER:
        util::make_jacobi_preconditioner(A,M);
        break;
      case IDENTITY_PRECONDITIONER:
        util::make_identity_preconditioner(A,M);
        break;
      default:
        logging << "util::solve_equation(...): Unrecognized preconditioner choice  " << preconditioner_choice << newline;
        break;
    }

    switch (solver_choice)
    {
      case CG_SOLVER:
        util::pcg(M, A, x, b, max_iterations, restart, absolute_tolerance, verbose, residuals);
        break;
      case CR_SOLVER:
        util::cr(M, A, x, b, max_iterations, restart, absolute_tolerance, verbose, residuals);
        break;
      case BICG_SOLVER:
      {
        util::COOMatrix<T> At = util::transpose(A);
        util::COOMatrix<T> Mt = util::transpose(M);
        util::bicg(A, At, x, b, M, Mt, max_iterations, absolute_tolerance, verbose, residuals);
      }
        break;
      case BICGSTAB_SOLVER:
        util::bicgstab(M, A, x, b, max_iterations, absolute_tolerance, verbose, residuals);
        break;
      case GMRES_SOLVER:
        util::gmres(M, A, x, b, max_iterations, restart, absolute_tolerance, verbose, residuals);
        break;
      default:
        logging << "util::solve_equation(): Unrecognized solver choice  " << solver_choice << newline;
        break;
    }

  }

#endif

}//namespace util

//UTIL_SOLVE_EQUATION_H
#endif
