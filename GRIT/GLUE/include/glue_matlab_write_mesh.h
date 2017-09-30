#ifndef GLUE_MATLAB_WRITE_MESH_H
#define GLUE_MATLAB_WRITE_MESH_H

#include <glue_triplet.h>

#include <sstream>
#include <string>
#include <vector>

namespace glue
{

  /**
   * This convenience function makes it easy to generate Matlab scripts from
   * within ones application. Say for intance one has a glue::Phase and wishes
   * to write triangle mesh of that phase into a matlab script then one simply
   * do as follows:
   *
   * glue::Phase A = glue::make_phase(1, my_grit_mesh);
   *
   * std::ofstream file("matlab.m");
   *
   * file << " T = " << glue::matlab_write_mesh(A.m_triangles) << ";" << std::endl;
   *
   * file.close();
   *
   * @param     An array of triangles to be writting into Matlab triangle mesh syntax.
   *
   * @return    A string with the generated matlab code.
   */
  inline std::string matlab_write_mesh( std::vector<Triplet> const & T )
  {
    std::stringstream output;

    output << "[";
    for (size_t i=0u; i < T.size(); ++i)
    {
      output << (T[i].i + 1) << " " <<  (T[i].j + 1) << " " <<  (T[i].k + 1) << ";" << std::endl;
    }
    output << "]";

    output.flush();

    return output.str();
  }

} // end namespace glue

// GLUE_MATLAB_WRITE_MESH_H
#endif
