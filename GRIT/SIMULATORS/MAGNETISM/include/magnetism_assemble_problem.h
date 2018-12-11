#ifndef MAGNETISM_ASSEMBLE_PROBLEM_H
#define MAGNETISM_ASSEMBLE_PROBLEM_H

#include <magnetism_find_dirichlet.h>
#include <magnetism_compute_triangle_centers.h>

#include <util_coo_matrix.h>
#include <util_is_finite.h>
#include <util_is_number.h>

#include <cmath>
#include <vector>
#include <cassert>

namespace magnetism
{

  namespace detail
  {

    inline int count(bool v1, bool v2, bool v3)
    {
      if(v1 && v2 && v3)
        return 3;

      if(v1 && v2)
        return 2;
      if(v1 && v3)
        return 2;
      if(v2 && v3)
        return 2;

      if(v1)
        return 1;
      if(v2)
        return 1;
      if(v3)
        return 1;

      return 0;
    }

  }// namespace detail


  template<typename T>
  inline void assemble_problem(
                               T                            const & Mx
                               , T                          const & My
                               , std::vector<unsigned int>  const & labels
                               , unsigned int               const & magnet_label
                               , std::vector<glue::Triplet> const & triangles
                               , std::vector<glue::Triplet> const & adjacent
                               , bool                       const & order_2nd
                               , std::vector<T>             const & px
                               , std::vector<T>             const & py
                               , util::COOMatrix<T>               & A
                               , std::vector<T>                   & b
                               )
  {
    using std::sqrt;
    using std::fabs;

    unsigned int const N = triangles.size();   // Number of triangles

    std::vector<T> cx;
    std::vector<T> cy;
    compute_triangle_centers( triangles, px, py, cx, cy);

    unsigned int const dirichlet = find_dirichlet(triangles, px, py);

    A.clear();
    A.init( N );
    b.resize(N);

    for (unsigned int r = 0u; r<N; ++r)
    {
      if(r == dirichlet)
      {
        A.insert(r,r,1.0);
        b[r] = 0.0;
        continue;
      }

      glue::Triplet const triangle = triangles[r];
      glue::Triplet const neighbor = adjacent[r];

      unsigned int const i = neighbor.i;
      unsigned int const j = neighbor.j;
      unsigned int const k = neighbor.k;

      bool const i_is_boundary = (i == glue::Triplet::UNDEFINED());
      bool const j_is_boundary = (j == glue::Triplet::UNDEFINED());
      bool const k_is_boundary = (k == glue::Triplet::UNDEFINED());

      assert( detail::count(i_is_boundary,j_is_boundary,k_is_boundary)<= 1 || !"assemble_problem(): Ear not allowed");

      //--- Get triangle corner points
      T const x_0 = px[triangle.i];
      T const y_0 = py[triangle.i];
      T const x_1 = px[triangle.j];
      T const y_1 = py[triangle.j];
      T const x_2 = px[triangle.k];
      T const y_2 = py[triangle.k];

      //--- Compute triangle edge lengths
      T const l_10 = sqrt( (x_1-x_0)*(x_1-x_0) + (y_1-y_0)*(y_1-y_0) );
      T const l_21 = sqrt( (x_2-x_1)*(x_2-x_1) + (y_2-y_1)*(y_2-y_1) );
      T const l_02 = sqrt( (x_0-x_2)*(x_0-x_2) + (y_0-y_2)*(y_0-y_2) );

      assert(is_finite(l_10) || !"assemble_problem() l_10 not finite");
      assert(is_finite(l_21) || !"assemble_problem() l_21 not finite");
      assert(is_finite(l_02) || !"assemble_problem() l_02 not finite");

      assert(is_number(l_10) || !"assemble_problem() l_10 not a number");
      assert(is_number(l_21) || !"assemble_problem() l_21 not a number");
      assert(is_number(l_02) || !"assemble_problem() l_02 not a number");

      assert(l_10 > 0.0 || !"assemble_problem() l_10 was non positive");
      assert(l_21 > 0.0 || !"assemble_problem() l_21 was non positive");
      assert(l_02 > 0.0 || !"assemble_problem() l_02 was non positive");

      //--- Compute outward unit normals of triangle edges
      //---
      //---  n_i =   - hat( x_k - x_j ) / length(x_k-x_j)
      //---
      T const nx_k = - (y_1 - y_0) / l_10;
      T const ny_k =   (x_1 - x_0) / l_10;
      T const nx_i = - (y_2 - y_1) / l_21;
      T const ny_i =   (x_2 - x_1) / l_21;
      T const nx_j = - (y_0 - y_2) / l_02;
      T const ny_j =   (x_0 - x_2) / l_02;

      assert(is_finite(nx_k) || !"assemble_problem() nx_k not finite"  );
      assert(is_number(nx_k) || !"assemble_problem() nx_k not a number");
      assert(is_finite(ny_k) || !"assemble_problem() nx_k not finite"  );
      assert(is_number(ny_k) || !"assemble_problem() nx_k not a number");

      assert(is_finite(nx_i) || !"assemble_problem() nx_i not finite"  );
      assert(is_number(nx_i) || !"assemble_problem() nx_i not a number");
      assert(is_finite(ny_i) || !"assemble_problem() nx_i not finite"  );
      assert(is_number(ny_i) || !"assemble_problem() nx_i not a number");

      assert(is_finite(nx_j) || !"assemble_problem() nx_j not finite"  );
      assert(is_number(nx_j) || !"assemble_problem() nx_j not a number");
      assert(is_finite(ny_j) || !"assemble_problem() nx_j not finite"  );
      assert(is_number(ny_j) || !"assemble_problem() nx_j not a number");

      //--- Get triangle mid-points of all neighbors and this triangle ---------
      //--- if no triangle exist then use edge mid point               ---------
      T const cx_r = cx[r];
      T const cy_r = cy[r];
      T const cx_i = i_is_boundary ? (cx[j]+cx[k])*0.5 : cx[i];
      T const cy_i = i_is_boundary ? (cy[j]+cy[k])*0.5 : cy[i];
      T const cx_j = j_is_boundary ? (cx[i]+cx[k])*0.5 : cx[j];
      T const cy_j = j_is_boundary ? (cy[i]+cy[k])*0.5 : cy[j];
      T const cx_k = k_is_boundary ? (cx[i]+cx[j])*0.5 : cx[k];
      T const cy_k = k_is_boundary ? (cy[i]+cy[j])*0.5 : cy[k];

      //--- Compute distances between neighboring face centers and this face
      T const l_ri = sqrt( (cx_i-cx_r)*(cx_i-cx_r) + (cy_i-cy_r)*(cy_i-cy_r) );
      T const l_rj = sqrt( (cx_j-cx_r)*(cx_j-cx_r) + (cy_j-cy_r)*(cy_j-cy_r) );
      T const l_rk = sqrt( (cx_k-cx_r)*(cx_k-cx_r) + (cy_k-cy_r)*(cy_k-cy_r) );

      assert(is_finite(l_ri) || !"assemble_problem() l_ri not finite");
      assert(is_finite(l_rj) || !"assemble_problem() l_rj not finite");
      assert(is_finite(l_rk) || !"assemble_problem() l_rk not finite");

      assert(is_number(l_ri) || !"assemble_problem() l_ri not a number");
      assert(is_number(l_rj) || !"assemble_problem() l_rj not a number");
      assert(is_number(l_rk) || !"assemble_problem() l_rk not a number");

      assert(l_ri > 0.0 || !"assemble_problem() l_ri was negative");
      assert(l_rj > 0.0 || !"assemble_problem() l_rj was negative");
      assert(l_rk > 0.0 || !"assemble_problem() l_rk was negative");

      //--- Compute the terms making up coefficient for A-matrix
      T const a_i  =  (!order_2nd && i_is_boundary) ? 0.0 : (l_21 / l_ri);
      T const a_j  =  (!order_2nd && j_is_boundary) ? 0.0 : (l_02 / l_rj);
      T const a_k  =  (!order_2nd && k_is_boundary) ? 0.0 : (l_10 / l_rk);

      assert(is_finite(a_i) || !"assemble_problem() a_i not finite");
      assert(is_finite(a_j) || !"assemble_problem() a_j not finite");
      assert(is_finite(a_k) || !"assemble_problem() a_k not finite");

      assert(is_number(a_i) || !"assemble_problem() a_i not a number");
      assert(is_number(a_j) || !"assemble_problem() a_j not a number");
      assert(is_number(a_k) || !"assemble_problem() a_k not a number");

      //--- Compute the A-matrix coefficients (ignoring boundary conditions)
      T const A_rr = - a_i - a_j - a_k;
      T const A_ri = a_i;
      T const A_rj = a_j;
      T const A_rk = a_k;

      //--- Compute effects of boundary conditions if any
      T mod_rr = 0.0;
      T mod_ri = 0.0;
      T mod_rj = 0.0;
      T mod_rk = 0.0;

      if(order_2nd)
      {
        //--- Modify coefficients of A-matrix to take boundary conditions into account

        T const nk_cross_ni  = nx_k * ny_i  -  ny_k * nx_i;
        T const nj_cross_nk  = nx_j * ny_k  -  ny_j * nx_k;
        T const ni_cross_nj  = nx_i * ny_j  -  ny_i * nx_j;

        assert(is_finite(nk_cross_ni) || !"assemble_problem() nk_cross_ni not finite");
        assert(is_finite(nj_cross_nk) || !"assemble_problem() nj_cross_nk not finite");
        assert(is_finite(ni_cross_nj) || !"assemble_problem() ni_cross_nj not finite");

        assert(is_number(nk_cross_ni) || !"assemble_problem() nk_cross_ni not a number");
        assert(is_number(nj_cross_nk) || !"assemble_problem() nj_cross_nk not a number");
        assert(is_number(ni_cross_nj) || !"assemble_problem() ni_cross_nj not a number");

        assert(fabs(nk_cross_ni)>0.0 || !"assemble_problem() nk_cross_ni was zero");
        assert(fabs(nj_cross_nk)>0.0 || !"assemble_problem() nj_cross_nk was zero");
        assert(fabs(ni_cross_nj)>0.0 || !"assemble_problem() ni_cross_nj was zero");

        if(i_is_boundary)
        {
          T const B_ij = - ( nk_cross_ni / nj_cross_nk ) * ( l_ri / l_rj );
          T const B_ik = - ( ni_cross_nj / nj_cross_nk ) * ( l_ri / l_rk );
          T const B_ir = 1.0 - B_ij - B_ik;

          mod_rr =  A_ri*B_ir;
          mod_ri = -A_ri;
          mod_rj =  A_ri*B_ij;
          mod_rk =  A_ri*B_ik;
        }

        if(j_is_boundary)
        {
          T const B_jk = - ( ni_cross_nj / nk_cross_ni) * ( l_rj / l_rk );
          T const B_ji = - ( nj_cross_nk / nk_cross_ni) * ( l_rj / l_ri );
          T const B_jr = 1.0 - B_jk - B_ji;

          mod_rr =  A_rj*B_jr;
          mod_ri =  A_rj*B_ji;
          mod_rj = -A_rj;
          mod_rk =  A_rj*B_jk;
        }

        if(k_is_boundary)
        {
          T const B_ki = - ( nj_cross_nk / ni_cross_nj ) * ( l_rk/  l_ri );
          T const B_kj = - ( nk_cross_ni / ni_cross_nj ) * ( l_rk / l_rj );
          T const B_kr = 1.0 - B_ki - B_kj;

          mod_rr =  A_rk*B_kr;
          mod_ri =  A_rk*B_ki;
          mod_rj =  A_rk*B_kj;
          mod_rk = -A_rk;
        }

        assert(is_finite(mod_rr) || !"assemble_problem() mod_rr not finite"  );
        assert(is_number(mod_rr) || !"assemble_problem() mod_rr not a number");
        assert(is_finite(mod_ri) || !"assemble_problem() mod_ri not finite"  );
        assert(is_number(mod_ri) || !"assemble_problem() mod_ri not a number");
        assert(is_finite(mod_rj) || !"assemble_problem() mod_rj not finite"  );
        assert(is_number(mod_rj) || !"assemble_problem() mod_rj not a number");
        assert(is_finite(mod_rk) || !"assemble_problem() mod_rk not finite"  );
        assert(is_number(mod_rk) || !"assemble_problem() mod_rk not a number");
      }

      T const Amod_rr = A_rr + mod_rr;
      T const Amod_ri = A_ri + mod_ri;
      T const Amod_rj = A_rj + mod_rj;
      T const Amod_rk = A_rk + mod_rk;

      //--- Compute terms makring up the b-vector, find sides with magnetization on
      //--- Assuming magnet is a closed point set and that phase "magnet_label" is the magnet.
      bool on_i = i_is_boundary ? false : labels[r] != labels[i] || labels[r]==magnet_label;
      bool on_j = j_is_boundary ? false : labels[r] != labels[j] || labels[r]==magnet_label;
      bool on_k = k_is_boundary ? false : labels[r] != labels[k] || labels[r]==magnet_label;


      //dipole_field_functors[phase_id].get_mx(midpoint_ix, iy)


      T const b_i  =  on_i ? ( nx_i*Mx +  ny_i*My )*l_21 : 0.0;
      T const b_j  =  on_j ? ( nx_j*Mx +  ny_j*My )*l_02 : 0.0;
      T const b_k  =  on_k ? ( nx_k*Mx +  ny_k*My )*l_10 : 0.0;

      //--- Assemble the terms into b-vector
      T const b_r  = b_i + b_j + b_k;

      if (fabs(Amod_rr) > 0.0)
      {
        A.insert(r,r,Amod_rr);
      }
      if (fabs(Amod_ri) > 0.0 && i!=dirichlet)
      {
        A.insert(r,i,Amod_ri);
      }
      if (fabs(Amod_rj) > 0.0 && j!=dirichlet)
      {
        A.insert(r,j,Amod_rj);
      }
      if (fabs(Amod_rk) > 0.0 && k!=dirichlet)
      {
        A.insert(r,k,Amod_rk);
      }

      b[r] = b_r;
    }
  }

}//namespace magnetism

// MAGNETISM_ASSEMBLE_PROBLEM_H
#endif
