#include <util_log.h>
#include <util_solve_equation_choices.h>

#include <cusp/coo_matrix.h>
#include <cusp/csr_matrix.h>
#include <cusp/precond/ainv.h>
#include <cusp/precond/diagonal.h>
#include <cusp/precond/aggregation/smoothed_aggregation.h>
#include <cusp/monitor.h>
#include <cusp/krylov/cg.h>
#include <cusp/krylov/cr.h>
#include <cusp/krylov/gmres.h>
#include <cusp/krylov/bicg.h>
#include <cusp/krylov/bicgstab.h>

#include <vector>

namespace util
{

  namespace detail
  {

    class DispatchSolver
    {
    protected:

      template<typename LinearOperator, typename Vector, typename Monitor, typename Preconditioner>
      void invoke_solver(
                         LinearOperator const & A
                         , Vector & x
                         , Vector & b
                         , Monitor & monitor
                         , Preconditioner & P
                         , unsigned int const & solver_choice
                         )
      {
        util::Log        logging;
        std::string const newline = util::Log::newline();

        switch (solver_choice)
        {
          case CG_SOLVER:
          {
            cusp::krylov::cg(A, x, b, monitor, P);
          }
            break;
          case CR_SOLVER:
          {
            cusp::krylov::cr(A, x, b, monitor, P);
          }
            break;
          case BICG_SOLVER:
          {
            cusp::krylov::bicg(A, A, x, b, monitor, P, P);
          }
            break;
          case BICGSTAB_SOLVER:
          {
            cusp::krylov::bicgstab(A, x, b, monitor, P);
          }
            break;
          case GMRES_SOLVER:
          {
            unsigned int restart = 10u;  // hardwired constant

            cusp::krylov::gmres(A, x, b, restart, monitor, P);
          }
            break;
          default:
            logging << "Unrecognized solver choice  " << solver_choice << newline;
            break;
        }
      }

    public:

      template<typename LinearOperator, typename Vector, typename Monitor >
      void solve(
                 LinearOperator const & A
                 , Vector & x
                 , Vector & b
                 , Monitor & monitor
                 , unsigned int const & precond_choice
                 , unsigned int const & solver_choice
                 )
      {
        typedef typename LinearOperator::value_type   T;
        typedef typename LinearOperator::index_type   I;
        typedef typename LinearOperator::memory_space M;

        util::Log        logging;
        std::string const newline = util::Log::newline();

        switch (precond_choice)
        {
          case DIAGONAL_PRECONDITIONER:
          {
            cusp::precond::diagonal<T, M> P(A);
            invoke_solver(A, x, b, monitor, P, solver_choice);
          }
            break;
          case IDENTITY_PRECONDITIONER:
          {
            cusp::identity_operator<T, M> P(A.num_rows, A.num_rows);
            invoke_solver(A, x, b, monitor, P, solver_choice);
          }
            break;
          case BRIDSON_AINV_PRECONDITIONER:
          {
            cusp::precond::bridson_ainv<T, M>  P(A, 0, -1, true, 2);
            invoke_solver(A, x, b, monitor, P, solver_choice);
          }
            break;
          default:
            logging << "Unrecognized preconditioner choice  " << precond_choice << newline;
            break;
        }
      }

    };

  }// namespace detail

  /**
   *
   * @tparam M2    The target memory space can be either cusp::host_memory or
   *               cusp::device_memory. This essesially controls whether a GPU
   *               or CPU computation is done.
   */
  template<typename M2,typename T>
  __host__ void solve_equation(
                                      unsigned int n
                                      , unsigned int k
                                      , std::vector<unsigned int> const & A_row_indices
                                      , std::vector<unsigned int> const & A_column_indices
                                      , std::vector<T           > const & A_values
                                      , std::vector<T           >       & x_values
                                      , std::vector<T           > const & b_values
                                      , unsigned int              const & max_iterations
                                      , T                         const & relative_tolerance
                                      , T                         const & absolute_tolerance
                                      , unsigned int              const & preconditioner_choice
                                      , unsigned int              const & solver_choice
                                      , std::vector<T>                  * residuals
                                      )
  {
    util::Log        logging;
    std::string const newline = util::Log::newline();

    //--- This simulator uses CUSP for number crunching. -------------------------
    //---
    //--- Read more about CUSP here
    //--- https://code.google.com/p/cusp-library/wiki/QuickStartGuide
    //---
    typedef unsigned int        I;
    typedef cusp::host_memory   M1;

    //--- Allocate storage for (n,n) matrix with k nonzeros ---------------------
    cusp::coo_matrix<I,  T,  M1>  A1(n,n,k);

    //--- Initialize matrix entries on host -------------------------------------
    A1.row_indices    = A_row_indices;
    A1.column_indices = A_column_indices;
    A1.values         = A_values;

    //--- Allocate space for two n-dimensional vectors and initialize them -------
    cusp::array1d<T, M1> x1(x_values);
    cusp::array1d<T, M1> b1(b_values);

    //--- Convert COO‐>CSR on the host and transfer to the device ----------------
    cusp::csr_matrix<I, T,  M2> A2  =  A1;

    //--- Transfer vectors to device ---------------------------------------------
    cusp::array1d<T, M2> x2 = x1;
    cusp::array1d<T, M2> b2 = b1;

    //--- Solve the linear system A * x = b with the Conjugate Gradient method ---
    cusp::convergence_monitor<T> monitor(b2, min( max_iterations  ,n), relative_tolerance, absolute_tolerance);

    detail::DispatchSolver dispatcher;

    dispatcher.solve(A2, x2, b2, monitor, preconditioner_choice, solver_choice);

    //--- Monitor will report solver progress and results ------------------------
    if (!monitor.converged())
    {
      logging << "Solver reached iteration limit " << monitor.iteration_limit() << " before converging";
      logging << " to " << monitor.tolerance() << " tolerance ";
      logging << " (" << monitor.residual_norm() << " final residual)" << newline;
    }

    //--- Read back solution from device to host ---------------------------------
    x1 = x2;
    thrust::copy( x1.begin(), x1.end(), x_values.begin() );

    if(residuals)
    {
      residuals->resize(monitor.residuals.size());
      thrust::copy( monitor.residuals.begin(), monitor.residuals.end(), residuals->begin() );
    }
  }

