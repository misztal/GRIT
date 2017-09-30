#ifndef MAGNETISM_CONSTANT_DIPOLE_FIELD_H
#define MAGNETISM_CONSTANT_DIPOLE_FIELD_H

#include <magnetism_dipole_field.h>

namespace magnetism
{
  /**
   * The Constant Dipole Field will always return the
   * same magnetisation regardless of the position within
   * a material.
   */
  class ConstantDipoleField
  : public DipoleField
  {
  protected:
    
    double m_mx;                 // x-component of magnetisation
    double m_my;                 // y-component of magnetisation
    
  public:
    
    
    ConstantDipoleField()
    : m_mx(0.0)
    , m_my(-1.0)
    {}

    ConstantDipoleField(double const & mx, double const & my)
    : m_mx(mx)
    , m_my(my)
    {}
    
  public:
    
    void operator()(double const & px, double const & py, double & mx, double & my) const
    {
      mx = this->m_mx;
      my = this->m_my;
    }
    
  };
} // namespace magnetism

// MAGNETISM_CONSTANT_DIPOLE_FIELD_H
#endif
