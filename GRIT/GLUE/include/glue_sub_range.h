#ifndef GLUE_SUB_RANGE_H
#define GLUE_SUB_RANGE_H

#include <glue_phase.h>
#include <glue_make_phase.h>

#include <grit_engine_2d.h>

#include <util_log.h>

#include <vector>
#include <string>
#include <cassert>
#include <stdexcept>  // needed for std::logic_error, std::invalid_argument

namespace glue
{

  struct VERTEX_ATTRIBUTE {};
  struct EDGE_ATTRIBUTE {};
  struct FACE_ATTRIBUTE {};

  /**
   * When one work on the whole mesh it can be usefull to be
   * able to lookup the labels of the triangles. This convenience
   * function extracts this label data.
   */
  inline void get_labels(
                         grit::engine2d_type const & engine
                         , std::vector<unsigned int> & labels
                         )
  {
    grit::SimplexSet const domain = engine.mesh().get_all_simplices();

    labels.clear();

    //--- Get iterators  -------------------------------------------------------
    grit::SimplexSet::simplex2_const_iterator tri_begin  = domain.begin2();
    grit::SimplexSet::simplex2_const_iterator tri_end    = domain.end2();

    //--- Build an array of triangles of domain ----------------------------------
    for (grit::SimplexSet::simplex2_const_iterator it = tri_begin; it != tri_end; ++it)
    {
      if (engine.mesh().is_valid_simplex(*it))
      {
        grit::Simplex2 const s = *it;
        labels.push_back( engine.mesh().label(s) );
      }
    }
  }
  
  namespace details
  {
    /**
     * Get sub range of values from mesh engine.
     * Given a set of vertcies from a phase extract their attribute values into a array.
     *
     * @param engine    The mesh engine instance holding all data.
     * @param phase     A glue::Phase object with vertex information
     * @param name      The string value containing the attribute name.
     * @param values    Upon return this argument holds the values of the range of vertices.
     */
    template<typename any_container_type>
    inline void get_sub_range_native(
                                     grit::engine2d_type  const & engine
                                     , glue::Phase        const & phase
                                     , std::string        const & name
                                     , any_container_type       & values
                                     , VERTEX_ATTRIBUTE   const & /*tag*/
                                     )
    {
      if(phase.m_labels.size()!=1u)
      {
        util::Log log;

        log << "get_sub_range() Invalid argument phase has multiple labels" << util::Log::newline();
        
        throw std::invalid_argument("trying to get sub range from non-existing vertex attribute");
      }

      if(!engine.attributes().exist_attribute(name, 0u) )
      {
        util::Log log;
        
        log << "get_sub_range() Vertex attribute " << name << " did not exist" << util::Log::newline();
        
        throw std::logic_error("trying to get sub range from non-existing vertex attribute");
      }

      unsigned int const N = phase.m_vertices.size();

      for(unsigned int i = 0; i < N; ++i)
      {
        grit::Simplex0 const & s = phase.m_vertices[i];

        values[i] = engine.attributes().get_attribute_value(name, s, phase.m_labels[0] );
      }
    }

    /**
     * This version extract attribute values from edges (simplex1's).
     */
    template<typename any_container_type>
    inline void get_sub_range_native(
                                     grit::engine2d_type  const & engine
                                     , glue::Phase        const & phase
                                     , std::string        const & name
                                     , any_container_type       & values
                                     , EDGE_ATTRIBUTE     const & /*tag*/
                                     )
    {
      if(! engine.attributes().exist_attribute(name, 1u) )
      {
        util::Log log;

        log << "get_sub_range() Edge attribute " << name << " did not exist" << util::Log::newline();

        throw std::logic_error("trying to get sub range from non-existing edge attribute");
      }

      unsigned int const N = phase.m_edges.size();

      for(unsigned int t = 0u; t < N; ++t)
      {
        unsigned int    const & i  = phase.m_edges[t].i;
        unsigned int    const & j  = phase.m_edges[t].j;

        grit::Simplex0 const & v0 = phase.m_vertices[i];
        grit::Simplex0 const & v1 = phase.m_vertices[j];

        grit::Simplex1 const & s = grit::make_simplex1(v0, v1);

        values[t] = engine.attributes().get_attribute_value(name, s);
      }
    }

