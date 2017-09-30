#ifndef ELASTICITY_TRACTION_H
#define ELASTICITY_TRACTION_H

#include <glue_tuple.h>

#include <util_distance_fields.h>

#include <vector>

namespace elasticity
{

  class Traction
  {
  public:

    std::vector<glue::Tuple> m_surface;    // Edges on where to apply traction on
    std::vector<double> m_tx;              // Current x-component of traction value on traction edge
    std::vector<double> m_ty;              // Current y-component of traction value on traction edge

  public:

    Traction()
    : m_surface()
    , m_tx()
    , m_ty()
    {}

  };

  inline Traction make_traction(
                                std::vector<glue::Tuple> const & surface
                                , std::vector<double> const & tx
                                , std::vector<double> const & ty
                                )
  {
    Traction info;

    info.m_surface = surface;
    info.m_tx = tx;
    info.m_ty = ty;

    return info;
  }


  template<typename F>
  inline Traction make_traction(
                                util::DistanceFieldExp<F> const & region
                                , std::vector<glue::Tuple> const & edges
                                , std::vector<double> const & x
                                , std::vector<double> const & y
                                , double const & tx
                                , double const & ty
                                )
  {
    Traction info;

    info.m_surface.clear();
    info.m_tx.clear();
    info.m_ty.clear();

    unsigned int const E = edges.size();

    for (unsigned int e = 0u; e < E; ++e)
    {
      glue::Tuple const & edge     = edges[e];
      bool        const   i_inside = region( x[edge.i],  y[edge.i] ) >= 0.0;
      bool        const   j_inside = region( x[edge.j],  y[edge.j] ) >= 0.0;

      if(i_inside && j_inside)
      {
        info.m_surface.push_back(edge);
        info.m_tx.push_back(tx);
        info.m_ty.push_back(ty);
      }
    }
    
    return info;
  }

}// end of namespace elasticity

// ELASTICITY_TRACTION_H
#endif
