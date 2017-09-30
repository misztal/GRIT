#ifndef GRIT_MOVE_OPERATION_H
#define GRIT_MOVE_OPERATION_H

#include <grit_interface_mesh_operation.h>
#include <grit_interface_mesh.h>

#include <grit_logic_expressions.h>
#include <grit_simplex.h>
#include <grit_simplex_set.h>
#include <grit_simplex_set_functions.h>

#include <util_log.h>

#include <string>


namespace grit
{
  namespace details
  {
    /**
     * This MoveOperation is bound to MoveMonitor (defined in monitors/grit_new_move_monitor.h)
     * The monitor finds the fraction of the time step, after which first triangle collapse happens
     * then saves the substep target positions in __move_substep_target_x and __move_substep_target_y
     * attribute vectors. This operation trivially moves the vertices to their substep target positions.
     */
    template< typename types>
    class MoveOperation
    : public InterfaceMeshOperation< Simplex0, types>
    {
    protected:

      typedef typename types::real_type        T;
      typedef typename types::vector3_type     V;
      typedef typename V::value_traits         VT;

      typedef typename types::math_types       MT;

      typedef typename types::attributes_type  AMT;
      typedef typename types::param_type       PT;

    protected:

      std::string  const & m_operation_name;        ///< Name used to retrieve parameters.
      unsigned int const   m_label;                 ///< Label of the submesh on which operation is performed.

      PT           const & m_parameters;            ///< Parameters container.

    public:

      MoveOperation(
                    std::string    const & operation_name
                    , unsigned int const & label
                    , PT           const & parameters
                    )
      : m_operation_name( operation_name)
      , m_label( label)
      , m_parameters( parameters)
      { }


      void init(
                 InterfaceMesh  const & mesh
               , AMT  & attributes
               )
      { }


      bool update_local_attributes(
                                    Simplex0 const & simplex
                                  , InterfaceMesh  const & mesh
                                  , AMT            & attributes
                                  )
      {
        if (IsSubmeshBoundary(mesh)(simplex) || IsBoundary(mesh)(simplex) || !IsValid(mesh)(simplex))
          return false;

        if (!IsInterface(mesh)(simplex) && m_parameters.use_only_interface())
          return false;

        if( attributes.exist_attribute("__move_substep_target_x", 0u)
         && attributes.exist_attribute("__move_substep_target_y", 0u))
        {
          V new_pos(VT::zero());
          new_pos[0u] = attributes.get_attribute_value( "__move_substep_target_x", simplex, m_label);
          new_pos[1u] = attributes.get_attribute_value( "__move_substep_target_y", simplex, m_label);

          attributes.set_current_value( simplex, new_pos);

          return true;
        }

        return false;
      }


      bool plan_local_connectivity_changes(
                                           Simplex0                 const & /* simplex */
                                           , InterfaceMesh                & /* mesh */
                                           , AMT                    const & /* attributes */
                                           , SimplexSet                   & /* new_simplices */
                                           , SimplexSet                   & /* old_simplices */
                                           , std::map<Simplex1, Simplex1> & /* simplex1_lut */
                                           , std::map<Simplex2, Simplex2> & /* simplex2_lut */
                                           )
      {
        return false;
      }

    }; // class MoveOperation
  } // end namespace details
} // end namespace grit

// GRIT_MOVE_OPERATION_H
#endif
