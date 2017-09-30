#ifndef GRIT_LOAD_FROM_MSH_H
#define GRIT_LOAD_FROM_MSH_H

#include <grit_interface_mesh.h>
#include <grit_attribute_vector.h>
#include <io/grit_io_edge_counter.h>

#include <util_index_helper.h>
#include <util_log.h>

#include <fstream>
#include <stdexcept>
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
  inline void load_from_msh(
                      std::string const & filename
                      , InterfaceMesh & mesh
                      , Simplex0AttributeVector<typename MT::vector3_type> & coords
                      )
  {
    util::Log log;

    std::string const newline = util::Log::newline();

    typedef typename MT::real_type    T;
    typedef typename MT::vector3_type V;
    typedef typename V::value_traits  VT;

    std::ifstream file( filename.c_str() );

    unsigned int vertex_count   = 0u;
    unsigned int triangle_count = 0u;

    std::map<unsigned int, Simplex0> lookup_simplex;

    io::EdgeCounter   edge_counter;

    if (!file.good())
    {
      log << "load_from_msh(): File " << filename << " not found" << newline;

      throw std::logic_error("filename does not exist");
    }

      std::string block_start = "";
      file >> block_start;
      if (block_start.compare("$MeshFormat")==0)
      {
        double version = 0.0;
        int file_type = 0;
        int data_size = 0;

        file >> version;
        file >> file_type;
        file >> data_size;

        if (version!=2.2)
        {
          log << "load_from_msh(): version should be 2.2 but was " << version << newline;

          throw std::logic_error("incompatible msh file");
        }
        if (file_type!=0)
        {
          log << "load_from_msh(): File type should be 0 but was " << file_type << newline;

          throw std::logic_error("incompatible msh file");
        }
        if (data_size!=8)
        {
          log << "load_from_msh(): Data size should be 8 but was " << data_size << newline;

          throw std::logic_error("incompatible msh file");
        }
      }
      std::string block_end = "";
      file >> block_end;
      if (block_end.compare("$EndMeshFormat")!=0)
      {
        log << "load_from_msh(): Expected to find $EndMeshFormat but found: " << block_end << newline;

        throw std::logic_error("corrupt msh file");
      }
      file >> block_start;
      if (block_start.compare("$Nodes")==0)
      {
        file >> vertex_count;

        unsigned int node_number = 0;
        double x_coord = 0.0;
        double y_coord = 0.0;
        double z_coord = 0.0;

        for(unsigned int i =0u; i < vertex_count;++i)
        {
          file >> node_number;
          file >> x_coord;
          file >> y_coord;
          file >> z_coord;

          Simplex0 const v = mesh.insert();

          lookup_simplex[node_number] = v;

          coords[v] = V(x_coord, y_coord, z_coord);
        }
      }
      file >> block_end;
      if (block_end.compare("$EndNodes")!=0)
      {
        log << "load_from_msh(): Expected to find $EndNodes but found: " << block_end << newline;

        throw std::logic_error("corrupt msh file");
      }

    file >> block_start;
    if (block_start.compare("$Elements")==0)
    {
      file >> triangle_count;

      unsigned int elm_number         = 0;
      unsigned int elm_type           = 0;
      unsigned int num_of_tags        = 0;
      unsigned int physical_entity    = 0;
      unsigned int geometrical_entity = 0;
      unsigned int i_node_number      = 0;
      unsigned int j_node_number      = 0;
      unsigned int k_node_number      = 0;

      for(unsigned int e =0u; e < triangle_count;++e)
      {
        file >> elm_number;
        file >> elm_type;
        file >> num_of_tags;
        file >> physical_entity;
        file >> geometrical_entity;
        file >> i_node_number;
        file >> j_node_number;
        file >> k_node_number;

        assert(i_node_number!=j_node_number || !"load_from_msh(): Element with vertex i == vertex j");
        assert(i_node_number!=k_node_number || !"load_from_msh(): Element with vertex i == vertex k");
        assert(j_node_number!=k_node_number || !"load_from_msh(): Element with vertex j == vertex k");

        if(elm_type!=2u)
        {
          log << "load_from_msh(): Expected to find elment type 2 (triangle) but found: " << elm_type << newline;

          throw std::logic_error("corrupt msh file");
        }
        if(num_of_tags!=2u)
        {
          log << "load_from_msh(): Expected to find two tags only but found: " << num_of_tags << newline;

          throw std::logic_error("corrupt msh file");
        }

        Simplex0 const i = lookup_simplex[i_node_number];
        Simplex0 const j = lookup_simplex[j_node_number];
        Simplex0 const k = lookup_simplex[k_node_number];

        V const xi          = coords[i];
        V const xj          = coords[j];
        V const xk          = coords[k];
        V const a           = xk-xj;
        V const b           = xi-xj;
        T const signed_area = a(0u)*b(1u)-b(0u)*a(1u);

        if(signed_area <= VT::zero() )
        {
          log << "load_from_msh(): Found triangle with non-positive orientation. Possible error in input file" << newline;
        }

        assert(edge_counter.count(i,j) <= 2u || !"load_from_msh(): non-manifold mesh input");
        assert(edge_counter.count(j,k) <= 2u || !"load_from_msh(): non-manifold mesh input");
        assert(edge_counter.count(k,i) <= 2u || !"load_from_msh(): non-manifold mesh input");

        Simplex2 const s2 = mesh.insert(i, j, k);

        mesh.label(s2)    = physical_entity;
      }
    }
    file >> block_end;
    if (block_end.compare("$EndElements")!=0)
    {
      log << "load_from_msh(): Expected to find $EndElements but found: " << block_end << newline;

      throw std::logic_error("corrupt msh file");
    }

    file.close();

    log << newline;
    log << "load_from_msh(" << filename << "): Vertices: " << vertex_count << ", Triangles: " << triangle_count << "." << newline;
    log << newline;
  }

}//end namespace grit

// GRIT_LOAD_FROM_MSH_H
#endif
