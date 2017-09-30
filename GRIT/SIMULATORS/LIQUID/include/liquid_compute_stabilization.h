#ifndef LIQUID_COMPUTE_STABILIZATION_H
#define LIQUID_COMPUTE_STABILIZATION_H

#include <liquid_parameters.h>

#include <util_coo_matrix.h>
#include <util_is_finite.h>
#include <util_is_number.h>

#include <glue_triplet.h>

#include <vector>
#include <cmath>       // needed for std::sqrt and std::fabs
#include <cassert>

namespace liquid
{

  inline util::COOMatrix<double> compute_stabilization(
                                         Parameters const & params
                                         , std::vector<glue::Triplet> const & triangles
                                         , std::vector<glue::Triplet> const & adjacent
                                         , std::vector<double> const & px        // Current spatial x-component of nodal positions
                                         , std::vector<double> const & py        // Current spatial y-component of nodal positions
                                         )
  {
    assert( is_valid(params) || !"compute_stabilization(): internal error");

    using namespace util;
    using std::sqrt;

    unsigned int const E = triangles.size();         // Number of liquid triangles

    assert(E>0u || !"compute_stabilization(): internal error, no liquid?");

    util::COOMatrix<double> S;

    S.init(E,E,4);

    //--- Compute triangle centers ---------------------------------------------
    std::vector<double> cx;      // x-component of triangle centers
    std::vector<double> cy;      // y-component of triangle centers

    cx.resize(E, 0.0);
    cy.resize(E, 0.0);

    std::fill( cx.begin(), cx.end(), 0.0 );
    std::fill( cy.begin(), cy.end(), 0.0 );

    for (unsigned int e = 0u; e<E; ++e)
    {
      glue::Triplet const triangle = triangles[e];
      cx[e] = (px[triangle.i] + px[triangle.j] + px[triangle.k])/3.0;
      cy[e] = (py[triangle.i] + py[triangle.j] + py[triangle.k])/3.0;
    }

    double const factor = params.epsilon() / params.rho();

    assert(is_finite(factor) || !"compute_stabilization() factor not finite");
    assert(is_number(factor) || !"compute_stabilization() factor not a number");
    assert(factor > 0.0      || !"compute_stabilization() factor was non positive");

    for(unsigned int e = 0u; e < E; ++e)
    {
      // Stabilization term is given by
      //
      //  nabla u - epsilon/rho nabla^2 p = 0
      //
      // Let triangle/elements e be given by nodes 0,1,2 and opposite edges/elements by i,j,k
      //
      //    By Gauss Divergence theorem
      //
      //      int_V (epsilon/rho) nabla^2 p dV = epsilon int_S (1/rho) nabla p dot n dS
      //
      //    by piece-wise constant normals and contant rho over element
      //
      //      int_V (epsilon/rho) nabla^2 p dV = epsilon  sum_{m in i,j,k} int_S_m (1/rho_e) nabla p dot n_m dS
      //
      //    by mid-point rule approximiation
      //
      //                                       = epsilon  sum_{m in i,j,k} (1/rho_e) [nabla p dot n_m ]_m l_m
      //
      // Observe that if m is not in liquid then [nabla p dot n_m] = 0 (no diffusion (stabilization)
      // across boundaries to different phases. Hence boundary conditions are easy dealth with
      // by simply ignoring non-liquid faces of the element.
      //
      // From Taylor approximation we have
      //
      //  (p_m - p_e) / l_me \approx  nabla p dot n_m
      //
      // The value of l_me can be found from geometric considerations.
      //
      //        *
      //       / \
      //      /   \
      //     /  m  \
      //    /   d_m  \
      //   *---------*     d_m = (2/3) A_m /l_m
      //    \   d_e /      d_e = (2/3) A_e /l_m
      //     \  e  /
      //      \   /        l_me  = d_m + d_e
      //       \ /
      //        *
      //
      // Putting it all together we find
      //
      //      ...  = epsilon  sum_{m in I,J,K} (3/2) (1/rho_e) (p_m - p_e) l_m^2 / (A_m + A_e)
      //
      // As shorthand we define the symbol
      //
      //
      //      s_em =  epsilon (3/2) (1/rho_e) l_m^2 / (A_m + A_e)   if m and e are neighbors and of same phase
      //               0                                            otherwise
      //
      // The final discretized stabilization of the divergence free condition is
      //
      //    P u - S p = 0
      //
      // where the e´th row of S is given by
      //
      //           [S]_ee = - s_ei - s_ej - s_ek
      //           [S]_ei =   s_ei
      //           [S]_ej =   s_ej
      //           [S]_ek =   s_ek
      //           [S]_en =   0 for all n notin {e,i,j,k}
      //
      // Notice that for a triangle e not on the boundary the e'th row of S will have found non-zero values.

      glue::Triplet const triangle = triangles[e];
      glue::Triplet const neighbor = adjacent[e];

      unsigned int const i = neighbor.i;
      unsigned int const j = neighbor.j;
      unsigned int const k = neighbor.k;

      bool const i_is_boundary = (i == glue::Triplet::UNDEFINED());
      bool const j_is_boundary = (j == glue::Triplet::UNDEFINED());
      bool const k_is_boundary = (k == glue::Triplet::UNDEFINED());

      //--- Get triangle corner points -----------------------------------------
      double const x_0 = px[triangle.i];
      double const y_0 = py[triangle.i];
      double const x_1 = px[triangle.j];
      double const y_1 = py[triangle.j];
      double const x_2 = px[triangle.k];
      double const y_2 = py[triangle.k];

      //--- Compute triangle edge lengths --------------------------------------
      double const l_10 = sqrt( (x_1-x_0)*(x_1-x_0) + (y_1-y_0)*(y_1-y_0) );
      double const l_21 = sqrt( (x_2-x_1)*(x_2-x_1) + (y_2-y_1)*(y_2-y_1) );
      double const l_02 = sqrt( (x_0-x_2)*(x_0-x_2) + (y_0-y_2)*(y_0-y_2) );

      assert(is_finite(l_10) || !"compute_stabilization() l_10 not finite");
      assert(is_finite(l_21) || !"compute_stabilization() l_21 not finite");
      assert(is_finite(l_02) || !"compute_stabilization() l_02 not finite");

      assert(is_number(l_10) || !"compute_stabilization() l_10 not a number");
      assert(is_number(l_21) || !"compute_stabilization() l_21 not a number");
      assert(is_number(l_02) || !"compute_stabilization() l_02 not a number");

      assert(l_10 > 0.0 || !"compute_stabilization() l_10 was non positive");
      assert(l_21 > 0.0 || !"compute_stabilization() l_21 was non positive");
      assert(l_02 > 0.0 || !"compute_stabilization() l_02 was non positive");

      //--- Compute outward unit normals of triangle edges
      //---
      //---  n_i =   - hat( x_k - x_j ) / length(x_k-x_j)
      //---
      double const nx_k = - (y_1 - y_0) / l_10;
      double const ny_k =   (x_1 - x_0) / l_10;
      double const nx_i = - (y_2 - y_1) / l_21;
      double const ny_i =   (x_2 - x_1) / l_21;
      double const nx_j = - (y_0 - y_2) / l_02;
      double const ny_j =   (x_0 - x_2) / l_02;

      assert(is_finite(nx_k) || !"compute_stabilization() nx_k not finite"  );
      assert(is_number(nx_k) || !"compute_stabilization() nx_k not a number");
      assert(is_finite(ny_k) || !"compute_stabilization() nx_k not finite"  );
      assert(is_number(ny_k) || !"compute_stabilization() nx_k not a number");

      assert(is_finite(nx_i) || !"compute_stabilization() nx_i not finite"  );
      assert(is_number(nx_i) || !"compute_stabilization() nx_i not a number");
      assert(is_finite(ny_i) || !"compute_stabilization() nx_i not finite"  );
      assert(is_number(ny_i) || !"compute_stabilization() nx_i not a number");

      assert(is_finite(nx_j) || !"compute_stabilization() nx_j not finite"  );
      assert(is_number(nx_j) || !"compute_stabilization() nx_j not a number");
      assert(is_finite(ny_j) || !"compute_stabilization() nx_j not finite"  );
      assert(is_number(ny_j) || !"compute_stabilization() nx_j not a number");

      //--- Get triangle mid-points of all neighbors and this triangle ---------
      double const cx_e = cx[e];
      double const cy_e = cy[e];
      double const cx_i = i_is_boundary ? 0.0 : cx[i];
      double const cy_i = i_is_boundary ? 0.0 : cy[i];
      double const cx_j = j_is_boundary ? 0.0 : cx[j];
      double const cy_j = j_is_boundary ? 0.0 : cy[j];
      double const cx_k = k_is_boundary ? 0.0 : cx[k];
      double const cy_k = k_is_boundary ? 0.0 : cy[k];

      //--- Compute distances between neighboring face centers and this face
      double const l_ei = sqrt( (cx_i-cx_e)*(cx_i-cx_e) + (cy_i-cy_e)*(cy_i-cy_e) );
      double const l_ej = sqrt( (cx_j-cx_e)*(cx_j-cx_e) + (cy_j-cy_e)*(cy_j-cy_e) );
      double const l_ek = sqrt( (cx_k-cx_e)*(cx_k-cx_e) + (cy_k-cy_e)*(cy_k-cy_e) );

      assert(is_finite(l_ei) || !"compute_stabilization() l_ei not finite");
      assert(is_finite(l_ej) || !"compute_stabilization() l_ej not finite");
      assert(is_finite(l_ek) || !"compute_stabilization() l_ek not finite");

      assert(is_number(l_ei) || !"compute_stabilization() l_ei not a number");
      assert(is_number(l_ej) || !"compute_stabilization() l_ej not a number");
      assert(is_number(l_ek) || !"compute_stabilization() l_ek not a number");

      assert(l_ei > 0.0 || !"compute_stabilization() l_ei was negative");
      assert(l_ej > 0.0 || !"compute_stabilization() l_ej was negative");
      assert(l_ek > 0.0 || !"compute_stabilization() l_ek was negative");

      //--- Compute the terms making up coefficient for A-matrix
      double const a_i  =  (i_is_boundary) ? 0.0 : factor*(l_21 / l_ei);
      double const a_j  =  (j_is_boundary) ? 0.0 : factor*(l_02 / l_ej);
      double const a_k  =  (k_is_boundary) ? 0.0 : factor*(l_10 / l_ek);

      assert(is_finite(a_i) || !"compute_stabilization() a_i not finite");
      assert(is_finite(a_j) || !"compute_stabilization() a_j not finite");
      assert(is_finite(a_k) || !"compute_stabilization() a_k not finite");

      assert(is_number(a_i) || !"compute_stabilization() a_i not a number");
      assert(is_number(a_j) || !"compute_stabilization() a_j not a number");
      assert(is_number(a_k) || !"compute_stabilization() a_k not a number");

      //--- Compute the A-matrix coefficients (ignoring boundary conditions)
      double const s_ee = - a_i - a_j - a_k;
      double const s_ei = a_i;
      double const s_ej = a_j;
      double const s_ek = a_k;

      if (fabs(s_ee) > 0.0)
      {
        S.insert(e,e,s_ee);
      }
      if (fabs(s_ei) > 0.0)
      {
        S.insert(e,i,s_ei);
      }

      if (fabs(s_ej) > 0.0)
      {
        S.insert(e,j,s_ej);
      }

      if (fabs(s_ek) > 0.0)
      {
        S.insert(e,k,s_ek);
      }
    }

    return S;
  }

}// namespace liquid

// LIQUID_COMPUTE_STABILIZATION_H
#endif
