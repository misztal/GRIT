#ifndef GRIT_DEFAULT_ALGORITHM_H
#define GRIT_DEFAULT_ALGORITHM_H

#include <grit_interface_algorithm.h>
#include <grit_interface_attribute_assignment.h>
#include <grit_batch.h>

#include <quality_measures/grit_always_bad_quality_measure.h>
#include <quality_measures/grit_interface_flatness_quality_measure.h>
#include <quality_measures/grit_delaunay_quality_measure.h>
#include <quality_measures/grit_threshold_quality_measure.h>
#include <quality_measures/grit_merge_edge_collapse_quality_measure.h>
#include <quality_measures/grit_merge_quality_measure.h>
#include <quality_measures/grit_vertex_split_quality_measure.h>
#include <quality_measures/grit_no_ears_quality_measure.h>

#include <mesh_operations/grit_edge_flip_operation.h>
#include <mesh_operations/grit_move_operation.h>
#include <mesh_operations/grit_laplacian_smoothing_operation.h>
#include <mesh_operations/grit_smoothing_interface_operation.h>
#include <mesh_operations/grit_edge_collapse_operation.h>
#include <mesh_operations/grit_interface_preserving_edge_collapse_operation.h>
#include <mesh_operations/grit_edge_split_operation.h>
#include <mesh_operations/grit_merge_operation.h>
#include <mesh_operations/grit_vertex_split_operation.h>

#include <attribute_assignment/grit_empty_attribute_assignment.h>
#include <attribute_assignment/grit_edge_collapse_attribute_assignment.h>
#include <attribute_assignment/grit_edge_split_attribute_assignment.h>
#include <attribute_assignment/grit_copy_attribute_assignment.h>
#include <attribute_assignment/grit_merge_attribute_assignment.h>
#include <attribute_assignment/grit_vertex_split_attribute_assignment.h>

#include <monitors/grit_move_monitor.h>

#include <util_log.h>

#include <string>
#include <vector>

