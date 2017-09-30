#ifndef GRIT_DO_OPERATIONS_H
#define GRIT_DO_OPERATIONS_H

#include <grit_interface_mesh.h>
#include <grit_interface_mesh_operation.h>
#include <grit_interface_attribute_assignment.h>
#include <grit_interface_quality_measure.h>

#include <grit_simplex.h>
#include <grit_simplex_set.h>
#include <grit_quality_analyzer.h>
#include <grit_logic_expressions.h>
#include <grit_attributes.h>

#include <utilities/grit_debug_render.h>

#include <util_log.h>

#include <map>

namespace grit
{
  namespace details
  {

    template<
        typename S
      , typename E
      , typename types
    >
    inline unsigned int do_operations(
                                      InterfaceMesh                        & mesh
                                      , QualityAnalyzer<S>                 & analyzer
                                      , InterfaceMeshOperation<S,types>    & operation
                                      , LogicExpression<E>           const & condition
                                      , InterfaceAttributeAssignment<types>   const & assign
                                      , typename types::param_type   const & parameters
                                      , Attributes<types>                  & attributes
    )
    {
      typedef AttributeManager<types>  attribute_manager;

      unsigned int operations_done = 0u;

      operation.init(mesh,attributes);   // Make sure any global initialization has occured if needed

      while (analyzer.has_next())
      {
        S const s = analyzer.pop();

        if (! IsValid(mesh)(s) )   // Make sure isValid is tested or we might run the risk of working on non-existing s
          continue;

        if (! condition(s))
          continue;

        if ( parameters.debug_render())
        {
          debug_render( s, mesh, parameters, attributes);
        }

        SimplexSet new_simplices;
        SimplexSet old_simplices;     // Does not exist in old design, might be needed to clean up AttributeVectors?

        // 2015-11-08 Kenny: If coordinates are attributes then there is really no
        //                   need to do a "mesh operation"... This more like a AttributeUpdate
        //                   operation.... This could suggest our design of MeshOperation is not
        //                   completely converged yet!!!
        // 2015-11-08 Marek: Yes, there's also some slightly ugly design in EdgeSplitOperation because of that.
        //                   I'll try to work through that in the next sitting.
        if ( operation.update_local_attributes(s, mesh, attributes))
        {
          ++operations_done;
          continue;
        }

        std::map<Simplex1, Simplex1> simplex1_lut;   // Lookup table of parenting info, if it exist
        std::map<Simplex2, Simplex2> simplex2_lut;   // Lookup table of parenting info, if it exist

        if (! operation.plan_local_connectivity_changes(s, mesh, attributes, new_simplices, old_simplices, simplex1_lut, simplex2_lut))
          continue;

        attribute_manager::insert( new_simplices, attributes);

        assign( new_simplices, old_simplices, mesh, parameters, attributes, simplex1_lut, simplex2_lut);

        mesh.replace(old_simplices, new_simplices, simplex2_lut, parameters);

        attribute_manager::remove( old_simplices, attributes);

        ++operations_done;
      }

      return operations_done;
    }

  } //end namespace details

}//end namespace grit

// GRIT_DO_OPERATIONS_H
#endif
