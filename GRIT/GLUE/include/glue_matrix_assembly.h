#ifndef GLUE_MATRIX_ASSEMBLY_H
#define GLUE_MATRIX_ASSEMBLY_H

#include <util_log.h>
#include <util_is_finite.h>
#include <util_is_number.h>
#include <util_coo_matrix.h>
#include <util_tensors.h>

#include <glue_triplet.h>
#include <glue_phase.h>
#include <glue_compute_neighbors.h>

#include <cassert>
#include <vector>
#include <map>

namespace glue
{

  /**
   * This kind of matrix assembly is typical for finite element methods (FEM). One
   * example of its usage would be in the discretization of a PDE involving a 2 dimensional
   * unknown field, u \in \Re^2.
   *
   * OBSERVE: This version of the function assumes that the unknown u-fiels
   * is discretized on the vertices of the mesh. Hence, one have a x and a y
   * value of u stored in each vertex.
   *
   * Using the FEM discretization process results to an elementwise discretization of
   * the PDE on the element (being the triangle, T,  made of i,j,k vertices)
   *
   *    PDE(u) on T ~   \mat E  [ ux_i uy_i x_j uy_j x_k uy_k  ]^T where \mat E \in \Re^{6 \times 6}
   *
   * \mat E can be described by a blocked format consisting of 3x3 grid made out of 2-by-2 mattrices
   * (Encoded as Tensor2's instances). That is
   *
   *       E_ii  E_ij E_ik
   *  E =  E_ji  E_jj E_jk
   *       E_ki  E_kj E_kk
   *
   * Where E_nm \in \Re^{2 \times 2} for all n,m \in \{ i, j, k\}. Observe
   * that i,j,k are the global index values, locally these corresponds to 0,1 and 2.
   *
   *
   *
   * @param triangles              An array of triangle indices
   * @param V                      The total number of vertices
   * @param Ae                     An array of E's, one for each triangle.
   * @param interlaced             Boolean flag that determines the memory layout.
   *                               If interlaced then data is stored as
   *                               x0 y0 x1 y1 x2 y2 ... xN yN otherwise it is
   *                               stored in a staggered layput as
   *                               x0 x1 x2 .... xN y0 y1 y2 ... yN
   * @param  guard_against_zeros   If this argument is set to true (default value if omitted)
   *                               then the method guards against user trying to insert
   *                               zeros. In that case if a zero is inserted then the method
   *                               just bails out. If set to false then the method allows
   *                               the end-user to insert a zero.
   * @param verbose                If set to true then full details is written to the log. If
   *                               set to false (default value) then only error information is
   *                               written to log.
   *
   * @return                    The resulting assembled matrix.
   */
  template <typename T>
  inline util::COOMatrix<T> matrix_assembly(
                                            std::vector<Triplet> const & triangles
                                            , unsigned int const & V
                                            , std::vector<util::Block3x3Tensor2> const & Ae
                                            , bool const & interlaced = true
                                            , bool const & guard_against_zeros = true
                                            , bool const & verbose = false
                                            )
  {
    util::COOMatrix<T> A;

    unsigned int const N = triangles.size();

    glue::Neighbors neighbors;
    glue::compute_neighbors(triangles, V, neighbors);

    unsigned int const nvars       = 2u*V;
    unsigned int const row_fill_in = (N * 9u * 2u);

    A.init(nvars,nvars,row_fill_in);

    for (unsigned int r = 0u; r < V; ++r)
    {
      unsigned int const start = neighbors.m_offset[r];
      unsigned int const end   = neighbors.m_offset[r + 1u];

      std::map <unsigned int, util::Tensor2> data_values;

      for (unsigned int e = start; e<end; ++e)
      {
        unsigned int const idx = neighbors.m_V2T[e].second;

        glue::Triplet         const & triangle = triangles[idx];
        util::Block3x3Tensor2 const & E        = Ae[idx];

        unsigned int const local_r   = glue::get_local_index(triangle, r);
        unsigned int const local_i   = 0u;
        unsigned int const local_j   = 1u;
        unsigned int const local_k   = 2u;

        unsigned int const global_i  = triangle.i;
        unsigned int const global_j  = triangle.j;
        unsigned int const global_k  = triangle.k;

        data_values[global_i] = util::add( data_values[global_i], E.m_block[local_r][local_i] );
        data_values[global_j] = util::add( data_values[global_j], E.m_block[local_r][local_j] );
        data_values[global_k] = util::add( data_values[global_k], E.m_block[local_r][local_k] );
      }

      std::map <unsigned int, util::Tensor2>::const_iterator data = data_values.begin();
      for(; data != data_values.end(); ++data )
      {
        unsigned int const c          = data->first;

        unsigned int const global_rx  = interlaced ? 2*r  + 0u : r + 0u;
        unsigned int const global_ry  = interlaced ? 2*r  + 1u : r + V;
        unsigned int const global_cx  = interlaced ? 2*c  + 0u : c + 0u;
        unsigned int const global_cy  = interlaced ? 2*c  + 1u : c + V;

        T const value_xx  = data->second.m_00;
        T const value_xy  = data->second.m_01;
        T const value_yx  = data->second.m_10;
        T const value_yy  = data->second.m_11;

        A.insert( global_rx, global_cx, value_xx, guard_against_zeros, verbose );
        A.insert( global_rx, global_cy, value_xy, guard_against_zeros, verbose );
        A.insert( global_ry, global_cx, value_yx, guard_against_zeros, verbose );
        A.insert( global_ry, global_cy, value_yy, guard_against_zeros, verbose );
      }

    }
    
    return A;
  }

