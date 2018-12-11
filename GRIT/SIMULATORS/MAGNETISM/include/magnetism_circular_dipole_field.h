#ifndef MAGNETISM_CIRCULAR_DIPOLE_FIELD_H
#define MAGNETISM_CIRCULAR_DIPOLE_FIELD_H

#include <magnetism_dipole_field.h>

#include <cmath> // Needed for std::sqrt

namespace magnetism
{
  class CircularDipoleField
  : public DipoleField
  {
  protected:
    
    double m_mx;                 // x-component of magnetisation
    double m_my;                 // y-component of magnetisation
    
    double m_cx;                 // x-component of center point
    double m_cy;                 // y-component of center point
    
  public:

    CircularDipoleField()
    : m_mx(0.0)
    , m_my(-1.0)
    , m_cx(0.0)
    , m_cy(0.0)
    {}

    CircularDipoleField(
                        double const & mx
                        , double const & my
                        , double const & cx
                        , double const & cy
                        )
    : m_mx(mx)
    , m_my(my)
    , m_cx(cx)
    , m_cy(cy)
    {}
    
  public:
    
    void operator()(double const & px, double const & py, double & mx, double & my) const
    {
      using std::sqrt;
      
      double const rx = px - this->m_cx;
      double const ry = py - this->m_cy;
      
      double const r = sqrt(rx*rx + ry*ry);
      
      double const cos_theta = rx / r;
      double const sin_theta = ry / r;
      
      //double const theta = atan2(sin_theta, cos_theta);
      
      mx = cos_theta * this->m_mx + sin_theta * this->m_my;
      my = -sin_theta * this->m_mx + cos_theta * this->m_my;
    }
    
  };
} // namespace magnetism

// MAGNETISM_CIRCULAR_DIPOLE_FIELD_H
#endif