namespace grit
{
  namespace details
  {
    template<typename types>
    class DefaultAlgorithm
    : public InterfaceAlgorithm<types>
    {
    protected:

      typedef          InterfaceAlgorithm<types>  base_class;

    public:

      typedef typename types::mesh_impl           MI;
      typedef typename types::attributes_type     AT;
      typedef typename types::param_type          PT;

      typedef          std::vector<unsigned int>  LV; //!< labels vector

    public:

      /**
       * The algorithm follows the scheme:
       * 1) perform mesh refinement and vertex-split (if needed).
       * 2) iterate 3 and 4 until all vertices are at prescribed final positions
       *    or max_iterations reached.
       * 3) improve mesh with smoothing, interface smoothing and edge flips.
       * 4) move all vertices as far as possible without inverting any triangles.
       * 5) perform merge (if needed) and coarsen the mesh.
       * 6) improve mesh with smoothing, interface smoothing and edge flips.
       */
      unsigned int run(
                       MI         & mesh
                       , AT       & attributes
                       , PT const & parameters
                       , LV const & labels
                       )
      {
        util::Log logging;
        unsigned int operations_done = 0u;

        if( parameters.verbose() && !(parameters.silent_threads()))
        {
          logging << "DefaultAlgorithm::run(): Inside DefaultAlgorithm ID = " << base_class::id() << util::Log::newline();
        }

        //--- The first group of mesh operations: mesh refinement and vertex split.
        //--- This is performed once per each call.
        {
          operations_done += batch<
                                   Simplex1
                                   , Not<IsInterface>
                                   , EdgeLengthQualityMeasure<types>
                                   , EdgeSplitOperation<types>
                                   , EdgeSplitAttributeAssignment<types>
                                   , types
                                   >(
                                     "refinement"
                                     , labels
                                     , !IsInterface(mesh)
                                     , parameters
                                     , attributes
                                     , mesh
                                     );

          operations_done += batch<
                                   Simplex1
                                   , IsInterface
                                   , EdgeLengthQualityMeasure<types>
                                   , EdgeSplitOperation<types>
                                   , EdgeSplitAttributeAssignment<types>
                                   , types
                                   >(
                                     "interface_refinement"
                                     , labels
                                     , IsInterface(mesh)
                                     , parameters
                                     , attributes
                                     , mesh
                                     );

          operations_done += batch<
                                   Simplex0
                                   , IsInterface
                                   , VertexSplitQualityMeasure<types>
                                   , VertexSplitOperation<types>
                                   , VertexSplitAttributeAssignment<types>
                                   , types
                                   >(
                                     "vertex_split"
                                     , labels
                                     , IsInterface(mesh)
                                     , parameters
                                     , attributes
                                     , mesh
                                     );
        }

        MoveMonitor<types> monitor( mesh, attributes, parameters);
        monitor.init(parameters);

        //--- Main loop: mesh improvement using smoothing, interface smoothing and edge flips,
        //--- and moving the vertices towards their final positions.
        do
        {
          operations_done += mesh_improvement( mesh, attributes, parameters, labels);

          monitor.update();

          operations_done += batch<
                                    Simplex0
                                  , Bool
                                  , AlwaysBadQualityMeasure<types,Simplex0>
                                  , MoveOperation<types>
                                  , EmptyAttributeAssignment<types>
                                  , types
                                  >(
                                    "move"
                                    , labels
                                    , Bool(true)
                                    , parameters
                                    , attributes
                                    , mesh
                                    );

          operations_done += mesh_improvement( mesh, attributes, parameters, labels);

        } while (monitor.next());

        //--- The post-movement group of mesh operations: merge phases and coarsening.
        //--- This is performed once per each call.
        {
          operations_done += batch<
                                   Simplex2
                                   , Bool
                                   , MergeQualityMeasure<types>
                                   , MergeOperation<types>
                                   , MergeAttributeAssignment<types>
                                   , types
                                   >(
                                     "merge"
                                     , labels
                                     , Bool(true)
                                     , parameters
                                     , attributes
                                     , mesh
                                     );
        operations_done += batch<
                                 Simplex0
                                 , Not<IsInterface>
                                 , AlwaysBadQualityMeasure<types,Simplex0>
                                 , LaplacianSmoothingOperation<types>
                                 , EmptyAttributeAssignment<types>
                                 , types
                                 >(
                                   "smoothing"
                                   , labels
                                   , !IsInterface(mesh)
                                   , parameters
                                   , attributes
                                   , mesh
                                   );

          operations_done += batch<
                                   Simplex1
                                   , Bool
                                   , MergeEdgeCollapseQualityMeasure<types>
                                   , EdgeCollapseOperation<types>
                                   , EdgeCollapseAttributeAssignment<types>
                                   , types
                                   >
                                   (
                                    "merge" // so we don't need another cfg file
                                    , labels
                                    , Bool(true)
                                    , parameters
                                    , attributes
                                    , mesh
                                    );

          operations_done += batch<
                                   Simplex1
                                   , Not<IsInterface>
                                   , EdgeLengthQualityMeasure<types>
                                   , InterfacePreservingEdgeCollapseOperation<types>
                                   , CopyAttributeAssignment<types>
                                   , types
                                   >(
                                     "coarsening"
                                     , labels
                                     , !IsInterface(mesh)
                                     , parameters
                                     , attributes
                                     , mesh
                                     );

          operations_done += batch<
                                   Simplex1
                                   , IsInterface
                                   , EdgeLengthQualityMeasure<types>
                                   , InterfacePreservingEdgeCollapseOperation<types>
                                   , CopyAttributeAssignment<types>
                                   , types
                                   >(
                                     "interface_coarsening"
                                     , labels
                                     , IsInterface(mesh)
                                     , parameters
                                     , attributes
                                     , mesh
                                     );

          if( parameters.no_ears() )
          {
            operations_done += batch<
                                     Simplex1
                                     , Not<IsInterface>
                                     , NoEarsQualityMeasure<types>
                                     , EdgeSplitOperation<types>
                                     , EdgeSplitAttributeAssignment<types>
                                     , types
                                     >(
                                       "ear_removal"
                                       , labels
                                       , !IsInterface(mesh)
                                       , parameters
                                       , attributes
                                       , mesh
                                       );
          }
        }

        //--- Final mesh improvement (for good quality mesh on output)
        //--- The following batch will not introduce ears from the configuration
        //--- left after ear_removal
        operations_done += mesh_improvement( mesh, attributes, parameters, labels);

        return operations_done;
      }

    protected:

      /**
       * Improves mesh using smoothing, interface smoothing and edge flips.
       */
      unsigned int mesh_improvement(
                                    MI         & mesh
                                    , AT       & attributes
                                    , PT const & parameters
                                    , LV const & labels
                                    )
      {
        unsigned int operations_done = 0u;

        operations_done += batch<
                                 Simplex1
                                 , Not<IsInterface>
                                 , DelaunayQualityMeasure<types>
                                 , EdgeFlipOperation<types>
                                 , CopyAttributeAssignment<types>
                                 , types
                                 >(
                                   "optimization"
                                   , labels
                                   , !IsInterface(mesh)
                                   , parameters
                                   , attributes
                                   , mesh
                                   );

        operations_done += batch<
                                 Simplex0
                                 , Not<IsInterface>
                                 , AlwaysBadQualityMeasure<types,Simplex0>
                                 , LaplacianSmoothingOperation<types>
                                 , EmptyAttributeAssignment<types>
                                 , types
                                 >(
                                   "smoothing"
                                   , labels
                                   , !IsInterface(mesh)
                                   , parameters
                                   , attributes
                                   , mesh
                                   );

        operations_done += batch<
                                 Simplex0
                                 , IsInterface
                                 , InterfaceFlatnessQualityMeasure<types>
                                 , SmoothingInterfaceOperation<types>
                                 , EmptyAttributeAssignment<types>
                                 , types
                                 >(
                                   "interface_smoothing"
                                   , labels
                                   , IsInterface(mesh)
                                   , parameters
                                   , attributes
                                   , mesh
                                   );

        return operations_done;
      }

    };

  } // namespace details

} // namespace grit

// GRIT_DEFAULT_ALGORITHM_H
#endif
