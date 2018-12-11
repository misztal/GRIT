#ifndef EROSION_CORE_OBJECT_H
#define EROSION_CORE_OBJECT_H

#include <glue_phase.h>

#include <vector>

namespace erosion
{

  class CoreObject
  {
  public:

    glue::Phase         m_liquid;
    glue::Phase         m_interface;

    std::vector<double> m_interface_edge_normals_x;
    std::vector<double> m_interface_edge_normals_y;

    std::vector<double> m_interface_vertex_normals_x;
    std::vector<double> m_interface_vertex_normals_y;
    std::vector<bool>   m_interface_vertex_strict_convex;
    std::vector<double> m_interface_vertex_theta;


    std::vector<double> m_interface_x;
    std::vector<double> m_interface_y;
    std::vector<double> m_liquid_x;
    std::vector<double> m_liquid_y;

    unsigned int        m_label;

  };

} // end namespace erosion

// EROSION_CORE_OBJECT_H
#endif
