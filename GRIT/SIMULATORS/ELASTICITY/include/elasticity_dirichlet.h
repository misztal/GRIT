#ifndef ELASTICITY_DIRICHLET_H
#define ELASTICITY_DIRICHLET_H

#include <glue_triplet.h>

#include <util_distance_fields.h>

#include <vector>

namespace elasticity
{
  class Dirichlet
  {
  public:

    std::vector<unsigned int> m_nodes;  // Nodes where Dirichlet conditions are applied
    std::vector<double>       m_x;      // current x-component of Dirichlet positions
    std::vector<double>       m_y;      // current y-component of Dirichlet positions

  public:

    Dirichlet()
    : m_nodes()
    , m_x()
    , m_y()
    {}

  };

  inline Dirichlet make_dirichlet(
                                  std::vector<unsigned int> const & nodes
                                  , std::vector<double> const & x
                                  , std::vector<double> const & y
                                  )
  {
    Dirichlet info;

    info.m_nodes = nodes;
    info.m_x = x;
    info.m_y = y;

    return info;
  }

  template<typename F>
  inline Dirichlet make_dirichlet(
                                  util::DistanceFieldExp<F> const & region
                                  , std::vector<glue::Triplet> const & triangles
                                  , std::vector<double> const & x
                                  , std::vector<double> const & y
                                  )
  {
    Dirichlet info;

    info.m_nodes.clear();
    info.m_x.clear();
    info.m_y.clear();

    unsigned int const N = x.size();
    for(unsigned int i = 0u; i < N;++i)
    {
      if(region(x[i],y[i]) >= 0.0)
      {
        info.m_nodes.push_back(i);
        info.m_x.push_back(x[i]);
        info.m_y.push_back(y[i]);
      }
    }

    return info;
  }

  inline void apply_dirichlet(
                              Dirichlet const & info
                              , std::vector<double> & px
                              , std::vector<double> & py
                              , std::vector<double> & vx
                              , std::vector<double> & vy
                              )
  {
    unsigned int const D  = info.m_nodes.size();

    for(unsigned int d = 0u; d < D; ++d)
    {
      unsigned int const v = info.m_nodes[d];

      vx[v] = 0.0;
      vy[v] = 0.0;
      px[v] = info.m_x[d];
      py[v] = info.m_y[d];
    }
  }


  inline bool test_dirichlet(
                             Dirichlet const & info
                             , std::vector<double> const & px
                             , std::vector<double> const & py
                             , std::vector<double> const & vx
                             , std::vector<double> const & vy
                             )
  {
    using std::fabs;

    unsigned int const D  = info.m_nodes.size();

    for(unsigned int d = 0u; d < D; ++d)
    {
      unsigned int const v = info.m_nodes[d];

      if( fabs(vx[v] - 0.0) > 0.0 )
        return false;
      
      if( fabs(vy[v] - 0.0) > 0.0 )
        return false;
      
      if( fabs(px[v] - info.m_x[d]) > 0.0 )
        return false;
      
      if( fabs(py[v] - info.m_y[d]) > 0.0 )
        return false;
    }
    
    return true;
  }
  
}// end of namespace elasticity

// ELASTICITY_DIRICHLET_H
#endif
