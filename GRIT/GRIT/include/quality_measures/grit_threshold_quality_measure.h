#ifndef GRIT_THRESHOLD_QUALITY_MESAURE_H
#define GRIT_THRESHOLD_QUALITY_MESAURE_H

#include <grit_interface_quality_measure.h>
#include <grit_interface_mesh.h>

#include <util_edge_length.h>
#include <util_triangle_area.h>
#include <util_maxmin_edge_ratio.h>
#include <util_area_edge_ratio.h>

#include <string>

namespace grit
{
  namespace details
  {
    template <typename types, typename S>
    class ThresholdQualityMeasureBase
      : public InterfaceQualityMeasure<S>
    {
    protected:

      typedef typename types::attributes_type        AT;
      typedef typename types::param_type             PT;

      typedef typename types::real_type              T;
      typedef typename types::vector3_type           V;

    protected:

      T                     m_lower;
      T                     m_upper;
      PT            const & m_parameters;
      InterfaceMesh const & m_mesh;
      AT            const & m_attributes;
      std::string           m_lower_threshold_attribute;
      std::string           m_upper_threshold_attribute;

    protected:

      bool in_inside_interval(
                              T   const & value
                              , S const & simplex
                              ) const
      {
        bool above_lower = (value>m_lower);

        if( m_lower_threshold_attribute.compare("") != 0)
        {
          if( m_attributes.exist_attribute_value( m_lower_threshold_attribute, simplex))
          {
            above_lower = (value > m_attributes.get_attribute_value( m_lower_threshold_attribute, simplex) );
          }
        }

        bool below_upper = (value<m_upper);

        if( m_upper_threshold_attribute.compare("") != 0)
        {
          if( m_attributes.exist_attribute_value( m_upper_threshold_attribute, simplex))
          {
            below_upper = (value < m_attributes.get_attribute_value( m_upper_threshold_attribute, simplex) );
          }
        }

        return (above_lower && below_upper);
      }

    protected:

      V const & get_coordinates( Simplex0 const & idx) const
      {
        return this->m_attributes.get_current_value( idx );
      }

    public:

      ThresholdQualityMeasureBase(
                                  std::string     const & operation_name
                                  , unsigned int  const & label
                                  , PT            const & parameters
                                  , InterfaceMesh const & mesh
                                  , AT            const & attributes
                                  )
      : m_lower(parameters.get_lower_threshold(operation_name,label))
      , m_upper(parameters.get_upper_threshold(operation_name,label))
      , m_parameters(parameters)
      , m_mesh(mesh)
      , m_attributes(attributes)
      {
        if( m_parameters.has_lower_threshold_attribute( operation_name ) )
        {
          m_lower_threshold_attribute = m_parameters.get_lower_threshold_attribute( operation_name );
        }
        else
        {
          m_lower_threshold_attribute = "";
        }

        if( m_parameters.has_upper_threshold_attribute( operation_name) )
        {
          m_upper_threshold_attribute = m_parameters.get_upper_threshold_attribute( operation_name );
        }
        else
        {
          m_upper_threshold_attribute = "";
        }
      }
    };


    template <typename types>
    class EdgeLengthQualityMeasure
    : public details::ThresholdQualityMeasureBase< types, Simplex1>
    {
    protected:

      typedef details::ThresholdQualityMeasureBase< types, Simplex1> base_class;

      typedef typename types::attributes_type        AT;
      typedef typename types::param_type             PT;
      typedef typename types::real_type              T;
      typedef typename types::vector3_type           V;

    public:

      EdgeLengthQualityMeasure(
                                 std::string     const & name
                                 , unsigned int  const & label
                                 , PT            const & parameters
                                 , InterfaceMesh const & mesh
                                 , AT            const & attributes
                                 )
      : base_class(
                   name
                   , label
                   , parameters
                   , mesh
                   , attributes
                   )
      {}

    public:

      bool is_bad(Simplex1 const & s) const
      {
        //--- Needed in case batch() passes unexisting simplex1 object
        //--- (f.ex. one removed earlier by EdgeCollapseOperation).
        if( !IsValid(base_class::m_mesh)(s))
        {
          return false;
        }

        V const v0    = base_class::get_coordinates( s.get_idx0() );
        V const v1    = base_class::get_coordinates( s.get_idx1() );
        T const value = util::edge_length(v0, v1);

        return base_class::in_inside_interval(value, s);
      }

    }; // end class EdgeLengthQualityMeasure


