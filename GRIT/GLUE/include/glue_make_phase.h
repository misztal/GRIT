#ifndef GLUE_MAKE_PHASE
#define GLUE_MAKE_PHASE

#include <glue_phase.h>

#include <grit.h>

#include <cassert>

namespace glue
{

  /**
   * Make a phase object.
   * This function extracts mesh information from GRIT and creates a phase object
   * corresponding to the specified simplex set.
   *
   * @param engine   The engine to extract the phase information from.
   * @param domain   The simplex set of what the phase object should correspond to.
   *
   * @return         Upon return one gets a glue::Phase object representing
   *                 the given simplex set in engine.
   */
  inline Phase make_phase(
                          grit::engine2d_type const & engine
                          , grit::SimplexSet const & domain
                          )
  {
    Phase phase;

    //--- Get iterators  -------------------------------------------------------
    grit::SimplexSet::simplex2_const_iterator tri_begin  = domain.begin2();
    grit::SimplexSet::simplex2_const_iterator tri_end    = domain.end2();
    grit::SimplexSet::simplex1_const_iterator edge_begin = domain.begin1();
    grit::SimplexSet::simplex1_const_iterator edge_end   = domain.end1();
    grit::SimplexSet::simplex0_const_iterator vert_begin = domain.begin0();
    grit::SimplexSet::simplex0_const_iterator vert_end   = domain.end0();

    //--- Make vertex coordinate vectors for domain vertices only --------------
    unsigned int vertex_count = 0;

    for (grit::SimplexSet::simplex0_const_iterator it = vert_begin; it != vert_end; ++it)
    {
      if (engine.mesh().is_valid_simplex(*it))
      {
        grit::Simplex0 const s = *it;

        phase.m_indices[ s.get_idx0() ] = vertex_count++;   // Add global to local index mapping

        phase.m_vertices.push_back(s);                      // Remember all global vertices in Object
      }
    }

    //--- Build an array of edges of domain ----------------------------------
    for (grit::SimplexSet::simplex1_const_iterator it = edge_begin; it != edge_end; ++it)
    {
      if (engine.mesh().is_valid_simplex(*it))
      {
        grit::Simplex1 const s = *it;

        Tuple edge;

        edge.i = phase.get_local_index(s.get_idx0());
        edge.j = phase.get_local_index(s.get_idx1());

        assert( engine.mesh().is_valid_simplex( s.get_simplex0_idx0() ) || !"make_phase(): vertex invalid");
        assert( engine.mesh().is_valid_simplex( s.get_simplex0_idx1() ) || !"make_phase(): vertex invalid");

        phase.m_edges.push_back(edge);
      }
    }

    //--- Build an array of triangles of domain ----------------------------------
    for (grit::SimplexSet::simplex2_const_iterator it = tri_begin; it != tri_end; ++it)
    {
      if (engine.mesh().is_valid_simplex(*it))
      {
        grit::Simplex2 const s = *it;

        Triplet triangle;
        triangle.i = phase.get_local_index(s.get_idx0());
        triangle.j = phase.get_local_index(s.get_idx1());
        triangle.k = phase.get_local_index(s.get_idx2());

        phase.m_triangles.push_back(triangle);

        unsigned int const label = engine.mesh().label( s );

        if ( std::find(phase.m_labels.begin(), phase.m_labels.end(), label) == phase.m_labels.end() )
          phase.m_labels.push_back(label);

      }
    }

    std::sort( phase.m_labels.begin(), phase.m_labels.end() );

    return phase;
  }

  /**
   * Make a phase object.
   * This function extracts mesh information from GRIT of the phase with
   * the given label and returns this information in a glue::Phase object.
   *
   * @param label   The label of the phase in GRIT that one wishes to obtain
   *                a glue::Phase object for.
   * @param engine   The engine to extract the phase information from.
   *
   * @return        Upon return one gets a glue::Phase object representing
   *                the given phase label in GRIT. If the phase label do not
   *                exist then one gets an "empty" phase.
   */
  inline Phase make_phase(
                          grit::engine2d_type const & engine
                          , unsigned int const & label
                          )
  {
    grit::SimplexSet const subset = grit::compute_phase(engine.mesh(), label);

    Phase phase = make_phase(engine, subset);

    assert( phase.m_labels.size() <= 1 || !"make_phase(): internal error too many labels");
    assert( phase.m_labels.size() >= 1 || !"make_phase(): internal error empty phase");
    assert( phase.m_labels[0] == label || !"make_phase(): internal error label did not much argument");

    return phase;
  }

  /**
   * Create a phase object representing the whole mesh.
   *
   * @param engine   The engine to extract the phase information from.
   */
  inline Phase make_phase( grit::engine2d_type const & engine)
  {
    grit::SimplexSet const all = engine.mesh().get_all_simplices();

    Phase phase = make_phase(engine, all);

    assert( phase.m_labels.size() > 0u || !"make_phase(): internal error must be at least one label in whole mesh");

    return phase;
  }

  /**
   * Make a phase object of the boundary of the specified phase.
   * This function extracts mesh information from GRIT of the phase with
   * the given label and returns this information in a glue::Phase object.
   *
   * @param label   The label of the phase in GRIT that one wishes to obtain
   *                a glue::Phase object corresponding to its boundary.
   *
   * @param engine   The engine to extract the phase information from.
   *
   * @return        Upon return one gets a glue::Phase object representing
   *                the boundary of the given phase label in GIT. If the phase
   *                label does not exist then one gets an "empty" phase.
   */
  inline Phase make_boundary_of_phase(
                          grit::engine2d_type const & engine
                          , unsigned int const & label
                          )
  {
    grit::SimplexSet const subset    = grit::compute_phase(engine.mesh(), label);
    grit::SimplexSet const boundary  = grit::compute_interface(engine.mesh(), subset );

    glue::Phase phase = glue::make_phase(engine, boundary);

    // We must initialize label data structure too as glue::make_phase only
    // fills this from triangle information and we have none.

    phase.m_labels.push_back(label);

    assert( phase.m_labels.size() <= 1 || !"make_phase(): internal error too many labels");
    assert( phase.m_labels.size() >= 1 || !"make_phase(): internal error empty phase");
    assert( phase.m_labels[0] == label || !"make_phase(): internal error label did not much argument");

    return phase;
  }


}// end namespace glue

// GLUE_MAKE_PHASE
#endif
