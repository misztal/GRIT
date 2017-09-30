#ifndef STEFAN_DOMAIN_H
#define STEFAN_DOMAIN_H

#include <glue_phase.h>

#include <vector>

namespace stefan
{

  class Domain
  {
  public:

    glue::Phase               m_all;
    glue::Phase               m_liquid;
    glue::Phase               m_solid;
    glue::Phase               m_interface;
    glue::Phase               m_boundary;

    std::vector<double>       m_interface_edge_normals_x;
    std::vector<double>       m_interface_edge_normals_y;

    std::vector<double>       m_interface_vertex_normals_x;
    std::vector<double>       m_interface_vertex_normals_y;
    std::vector<bool>         m_interface_vertex_strict_convex;
    std::vector<double>       m_interface_vertex_theta;

    std::vector<double>       m_curvature;

    std::vector<double>       m_interface_x;
    std::vector<double>       m_interface_y;
    std::vector<double>       m_x;
    std::vector<double>       m_y;

    std::vector<unsigned int> m_labels;

    unsigned int              m_label_liquid;
    unsigned int              m_label_solid;

  }; // class Domain

} // end namespace stefan

// STEFAN_DOMAIN_H
#endif
