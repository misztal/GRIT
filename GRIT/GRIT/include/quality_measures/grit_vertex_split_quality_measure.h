#ifndef GRIT_VERTEX_SPLIT_QUALITY_MEASURE_H
#define GRIT_VERTEX_SPLIT_QUALITY_MEASURE_H

#include <grit_interface_quality_measure.h>

#include <grit_logic_expressions.h>

namespace grit
{
  namespace details
  {
    template <typename types>
    class VertexSplitQualityMeasure
    : public InterfaceQualityMeasure<Simplex0>
    {
    protected:

      typedef typename types::attributes_type  AMT;
      typedef typename types::param_type       PT;
      typedef typename types::math_types       MT;
      typedef typename types::real_type        T;
      typedef typename types::vector3_type     V;

      typedef typename V::value_traits         VT;

    protected:

      InterfaceMesh const & m_mesh;
      AMT           const & m_attributes;
      PT            const & m_parameters;

      unsigned int  const & m_label;

      T             const   m_distance_threshold; ///< the maximum distance between two target positions imposed by two different phases
                                                  ///< for which they're considered to be moving "as one"

    protected:

      V const & get_coordinates( Simplex0 const & s) const
      {
        return this->m_attributes.current()[ s ];
      }

    public:

      VertexSplitQualityMeasure(
                                std::string    const & operation_name
                                , unsigned int const & label
                                , PT           const & parameters
                                , InterfaceMesh      const & mesh
                                , AMT          const & attributes
                                )
      : m_mesh(mesh)
      , m_attributes(attributes)
      , m_parameters(parameters)
      , m_label(label)
      , m_distance_threshold(parameters.get_distance_threshold( operation_name, label))
      {}

      bool is_bad(Simplex0 const & s) const
      {
        if( IsSubmeshBoundary(m_mesh)(s) || !IsInterface(m_mesh)(s))
          return false;

        // 2016-01-17 Marek: for now I assume that we fill in the "void" with ambient
        //                   labeled triangles, if ambient is undefined, vertex split does
        //                   not take place. In the future we should think further about it,
        //                   in multi-physics simulation we might want objects touching and
        //                   separating while embedded in a different phase.
        if( !(m_parameters.use_ambient()) || (m_label==m_parameters.ambient_label()))
          return false;

        std::vector<unsigned int> const & labels = m_attributes.get_simplex0_labels(s);

        //--- only one phase assigned (other phase is ambient)
        if( labels.size() < 2u)
          return false;

        // 2016-01-17 Marek: current solution: if the target positions of the vertex imposed
        //                   by different phases are closer than given threshold, the operation
        //                   is not performed. Otherwise vertex split is performed. Note! if
        //                   the prescribed motion yields interpenetration, the result might not
        //                   be what it is expected.
        std::vector<unsigned int>::const_iterator label_it = labels.begin();

        //--- the target position associated with this label.
        V const & target0 = m_attributes.get_target_value( s, m_label);

        for( ; label_it != labels.end(); ++label_it)
        {
          unsigned int const & label = *label_it;

          if( label != m_label)
          {
            V const & target1 = m_attributes.get_target_value( s, label);

            if( MT::length(target1-target0) > m_distance_threshold)
              return true;
          }
        }

        return false;
      }
    }; // class VertexSplitQualityMeasure

  } // namespace details

} // namespace grit

// GRIT_VERTEX_SPLIT_QUALITY_MEASURE_H
#endif
