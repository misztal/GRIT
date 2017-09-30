#ifndef GRIT_NO_EARS_QUALITY_MEASURE_H
#define GRIT_NO_EARS_QUALITY_MEASURE_H

#include <grit_interface_quality_measure.h>
#include <grit_logic_expressions.h>

namespace grit
{
  namespace details
  {
    template< typename types >
    class NoEarsQualityMeasure
    : public InterfaceQualityMeasure<Simplex1>
    {
    protected:

      typedef typename types::attributes_type  AT;
      typedef typename types::param_type       PT;

    protected:

      InterfaceMesh const & m_mesh;

    public:

      NoEarsQualityMeasure(
                           std::string     const & name
                           , unsigned int  const & label
                           , PT            const & parameters
                           , InterfaceMesh const & mesh
                           , AT            const & attributes
                           )
      : m_mesh(mesh)
      {}

      bool is_bad(Simplex1 const & s) const
      {
        if( !IsValid(m_mesh)(s) || IsBoundary(m_mesh)(s) || IsInterface(m_mesh)(s))
        {
          return false;
        }

        SimplexSet const S = m_mesh.star(s);

        SimplexSet::simplex2_const_iterator it = S.begin2();

        for( ; it != S.end2(); ++it )
        {
          Simplex2 const & t = *it;

          SimplexSet const dS = m_mesh.boundary(t);

          SimplexSet const B  = filter( dS, IsDimension(m_mesh, 1u) && IsBoundary( m_mesh));
          SimplexSet const I  = filter( dS, IsDimension(m_mesh, 1u) && IsInterface(m_mesh));

          if( B.size(1u) > 1u ) return true;
          if( I.size(1u) > 1u ) return true;
        }

        return false;
      }

    };

  } // namespace details
} // namespace grit

// GRIT_NO_EARS_QUALITY_MEASURE_H
#endif
