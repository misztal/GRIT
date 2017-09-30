#ifndef ELASTICITY_COMPUTE_POSITION_UPDATE_H
#define ELASTICITY_COMPUTE_POSITION_UPDATE_H

#include <util_is_finite.h>

#include <vector>

namespace elasticity
{

  inline void compute_position_update(
                                      double              const & dt
                                      , std::vector<double> const & px
                                      , std::vector<double> const & py
                                      , std::vector<double> const & vx
                                      , std::vector<double> const & vy
                                      , std::vector<double>       & px_new
                                      , std::vector<double>       & py_new
                                      )
  {
    unsigned int const V = px.size();

    for(unsigned int v = 0u; v < V; ++v)
    {
      assert( is_finite(vx[v]) || !"compute_position_update(): vx[v] was not finite");
      assert( is_finite(vy[v]) || !"compute_position_update(): vy[v] was not finite");

      px_new[v] = px[v] + dt*vx[v];
      py_new[v] = py[v] + dt*vy[v];

      assert( is_finite(px_new[v]) || !"compute_position_update(): px_new[v] was not finite");
      assert( is_finite(py_new[v]) || !"compute_position_update(): py_new[v] was not finite");
    }
    
  }
  

}// end of namespace elasticity

// ELASTICITY_COMPUTE_POSITION_UPDATE_H
#endif
