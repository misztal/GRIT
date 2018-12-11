#ifndef LIQUID_ELEMENT_ARRAY_INFO_H
#define LIQUID_ELEMENT_ARRAY_INFO_H

#include <util_tensors.h>
#include <glue_tuple.h>

#include <vector>

namespace liquid
{

  /**
   *
   * Observe that the spatial coordinate memory layout for local element
   * consisting of local nodes 0, 1, 2 (mapping to global i, j, k)  is
   * given by
   *
   *    x_i
   *    y_i
   *    x_j
   *    y_j
   *    x_k
   *    y_k
   *
   * The pressure field is assumed colocated and the pressure memory layout
   * is given by
   *
   *    p_i
   *    p_j
   *    p_k
   *
   */
  class ElementArrayInfo
  {
  public:

    unsigned int                       m_V;     ///< Number of vertices/nodes
    unsigned int                       m_E;     ///< Number of triangles/elements
    unsigned int                       m_K;     ///< Number of edges/faces of the free surface

    std::vector<double>                m_A;     ///< Triangle area in spactial space
    std::vector<util::Block3x3Tensor2> m_Me;    ///< Element block mass matrix
    std::vector<util::Block3x3Tensor2> m_Be;    ///< Element block body forces
    std::vector<util::Block3x3Tensor2> m_De;    ///< Element block diffusion matrix
    std::vector<util::Block3x1Tensor1> m_Pe;    ///< Element block pressure gradients
    std::vector<util::Block2x1Tensor1> m_Fs;    ///< Surface tension forces

    std::vector<double>        m_bx;    ///< x-component of body force densities (per node)
    std::vector<double>        m_by;    ///< x-component of body force densities (per node)

    std::vector<glue::Tuple>   m_free_surface;  ///< The tuples that make up the free surface.

  public:

    ElementArrayInfo()
    : m_V(0u)
    , m_E(0u)
    , m_K(0u)
    , m_A()
    , m_Me()
    , m_Be()
    , m_De()
    , m_Pe()
    , m_Fs()
    , m_bx()
    , m_by()
    {}

  public:

    void clear()
    {
      m_V = 0u;
      m_E = 0u;
      m_K = 0u;
      m_A.clear();
      m_Me.clear();
      m_Be.clear();
      m_De.clear();
      m_Pe.clear();
      m_Fs.clear();
      m_bx.clear();
      m_by.clear();
    }

    void resize(unsigned int const & V, unsigned int const & E, unsigned int const & K)
    {
      m_V  = V;
      m_E  = E;
      m_K  = K;

      m_A.resize(E);
      m_Me.resize(E);
      m_Be.resize(E);
      m_De.resize(E);
      m_Pe.resize(E);
      m_Fs.resize(K);
      m_bx.resize(V);
      m_by.resize(V);
      m_free_surface.resize(K);
    }

  };

}// namespace liquid

// LIQUID_ELEMENT_ARRAY_INFO_H
#endif