    /**
     * This version extract attribute values from triangles (simplex2's).
     */
    template<typename any_container_type>
    inline void get_sub_range_native(
                                     grit::engine2d_type   const & engine
                                     , glue::Phase         const & phase
                                     , std::string         const & name
                                     , any_container_type        & values
                                     , FACE_ATTRIBUTE      const & /*tag*/
                                     )
    {
      if(! engine.attributes().exist_attribute(name, 2u) )
      {
        util::Log log;

        log << "get_sub_range() Face attribute " << name << " did not exist" << util::Log::newline();

        throw std::logic_error("trying to get sub range from non-existing face attribute");
      }

      unsigned int const N = phase.m_triangles.size();

      for(unsigned int t = 0; t < N; ++t)
      {
        unsigned int   const & i  = phase.m_triangles[t].i;
        unsigned int   const & j  = phase.m_triangles[t].j;
        unsigned int   const & k  = phase.m_triangles[t].k;

        grit::Simplex0 const & v0 = phase.m_vertices[i];
        grit::Simplex0 const & v1 = phase.m_vertices[j];
        grit::Simplex0 const & v2 = phase.m_vertices[k];

        grit::Simplex2 const & s = grit::make_simplex2(v0, v1, v2);

        values[t] = engine.attributes().get_attribute_value(name, s);
      }
    }

    /**
     * Gets current coordinates of the vertices in @param phase.
     */
    template<typename any_container_type>
    inline void get_sub_range_current_native(
                                             grit::engine2d_type  const & engine
                                             , glue::Phase        const & phase
                                             , any_container_type       & x
                                             , any_container_type       & y
                                             )
    {
      typedef grit::default_grit_types::vector3_type V;

      unsigned int const N = phase.m_vertices.size();

      for(unsigned int i = 0; i < N; ++i)
      {
        grit::Simplex0 const & s = phase.m_vertices[i];

        V const & p = engine.attributes().get_current_value(s);

        x[i] = p(0);
        y[i] = p(1);
      }
    }
    
    /**
     * Gets target coordinates of the vertices in @param phase
     */
    template<typename any_container_type>
    inline void get_sub_range_target_native(
                                            grit::engine2d_type  const & engine
                                            , glue::Phase        const & phase
                                            , any_container_type       & x
                                            , any_container_type       & y
                                            )
    {
      typedef grit::default_grit_types::vector3_type V;

      if(phase.m_labels.size()!=1u)
      {
        util::Log log;

        log << "get_sub_range_target() Invalid argument phase has multiple labels" << util::Log::newline();

        throw std::invalid_argument("trying to get sub range from non-existing vertex attribute");
      }

      unsigned int const N = phase.m_vertices.size();

      for(unsigned int i = 0; i < N; ++i)
      {
        grit::Simplex0 const & s = phase.m_vertices[i];

        V const & p = engine.attributes().get_target_value( s, phase.m_labels[0] );

        x[i] = p(0);
        y[i] = p(1);
      }
    }

    /**
     * Set sub range.
     * This function is the symmetric counter part of the
     * correspoding glue::get_sub_range() function.
     */
    template<typename any_container_type>
    inline void set_sub_range_native(
                                     grit::engine2d_type        & engine
                                     , glue::Phase        const & phase
                                     , std::string        const & name
                                     , any_container_type const & values
                                     , VERTEX_ATTRIBUTE   const & /*tag*/
                                     )

    {
      if(! engine.attributes().exist_attribute(name, 0u) )
      {
        util::Log log;

        log << "set_sub_range():  Vertex attribute " << name << " did not exist" << util::Log::newline();

        throw std::logic_error("trying to set sub range from non-existing vertex attribute");
      }

      if(phase.m_labels.size()!=1u)
      {
        util::Log log;

        log << "set_sub_range() Invalid argument phase has multiple labels" << util::Log::newline();

        throw std::invalid_argument("trying to get sub range from non-existing vertex attribute");
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

        engine.attributes().set_attribute_value(name, s, phase.m_labels[0], values[i]);
      }
    }

