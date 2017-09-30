#ifndef GRIT_OPENTISSUE_MESH_H
#define GRIT_OPENTISSUE_MESH_H

#include <grit_interface_mesh.h>

#include <grit_simplex.h>
#include <grit_attribute_vector.h>
#include <grit_simplex_set_functions.h>
#include <grit_parameters.h>

#include <util_is_ccw.h>
#include <util_boolean.h>
#include <util_integer.h>


#include <OpenTissue/core/containers/mesh/polymesh/polymesh.h>

namespace grit
{

  template<typename mesh_type>
  class OpenTissueMesh
  : public InterfaceMesh
  {
  private:

    typedef typename mesh_type::face_handle              face_handle;
    typedef typename mesh_type::edge_handle              edge_handle;
    typedef typename mesh_type::vertex_handle            vertex_handle;
    typedef typename mesh_type::halfedge_handle          halfedge_handle;

    typedef typename mesh_type::face_type                face_type;
    typedef typename mesh_type::edge_type                edge_type;
    typedef typename mesh_type::vertex_type              vertex_type;
    typedef typename mesh_type::halfedge_type            halfedge_type;
    typedef typename mesh_type::math_types::real_type    real_type;
    typedef typename mesh_type::math_types::vector3_type vector3_type;

  private:

    mesh_type                              m_mesh;
    Simplex0AttributeVector<util::Boolean> m_submesh_boundary;
    Simplex2AttributeVector<unsigned int>  m_phase; // 2015-11-04 Marek: this should be refactored to "label"

  public:

    // 2015-12-04 Kenny code review: Hmm, this should be killed! Coordinates
    //                               are stored in attribute manager as current... so
    //                               no need for this member any more.
    Simplex0AttributeVector<vector3_type> * m_coords; // TODO: change to reference and use constructor to create it?

  private:

    /**
     * Upon return face0 and face1 holds the handles of the faces adjacent
     * to an edge.
     */
    void get_faces_from_edge(
                             edge_handle const & edge
                             , face_handle & face0
                             , face_handle & face1
                             ) const
    {
      edge_type etype = * (m_mesh.get_edge_iterator(edge));

      halfedge_handle halfedge0 = etype.get_halfedge0_handle();
      halfedge_handle halfedge1 = etype.get_halfedge1_handle();

      halfedge_type htype0 = *(m_mesh.get_halfedge_iterator(halfedge0));
      halfedge_type htype1 = *(m_mesh.get_halfedge_iterator(halfedge1));

      face0 = htype0.get_face_handle();
      face1 = htype1.get_face_handle();
    }

    bool simplex_is_face(Simplex2 const & s, face_handle const & f) const
    {
      vertex_handle v0, v1, v2;

      get_vertices_from_face(f, v0, v1, v2);

      // 2015-08-01 Kenny: There is no restrictions on the order of simplex 0
      // in s. Hence, we need to test every possible combination. Although we
      // do know that the base mesh type keeps faces in CCW order this can not
      // really be exploited as any order is allowed for a Simplex2.

      if (v0.get_idx() == s.get_idx0() && v1.get_idx() == s.get_idx1() && v2.get_idx() == s.get_idx2()) return true;
      if (v0.get_idx() == s.get_idx0() && v1.get_idx() == s.get_idx2() && v2.get_idx() == s.get_idx1()) return true;
      if (v0.get_idx() == s.get_idx1() && v1.get_idx() == s.get_idx0() && v2.get_idx() == s.get_idx2()) return true;
      if (v0.get_idx() == s.get_idx1() && v1.get_idx() == s.get_idx2() && v2.get_idx() == s.get_idx0()) return true;
      if (v0.get_idx() == s.get_idx2() && v1.get_idx() == s.get_idx0() && v2.get_idx() == s.get_idx1()) return true;
      if (v0.get_idx() == s.get_idx2() && v1.get_idx() == s.get_idx1() && v2.get_idx() == s.get_idx0()) return true;

      return false;
    }

    face_handle simplex_to_face(Simplex2 const & s) const
    {
      vertex_type v = *(m_mesh.get_vertex_iterator(s.get_idx0()));

      typename mesh_type::const_vertex_edge_circulator e(v);
      typename mesh_type::const_vertex_edge_circulator eend;

      for (; e != eend; ++e)
      {
        edge_handle eh = e->get_handle();

        face_handle f1, f2;

        get_faces_from_edge(eh, f1, f2);

        if (m_mesh.is_valid_face_handle(f1) && simplex_is_face(s, f1))
          return f1;
        if (m_mesh.is_valid_face_handle(f2) && simplex_is_face(s, f2))
          return f2;
      }

      return face_handle();
    }

    Simplex2 face_to_simplex(face_handle const & face) const
    {
      if (!m_mesh.is_valid_face_handle(face))
        return Simplex2();

      face_type f = *(m_mesh.get_face_iterator(face));

      typename mesh_type::const_face_vertex_circulator v(f);

      Simplex2::index_type const v0 = v->get_handle().get_idx(); ++v;
      Simplex2::index_type const v1 = v->get_handle().get_idx(); ++v;
      Simplex2::index_type const v2 = v->get_handle().get_idx();

      return Simplex2(v0, v1, v2);
    }

    edge_handle simplex_to_edge(Simplex1 const & s) const
    {
      vertex_type v = *( m_mesh.get_vertex_iterator(s.get_idx0()) );

      typename mesh_type::const_vertex_halfedge_circulator h(v);
      typename mesh_type::const_vertex_halfedge_circulator hend;

      for (; h != hend; ++h)
      {
        halfedge_type htype = *(m_mesh.get_halfedge_iterator(h->get_handle()));

        vertex_handle v0 = htype.get_destination_handle();
        vertex_handle v1 = htype.get_origin_handle();

        edge_handle e = htype.get_edge_handle();

        if (v0.get_idx() == s.get_idx0() && v1.get_idx() == s.get_idx1())
          return e;

        if (v0.get_idx() == s.get_idx1() && v1.get_idx() == s.get_idx0())
          return e;
      }

      return edge_handle();
    }

    Simplex1 edge_to_simplex(edge_handle const & edge) const
    {
      edge_type etype = *m_mesh.get_edge_iterator(edge);

      halfedge_handle halfedge0 = etype.get_halfedge0_handle();
      halfedge_handle halfedge1 = etype.get_halfedge1_handle();

      halfedge_type htype0 = *(m_mesh.get_halfedge_iterator(halfedge0));
      halfedge_type htype1 = *(m_mesh.get_halfedge_iterator(halfedge1));

      vertex_handle vertex0 = htype0.get_destination_handle();
      vertex_handle vertex1 = htype1.get_destination_handle();

      return Simplex1(vertex0.get_idx(), vertex1.get_idx());
    }

    vertex_handle simplex_to_vertex(Simplex0 const & s) const
    {
      return m_mesh.get_vertex_handle(s.get_idx0());
    }

    Simplex0 vertex_to_simplex(vertex_handle const & vertex) const
    {
      return Simplex0(vertex.get_idx());
    }

    void get_vertices_from_face(
                                face_handle const & face
                                , vertex_handle & vertex0
                                , vertex_handle & vertex1
                                , vertex_handle & vertex2
                                ) const
    {
      face_type ftype = *(m_mesh.get_face_iterator(face));

      typename mesh_type::const_face_vertex_circulator v(ftype);

      vertex0 = v->get_handle(); ++v;
      vertex1 = v->get_handle(); ++v;
      vertex2 = v->get_handle();
    }

    void get_vertices_from_edge(
                                edge_handle const & edge
                                , vertex_handle & vertex0
                                , vertex_handle & vertex1
                                ) const
    {
      edge_type etype = *( m_mesh.get_edge_iterator(edge) );

      halfedge_handle halfedge0 = etype.get_halfedge0_handle();
      halfedge_handle halfedge1 = etype.get_halfedge1_handle();

      halfedge_type htype0 = *(m_mesh.get_halfedge_iterator(halfedge0));
      halfedge_type htype1 = *(m_mesh.get_halfedge_iterator(halfedge1));

      vertex0 = htype0.get_destination_handle();
      vertex1 = htype1.get_destination_handle();
    }

  public:

    OpenTissueMesh()
    {}

    SimplexSet boundary(Simplex2 const & s) const
    {
      Simplex0 s0 = s.get_simplex0_idx0();
      Simplex0 s1 = s.get_simplex0_idx1();
      Simplex0 s2 = s.get_simplex0_idx2();
      SimplexSet simplex_set;
      simplex_set.insert(Simplex1(s0, s1));
      simplex_set.insert(Simplex1(s1, s2));
      simplex_set.insert(Simplex1(s2, s0));

      return simplex_set;
    }

    SimplexSet boundary(Simplex1 const & s) const
    {
      vertex_handle vertex0, vertex1;

      get_vertices_from_edge(simplex_to_edge(s), vertex0, vertex1);

      SimplexSet simplex_set;

      simplex_set.insert(vertex_to_simplex(vertex0));
      simplex_set.insert(vertex_to_simplex(vertex1));

      return simplex_set;
    }


    SimplexSet boundary(Simplex0 const & s) const
    {
      return SimplexSet(); // Do nothing
    }

    SimplexSet full_boundary(Simplex2 const & s) const
    {
      face_type ftype = *m_mesh.get_face_iterator(simplex_to_face(s));

      typename mesh_type::const_face_vertex_circulator v(ftype), vend;
      typename mesh_type::const_face_edge_circulator e(ftype), eend;

      SimplexSet simplex_set;

      for (; e != eend; ++e) { simplex_set.insert(edge_to_simplex(e->get_handle())); }
      for (; v != vend; ++v) { simplex_set.insert(vertex_to_simplex(v->get_handle())); }

      return simplex_set;
    }

    SimplexSet full_boundary(Simplex1 const & s) const
    {
      vertex_handle vertex0, vertex1;

      get_vertices_from_edge(simplex_to_edge(s), vertex0, vertex1);

      SimplexSet simplex_set;

      simplex_set.insert(vertex_to_simplex(vertex0));
      simplex_set.insert(vertex_to_simplex(vertex1));

      return simplex_set;
    }

    SimplexSet full_boundary(Simplex0 const & s) const
    {
      // Do nothing
      return SimplexSet();
    }

    SimplexSet star(Simplex2 const & s) const
    {
      SimplexSet simplex_set;
      simplex_set.insert(s);

      return simplex_set;
    }

    SimplexSet star(Simplex1 const & s) const
    {
      face_handle face0, face1;
      edge_handle edge = simplex_to_edge(s);

      get_faces_from_edge(edge, face0, face1);

      SimplexSet simplex_set;

      // If we have an edge on the boundary of the mesh, there will only be one triangle
      if (m_mesh.is_valid_face_handle(face0)) simplex_set.insert(face_to_simplex(face0));
      if (m_mesh.is_valid_face_handle(face1)) simplex_set.insert(face_to_simplex(face1));

      simplex_set.insert(s);

      return simplex_set;
    }

    SimplexSet star(Simplex0 const & s) const
    {
      typename mesh_type::vertex_type vtype = *m_mesh.get_vertex_iterator(simplex_to_vertex(s));

      typename mesh_type::const_vertex_face_circulator f(vtype), fend;
      typename mesh_type::const_vertex_edge_circulator e(vtype), eend;

      SimplexSet simplex_set;

      //for (; f != fend; ++f) { if (m_mesh.is_valid_face_handle(f->get_handle())) simplex_set.insert(face_to_simplex(f->get_handle())); }
      //for (; e != eend; ++e) { if (m_mesh.is_valid_edge_handle(e->get_handle())) simplex_set.insert(edge_to_simplex(e->get_handle())); }

      for (; e != eend; ++e)
      {
        edge_handle edge = e->get_handle();
        if (m_mesh.is_valid_edge_handle(edge))
        {
          simplex_set.insert(edge_to_simplex(edge));

          face_handle face0, face1;
          get_faces_from_edge(edge, face0, face1);

          if (m_mesh.is_valid_face_handle(face0)) simplex_set.insert(face_to_simplex(face0));
          if (m_mesh.is_valid_face_handle(face1)) simplex_set.insert(face_to_simplex(face1));
        }
      }

      simplex_set.insert(s);

      return simplex_set;
    }

    SimplexSet star(SimplexSet const & simplex_set) const
    {
      SimplexSet::simplex0_const_iterator s0 = simplex_set.begin0();
      SimplexSet::simplex1_const_iterator s1 = simplex_set.begin1();
      SimplexSet::simplex2_const_iterator s2 = simplex_set.begin2();

      SimplexSet result;

      for (; s0 != simplex_set.end0(); ++s0) { result.insert(star(*s0)); }
      for (; s1 != simplex_set.end1(); ++s1) { result.insert(star(*s1)); }
      for (; s2 != simplex_set.end2(); ++s2) { result.insert(star(*s2)); }

      return result;
    }

    SimplexSet closure(Simplex2 const & s) const
    {
      typename mesh_type::face_type ftype = *m_mesh.get_face_iterator(simplex_to_face(s));

      typename mesh_type::const_face_edge_circulator e(ftype), eend;
      typename mesh_type::const_face_vertex_circulator v(ftype), vend;

      SimplexSet simplex_set;

      for (; e != eend; ++e) { simplex_set.insert(edge_to_simplex(e->get_handle())); }
      for (; v != vend; ++v) { simplex_set.insert(vertex_to_simplex(v->get_handle())); }
      simplex_set.insert(s);

      return simplex_set;
    }

    SimplexSet closure(Simplex1 const & s) const
    {
      typename mesh_type::vertex_handle vertex0, vertex1;

      get_vertices_from_edge(simplex_to_edge(s), vertex0, vertex1);

      SimplexSet simplex_set;

      simplex_set.insert(s);

      simplex_set.insert(vertex_to_simplex(vertex0));
      simplex_set.insert(vertex_to_simplex(vertex1));

      return simplex_set;
    }

    SimplexSet closure(Simplex0 const & s) const
    {
      SimplexSet simplex_set;
      simplex_set.insert(s);

      return simplex_set;
    }

    SimplexSet closure(SimplexSet const & simplex_set) const
    {
      SimplexSet::simplex0_const_iterator s0 = simplex_set.begin0();
      SimplexSet::simplex1_const_iterator s1 = simplex_set.begin1();
      SimplexSet::simplex2_const_iterator s2 = simplex_set.begin2();

      SimplexSet result;

      for (; s0 != simplex_set.end0(); ++s0) { result.insert(closure(*s0)); }
      for (; s1 != simplex_set.end1(); ++s1) { result.insert(closure(*s1)); }
      for (; s2 != simplex_set.end2(); ++s2) { result.insert(closure(*s2)); }

      return result;
    }

    SimplexSet link(Simplex0 const & s) const
    {
      SimplexSet sigma0 = closure(star(s));
      SimplexSet sigma1 = star(closure(s));

      return difference(sigma0, sigma1);
    }

    bool & submesh_boundary(Simplex0 const & s)
    {
      return m_submesh_boundary[s].value;
    }

    bool is_submesh_boundary(Simplex0 const & s) const
    {
      // 2015-12-04 Kenny: If no submesh exist then obvously no simplex0
      // can be a submesh boundary... this may occur if no decomposition
      // is done..
      if(m_submesh_boundary.size()==0)
        return false;

      return m_submesh_boundary[s].value;
    }

    bool is_submesh_boundary(Simplex1 const & s) const
    {
      face_handle f1, f2;
      get_faces_from_edge(simplex_to_edge(s), f1, f2);

      if (m_mesh.is_valid_face_handle(f1) &&
          m_mesh.is_valid_face_handle(f2))
        return false; // if the mesh has an "ear" where both vertices are on the boundary, but the edge is not because it still has two faces

      return is_submesh_boundary(s.get_simplex0_idx0()) &&
             is_submesh_boundary(s.get_simplex0_idx1());
    }

    bool is_submesh_boundary(SimplexSet const & simplex_set) const
    {
      typedef SimplexSet::simplex0_const_iterator simplex0_it;
      typedef SimplexSet::simplex1_const_iterator simplex1_it;

      for (simplex0_it s = simplex_set.begin0(); s != simplex_set.end0(); ++s)
      {
        if (is_submesh_boundary(*s))
          return true;
      }

      for (simplex1_it s = simplex_set.begin1(); s != simplex_set.end1(); ++s)
      {
        if (is_submesh_boundary(*s))
          return true;
      }

      return false;
    }

    bool is_boundary(edge_handle const & e) const
    {
      face_handle f1, f2;
      get_faces_from_edge(e, f1, f2);

      if (!m_mesh.is_valid_face_handle(f1))
        return !is_submesh_boundary(edge_to_simplex(e));

      if (!m_mesh.is_valid_face_handle(f2))
        return !is_submesh_boundary(edge_to_simplex(e));

      return false;
    }

    bool is_boundary(Simplex1 const & s) const
    {
      return is_boundary(simplex_to_edge(s));
    }

    bool is_boundary(Simplex0 const & s) const
    {
      vertex_type v = *m_mesh.get_vertex_iterator(s.get_idx0());
      typename mesh_type::const_vertex_edge_circulator e(v), eend;

      for (; e != eend; ++e)
      {
        if (is_boundary(e->get_handle()))
          return true;
      }

      return false;
    }

    unsigned int & label(Simplex2 const & s)
    {
      return m_phase[s];
    }

    unsigned int const & label(Simplex2 const & s) const
    {
      return m_phase[s];
    }

    // 29-03-2015 Marek: this now only returns true for those interface edges, which do not lie on the boundary.
    bool is_interface(edge_handle const & e) const
    {
      face_handle f1;
      face_handle f2;
      get_faces_from_edge(e, f1, f2);

      if (m_mesh.is_valid_face_handle(f1) && m_mesh.is_valid_face_handle(f2))
        return label(face_to_simplex(f1)) != label(face_to_simplex(f2));

      return false;
    }

    bool is_interface(Simplex1 const & s) const
    {
      return is_interface(simplex_to_edge(s));
    }

    bool is_interface(Simplex0 const & s) const
    {
      vertex_type v = *m_mesh.get_vertex_iterator(s.get_idx0());
      typename mesh_type::const_vertex_edge_circulator e(v), eend;

      for (; e != eend; ++e)
      {
        if (is_interface(e->get_handle()))
          return true;
      }

      return false;
    }

    bool is_interface(SimplexSet const & simplex_set) const
    {
      typedef SimplexSet::simplex0_const_iterator simplex0_it;
      typedef SimplexSet::simplex1_const_iterator simplex1_it;

      for (simplex0_it s = simplex_set.begin0(); s != simplex_set.end0(); ++s)
      {
        if (is_interface(*s))
          return true;
      }

      for (simplex1_it s = simplex_set.begin1(); s != simplex_set.end1(); ++s)
      {
        if (is_interface(*s))
          return true;
      }

      return false;
    }

    bool is_valid_simplex(Simplex0 const & s) const
    {
      return m_mesh.is_valid_vertex_handle(vertex_handle(s.get_idx0()));
    }

    bool is_valid_simplex(Simplex1 const & s) const
    {
      if (!m_mesh.is_valid_vertex_handle(vertex_handle(s.get_idx0())))
        return false;

      if (!m_mesh.is_valid_vertex_handle(vertex_handle(s.get_idx1())))
        return false;

      edge_handle e = simplex_to_edge(s);

      return m_mesh.is_valid_edge_handle(e);
    }

    bool is_valid_simplex(Simplex2 const & s) const
    {
      if (!is_valid_simplex(s.get_simplex0_idx0()) ||
          !is_valid_simplex(s.get_simplex0_idx1()) ||
          !is_valid_simplex(s.get_simplex0_idx2()))
        return false;

      face_handle f = simplex_to_face(s);

      return m_mesh.is_valid_face_handle(f);
    }

    /**
     * This method checks whether the edge is connected to at least one triangle.
     */
    bool is_manifold(edge_handle const & e) const
    {
      face_handle f1;
      face_handle f2;
      get_faces_from_edge(e, f1, f2);

      if (!m_mesh.is_valid_face_handle(f1) && !m_mesh.is_valid_face_handle(f2))
        return false;

      return true;
    }

    bool is_manifold(Simplex1 const & s) const
    {
      return is_manifold(simplex_to_edge(s));
    }

    /**
     * This method checks whether the vertex neighborhood is 2-manifold.
     */
    bool is_manifold(Simplex0 const & s) const
    {
      vertex_type v = *m_mesh.get_vertex_iterator(s.get_idx0());

      typename mesh_type::const_vertex_edge_circulator e(v), eend;

      unsigned int n_edges = 0;
      unsigned int n_boundary_edges = 0;

      for (; e != eend; ++e)
      {
        ++n_edges;

        if (!is_manifold(e->get_handle()))
          return false;

        if (is_boundary(e->get_handle()) || is_submesh_boundary(edge_to_simplex(e->get_handle())))
          ++n_boundary_edges;
      }

      if (n_edges < 2)
        return false;

      if (n_boundary_edges != 0 && n_boundary_edges != 2)
        return false;

      return true;
    }

    /**
     * Test if triangle is an ear.
     */
    bool is_ear(Simplex2 const & s) const
    {
      typedef typename mesh_type::const_face_edge_circulator edge_circulator;

      face_type ftype = *m_mesh.get_face_iterator( simplex_to_face(s) );

      edge_circulator edge(ftype);
      edge_circulator end;

      int cnt = 0u;

      for (; edge != end; ++edge)
      {
        edge_handle const & e = edge->get_handle();

        face_handle f1;
        face_handle f2;
        get_faces_from_edge(e, f1, f2);

        cnt = (!m_mesh.is_valid_face_handle(f1)) ? cnt+1 : cnt;
        cnt = (!m_mesh.is_valid_face_handle(f2)) ? cnt+1 : cnt;
      }

      return cnt>=2;
    }

    void remove(Simplex0 const & s)
    {
      m_mesh.remove_vertex(simplex_to_vertex(s));
    }

    void remove(Simplex1 const & s)
    {

    }

    void remove(Simplex2 const & s)
    {
      m_mesh.remove_face(simplex_to_face(s));
    }

    void remove(SimplexSet const & simplex_set)
    {
      SimplexSet::simplex0_const_iterator s0 = simplex_set.begin0();
      SimplexSet::simplex1_const_iterator s1 = simplex_set.begin1();
      SimplexSet::simplex2_const_iterator s2 = simplex_set.begin2();

      // Note: we delete higher order simplices first because lower order might depend on them
      for (; s2 != simplex_set.end2(); ++s2) { remove(*s2); }
      for (; s1 != simplex_set.end1(); ++s1) { remove(*s1); }
      for (; s0 != simplex_set.end0(); ++s0) { remove(*s0); }
    }

    void replace(
                 SimplexSet const & old_simplices
                 , SimplexSet const & new_simplices
                 , std::map<Simplex2,Simplex2> const & parent_lut
                 , details::Parameters const & parameters
                 )
    {
      std::map<Simplex2, unsigned int> labels;

      SimplexSet::simplex2_const_iterator it = new_simplices.begin2();
      for( ; it != new_simplices.end2(); ++it)
      {
        Simplex2 const & t = *it;
        if( parent_lut.find(t) != parent_lut.end())
        {
          labels[t] = label(parent_lut.at(t));
        }
        else
        {
          //--- If no ambient label is defined, crash.
          assert( parameters.use_ambient() || !"OpenTissueMesh::replace() label missing");

          labels[t] = parameters.ambient_label();
        }
      }

      remove(old_simplices);

      it = new_simplices.begin2();
      for( ; it != new_simplices.end2(); ++it)
      {
        Simplex2 const & t = *it;
        label(insert( t.get_idx0(), t.get_idx1(), t.get_idx2())) = labels[t];
      }

    }

    Simplex2 insert(Simplex0 const & s0, Simplex0 const & s1, Simplex0 const & s2)
    {
      vertex_handle v0 = vertex_handle(s0.get_idx0());
      vertex_handle v1 = vertex_handle(s1.get_idx0());
      vertex_handle v2 = vertex_handle(s2.get_idx0());

      Simplex2 simplex = Simplex2(s0.get_idx0(), s1.get_idx0(), s2.get_idx0());

      if (orientation(simplex) == negative)
      {
        return face_to_simplex(m_mesh.add_face(v0, v1, v2));
      }
      else
      {
        return face_to_simplex(m_mesh.add_face(v0, v2, v1));
      }
    }

    Simplex0 insert()
    {
      typename mesh_type::math_types::vector3_type vector(0, 0, 0);
      vertex_handle vertex = m_mesh.add_vertex(vector);

      return vertex_to_simplex(vertex);
    }

    sign_type orientation(Simplex2 const & s) const
    {
      vector3_type c0 = (*m_coords)[Simplex0(s.get_idx0())];
      vector3_type c1 = (*m_coords)[Simplex0(s.get_idx1())];
      vector3_type c2 = (*m_coords)[Simplex0(s.get_idx2())];

      if (util::is_ccw(c0, c1, c2))
        return negative;
      else
        return positive;
    }

    SimplexSet get_all_simplices() const
    {
      SimplexSet simplex_set;

      typename mesh_type::const_vertex_iterator vend = m_mesh.vertex_end();
      typename mesh_type::const_vertex_iterator v    = m_mesh.vertex_begin();

      typename mesh_type::const_edge_iterator eend   = m_mesh.edge_end();
      typename mesh_type::const_edge_iterator e      = m_mesh.edge_begin();

      typename mesh_type::const_face_iterator fend   = m_mesh.face_end();
      typename mesh_type::const_face_iterator f      = m_mesh.face_begin();

      for (; v != vend; ++v) { simplex_set.insert(vertex_to_simplex(v->get_handle())); }
      for (; e != eend; ++e) { simplex_set.insert(edge_to_simplex(e->get_handle())); }
      for (; f != fend; ++f) { simplex_set.insert(face_to_simplex(f->get_handle())); }

      return simplex_set;
    }

    SimplexSet get_simplices(unsigned int const & label_value) const
    {
      SimplexSet simplex_set;

      typename mesh_type::const_face_iterator fend   = m_mesh.face_end();
      typename mesh_type::const_face_iterator f      = m_mesh.face_begin();

      for (; f != fend; ++f)
      {
        Simplex2 face = face_to_simplex(f->get_handle());
        if ( label(face) == label_value)
          simplex_set.insert(face);
      }

      return simplex_set;
    }

    void remove_all_simplices()
    {
      SimplexSet all_simplics = get_all_simplices();
      remove(all_simplics);
    }

  };

} //end namespace grit

// GRIT_OPENTISSUE_MESH_H
#endif