    template <typename types>
    class TriangleAreaQualityMeasure
    : public details::ThresholdQualityMeasureBase< types, Simplex2>
    {
    protected:

      typedef details::ThresholdQualityMeasureBase< types, Simplex2> base_class;

      typedef typename types::attribute_manager_type AT;
      typedef typename types::param_type             PT;
      typedef typename types::real_type              T;
      typedef typename types::vector3_type           V;

    public:

      TriangleAreaQualityMeasure(
                                 std::string     const & name
                                 , unsigned int  const & label
                                 , PT            const & parameters
                                 , InterfaceMesh const & mesh
                                 , AT            const & attributes
                                 )
      : base_class(
                   name
                   , label
                   , parameters
                   , mesh
                   , attributes
                   )
      {}

    public:

      bool is_bad(Simplex2 const & s) const
      {
        //--- Needed in case batch() passes unexisting simplex2 object
        //--- (f.ex. one removed earlier by EdgeCollapseOperation).
        if( !IsValid(base_class::m_mesh)(s))
        {
          return false;
        }

        V const v0 = base_class::get_coordinates( s.get_idx0() );
        V const v1 = base_class::get_coordinates( s.get_idx1() );
        V const v2 = base_class::get_coordinates( s.get_idx2() );

        T const value = util::triangle_area(v0, v1, v2);

        return this->is_inside_interval(value, s);
      }

    }; // end class TriangleAreaQualityMeasure


    template <typename types>
    class MaxMinEdgeRatioQualityMeasure
    : public details::ThresholdQualityMeasureBase< types, Simplex2>
    {
    protected:

      typedef details::ThresholdQualityMeasureBase< types, Simplex2> base_class;

      typedef typename types::attribute_manager_type AT;
      typedef typename types::param_type             PT;
      typedef typename types::real_type              T;
      typedef typename types::vector3_type           V;

    public:

      MaxMinEdgeRatioQualityMeasure(
                                    std::string     const & name
                                    , unsigned int  const & label
                                    , PT            const & parameters
                                    , InterfaceMesh const & mesh
                                    , AT            const & attributes
                                    )
      : base_class(
                   name
                   , label
                   , parameters
                   , mesh
                   , attributes
                   )
      {}

    public:

      bool is_bad(Simplex2 const & s) const
      {
        //--- Needed in case batch() passes unexisting simplex2 object
        //--- (f.ex. one removed earlier by EdgeCollapseOperation).
        if( !IsValid(base_class::m_mesh)(s))
        {
          return false;
        }

        V const v0 = base_class::get_coordinates( s.get_idx0() );
        V const v1 = base_class::get_coordinates( s.get_idx1() );
        V const v2 = base_class::get_coordinates( s.get_idx2() );

        T const value = util::maxmin_edge_ratio(v0, v1, v2);

        return base_class::is_inside_interval(value, s);
      }

    }; // end class MaxMinEdgeRatioQualityMeasure


    template <typename types>
    class AreaEdgeRatioQualityMeasure
    : public details::ThresholdQualityMeasureBase< types, Simplex2>
    {
    protected:

      typedef details::ThresholdQualityMeasureBase< types, Simplex2> base_class;

      typedef typename types::attribute_manager_type AT;
      typedef typename types::param_type             PT;
      typedef typename types::real_type              T;
      typedef typename types::vector3_type           V;

    public:

      AreaEdgeRatioQualityMeasure(
                                 std::string     const & name
                                 , unsigned int  const & label
                                 , PT            const & parameters
                                 , InterfaceMesh const & mesh
                                 , AT            const & attributes
                                 )
      : base_class(
                   name
                   , label
                   , parameters
                   , mesh
                   , attributes
                   )
      {}

    public:

      bool is_bad(Simplex2 const & s) const
      {
        //--- Needed in case batch() passes unexisting simplex2 object
        //--- (f.ex. one removed earlier by EdgeCollapseOperation).
        if( !IsValid(base_class::m_mesh)(s))
        {
          return false;
        }

        V const v0 = base_class::get_coordinates( s.get_idx0() );
        V const v1 = base_class::get_coordinates( s.get_idx1() );
        V const v2 = base_class::get_coordinates( s.get_idx2() );

        T const value = util::area_edge_ratio(v0, v1, v2);

        return base_class::is_inside_interval(value, s);
      }

    }; // end class AreaEdgeRatioQualityMeasure

  } //end namespace details
}//end namespace grit

// GRIT_THRESHOLD_QUALITY_MESAURE_H
#endif
