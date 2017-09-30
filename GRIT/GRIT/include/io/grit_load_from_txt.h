#ifndef GRIT_LOAD_FROM_TXT_H
#define GRIT_LOAD_FROM_TXT_H

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


  template <typename MT>
  inline void load_from_txt(
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

    std::vector<Simplex0> import_to_mesh = std::vector<Simplex0>();

    io::EdgeCounter   edge_counter;

    if (!file.good())
    {
      log << "load_from_txt(): File " << filename << " not found" << newline;

      throw std::invalid_argument("filename does not exist");
    }

    while (!file.eof())
    {
      char c = '#';

      file >> c;

      if (c == 'v')
      {
        T x = VT::zero();
        T y = VT::zero();

        file >> x >> y;

        Simplex0 v = mesh.insert();
        import_to_mesh.push_back(v);

        coords[v] = V(x, y, VT::zero() );

        vertex_count++;
      }

      else if (c == 't')
      {
        unsigned int idx0 = 0u;
        unsigned int idx1 = 0u;
        unsigned int idx2 = 0u;

        file >> idx0 >> idx1 >> idx2;

        assert(idx0 > 0u || !"load_from_txt(): File format is expected to use one-based indexing of vertices.");
        assert(idx1 > 0u || !"load_from_txt(): File format is expected to use one-based indexing of vertices.");
        assert(idx2 > 0u || !"load_from_txt(): File format is expected to use one-based indexing of vertices.");

        assert(idx0 <= vertex_count || !"load_from_txt(): Vertex index out of bounds");
        assert(idx1 <= vertex_count || !"load_from_txt(): Vertex index out of bounds");
        assert(idx2 <= vertex_count || !"load_from_txt(): Vertex index out of bounds");

        assert(idx0!=idx1 || !"load_from_txt(): Element with vertex 0 == vertex 1");
        assert(idx0!=idx2 || !"load_from_txt(): Element with vertex 0 == vertex 2");
        assert(idx1!=idx2 || !"load_from_txt(): Element with vertex 1 == vertex 2");

        Simplex0 s0 = import_to_mesh[ util::to_zero_based(idx0) ];
        Simplex0 s1 = import_to_mesh[ util::to_zero_based(idx1) ];
        Simplex0 s2 = import_to_mesh[ util::to_zero_based(idx2) ];

        V const xi          = coords[s0];
        V const xj          = coords[s1];
        V const xk          = coords[s2];
        V const a           = xk-xj;
        V const b           = xi-xj;
        T const signed_area = a(0u)*b(1u)-b(0u)*a(1u);

        if(signed_area <= VT::zero() )
        {
          log << "load_from_txt(): Found triangle with non-positive orientation. Possible error in input file" << newline;
        }

        assert(edge_counter.count(s0,s1) <= 2u || !"load_from_txt(): non-manifold mesh input");
        assert(edge_counter.count(s1,s2) <= 2u || !"load_from_txt(): non-manifold mesh input");
        assert(edge_counter.count(s2,s0) <= 2u || !"load_from_txt(): non-manifold mesh input");

        mesh.insert(s0, s1, s2);

        triangle_count++;
      }
      else if (c == 'l')
      {
        unsigned int idx0 = 0u;
        unsigned int idx1 = 0u;
        unsigned int idx2 = 0u;

        unsigned int phase = -1;

        file >> idx0 >> idx1 >> idx2 >> phase;

        assert(idx0 > 0u || !"load_from_txt(): File format is expected to use one-based indexing of vertices.");
        assert(idx1 > 0u || !"load_from_txt(): File format is expected to use one-based indexing of vertices.");
        assert(idx2 > 0u || !"load_from_txt(): File format is expected to use one-based indexing of vertices.");

        assert(idx0 <= vertex_count || !"load_from_txt(): Vertex index out of bounds");
        assert(idx1 <= vertex_count || !"load_from_txt(): Vertex index out of bounds");
        assert(idx2 <= vertex_count || !"load_from_txt(): Vertex index out of bounds");

        Simplex0 s0 = import_to_mesh[ util::to_zero_based(idx0) ];
        Simplex0 s1 = import_to_mesh[ util::to_zero_based(idx1) ];
        Simplex0 s2 = import_to_mesh[ util::to_zero_based(idx2) ];

        mesh.label( Simplex2(s0, s1, s2) ) = phase;
      }
      else if (c == '#') // end of file
      {
        break;
      }
      else
      {
        log << "load_from_txt(): Unknown type: " << c << newline;
      }
    }

    log << newline;
    log << "load_from_txt("<< filename << "): Vertices: " << vertex_count << ", Triangles: " << triangle_count << "." << newline;
    log << newline;
  }

}//end namespace grit

// GRIT_LOAD_FROM_TXT_H
#endif