    /**
     * This version sets attribute values for edges (simplex1's).
     */
    template<typename any_container_type>
    inline void set_sub_range_native(
                                     grit::engine2d_type        & engine
                                     , glue::Phase        const & phase
                                     , std::string        const & name
                                     , any_container_type const & values
                                     , EDGE_ATTRIBUTE     const & /*tag*/
                                     )
    {
      if(phase.m_edges.size() != values.size())
      {
        util::Log log;

        log << "set_sub_range():  the number of edges must be equal to the number of values" << util::Log::newline();

        throw std::invalid_argument("edges and values must be of same size");
      }

      unsigned int const N = phase.m_edges.size();

      for(unsigned int t = 0u; t < N; ++t)
      {
        unsigned int   const & i  = phase.m_edges[t].i;  // Local space indices
        unsigned int   const & j  = phase.m_edges[t].j;

        grit::Simplex0 const & v0 = phase.m_vertices[i];
        grit::Simplex0 const & v1 = phase.m_vertices[j];

        grit::Simplex1 const & s = grit::make_simplex1(v0, v1);

        engine.attributes().set_attribute_value(name, s, values[t]);
      }
    }

    /**
     * This version sets attribute values for triangles (simplex2's).
     */
    template<typename any_container_type>
    inline void set_sub_range_native(
                                     grit::engine2d_type        & engine
                                     , glue::Phase        const & phase
                                     , std::string        const & name
                                     , any_container_type const & values
                                     , FACE_ATTRIBUTE     const & /*tag*/
                                     )
    {
      if(phase.m_triangles.size() != values.size())
      {
        util::Log log;

        log << "set_sub_range():  the number of triangles must be equal to the number of values" << util::Log::newline();

        throw std::invalid_argument("triangles and values must be of same size");
      }

      unsigned int const N = phase.m_triangles.size();

      for(unsigned int t = 0; t < N; ++t)
      {
        unsigned int   const & i  = phase.m_triangles[t].i;  // Local space indices
        unsigned int   const & j  = phase.m_triangles[t].j;
        unsigned int   const & k  = phase.m_triangles[t].k;

        grit::Simplex0 const & v0 = phase.m_vertices[i];
        grit::Simplex0 const & v1 = phase.m_vertices[j];
        grit::Simplex0 const & v2 = phase.m_vertices[k];

        grit::Simplex2 const & s = grit::make_simplex2(v0, v1, v2);

        engine.attributes().set_attribute_value(name, s, values[t]);
      }
    }

    template<typename any_container_type>
    inline void set_sub_range_current_native(
                                             grit::engine2d_type        & engine
                                             , glue::Phase        const & phase
                                             , any_container_type const & x
                                             , any_container_type const & y
                                             )
    {
      typedef grit::default_grit_types::vector3_type V;

      if(phase.m_vertices.size() != x.size())
      {
        util::Log log;

        log << "set_sub_range_current(): The number of vertices must be equal to the number of x-values" << util::Log::newline();

        throw std::invalid_argument("Vertices and x-values must be of same size");
      }

      if(phase.m_vertices.size() != y.size())
      {
        util::Log log;

        log << "set_sub_range_current(): The number of vertices must be equal to the number of y-values" << util::Log::newline();

        throw std::invalid_argument("Vertices and y-values must be of same size");
      }

      unsigned int const N = phase.m_vertices.size();

      for(unsigned int i = 0; i < N; ++i)
      {
        grit::Simplex0 const & s = phase.m_vertices[i];

        V const p( x[i], y[i], 0.0);

        engine.attributes().set_current_value( s, p);
      }
    }

