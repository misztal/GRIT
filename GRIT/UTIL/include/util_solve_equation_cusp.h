#ifndef UTIL_SOLVE_EQUATION_CUSP_H
#define UTIL_SOLVE_EQUATION_CUSP_H

#include <vector>

namespace util
{

#ifdef CUDA_FOUND

  void solve_equation_host(
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
                           );

  void solve_equation_device(
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
                             );

  void solve_equation_host(
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
                           );

  void solve_equation_device(
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
                             );

#endif

}//namespace util

//UTIL_SOLVE_EQUATION_CUSP_H
#endif
