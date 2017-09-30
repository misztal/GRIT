#ifndef UTIL_SOLVE_EQUATION_CHOICES_H
#define UTIL_SOLVE_EQUATION_CHOICES_H

namespace util
{

  /**
   * Solver choices
   */
  unsigned int const CG_SOLVER       = 0u;
  unsigned int const CR_SOLVER       = 1u;
  unsigned int const BICG_SOLVER     = 2u;
  unsigned int const BICGSTAB_SOLVER = 3u;
  unsigned int const GMRES_SOLVER    = 4u;

  /**
   * Preconditioner choices
   */
  unsigned int const DIAGONAL_PRECONDITIONER     = 0u;
  unsigned int const IDENTITY_PRECONDITIONER     = 1u;
  unsigned int const BRIDSON_AINV_PRECONDITIONER = 2u;

}//namespace util

//UTIL_SOLVE_EQUATION_CHOICES_H
#endif