  /**
   * Overloaded version that makes it more convenient to program using
   * the glue::Phase structure if so desried.
   */
  template <typename T>
  inline util::COOMatrix<T> matrix_assembly(
                                            glue::Phase const & phase
                                            , std::vector<util::Block3x3Tensor2> const & Ae
                                            , bool const & interlaced = true
                                            , bool const & guard_against_zeros = true
                                            , bool const & verbose = false
                                            )
  {
    return matrix_assembly<T>(
                              phase.m_triangles
                              , phase.m_vertices.size()
                              , Ae
                              , interlaced
                              , guard_against_zeros
                              , verbose
                              );
  }

  /**
   * This overloaded version is similar to the Block3x3Tensor2 element
   * version, except that here the unknown field u \in \Re and hence only
   * one single scalar value is stored in each vertex of the mesh.
   */
  template <typename T>
  inline util::COOMatrix<T> matrix_assembly(
                                            std::vector<Triplet> const & triangles
                                            , unsigned int const & V
                                            , std::vector<util::Block3x3Tensor0> const & Ae
                                            , bool const & guard_against_zeros = true
                                            , bool const & verbose = false
                                            )
  {
    util::COOMatrix<T> A;

    unsigned int const N = triangles.size();

    glue::Neighbors neighbors;
    glue::compute_neighbors(triangles, V, neighbors);

    unsigned int const nvars       = V;
    unsigned int const row_fill_in = (N * 9u);

    A.init(nvars,nvars,row_fill_in);

    for (unsigned int r = 0u; r < V; ++r)
    {
      unsigned int const start = neighbors.m_offset[r];
      unsigned int const end   = neighbors.m_offset[r + 1u];

      std::map <unsigned int, util::Tensor0> data_values;

      for (unsigned int e = start; e<end; ++e)
      {
        unsigned int const idx = neighbors.m_V2T[e].second;

        glue::Triplet         const & triangle = triangles[idx];
        util::Block3x3Tensor0 const & E        = Ae[idx];

        unsigned int const local_r   = glue::get_local_index(triangle, r);
        unsigned int const local_i   = 0u;
        unsigned int const local_j   = 1u;
        unsigned int const local_k   = 2u;

        unsigned int const global_i  = triangle.i;
        unsigned int const global_j  = triangle.j;
        unsigned int const global_k  = triangle.k;

        data_values[global_i] = data_values[global_i] + E.m_block[local_r][local_i];
        data_values[global_j] = data_values[global_j] + E.m_block[local_r][local_j];
        data_values[global_k] = data_values[global_k] + E.m_block[local_r][local_k];
      }

      std::map <unsigned int, util::Tensor0>::const_iterator data = data_values.begin();
      for(; data != data_values.end(); ++data )
      {
        unsigned int const c          = data->first;

        T const value  = data->second;

        A.insert( r, c, value, guard_against_zeros, verbose );
      }
      
    }
    
    return A;
  }

