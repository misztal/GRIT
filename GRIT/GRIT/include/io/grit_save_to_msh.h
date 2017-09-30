#ifndef GRIT_SAVE_TO_MSH_H
#define GRIT_SAVE_TO_MSH_H

#include <grit_interface_mesh.h>
#include <grit_attribute_vector.h>

#include <util_index_helper.h>
#include <util_log.h>

#include <fstream>
#include <iostream>
#include <vector>

namespace grit
{

  /**
   * Loads mesh data from gmsh "msh" file format.
   *
   * Following is taken from http://geuz.org/gmsh/doc/texinfo/gmsh.html#MSH-ASCII-file-format
   *
   *
   * $MeshFormat
   * version-number file-type data-size
   * $EndMeshFormat
   * $Nodes
   * number-of-nodes
   * node-number x-coord y-coord z-coord
   * …
   * $EndNodes
   * $Elements
   * number-of-elements
   * elm-number elm-type number-of-tags < tag > … node-number-list
   * …
   * $EndElements
   *
   *
   * where
   *
   * version-number
   * is a real number equal to 2.2
   *
   * file-type
   * is an integer equal to 0 in the ASCII file format.
   *
   * data-size
   * is an integer equal to the size of the floating point numbers used in the file (currently only data-size = sizeof(double) is supported).
   *
   * number-of-nodes
   * is the number of nodes in the mesh.
   *
   * node-number
   * is the number (index) of the n-th node in the mesh; node-number must be
   * a postive (non-zero) integer. Note that the node-numbers do not necessarily
   * have to form a dense nor an ordered sequence.
   *
   * x-coord y-coord z-coord
   * are the floating point values giving the X, Y and Z coordinates of the n-th node.
   *
   * number-of-elements
   * is the number of elements in the mesh.
   *
   * elm-number
   * is the number (index) of the n-th element in the mesh; elm-number must
   * be a postive (non-zero) integer. Note that the elm-numbers do not necessarily
   * have to form a dense nor an ordered sequence.
   *
   * elm-type
   * defines the geometrical type of the n-th element: 2 means 3-node triangle.
   *
   * number-of-tags
   * gives the number of integer tags that follow for the n-th element.
   * By default, the first tag is the number of the physical entity to which
   * the element belongs; the second is the number of the elementary geometrical
   * entity to which the element belongs; the third is the number of mesh
   * partitions to which the element belongs, followed by the partition ids
   * (negative partition ids indicate ghost cells). A zero tag is equivalent
   * to no tag. Gmsh and most codes using the MSH 2 format require at least the
   * first two tags (physical and elementary tags).
   *
   *
   */
  template <typename MT>
  inline void save_to_msh(
                          std::string const & filename
                          , InterfaceMesh & mesh
                          , Simplex0AttributeVector<typename MT::vector3_type> const & coords
                          )
  {
    util::Log log;

    std::string const newline = util::Log::newline();

    SimplexSet const all = mesh.get_all_simplices();

    unsigned int vertex_count   = all.size(0);
    unsigned int triangle_count = all.size(2);

    SimplexSet::simplex0_const_iterator begin0 = all.begin0();
    SimplexSet::simplex0_const_iterator end0   = all.end0();
    SimplexSet::simplex2_const_iterator begin2 = all.begin2();
    SimplexSet::simplex2_const_iterator end2   = all.end2();

    unsigned int max_id = 0;

    for (SimplexSet::simplex0_const_iterator it = begin0; it != end0; ++it)
    {
      Simplex0 const s = *it;

      if ( s.get_idx0() > max_id )
        max_id = s.get_idx0();
    }

    const unsigned int unassigned = 0xFFFFFFFF;

    std::vector< unsigned int > id_2_counter(max_id+1, unassigned);


    std::ofstream file( filename.c_str() , std::ofstream::out );
    if (!file.good())
    {
      log << "File " << filename << " could not be save" << newline;
      return;
    }
    {
      file << "$MeshFormat" << std::endl;
      double version = 2.2;
      int file_type = 0;
      int data_size = 8;
      file << version << " " << file_type << " " << data_size << std::endl;
      file << "$EndMeshFormat" << std::endl;
    }
    {
      file << "$Nodes" << std::endl;
      file << vertex_count << std::endl;
      unsigned int counter = 0;
      for( SimplexSet::simplex0_const_iterator iter=begin0; iter != end0; ++iter)
      {
        Simplex0 const s = *iter;
        id_2_counter[ s.get_idx0() ] = counter++;

        unsigned int node_number = util::to_one_based( counter );
        double x_coord = coords[s][0];
        double y_coord = coords[s][1];
        double z_coord = coords[s][2];

        file << node_number << " " << x_coord << " " << y_coord << " " << z_coord << std::endl;
      }
      file << "$EndNodes" << std::endl;
    }
    {
      file <<  "$Elements" << std::endl;
      file << triangle_count << std::endl;
      unsigned int elm_number = 1;
      for( SimplexSet::simplex2_const_iterator iter=begin2; iter != end2; ++iter, ++elm_number)
      {
        Simplex2 const s = *iter;

        unsigned int elm_type           = 2;
        unsigned int num_of_tags        = 2;
        unsigned int physical_entity    = mesh.label(s);
        unsigned int geometrical_entity = mesh.label(s);
        unsigned int i_node             = util::to_one_based( id_2_counter[ s.get_idx0() ] );
        unsigned int j_node             = util::to_one_based( id_2_counter[ s.get_idx1() ] );
        unsigned int k_node             = util::to_one_based( id_2_counter[ s.get_idx2() ] );

        file << elm_number << " " << elm_type << " "
             << num_of_tags << " " << physical_entity << " "
             << geometrical_entity << " "  << i_node << " "
             << j_node << " " << k_node << std::endl;
      }
      file << "$EndElements" << std::endl;
    }
    file.flush();
    file.close();
  
    log << "save_to_msh(...): Vertices: " << vertex_count << ", Triangles: " << triangle_count << "." << newline;
  }

