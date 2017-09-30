#ifndef GLUE_COMPUTE_CONTACT_POINTS_H
#define GLUE_COMPUTE_CONTACT_POINTS_H

#include <glue_phase.h>
#include <glue_contact_point.h>

#include <grit_interface_mesh.h>
#include <grit_attribute_vector.h>

#include <vector>
#include <cmath>

namespace glue
{
  /**
   * Compute contact points.
   * This function extract all interface boundaries between different phases
   * in GRIT and creates a contact manifold representation of those interfaces.
   *
   * One may think of this as a point-sampling of the piece-wise linear contact
   * interfaces in GRIT. A point-sample is placed exactly at the vertices on
   * the contact interface and contact normals are computed.
   *
   * The computations involves a sorting of contact information to make sure
   * contacts are unique and their order are deterministic (always the same
   * if simulations are re-run). All computations are pretty straightforward
   * to massively parallelize however this is currently not done.
   *
   * The contact normals are defined in a weak form (smoothed) such that at
   * two parallel interface edges the normal is exactly orthogonal to the
   * interface. At a cusp the normal is ill-defined (could be any vector in
   * the normal cone at the "cusp" vertex) hence we choice the mean normal
   * as our definition of a contact normal. This has the beauty of giving
   * us a consistent definition of normals on vertices at the open boundary
   * of the contact interface and even works in degenerate cases such
   * as a single point of contact.
   *
   * @see ContactPoint for more details on how contact information is presented.
   *
   */
  template<typename phase_type>
  inline void compute_contact_points(
                                     grit::engine2d_type        const & engine
                                     , std::vector<phase_type>   const & objects
                                     , std::vector<ContactPoint>       & contacts
                                     , bool                      const & ignore_ambient = true
                                     , unsigned int              const & ambient_label = 0u
                                     )
  {
    using std::sqrt;

    typedef typename grit::engine2d_type::MT::vector3_type  V;

    contacts.clear();

    std::vector<ContactPoint> buffer;

    grit::SimplexSet                          const all        = engine.mesh().get_all_simplices();
    grit::SimplexSet::simplex1_const_iterator const edge_begin = all.begin1();
    grit::SimplexSet::simplex1_const_iterator const edge_end   = all.end1();

    for (grit::SimplexSet::simplex1_const_iterator edge = edge_begin; edge != edge_end; ++edge)
    {
      grit::Simplex1 const s = *edge;

      if (! engine.mesh().is_interface(s))
        continue;

      grit::SimplexSet vertices = engine.mesh().boundary(s);
      grit::SimplexSet faces    = engine.mesh().star(s);

      assert( vertices.size(0)==2 || !"compute_contact_points(): internal error?");

      grit::SimplexSet::simplex0_const_iterator vertex = vertices.begin0();

      grit::Simplex0 const v0 = *vertex++;
      grit::Simplex0 const v1 = *vertex++;

      assert( faces.size(2)==2 || !"compute_contact_points(): internal error?");

      grit::SimplexSet::simplex2_const_iterator face = faces.begin2();

      grit::Simplex2 const f0 = *face++;
      grit::Simplex2 const f1 = *face++;

      unsigned int const label_0 = engine.mesh().label(f0);
      unsigned int const label_1 = engine.mesh().label(f1);

      if(ignore_ambient && label_0==ambient_label)
        continue;

      if(ignore_ambient && label_1 == ambient_label)
        continue;

      V const & a0 = engine.attributes().get_current_value( f0.get_idx0() );
      V const & a1 = engine.attributes().get_current_value( f0.get_idx1() );
      V const & a2 = engine.attributes().get_current_value( f0.get_idx2() );

      V const & b0 = engine.attributes().get_current_value( f1.get_idx0() );
      V const & b1 = engine.attributes().get_current_value( f1.get_idx1() );
      V const & b2 = engine.attributes().get_current_value( f1.get_idx2() );

      V      const c0          = (a0 + a1 + a2)/3.0;
      V      const c1          = (b0 + b1 + b2)/3.0;

      bool   const f0_is_A     = label_0 < label_1;
      int    const A           = f0_is_A ? label_0 : label_1;
      int    const B           = f0_is_A ? label_1 : label_0;

      double const cxA         = f0_is_A ? c0[0] : c1[0];
      double const cyA         = f0_is_A ? c0[1] : c1[1];
      double const cxB         = f0_is_A ? c1[0] : c0[0];
      double const cyB         = f0_is_A ? c1[1] : c0[1];

      double const px0         = engine.attributes().get_current_value(v0)[0];
      double const py0         = engine.attributes().get_current_value(v0)[1];
      double const px1         = engine.attributes().get_current_value(v1)[0];
      double const py1         = engine.attributes().get_current_value(v1)[1];

      double const ex          = px1-px0;
      double const ey          = py1-py0;
      bool   const flip_normal =  ((cyB-cyA)*ex -(cxB-cxA)*ey)>0 ? false : true;
      double const mx          =  flip_normal ?  ey : -ey;
      double const my          =  flip_normal ? -ex :  ex;
      double const nx          = mx / sqrt(mx*mx + my*my);
      double const ny          = my / sqrt(mx*mx + my*my);

      int const vertex_A0      =  objects[A].get_local_index( v0 );
      int const vertex_A1      =  objects[A].get_local_index( v1 );
      int const vertex_B0      =  objects[B].get_local_index( v0 );
      int const vertex_B1      =  objects[B].get_local_index( v1 );

      ContactPoint const CP0 = make_contact_point(px0, py0, nx, ny, A, B, vertex_A0, vertex_B0);
      ContactPoint const CP1 = make_contact_point(px1, py1, nx, ny, A, B, vertex_A1, vertex_B1);

      buffer.push_back(CP0);
      buffer.push_back(CP1);
    }

    std::sort( buffer.begin(), buffer.end() );

    //--- At this point there might be multiple contacts bewtween the same two
    //--- phases at the exact same position but possible with different normals
    //---
    //--- If only one contact exist then it is trivially unique
    //---
    //--- If multiple contacts exist but all have the same normal then they
    //--- can be reduced to one contact point only
    //---
    //--- If multiple contacts exist but all have different normals then they can
    //--- be replaced by one contact point having the mean normal assigned to it
    //---
    //--- Hence using these rules we scan through the buffer of contact points and
    //-- reduce "redundant" contact points to a single unique contact points.

    std::vector<ContactPoint>::const_iterator begin = buffer.begin();
    std::vector<ContactPoint>::const_iterator end   = buffer.end();
    std::vector<ContactPoint>::const_iterator iter;
    std::vector<ContactPoint>::const_iterator first;

    // 2014-07-28 Kenny code review: This for loop is a little more difficult
    // to massively parallelize, one needs a two-pass strategy. First pass do
    // not store contacts but simply determines the number of contacts (a prefix
    // sum is most likely needed for this). After first pass one can allocate
    // space for all contacts and the second pass can then start. The second
    // pass can use the pre-fix sum result to offset into the allocated
    // contact array.
    for (iter=begin; iter!=end;)
    {
      first = iter;
      double mx = 0.0;
      double my = 0.0;
      for (; iter!=end && iter->weak_compare(*first); ++iter)
      {
        mx += iter->m_nx;
        my += iter->m_ny;
      }
      double const nx = mx/sqrt(mx*mx+my*my);
      double const ny = my/sqrt(mx*mx+my*my);

      ContactPoint cp = make_contact_point(
                                           first->m_px
                                           , first->m_py
                                           , nx
                                           , ny
                                           , first->m_A
                                           , first->m_B
                                           , first->m_vertex_A
                                           , first->m_vertex_B
                                           );
      contacts.push_back(cp);
    }
  }
} // end namespace glue

// GLUE_COMPUTE_CONTACT_POINTS_H
#endif
