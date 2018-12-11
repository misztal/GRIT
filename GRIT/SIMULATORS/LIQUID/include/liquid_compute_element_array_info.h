#ifndef LIQUID_COMPUTE_ELEMENT_ARRAY_INFO_H
#define LIQUID_COMPUTE_ELEMENT_ARRAY_INFO_H

#include <liquid_parameters.h>
#include <liquid_element_array_info.h>

#include <util_tensors.h>
#include <util_is_finite.h>
#include <util_is_number.h>

#include <glue_tuple.h>
#include <glue_triplet.h>

#include <vector>
#include <cmath>       // needed for std::sqrt
#include <cassert>

namespace liquid
{

  inline void compute_element_array_info(
                                         Parameters const & params
                                         , std::vector<glue::Triplet> const & triangles
                                         , std::vector<glue::Triplet> const & adjacent
                                         , std::vector<double> const & px        // Current spatial x-component of nodal positions
                                         , std::vector<double> const & py        // Current spatial  y-component of nodal positions
                                         , std::vector<double> const & vx        // Current x-component of nodal velocited
                                         , std::vector<double> const & vy        // Current y-component of nodal positions
                                         , ElementArrayInfo & info
                                         )
  {
    assert( is_valid(params) || !"compute_element_array_info(): internal error");

    using namespace util;
    using std::sqrt;

    unsigned int const V  = px.size();               // Number of liquid vertices
    unsigned int const E = triangles.size();         // Number of liquid triangles

    assert(V>0u || !"compute_stabilization(): internal error, no liquid?");
    assert(E>0u || !"compute_stabilization(): internal error, no liquid?");


    //--- Extract the tuples that defines the edges of the free surface --------
    std::vector<glue::Tuple>  free_surface;

    for(unsigned int e = 0u; e < E; ++e)
    {
      unsigned int const opposite_i = adjacent[e].i;
      unsigned int const opposite_j = adjacent[e].j;
      unsigned int const opposite_k = adjacent[e].k;

      if (opposite_i == glue::Triplet::UNDEFINED() )
      {
        free_surface.push_back(  glue::make_tuple( triangles[e].j, triangles[e].k ) );
      }

      if (opposite_j == glue::Triplet::UNDEFINED() )
      {
        free_surface.push_back(  glue::make_tuple( triangles[e].k, triangles[e].i ) );
      }

      if (opposite_k == glue::Triplet::UNDEFINED() )
      {
        free_surface.push_back(  glue::make_tuple( triangles[e].i, triangles[e].j ) );
      }
    }

    unsigned int const K = free_surface.size();      // Number of edges of free surface of fluid

    info.resize(V,E,K);

    info.m_free_surface = free_surface;

    // The equations of motion are given as
    //
    // M dot(u) - B f - P p + D u = 0
    //
    // where u is a vector of nodal velocities (both x and y component). The vector
    // f is the external nodal force densities. For gravity node i'th would
    // have f_i = rho*(0, -g)^T. The nodal pressure vector is given by p.
    //
    // M is the mass matrix, D is the diffusion matrix, B is the body force matrix
    // and P is the matrix representations of the gradient operator.
    //

    for(unsigned int e = 0u; e < E; ++e)
    {
      unsigned int const i = triangles[e].i;
      unsigned int const j = triangles[e].j;
      unsigned int const k = triangles[e].k;

      assert(i!=j                          || !"compute_element_array_info() internal error");
      assert(i!=k                          || !"compute_element_array_info() internal error");
      assert(j!=k                          || !"compute_element_array_info() internal error");
      assert(i!=glue::Triplet::UNDEFINED() || !"compute_element_array_info() internal error");
      assert(j!=glue::Triplet::UNDEFINED() || !"compute_element_array_info() internal error");
      assert(k!=glue::Triplet::UNDEFINED() || !"compute_element_array_info() internal error");

      double const pxi = px[i];
      double const pxj = px[j];
      double const pxk = px[k];

      double const pyi = py[i];
      double const pyj = py[j];
      double const pyk = py[k];

      double const ex_i = (pxk-pxj);
      double const ex_j = (pxi-pxk);
      double const ex_k = (pxj-pxi);

      double const ey_i = (pyk-pyj);
      double const ey_j = (pyi-pyk);
      double const ey_k = (pyj-pyi);

      double const Ae   = (ex_k*ey_i - ey_k*ex_i)/2.0;

      assert(is_finite(Ae) || !"compute_element_array_info() not finite number");
      assert(is_number(Ae) || !"compute_element_array_info() not a number");
      assert(Ae > 0.0      || !"compute_element_array_info() non positive");

      info.m_A[e]       = Ae;

      // Assuming rho to be constant and that we have linear shape functions we can
      // by change of variables rewrite the mass matrix block computation into
      //
      //   M_{ij}^e = int_A^e rho N_i N_j dA I_{2x2}
      //            =  2 A^e rho int_{0}^{1} int_{0}^{1-v} L_i L_j du dv
      //
      // where the isoparametric linear shape functions are given by
      //
      //   L_i = 1-v-u
      //   L_j = u
      //   L_k = v
      //
      //  This can now be solved analytically for a closed form soltuion

      /*

       % Integrating shape functions in isoparametric frame

       clear all;
       close all;
       clc;

       syms x y real

       Li = 1 - u - v
       Lj = u
       Lk = v

       L = [ int(int(Li*Li,v,0,1-u),u, 0, 1), int(int(Li*Lj,v,0,1-u),u, 0, 1), int(int(Li*Lk,v,0,1-u),u, 0, 1);
       int(int(Lj*Li,v,0,1-u),u, 0, 1), int(int(Lj*Lj,v,0,1-u),u, 0, 1), int(int(Lj*Lk,v,0,1-u),u, 0, 1);
       int(int(Lk*Li,v,0,1-u),u, 0, 1), int(int(Lk*Lj,v,0,1-u),u, 0, 1), int(int(Lk*Lk,v,0,1-u),u, 0, 1)]

       */
      util::Block3x3Tensor2 L;
      L.m_block[0][0] = mul(2.0, identity() );
      L.m_block[0][1] = identity();
      L.m_block[0][2] = identity();
      L.m_block[1][0] = identity();
      L.m_block[1][1] = mul(2.0, identity() );
      L.m_block[1][2] = identity();
      L.m_block[2][0] = identity();
      L.m_block[2][1] = identity();
      L.m_block[2][2] = mul(2.0, identity() );

      // Resulting in the element mass block matrix
      //
      //                       | 2 I_2x2     I_2x2     I_2x2 |
      //   M^e =  (rho A^e/12) |   I_2x2   2 I_2x2     I_2x2 |
      //                       |   I_2x2     I_2x2   2 I_2x2 |
      //
      info.m_Me[e] = mul( params.rho() * Ae/12.0, L );

      // Observe that from this we immidiately also find
      //
      //                   | 2 I_2x2     I_2x2     I_2x2 |
      //   B^e =  (A^e/12) |   I_2x2   2 I_2x2     I_2x2 |
      //                   |   I_2x2     I_2x2   2 I_2x2 |
      //
      info.m_Be[e] = mul( Ae/12.0, L );

      // Next we have the element wise diffusion matrix given by
      //
      //   D_{ij}^e  = int_A^e  mu ( nabla N_i^T nabla N_j I_2x2 + nabla N_i nabla N_j^T  ) dA
      //
      // The gradient of the shape functions can be found from geoemtry to be
      //
      //      nabla N_j = - hat(x_i - x_k)/ (2.0 A^e)
      //
      //
      // here x_k and x_i are the nodal positions (both x and y component) of node
      // k and i respectively and hat is the hat operator. By cyclic permuation we
      //  find
      //
      //      nabla N_k = - hat(x_j - x_i)/ (2.0 A^e)
      //      nabla N_i = - hat(x_k - x_j)/ (2.0 A^e)
      //
      double const hat_ex_i = -ey_i;
      double const hat_ey_i =  ex_i;
      double const hat_ex_j = -ey_j;
      double const hat_ey_j =  ex_j;
      double const hat_ex_k = -ey_k;
      double const hat_ey_k =  ex_k;

      double const factor = 1.0 / (2.0*Ae);

      Tensor1 const nabla_N_i = mul( factor, make(hat_ex_i, hat_ey_i) );
      Tensor1 const nabla_N_j = mul( factor, make(hat_ex_j, hat_ey_j) );
      Tensor1 const nabla_N_k = mul( factor, make(hat_ex_k, hat_ey_k) );

      assert(is_valid(nabla_N_i) || !"compute_element_array_info() nabla_N_i invalid");
      assert(is_valid(nabla_N_j) || !"compute_element_array_info() nabla_N_i invalid");
      assert(is_valid(nabla_N_k) || !"compute_element_array_info() nabla_N_i invalid");

      Tensor2 const D00 = mul(Ae * params.mu(), add(mul(inner_prod(nabla_N_i, nabla_N_i), identity()), outer_prod(nabla_N_i, nabla_N_i)));
      Tensor2 const D01 = mul(Ae * params.mu(), add(mul(inner_prod(nabla_N_i, nabla_N_j), identity()), outer_prod(nabla_N_i, nabla_N_j)));
      Tensor2 const D02 = mul(Ae * params.mu(), add(mul(inner_prod(nabla_N_i, nabla_N_k), identity()), outer_prod(nabla_N_i, nabla_N_k)));
      Tensor2 const D10 = mul(Ae * params.mu(), add(mul(inner_prod(nabla_N_j, nabla_N_i), identity()), outer_prod(nabla_N_j, nabla_N_i)));
      Tensor2 const D11 = mul(Ae * params.mu(), add(mul(inner_prod(nabla_N_j, nabla_N_j), identity()), outer_prod(nabla_N_j, nabla_N_j)));
      Tensor2 const D12 = mul(Ae * params.mu(), add(mul(inner_prod(nabla_N_j, nabla_N_k), identity()), outer_prod(nabla_N_j, nabla_N_k)));
      Tensor2 const D20 = mul(Ae * params.mu(), add(mul(inner_prod(nabla_N_k, nabla_N_i), identity()), outer_prod(nabla_N_k, nabla_N_i)));
      Tensor2 const D21 = mul(Ae * params.mu(), add(mul(inner_prod(nabla_N_k, nabla_N_j), identity()), outer_prod(nabla_N_k, nabla_N_j)));
      Tensor2 const D22 = mul(Ae * params.mu(), add(mul(inner_prod(nabla_N_k, nabla_N_k), identity()), outer_prod(nabla_N_k, nabla_N_k)));

      assert( is_valid(D00)  || !"compute_element_array_info() D00 invalid");
      assert( is_valid(D01)  || !"compute_element_array_info() D01 invalid");
      assert( is_valid(D02)  || !"compute_element_array_info() D02 invalid");
      assert( is_valid(D10)  || !"compute_element_array_info() D10 invalid");
      assert( is_valid(D11)  || !"compute_element_array_info() D11 invalid");
      assert( is_valid(D12)  || !"compute_element_array_info() D12 invalid");
      assert( is_valid(D20)  || !"compute_element_array_info() D20 invalid");
      assert( is_valid(D21)  || !"compute_element_array_info() D21 invalid");
      assert( is_valid(D22)  || !"compute_element_array_info() D22 invalid");

      info.m_De[e].m_block[0][0] = D00;
      info.m_De[e].m_block[0][1] = D01;
      info.m_De[e].m_block[0][2] = D02;
      info.m_De[e].m_block[1][0] = D10;
      info.m_De[e].m_block[1][1] = D11;
      info.m_De[e].m_block[1][2] = D12;
      info.m_De[e].m_block[2][0] = D20;
      info.m_De[e].m_block[2][1] = D21;
      info.m_De[e].m_block[2][2] = D22;

      // The pressure gradient operator is discretized by
      //
      //    P_i^e  = int_A^e  nabla N_i dA = A^e nabla N_i
      //
      Tensor1 const P_ie = mul(Ae, nabla_N_i);
      Tensor1 const P_je = mul(Ae, nabla_N_j);
      Tensor1 const P_ke = mul(Ae, nabla_N_k);

      assert( is_valid(P_ie) || !"compute_element_array_info() P_ie invalid" );
      assert( is_valid(P_je) || !"compute_element_array_info() P_je invalid" );
      assert( is_valid(P_ke) || !"compute_element_array_info() P_ke invalid" );

      info.m_Pe[e].m_block[0] = P_ie;
      info.m_Pe[e].m_block[1] = P_je;
      info.m_Pe[e].m_block[2] = P_ke;
    }

    for(unsigned int k = 0u; k < K; ++k)
    {
      unsigned int const i = free_surface[k].i;
      unsigned int const j = free_surface[k].j;

      assert(i!=j                          || !"compute_element_array_info() internal error");
      assert(i!=glue::Triplet::UNDEFINED() || !"compute_element_array_info() internal error");
      assert(j!=glue::Triplet::UNDEFINED() || !"compute_element_array_info() internal error");

      double const px_i    = px[i];
      double const px_j    = px[j];
      double const py_i    = py[i];
      double const py_j    = py[j];
      double const ex_ij   = px_i - px_j;
      double const ey_ij   = py_i - py_j;
      double const l       = sqrt(ex_ij*ex_ij + ey_ij*ey_ij);

      assert(is_finite(l) || !"compute_element_array_info() not finite number");
      assert(is_number(l) || !"compute_element_array_info() not a number"     );
      assert(l > 0.0      || !"compute_element_array_info() non positive"     );

      double const nabla_ex_i = ex_ij / l;
      double const nabla_ey_i = ey_ij / l;
      double const nabla_ex_j = - nabla_ex_i;
      double const nabla_ey_j = - nabla_ey_i;

      double const fx_ki = - nabla_ex_i * params.gamma() * params.dt();
      double const fy_ki = - nabla_ey_i * params.gamma() * params.dt();
      double const fx_kj = - nabla_ex_j * params.gamma() * params.dt();
      double const fy_kj = - nabla_ey_j * params.gamma() * params.dt();

      assert(is_finite(fx_ki) || !"compute_element_array_info() not finite number");
      assert(is_number(fx_ki) || !"compute_element_array_info() not a number"     );
      assert(is_finite(fy_ki) || !"compute_element_array_info() not finite number");
      assert(is_number(fy_ki) || !"compute_element_array_info() not a number"     );
      assert(is_finite(fx_kj) || !"compute_element_array_info() not finite number");
      assert(is_number(fx_kj) || !"compute_element_array_info() not a number"     );
      assert(is_finite(fy_kj) || !"compute_element_array_info() not finite number");
      assert(is_number(fy_kj) || !"compute_element_array_info() not a number"     );

      info.m_Fs[k].m_block[0u] = make( fx_ki, fy_ki );
      info.m_Fs[k].m_block[1u] = make( fx_kj, fy_kj );
    }

    for(unsigned int v = 0u; v < V; ++v)
    {
      info.m_bx[v] = 0.0;
      info.m_by[v] = - params.g();
    }

  }

}// namespace liquid

// LIQUID_COMPUTE_ELEMENT_ARRAY_INFO_H
#endif
