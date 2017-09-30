#ifndef GRIT_MERGE_EDGE_COLLAPSE_QUALITY_MEASURE_H
#define GRIT_MERGE_EDGE_COLLAPSE_QUALITY_MEASURE_H

#include <grit_interface_quality_measure.h>
#include <grit_logic_expressions.h>

namespace grit
{
  namespace details
  {
    template <typename types>
    class MergeEdgeCollapseQualityMeasure
    : public InterfaceQualityMeasure<Simplex1>
    {
    protected:

      typedef typename types::attributes_type        AT;
      typedef typename types::param_type             PT;

      typedef typename types::real_type              T;
      typedef typename types::vector3_type           V;

    protected:

      T                   m_distance;
      InterfaceMesh const & m_mesh;
      AT          const & m_attributes;
      PT          const & m_parameters;

    protected:

      V const & get_coordinates( Simplex0 const & idx) const
      {
        return this->m_attributes.get_current_value( idx );
      }

    public:

      MergeEdgeCollapseQualityMeasure(
                                  std::string     const & operation_name
                                  , unsigned int  const & label
                                  , PT            const & parameters
                                  , InterfaceMesh const & mesh
                                  , AT            const & attributes
                                  )
      : m_distance(parameters.get_distance_threshold(operation_name,label))
      , m_mesh(mesh)
      , m_attributes(attributes)
      , m_parameters(parameters)
      { }

      bool is_bad(Simplex1 const & s) const
      {
        Simplex0 const & v0 = s.get_idx0();
        Simplex0 const & v1 = s.get_idx1();

        if( IsInterface(m_mesh)(s) || !IsInterface(m_mesh)(v0) || !IsInterface(m_mesh)(v1))
          return false;

        V const p0    = get_coordinates( v0 );
        V const p1    = get_coordinates( v1 );
        T const value = util::edge_length(p0, p1);

        return (value < m_distance);
      }

    };

  } //end namespace details
}//end namespace grit


// GRIT_MERGE_EDGE_COLLAPSE_QUALITY_MEASURE_H
#endif
