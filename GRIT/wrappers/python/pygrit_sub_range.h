#ifndef PYGRIT_SUB_RANGE_H
#define PYGRIT_SUB_RANGE_H

#include <glue.h>
#include <grit.h>

#include "pygrit_types.h"

namespace pygrit
{
  void get_sub_range(
                     grit::engine2d_type      const & engine
                     , glue::Phase            const & phase
                     , std::string            const & name
                     , py_dense_array_double        & values
                     , glue::VERTEX_ATTRIBUTE const & /*tag*/
                     )
  {
    if( phase.m_vertices.size() != values.size() )
    {
      util::Log log;

      log << "get_sub_range(): The number of vertices must be equal to the size of values array" << util::Log::newline();
      
      throw std::invalid_argument("Vertices and values must be of same size");
    }

    glue::get_sub_range( engine, phase, name, values, glue::VERTEX_ATTRIBUTE() );
  }

  void get_sub_range(
                     grit::engine2d_type      const & engine
                     , glue::Phase            const & phase
                     , std::string            const & name
                     , py_dense_array_double        & values
                     , glue::EDGE_ATTRIBUTE   const & /*tag*/
                     )
  {
    if( phase.m_edges.size() != values.size() )
    {
      util::Log log;

      log << "get_sub_range(): The number of edges must be equal to the size of values array" << util::Log::newline();
      
      throw std::invalid_argument("Edges and values must be of same size");
    }

    glue::get_sub_range( engine, phase, name, values, glue::EDGE_ATTRIBUTE() );
  }

  void get_sub_range(
                     grit::engine2d_type      const & engine
                     , glue::Phase            const & phase
                     , std::string            const & name
                     , py_dense_array_double        & values
                     , glue::FACE_ATTRIBUTE   const & /*tag*/
                     )
  {
    if( phase.m_triangles.size() != values.size() )
    {
      util::Log log;

      log << "get_sub_range(): The number of triangles must be equal to the size of values array" << util::Log::newline();
      
      throw std::invalid_argument("Triangles and values must be of same size");
    }

    glue::get_sub_range( engine, phase, name, values, glue::FACE_ATTRIBUTE() );
  }

  void get_sub_range_current(
                             grit::engine2d_type     const & engine
                             , glue::Phase           const & phase
                             , py_dense_array_double       & x
                             , py_dense_array_double       & y
                             )
  {
    if( phase.m_vertices.size() != x.size() )
    {
      util::Log log;

      log << "get_sub_range_current(): The number of vertices must be equal to the size of x array" << util::Log::newline();
      
      throw std::invalid_argument("Vertices and x-values must be of same size");
    }
    
    if( phase.m_vertices.size() != y.size() )
    {
      util::Log log;
      
      log << "get_sub_range_current(): The number of vertices must be equal to the size of y array" << util::Log::newline();
      
      throw std::invalid_argument("Vertices and y-values must be of same size");
    }

    glue::get_sub_range_current( engine, phase, x, y );
  }

  void get_sub_range_target(
                            grit::engine2d_type     const & engine
                            , glue::Phase           const & phase
                            , py_dense_array_double       & x
                            , py_dense_array_double       & y
                            )
  {
    if( phase.m_vertices.size() != x.size() )
    {
      util::Log log;

      log << "get_sub_range_target(): The number of vertices must be equal to the size of x array" << util::Log::newline();
      
      throw std::invalid_argument("Vertices and x-values must be of same size");
    }
    
    if( phase.m_vertices.size() != y.size() )
    {
      util::Log log;
      
      log << "get_sub_range_target(): The number of vertices must be equal to the size of y array" << util::Log::newline();
      
      throw std::invalid_argument("Vertices and y-values must be of same size");
    }

    glue::get_sub_range_target( engine, phase, x, y );
  }

  void set_sub_range(
                     grit::engine2d_type            & engine
                     , glue::Phase            const & phase
                     , std::string            const & name
                     , py_dense_array_double  const & values
                     , glue::VERTEX_ATTRIBUTE const & /*tag*/
                     )
  {
    glue::set_sub_range( engine, phase, name, values, glue::VERTEX_ATTRIBUTE() );
  }
  
  void set_sub_range(
                     grit::engine2d_type           & engine
                     , glue::Phase           const & phase
                     , std::string           const & name
                     , py_dense_array_double const & values
                     , glue::EDGE_ATTRIBUTE  const & /*tag*/
                     )
  {
    glue::set_sub_range( engine, phase, name, values, glue::EDGE_ATTRIBUTE() );
  }
  
  void set_sub_range(
                     grit::engine2d_type           & engine
                     , glue::Phase           const & phase
                     , std::string           const & name
                     , py_dense_array_double const & values
                     , glue::FACE_ATTRIBUTE  const & /*tag*/
                     )
  {
    glue::set_sub_range( engine, phase, name, values, glue::FACE_ATTRIBUTE() );
  }
  
  void set_sub_range_current(
                             grit::engine2d_type           & engine
                             , glue::Phase           const & phase
                             , py_dense_array_double const & x
                             , py_dense_array_double const & y
                             )
  {
    glue::set_sub_range_current( engine, phase, x, y );
  }

  void set_sub_range_target(
                            grit::engine2d_type           & engine
                            , glue::Phase           const & phase
                            , py_dense_array_double const & x
                            , py_dense_array_double const & y
                            , bool                  const & using_partial_data = false
                            )
  {
    glue::set_sub_range_target( engine, phase, x, y, using_partial_data );
  }
} // namespace pygrit

// PYGRIT_SUB_RANGE_H
#endif
