#ifndef GRIT_MOVE_MONITOR
#define GRIT_MOVE_MONITOR

#include <grit_interface_monitor.h>
#include <grit_interface_mesh.h>

#include <grit_logic_expressions.h>
#include <grit_simplex.h>
#include <grit_simplex_set.h>
#include <grit_simplex_set_functions.h>

#include <monitors/grit_count_monitor.h>

#include <util_collapse_time.h>
#include <util_log.h>

#include <vector>

namespace grit
{
  namespace details
  {
    /// This monitor is used by the default algorithm...
    template<typename types>
    class MoveMonitor
    : public InterfaceMonitor<types>
    {
    protected:

      typedef typename types::attributes_type        AT;
      typedef typename types::param_type             PT;

      typedef typename types::real_type              T;
      typedef typename types::vector3_type           V;

      typedef typename V::value_traits               VT;

    protected:

      typedef InterfaceMonitor<types> base_class;

    protected:

      InterfaceMesh        const & m_mesh;
      PT                   const & m_parameters;

      AT                         & m_attributes;

      T                            m_progress;    ///< the current time value (sub-timestep).

      T                            m_strength;    ///< this is the amount of displacement performed to the next "collision" site
                                                  ///< so that MoveOperation does not produce 0-area triangles.

      T                            m_distance_threshold; ///< this threshold is used by i.e. sliding demo and allows for the interaces
                                                         ///< to come into contact asymptotically. If m_progress > 1.0 - m_threshold,
                                                         ///< then the movement is condsidered as completed.

      CountMonitor<types>          m_counter;

    public:

      MoveMonitor(
                  InterfaceMesh const & mesh
                  , AT                & attributes
                  , PT          const & parameters
                  )
      : m_mesh( mesh)
      , m_parameters( parameters)
      , m_attributes( attributes)
      , m_progress( VT::zero())
      , m_counter( parameters.max_iterations( "algorithm"), parameters.verbose())
      {
        std::vector< unsigned int> const & labels = parameters.labels();

        for( unsigned int i = 0u; i < labels.size(); ++i)
        {
          if( !parameters.use_ambient() || labels[i] != parameters.ambient_label())
          {
            unsigned int const label = labels[i];
            m_strength           = parameters.get_strength(  "move", label);
            m_distance_threshold = parameters.get_distance_threshold( "move", label);
          }
        }

        if( !m_attributes.exist_attribute( "__target_x", 0u)
         || !m_attributes.exist_attribute( "__target_y", 0u))
        {
          return;
        }

        m_attributes.create_attribute( "__velocity_x", 0u);
        m_attributes.create_attribute( "__velocity_y", 0u);

        m_attributes.create_attribute( "__move_substep_target_x", 0u);
        m_attributes.create_attribute( "__move_substep_target_y", 0u);

        SimplexSet const vertices = filter( m_mesh.get_all_simplices(), IsDimension( m_mesh, 0));

        SimplexSet::simplex0_const_iterator begin0 = vertices.begin0();
        SimplexSet::simplex0_const_iterator end0   = vertices.end0();
        SimplexSet::simplex0_const_iterator iter0  = begin0;

        for (; iter0 != end0; ++iter0)
        {
          Simplex0 const & s0 = *iter0;

          std::vector<unsigned int> const & labels = m_attributes.get_simplex0_labels(s0);
          for( unsigned int k = 0u; k < labels.size(); ++k)
          {
            unsigned int const & label = labels[k];

            if (!(m_parameters.use_ambient()) || (label != m_parameters.ambient_label()))
            {
              V const & p = m_attributes.get_current_value(s0);

              m_attributes.set_attribute_value( "__move_substep_target_x", s0, label, p[0u]);
              m_attributes.set_attribute_value( "__move_substep_target_y", s0, label, p[1u]);

              if ((!IsBoundary(m_mesh)(s0) && !IsSubmeshBoundary(m_mesh)(s0))
                  && (IsInterface(m_mesh)(s0) || !m_parameters.use_only_interface()))
              {
                V u = m_attributes.get_target_value(s0, label) - p;

                if( !is_finite(u[0u]) || !is_number(u[0u]))
                {
                  u[0u] = 0.0;
                }
                if( !is_finite(u[1u]) || !is_number(u[1u]))
                {
                  u[1u] = 0.0;
                }

                m_attributes.set_attribute_value( "__velocity_x", s0, label, u[0u]);
                m_attributes.set_attribute_value( "__velocity_y", s0, label, u[1u]);
              }
              else
              {
                m_attributes.set_attribute_value( "__velocity_x", s0, label, VT::zero());
                m_attributes.set_attribute_value( "__velocity_y", s0, label, VT::zero());
              }
            }
          }
        }
      }

      bool next()
      {
        if (!m_counter.next())
          return false;

        if (m_progress >= VT::one()-m_distance_threshold)
          return false;

        else
          return true;
      }

      void reset()
      {
        m_progress = VT::zero();
        m_counter.reset();
      }

      // 2015-10-30 Marek: this relies on the assumption that the each vertex is assigned the same velocity by each
      //                   phase it belongs to. That means the splitting (if two or more phases impose different
      //                   targets on the vertex) must be performed before update(...) is called. We have
      //                   to think what to do in the "penetration" case.
      void update()
      {
        util::Log logging;

        if( !m_attributes.exist_attribute( "__target_x", 0u)
         || !m_attributes.exist_attribute( "__target_y", 0u))
        {
          m_progress = VT::one();
          return;
        }

        SimplexSet const triangles = filter( m_mesh.get_all_simplices(), IsDimension( m_mesh, 2));

        SimplexSet::simplex2_const_iterator begin = triangles.begin2();
        SimplexSet::simplex2_const_iterator end   = triangles.end2();
        SimplexSet::simplex2_const_iterator iter  = begin;

        T first_collapse_time = VT::infinity();

        for( ; iter != end; ++iter)
        {
          Simplex2 const & s = *iter;

          std::vector<Simplex0> nodes(3u);
          nodes[0u] = s.get_idx0();
          nodes[1u] = s.get_idx1(),
          nodes[2u] = s.get_idx2();

          V const & v0 = m_attributes.get_current_value(nodes[0u]),
                    v1 = m_attributes.get_current_value(nodes[1u]),
                    v2 = m_attributes.get_current_value(nodes[2u]);

          std::vector<V> velocities(3u, V(VT::zero()));

          for( unsigned int i = 0; i < 3u; ++i)
          {
            Simplex0 const & s0 = nodes[i];
            std::vector<unsigned int> const & labels = m_attributes.get_simplex0_labels(s0);
            for( unsigned int k = 0u; k < labels.size(); ++k)
            {
              unsigned int const & label = labels[k];

              if (!(m_parameters.use_ambient()) || (label != m_parameters.ambient_label()))
              {
                velocities[i][0u] = m_attributes.get_attribute_value( "__velocity_x", s0, label);
                velocities[i][1u] = m_attributes.get_attribute_value( "__velocity_y", s0, label);
              }
            }
          }

          T t = util::collapse_time(
                                    v0
                                    , v1
                                    , v2
                                    , velocities[0u]
                                    , velocities[1u]
                                    , velocities[2u]
                                    );

          if (t > VT::zero() && t < first_collapse_time)
            first_collapse_time = t;
        }

        if (first_collapse_time > VT::one())
          first_collapse_time = VT::one();
        else
          first_collapse_time *= m_strength;

        if (m_parameters.verbose() && !m_parameters.silent_threads())
          logging << "Earliest collapse time " << first_collapse_time << util::Log::newline();

        SimplexSet const vertices = filter( m_mesh.get_all_simplices(), IsDimension( m_mesh, 0));

        T remaining_time = VT::one() - m_progress;

        T dt = std::min(remaining_time, first_collapse_time);

        SimplexSet::simplex0_const_iterator begin0 = vertices.begin0();
        SimplexSet::simplex0_const_iterator end0   = vertices.end0();
        SimplexSet::simplex0_const_iterator iter0  = begin0;

        for (; iter0 != end0; ++iter0)
        {
          Simplex0 const s0 = *iter0;
          std::vector<unsigned int> const & labels = m_attributes.get_simplex0_labels(s0);
          for (unsigned int k = 0u; k < labels.size(); ++k)
          {
            unsigned int const & label = labels[k];

            if (!(m_parameters.use_ambient()) || (label != m_parameters.ambient_label()))
            {
              if ((!IsBoundary(m_mesh)(s0) && !IsSubmeshBoundary(m_mesh)(s0))
                  && (IsInterface(m_mesh)(s0) || !m_parameters.use_only_interface()))
              {
                T px = m_attributes.get_current_value(s0)[0u] + dt * m_attributes.get_attribute_value( "__velocity_x", s0, label);
                T py = m_attributes.get_current_value(s0)[1u] + dt * m_attributes.get_attribute_value( "__velocity_y", s0, label);

                m_attributes.set_attribute_value( "__move_substep_target_x", s0, label, px);
                m_attributes.set_attribute_value( "__move_substep_target_y", s0, label, py);
              }
              else
              {
                V pos = m_attributes.get_current_value(s0);
                m_attributes.set_attribute_value( "__move_substep_target_x", s0, label, pos[0u]);
                m_attributes.set_attribute_value( "__move_substep_target_y", s0, label, pos[1u]);
              }
            }
          }
        }

        m_progress += first_collapse_time;

        if (m_parameters.verbose() && !m_parameters.silent_threads())
          logging << "Progress " << m_progress << util::Log::newline();

        m_counter.update();
      }

      void init(PT const & params)
      {
        m_progress = VT::zero();
        m_counter.reset();
      }

    }; // class CountMonitor

  } // end namespace details
} // end namespace grit

// GRIT_COUNT_MONITOR
#endif