  __host__ void solve_equation_host(
                                           unsigned int n
                                           , unsigned int k
                                           , std::vector<unsigned int> const & A_row_indices
                                           , std::vector<unsigned int> const & A_column_indices
                                           , std::vector<float       > const & A_values
                                           , std::vector<float       >       & x_values
                                           , std::vector<float       > const & b_values
                                           , unsigned int              const & max_iterations
                                           , float                     const & relative_tolerance
                                           , float                     const & absolute_tolerance
                                           , unsigned int              const & preconditioner_choice
                                           , unsigned int              const & solver_choice
                                           , std::vector<float>              * residuals
                                           )
  {
    typedef cusp::host_memory   M2;

    solve_equation<M2,float>(
                             n
                             , k
                             , A_row_indices
                             , A_column_indices
                             , A_values
                             , x_values
                             , b_values
                             , max_iterations
                             , relative_tolerance
                             , absolute_tolerance
                             , preconditioner_choice
                             , solver_choice
                             ,residuals
                             );
  }

  __host__ void solve_equation_device(
                                             unsigned int n
                                             , unsigned int k
                                             , std::vector<unsigned int> const & A_row_indices
                                             , std::vector<unsigned int> const & A_column_indices
                                             , std::vector<float       > const & A_values
                                             , std::vector<float       >       & x_values
                                             , std::vector<float       > const & b_values
                                             , unsigned int              const & max_iterations
                                             , float                     const & relative_tolerance
                                             , float                     const & absolute_tolerance
                                             , unsigned int              const & preconditioner_choice
                                             , unsigned int              const & solver_choice
                                             , std::vector<float>              * residuals
                                             )
  {
    typedef cusp::device_memory M2;

    solve_equation<M2,float>(
                             n
                             , k
                             , A_row_indices
                             , A_column_indices
                             , A_values
                             , x_values
                             , b_values
                             , max_iterations
                             , relative_tolerance
                             , absolute_tolerance
                             , preconditioner_choice
                             , solver_choice
                             ,residuals
                             );
  }

  __host__ void solve_equation_host(
                                           unsigned int n
                                           , unsigned int k
                                           , std::vector<unsigned int> const & A_row_indices
                                           , std::vector<unsigned int> const & A_column_indices
                                           , std::vector<double      > const & A_values
                                           , std::vector<double      >       & x_values
                                           , std::vector<double      > const & b_values
                                           , unsigned int              const & max_iterations
                                           , double                    const & relative_tolerance
                                           , double                    const & absolute_tolerance
                                           , unsigned int              const & preconditioner_choice
                                           , unsigned int              const & solver_choice
                                           , std::vector<double>             * residuals
                                           )
  {
    typedef cusp::host_memory   M2;

    solve_equation<M2,double>(
                              n
                              , k
                              , A_row_indices
                              , A_column_indices
                              , A_values
                              , x_values
                              , b_values
                              , max_iterations
                              , relative_tolerance
                              , absolute_tolerance
                              , preconditioner_choice
                              , solver_choice
                              ,residuals
                              );
  }

  __host__ void solve_equation_device(
                                             unsigned int n
                                             , unsigned int k
                                             , std::vector<unsigned int> const & A_row_indices
                                             , std::vector<unsigned int> const & A_column_indices
                                             , std::vector<double      > const & A_values
                                             , std::vector<double      >       & x_values
                                             , std::vector<double      > const & b_values
                                             , unsigned int              const & max_iterations
                                             , double                    const & relative_tolerance
                                             , double                    const & absolute_tolerance
                                             , unsigned int              const & preconditioner_choice
                                             , unsigned int              const & solver_choice
                                             , std::vector<double>             * residuals
                                             )
  {
    typedef cusp::device_memory M2;

    solve_equation<M2,double>(
                              n
                              , k
                              , A_row_indices
                              , A_column_indices
                              , A_values
                              , x_values
                              , b_values
                              , max_iterations
                              , relative_tolerance
                              , absolute_tolerance
                              , preconditioner_choice
                              , solver_choice
                              ,residuals
                              );
  }
  
}//namespace util