  /*template <typename MT>
  inline void save_to_msh(
                          std::string const & filename
                          , InterfaceMesh & mesh
                          , Simplex0AttributeVector<typename MT::vector3_type> const & coords
                          )
  {
    util::Log log;

    std::string const newline = util::Log::newline();

    typedef typename MT::real_type    T;
    typedef typename MT::vector3_type V;

    SimplexSet const all = mesh.get_all_simplices();

    unsigned int vertex_count   = all.size(0);
    unsigned int triangle_count = all.size(2);

    SimplexSet::simplex0_const_iterator begin0 = all.begin0();
    SimplexSet::simplex0_const_iterator end0   = all.end0();
    SimplexSet::simplex2_const_iterator begin2 = all.begin2();
    SimplexSet::simplex2_const_iterator end2   = all.end2();

    std::ofstream file( filename.c_str() , std::ofstream::out );
    if (!file.good())
    {
      log << "File " << filename << " could not be save" << newline;
      return;
    }
    {
      file << "$MeshFormat" << std::endl;
      double version = 2.2;
      int file_type = 0;
      int data_size = 8;
      file << version << " " << file_type << " " << data_size << std::endl;
      file << "$EndMeshFormat" << std::endl;
    }
    {
      file << "$Nodes" << std::endl;
      file << vertex_count << std::endl;
      for( SimplexSet::simplex0_const_iterator iter=begin0; iter != end0; ++iter)
      {
        Simplex0 const s = *iter;

        unsigned int node_number = util::to_one_based( s.get_idx0() );
        double x_coord = coords[s][0];
        double y_coord = coords[s][1];
        double z_coord = coords[s][2];

        file << node_number << " " << x_coord << " " << y_coord << " " << z_coord << std::endl;
      }
      file << "$EndNodes" << std::endl;
    }
    {
      file <<  "$Elements" << std::endl;
      file << triangle_count << std::endl;
      unsigned int elm_number = 1;
      for( SimplexSet::simplex2_const_iterator iter=begin2; iter != end2; ++iter, ++elm_number)
      {
        Simplex2 const s = *iter;

        unsigned int elm_type           = 2;
        unsigned int num_of_tags        = 2;
        unsigned int physical_entity    = mesh.label(s);
        unsigned int geometrical_entity = mesh.label(s);
        unsigned int i_node             = util::to_one_based( s.get_idx0() );
        unsigned int j_node             = util::to_one_based( s.get_idx1() );
        unsigned int k_node             = util::to_one_based( s.get_idx2() );

        file << elm_number << " " << elm_type << " "
             << num_of_tags << " " << physical_entity << " "
             << geometrical_entity << " "  << i_node << " "
             << j_node << " " << k_node << std::endl;
      }
      file << "$EndElements" << std::endl;
    }
    file.flush();
    file.close();
  
    log << "save_to_msh(...): Vertices: " << vertex_count << ", Triangles: " << triangle_count << "." << newline;
    }*/

}//end namespace grit
  
// GRIT_SAVE_TO_MSH_H
#endif
