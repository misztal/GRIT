#ifndef MAGNETISM_DIPOLE_FIELD_H
#define MAGNETISM_DIPOLE_FIELD_H

namespace magnetism
{
  
    class DipoleField
    {
    public:
      
      /**
       * Evaluate Dipole Field at given Position.
       *
       * @param px   The x-coordinate of the evlauation point.
       * @param py   The y-coordinate of the evlauation point.
       * @param mx   The x-coordinate of the dipole field at (px,py).
       * @param my   The y-coordinate of the dipole field at (px,py).
       *
       */
      virtual void operator()(double const & px, double const & py, double & mx, double & my ) const = 0;
      
    };
  
} // namespace magnetism

// MAGNETISM_DIPOLE_FIELD_H
#endif
