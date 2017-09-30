#ifndef ELASTICITY_OBJECT_H
#define ELASTICITY_OBJECT_H

#include <elasticity_traction.h>
#include <elasticity_dirichlet.h>

#include <glue_phase.h>

#include <vector>
#include <cassert>
#include <map>

namespace elasticity
{

  class Object
  : public glue::Phase
  {
  public:

    std::vector<double> m_px0;         ///< x-coordinate of material coordinates
    std::vector<double> m_py0;         ///< y-coordinate of material coordinates
    std::vector<double> m_px;          ///< x-coordinate of spatial coordinates
    std::vector<double> m_py;          ///< y-coordinate of spatial coordinates
    std::vector<double> m_vx;          ///< x-coordinate of spatial velocities
    std::vector<double> m_vy;          ///< y-coordinate of spatial velocities
    std::vector<double> m_px_new;      ///< Updated x-coordinate of spatial coordinates
    std::vector<double> m_py_new;      ///< Updated y-coordinate of spatial coordinates
    std::vector<double> m_vx_new;      ///< Updated x-coordinate of spatial velocities
    std::vector<double> m_vy_new;      ///< Updated y-coordinate of spatial velocities

    Traction            m_traction;    ///< Traction conditions
    Dirichlet           m_dirichlet;   ///< Dirichlet conditions

  public:

    Object()
    : glue::Phase()
    {}

    Object(Object const & obj)
    {
      *this = obj;
    }

    Object(glue::Phase const & phase)
    : glue::Phase(phase)
    {}

    virtual ~Object(){}

    Object const & operator=(Object const & obj)
    {
      if(this != &obj)
      {
        glue::Phase::operator=(obj);  // Call base class operator?

        this->m_px0 = obj.m_px0;
        this->m_py0 = obj.m_py0;
        this->m_px = obj.m_px;
        this->m_py = obj.m_py;
        this->m_vx = obj.m_vx;
        this->m_vy = obj.m_vy;

        this->m_px_new = obj.m_px_new;
        this->m_py_new = obj.m_py_new;
        this->m_vx_new = obj.m_vx_new;
        this->m_vy_new = obj.m_vy_new;
        
        this->m_traction  = obj.m_traction;
        this->m_dirichlet = obj.m_dirichlet;
      }
      return *this;
    }
    
  };

}// end of namespace elasticity

// ELASTICITY_OBJECT_H
#endif
