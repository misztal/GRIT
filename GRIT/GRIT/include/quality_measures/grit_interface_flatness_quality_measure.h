#ifndef GRIT_INTERFACE_FLATNESS_QUALITY_MESAURE_H
#define GRIT_INTERFACE_FLATNESS_QUALITY_MESAURE_H

#include <grit_interface_quality_measure.h>
#include <grit_interface_mesh.h>

#include <grit_logic_expressions.h>
#include <grit_simplex.h>
#include <grit_simplex_set.h>
#include <grit_simplex_set_functions.h>

#include <utilities/grit_extract_simplices.h>

#include <util_angle.h>

#include <string>

namespace grit
{
  namespace details
  {
    template< typename types>
    class InterfaceFlatnessQualityMeasure
      : public InterfaceQualityMeasure<Simplex0>
    {
    protected:

      typedef typename types::attributes_type  AT;
      typedef typename types::param_type       PT;
      typedef typename types::vector3_type     V;
      typedef typename types::real_type        T;
      typedef typename types::math_types        MT;

    protected:

      InterfaceMesh const & m_mesh;

      AT            const & m_attributes;

      T             const   m_angle_threshold;

    public:

      InterfaceFlatnessQualityMeasure(
                                      std::string     const & operation_name
                                      , unsigned int  const & label
                                      , PT            const & parameters
                                      , InterfaceMesh const & mesh
                                      , AT            const & attributes
                                      )
      : m_mesh(mesh)
      , m_attributes(attributes)
      , m_angle_threshold(MT::deg_to_rad(parameters.get_angle_threshold(operation_name,label)))
      {}

      bool is_bad(Simplex0 const & s) const
      {
        if( IsSubmeshBoundary(m_mesh)(s) || !IsInterface(m_mesh)(s) || IsBoundary(m_mesh)(s))
          return false;

        V const & v = m_attributes.get_current_value(s);

        //--- interface edges adjacent to s
        SimplexSet const S = filter( m_mesh.star(s), IsDimension( m_mesh, 1u) && IsInterface(m_mesh));

        //--- neighboring interface vertices
        SimplexSet       C = filter( m_mesh.closure(S), IsDimension( m_mesh, 0u));

        C.remove(s);

        if (C.size(0u) != 2u)
        {
          return false;
        }

        //--- checking how flat is the interface around s
        Simplex0 p,q;
        extract_simplices(p,q,C);

        V const & u  = m_attributes.get_current_value(p);
        V const & w  = m_attributes.get_current_value(q);

        if (util::angle(u,v,w) > m_angle_threshold)
        {
          return true;
        }

        return false;
      }

   };

  } //end namespace details
}//end namespace grit

// GRIT_INTERFACE_FLATNESS_QUALITY_MESAURE_H
#endif

