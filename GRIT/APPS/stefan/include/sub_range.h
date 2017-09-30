#ifndef SUB_RANGE_H
#define SUB_RANGE_H

#include <glue_sub_range.h>

namespace details
{

  /**
   * 2017-02-11 Marek: this is a version allowing for reading from multi-phase Phase objects
   * Get sub range of values from mesh engine.
   * Given a set of vertcies from a phase extract their attribute values into a array.
   *
   * @param engine    The mesh engine instance holding all data.
   * @param phase     A glue::Phase object with vertex information
   * @param name      The string value containing the attribute name.
   * @param values    Upon return this argument holds the values of the range of vertices.
   */
  inline void get_sub_range(
                              grit::engine2d_type    const & engine
                            , glue::Phase            const & phase
                            , std::string            const & name
                            , std::vector<double>          & values
                            , glue::VERTEX_ATTRIBUTE const & /*tag*/
                            )
  {
    if( !engine.attributes().exist_attribute(name, 0u) )
    {
      util::Log log;

      log << "get_sub_range() Vertex attribute " << name << " did not exist" << util::Log::newline();

      throw std::logic_error("trying to get sub range from non-existing vertex attribute");
    }

    unsigned int const N = phase.m_vertices.size();

    values.clear();
    values.resize(N,0.0);

    for(unsigned int i = 0; i < N; ++i)
    {
      grit::Simplex0 const & s = phase.m_vertices[i];

      std::vector<unsigned int> const & labels = engine.attributes().get_simplex0_labels(s);

      for(unsigned int l = 0; l < labels.size(); ++l)
      {
        values[i] = engine.attributes().get_attribute_value(name, s, labels[l] );
      }
    }
  }



  inline void set_sub_range(
                              grit::engine2d_type          & engine
                            , glue::Phase            const & phase
                            , std::string            const & name
                            , std::vector<double>    const & values
                            , glue::VERTEX_ATTRIBUTE const & /*tag*/
                            )
  {
    if( !engine.attributes().exist_attribute(name, 0u) )
    {
      util::Log log;

      log << "set_sub_range():  Vertex attribute " << name << " did not exist" << util::Log::newline();

      throw std::logic_error("trying to set sub range from non-existing vertex attribute");
    }

    if(phase.m_vertices.size() != values.size())
    {
      util::Log log;

      log << "set_sub_range():  the number of vertices must be equal to the number of values" << util::Log::newline();

      throw std::invalid_argument("vertices and values must be of same size");
    }

    unsigned int const N = phase.m_vertices.size();

    for(unsigned int i = 0; i < N; ++i)
    {
      grit::Simplex0 const & s = phase.m_vertices[i];

      std::vector<unsigned int> const & labels = engine.attributes().get_simplex0_labels(s);

      for(unsigned int l = 0; l < labels.size(); ++l)
      {
        engine.attributes().set_attribute_value(name, s, labels[l], values[i]);
      }
    }
  }


  /**
   * Sets target coordinates for sub range.
   * Boolean argument using_partial_data should be true when x and y does not have data
   * for every vertex with same label.
   */
  inline void set_sub_range_target(
                                     grit::engine2d_type       & engine
                                   , glue::Phase         const & phase
                                   , std::vector<double> const & x
                                   , std::vector<double> const & y
                                   , bool                const & using_partial_data = false
                                   )
  {
    typedef grit::default_grit_types::vector3_type V;

    if( phase.m_vertices.size() != x.size() )
    {
      util::Log log;

      log << "set_sub_range_target(): The number of vertices must be equal to the number of x-values" << util::Log::newline();

      throw std::invalid_argument("Vertices and x-values must be of same size");
    }

    if( phase.m_vertices.size() != y.size() )
    {
      util::Log log;

      log << "set_sub_range_target(): The number of vertices must be equal to the number of y-values" << util::Log::newline();

      throw std::invalid_argument("Vertices and y-values must be of same size");
    }

    bool override_partial_data_flag = false;

    if( phase.m_triangles.empty() && !phase.m_edges.empty() && !using_partial_data )
    {
      util::Log log;

      log << "set_sub_range_target(): Your phase object suggest you are setting partial data of the phase overriding using_partial_data flag."
          << util::Log::newline();

      override_partial_data_flag = true;
    }

    // In the case where the user has only sepcified target values for some vertices with given label
    if( override_partial_data_flag || using_partial_data )
    {
      unsigned int const L = phase.m_labels.size();

      for( unsigned int l=0u; l<L; ++l)
      {
        unsigned int const phase_label = phase.m_labels[l];

        glue::Phase entire_phase = glue::make_phase( engine, phase_label);

        // Default target value is current
        std::vector<double> target_x;
        std::vector<double> target_y;
        glue::get_sub_range_current( engine, entire_phase, target_x, target_y);

        // Setting target values to the ones speified by x and y from function input
        unsigned int const M = phase.m_vertices.size();

        for(unsigned int i = 0; i < M; ++i)
        {
          grit::Simplex0 const & s = phase.m_vertices[i];
          unsigned int local_idx = entire_phase.get_local_index(s);

          target_x[local_idx] = x[i];
          target_y[local_idx] = y[i];
        }

        // Update target values in engine
        unsigned int const N = entire_phase.m_vertices.size();

        for(unsigned int i = 0; i < N; ++i)
        {
          grit::Simplex0 const & s = entire_phase.m_vertices[i];

          V const p = V(target_x[i], target_y[i], 0.0);

          engine.attributes().set_target_value( s, phase_label, p );
        }
      }
    }
    else // when all target values are specified - all is good
    {
      // Update target values in engine
      unsigned int const N = phase.m_vertices.size();

      for(unsigned int i = 0; i < N; ++i)
      {
        grit::Simplex0 const & s = phase.m_vertices[i];

        V const p = V(x[i], y[i], 0.0);

        std::vector<unsigned int> const & labels = engine.attributes().get_simplex0_labels(s);

        for(unsigned int l = 0; l < labels.size(); ++l)
        {
          engine.attributes().set_target_value( s, labels[l], p);
        }
      }
    }
  }


} // namespace details

// SUB_RANGE_H
#endif