    /**
     * Sets target coordinates for sub range.
     *
     * @param using_partial_data        Boolean argument that is true when x and y
     *                                  does not have data for every vertex with
     *                                  same label.
     */
    template<typename any_container_type>
    inline void set_sub_range_target_native(
                                            grit::engine2d_type         & engine
                                            , glue::Phase         const & phase
                                            , any_container_type  const & x
                                            , any_container_type  const & y
                                            , bool                const & using_partial_data = false
                                            )
    {
      typedef grit::default_grit_types::vector3_type V;

      if(phase.m_vertices.size() != x.size())
      {
        util::Log log;
      
        log << "set_sub_range_target(): The number of vertices must be equal to the number of x-values" << util::Log::newline();
       
        throw std::invalid_argument("Vertices and x-values must be of same size");
      }

      if(phase.m_vertices.size() != y.size())
      {
        util::Log log;
        
        log << "set_sub_range_target(): The number of vertices must be equal to the number of y-values" << util::Log::newline();
        
        throw std::invalid_argument("Vertices and y-values must be of same size");
      }

      if(phase.m_labels.size()!=1u)
      {
        util::Log log;

        log << "set_sub_range_target() Invalid argument labels must have size one" << util::Log::newline();
         
        throw std::invalid_argument("trying to get sub range from non-existing vertex attribute");
      }

      bool override_partial_data_flag = false;
      if(phase.m_triangles.empty() && !phase.m_edges.empty() && !using_partial_data)
      {
        util::Log log;

        log << "set_sub_range_target(): Your phase object suggest you are setting partial data of the phase overriding using_partial_data flag."
            << util::Log::newline();

        override_partial_data_flag = true;
      }

      // In case where the user has only specified target values for some vertices with given label
      if(override_partial_data_flag || using_partial_data)
      {
        unsigned int const phase_label = phase.m_labels[0];

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

          engine.attributes().set_target_value( s, phase_label, p);
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

          engine.attributes().set_target_value( s, phase.m_labels[0], p);
        }
      }
    }

  } // namespace details
  
  template<typename container_type>
  inline void get_sub_range(
                            grit::engine2d_type const & engine
                            , glue::Phase       const & phase
                            , std::string       const & name
                            , container_type          & values
                            , VERTEX_ATTRIBUTE  const & /*tag*/
                            )
  {
    details::get_sub_range_native( engine, phase, name, values, VERTEX_ATTRIBUTE() );
  }

  template<>
  inline void get_sub_range<typename std::vector<double> >(
                                                           grit::engine2d_type   const & engine
                                                           , glue::Phase         const & phase
                                                           , std::string         const & name
                                                           , std::vector<double>       & values
                                                           , VERTEX_ATTRIBUTE    const & /*tag*/
                                                           )
  {
    unsigned int const N = phase.m_vertices.size();

    values.clear();
    values.resize(N,0.0);

    details::get_sub_range_native( engine, phase, name, values, VERTEX_ATTRIBUTE() );
  }

  template<typename container_type>
  inline void get_sub_range(
                            grit::engine2d_type const & engine
                            , glue::Phase       const & phase
                            , std::string       const & name
                            , container_type          & values
                            , EDGE_ATTRIBUTE    const & /*tag*/
                            )
  {
    details::get_sub_range_native( engine, phase, name, values, EDGE_ATTRIBUTE() );
  }

  template<>
  inline void get_sub_range<typename std::vector<double> >(
                                                           grit::engine2d_type   const & engine
                                                           , glue::Phase         const & phase
                                                           , std::string         const & name
                                                           , std::vector<double>       & values
                                                           , EDGE_ATTRIBUTE      const & /*tag*/
                                                           )
  {
    unsigned int const N = phase.m_edges.size();

    values.clear();
    values.resize(N,0.0);

    details::get_sub_range_native( engine, phase, name, values, EDGE_ATTRIBUTE() );
  }

  template<typename container_type>
  inline void get_sub_range(
                            grit::engine2d_type const & engine
                            , glue::Phase       const & phase
                            , std::string       const & name
                            , container_type          & values
                            , FACE_ATTRIBUTE    const & /*tag*/
                            )
  {
    details::get_sub_range_native( engine, phase, name, values, FACE_ATTRIBUTE() );
  }

  template<>
  inline void get_sub_range<typename std::vector<double> >(
                                                           grit::engine2d_type   const & engine
                                                           , glue::Phase         const & phase
                                                           , std::string         const & name
                                                           , std::vector<double>       & values
                                                           , FACE_ATTRIBUTE      const & /*tag*/
                                                           )
  {
    unsigned int const N = phase.m_triangles.size();

    values.clear();
    values.resize(N,0.0);

    details::get_sub_range_native( engine, phase, name, values, glue::FACE_ATTRIBUTE() );
  }

  template<typename container_type>
  inline void get_sub_range_current(
                                    grit::engine2d_type const & engine
                                    , glue::Phase       const & phase
                                    , container_type          & x
                                    , container_type          & y
                                    )
  {
    details::get_sub_range_current_native( engine, phase, x, y );
  }

  template<>
  inline void get_sub_range_current<std::vector<double> >(
                                                          grit::engine2d_type   const & engine
                                                          , glue::Phase         const & phase
                                                          , std::vector<double>       & x
                                                          , std::vector<double>       & y
                                                          )
  {
    unsigned int const N = phase.m_vertices.size();

    x.clear();
    x.resize(N,0.0);

    y.clear();
    y.resize(N,0.0);

    details::get_sub_range_current_native( engine, phase, x, y );
  }

  template<typename container_type>
  inline void get_sub_range_target(
                                   grit::engine2d_type const & engine
                                   , glue::Phase       const & phase
                                   , container_type          & x
                                   , container_type          & y
                                   )
  {
    details::get_sub_range_target_native( engine, phase, x, y );
  }

  template<>
  inline void get_sub_range_target<std::vector<double> >(
                                                         grit::engine2d_type   const & engine
                                                         , glue::Phase         const & phase
                                                         , std::vector<double>       & x
                                                         , std::vector<double>       & y
                                                         )
  {
    unsigned int const N = phase.m_vertices.size();

    x.clear();
    x.resize(N,0.0);

    y.clear();
    y.resize(N,0.0);

    details::get_sub_range_target_native( engine, phase, x, y );
  }

  template<typename container_type>
  inline void set_sub_range(
                            grit::engine2d_type       & engine
                            , glue::Phase       const & phase
                            , std::string       const & name
                            , container_type    const & values
                            , VERTEX_ATTRIBUTE  const & /*tag*/
                            )
  {
    details::set_sub_range_native( engine, phase, name, values, VERTEX_ATTRIBUTE() );
  }

  template<typename container_type>
  inline void set_sub_range(
                            grit::engine2d_type       & engine
                            , glue::Phase       const & phase
                            , std::string       const & name
                            , container_type    const & values
                            , EDGE_ATTRIBUTE    const & /*tag*/
                            )
  {
    details::set_sub_range_native( engine, phase, name, values, EDGE_ATTRIBUTE() );
  }
  
  template<typename container_type>
  inline void set_sub_range(
                            grit::engine2d_type       & engine
                            , glue::Phase       const & phase
                            , std::string       const & name
                            , container_type    const & values
                            , FACE_ATTRIBUTE    const & /*tag*/
                            )
  {
    details::set_sub_range_native( engine, phase, name, values, FACE_ATTRIBUTE() );
  }

  template<typename container_type>
  inline void set_sub_range_current(
                                    grit::engine2d_type         & engine
                                    , glue::Phase         const & phase
                                    , container_type      const & x
                                    , container_type      const & y
                                    )
  {
    details::set_sub_range_current_native( engine, phase, x, y );
  }

  template<typename container_type>
  inline void set_sub_range_target(
                                   grit::engine2d_type         & engine
                                   , glue::Phase         const & phase
                                   , container_type      const & x
                                   , container_type      const & y
                                   , bool                const & using_partial_data = false
                                   )
  {
    details::set_sub_range_target_native( engine, phase, x, y, using_partial_data );
  }  

} // end namespace glue

// GLUE_SUB_RANGE_H
#endif