  /**
   * Overloaded version that makes it more convenient to program using
   * the glue::Phase structure if so desried.
   */
  template <typename T>
  inline util::COOMatrix<T> matrix_assembly(
                                            glue::Phase const & phase
                                            , std::vector<util::Block3x3Tensor0> const & Ae
                                            , bool const & guard_against_zeros = true
                                            , bool const & verbose = false
                                            )
  {
    return matrix_assembly<T>(
                              phase.m_triangles
                              , phase.m_vertices.size()
                              , Ae
                              , guard_against_zeros
                              , verbose
                              );
  }


  /**
   * This overloaded version is special as it assumes the unknown scalar field
   * is discrtized on the cell centers (per triangle rather than per vertex).
   * This is typical for pressure forces in fluid simulations where the PDE term
   * could be written as
   *
   *   nabla p  on triangle T ~ \mat E p_t  where E \in \Re^{6 \times 1}.
   *
   *  \mat E can be described as made of blocks E_i, E_j and E_k \in Re^2 such that
   *
   *       E_i
   *  E =  E_j
   *       E_k
   *
   * Observe the argument Ae stores E differnetly as a contigous block of E_i, E_j and E_k.
   *
   */
  template <typename T>
  inline util::COOMatrix<T> matrix_assembly(
                                            std::vector<Triplet> const & triangles
                                            , unsigned int const & V
                                            , std::vector<util::Block3x1Tensor1> const & Ae
                                            , bool const & interlaced = true
                                            , bool const & guard_against_zeros = true
                                            , bool const & verbose = false
                                            )
  {
    util::COOMatrix<T> A;

    unsigned int const N = triangles.size();

    glue::Neighbors neighbors;
    glue::compute_neighbors(triangles, V, neighbors);

    unsigned int const nvars       = N;
    unsigned int const row_fill_in = (N * 9u);

    A.init(2u*V, nvars,row_fill_in);

    for (unsigned int r = 0u; r < V; ++r)
    {
      unsigned int const start = neighbors.m_offset[r];
      unsigned int const end   = neighbors.m_offset[r + 1u];

      std::map <unsigned int, util::Tensor1> data_values;

      for (unsigned int e = start; e<end; ++e)
      {
        unsigned int const idx = neighbors.m_V2T[e].second;

        glue::Triplet         const & triangle = triangles[idx];
        util::Block3x1Tensor1 const & E        = Ae[idx];

        unsigned int const local_r   = glue::get_local_index(triangle, r);

        data_values[idx] = E.m_block[local_r];
      }

      std::map <unsigned int, util::Tensor1>::const_iterator data = data_values.begin();
      for(; data != data_values.end(); ++data )
      {
        unsigned int const c          = data->first;

        util::Tensor1 const value  = data->second;

        unsigned int const rx  = interlaced ? 2*r  + 0u : r + 0u;
        unsigned int const ry  = interlaced ? 2*r  + 1u : r + V;

        A.insert( rx, c, value.m_0, guard_against_zeros, verbose );
        A.insert( ry, c, value.m_1, guard_against_zeros, verbose );
      }
      
    }
    
    return A;
  }

  /**
   * Overloaded version that makes it more convenient to program using
   * the glue::Phase structure if so desried.
   */
  template <typename T>
  inline util::COOMatrix<T> matrix_assembly(
                                            glue::Phase const & phase
                                            , std::vector<util::Block3x1Tensor1> const & Ae
                                            , bool const & interlaced = true
                                            , bool const & guard_against_zeros = true
                                            , bool const & verbose = false
                                            )
  {
    return matrix_assembly<T>(
                              phase.m_triangles
                              , phase.m_vertices.size()
                              , Ae, interlaced
                              , guard_against_zeros
                              , verbose
                              );
  }

}//namespace glue

// GLUE_MATRIX_ASSEMBLY_H
#endif
