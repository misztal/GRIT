#ifndef GRIT_H
#define GRIT_H

#include <grit_simplex.h>
#include <grit_simplex_set.h>
#include <grit_logic_expressions.h>
#include <grit_simplex_set_functions.h>
#include <grit_attribute_vector.h>
#include <grit_opentissue_math_types.h>
#include <grit_opentissue_mesh.h>

#include <grit_domain_manager.h>
#include <grit_attribute_manager.h>

#include <grit_parameters.h>
#include <grit_pthread_scheduler.h>
#include <grit_quality_analyzer.h>
#include <grit_slab_decomposition.h>
#include <grit_subdomain.h>
#include <grit_default_algorithm.h>
#include <grit_do_operations.h>
#include <grit_batch.h>
#include <grit_default_types.h>
#include <grit_engine_2d.h>

#include <grit_interface_algorithm.h>
#include <grit_interface_attribute_assignment.h>
#include <grit_interface_domain_decomposition.h>
#include <grit_interface_mesh.h>
#include <grit_interface_mesh_operation.h>
#include <grit_interface_monitor.h>
#include <grit_interface_quality_measure.h>
#include <grit_interface_scheduler.h>

#include <attribute_assignment/grit_copy_attribute_assignment.h>
#include <attribute_assignment/grit_empty_attribute_assignment.h>
#include <attribute_assignment/grit_edge_split_attribute_assignment.h>
#include <attribute_assignment/grit_merge_attribute_assignment.h>
#include <attribute_assignment/grit_vertex_split_attribute_assignment.h>

#include <mesh_operations/grit_edge_collapse_operation.h>
#include <mesh_operations/grit_laplacian_smoothing_operation.h>
#include <mesh_operations/grit_edge_flip_operation.h>
#include <mesh_operations/grit_merge_operation.h>
#include <mesh_operations/grit_edge_split_operation.h>
#include <mesh_operations/grit_move_operation.h>
#include <mesh_operations/grit_smoothing_interface_operation.h>
#include <mesh_operations/grit_vertex_split_operation.h>

#include <quality_measures/grit_always_bad_quality_measure.h>
#include <quality_measures/grit_merge_quality_measure.h>
#include <quality_measures/grit_delaunay_quality_measure.h>
#include <quality_measures/grit_threshold_quality_measure.h>
#include <quality_measures/grit_interface_flatness_quality_measure.h>
#include <quality_measures/grit_vertex_split_quality_measure.h>

#include <monitors/grit_count_monitor.h>
#include <monitors/grit_move_monitor.h>

#include <io/grit_load_from_txt.h>
#include <io/grit_save_to_txt.h>
#include <io/grit_load_from_msh.h>
#include <io/grit_save_to_msh.h>

#include <utilities/grit_init_engine_with_mesh_file.h>
#include <utilities/grit_is_manifold.h>
#include <utilities/grit_has_ear.h>
#include <utilities/grit_has_multiple_vertices.h>
#include <utilities/grit_has_simple_boundary.h>
#include <utilities/grit_make_unit.h>
#include <utilities/grit_relabel_phase.h>
#include <utilities/grit_accumulate_attributes.h>
#include <utilities/grit_compute_bounding_box.h>
#include <utilities/grit_compute_midpoint.h>
#include <utilities/grit_compute_phase_labels.h>
#include <utilities/grit_extract_simplices.h>
#include <utilities/grit_opposite_vertex.h>
#include <utilities/grit_longest_edge.h>
#include <utilities/grit_is_any_inverted.h>
#include <utilities/grit_find_collapsable_vertices.h>
#include <utilities/grit_changes_phase_area.h>

// GRIT_H
#endif
