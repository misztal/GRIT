#ifndef STEFAN_MAKE_DOMAIN_H
#define STEFAN_MAKE_DOMAIN_H

#include <stefan_domain.h>

#include <grit.h>

#include <glue_sub_range.h>
#include <glue_phase.h>
#include <glue_make_phase.h>

#include <cassert>
#include <vector>
#include <cmath>

namespace stefan
{

  namespace details
  {

    inline double PI() {      return 3.141592653589793;    }

    /**
     * Give vertex indices of two edges that we know share exactly
     * one vertex this funciton detects the shared vertex index
     * and returns the reduced set of vertex indices.
     *
     * @param   i       Upon return holds the non-shared vertex of edge A.
     * @param   j       Upon return holds the shared vertex of edge A and B.
     * @param   k       Upon return holds the non-shared vertex of edge B.
     */
    inline void reduce_shared_index(
                                  unsigned int const & a0
                                , unsigned int const & a1
                                , unsigned int const & b0
                                , unsigned int const & b1
                                , unsigned int       & i
                                , unsigned int       & j
                                , unsigned int       & k
                                )
    {
      if( a0==b0 )
      {
        i = a1;
        j = a0;
        k = b1;
      }

      if( a0==b1 )
      {
        i = a1;
        j = a0;
        k = b0;
      }

      if( a1==b0 )
      {
        i = a0;
        j = a1;
        k = b1;
      }

      if( a1==b1 )
      {
        i = a0;
        j = a1;
        k = b0;
      }

      assert(i!=j || !"reduce_shared_index(): internal error");
      assert(i!=k || !"reduce_shared_index(): internal error");
      assert(j!=k || !"reduce_shared_index(): internal error");
    }

    /**
     * We want the angle measured from edge A to edge B to be the interior
     * angle. Hence we wish to know if the order of A and B are correct, or
     * we risk to measure the "outside" angle instead of the inside angle.
     */
    inline bool should_flip_edge_order(
                                       double const & ea_x
                                       , double const & ea_y
                                       , double const & eb_x
                                       , double const & eb_y
                                       , double const & na_x
                                       , double const & na_y
                                       , double const & nb_x
                                       , double const & nb_y
    )
    {
      // We assume that edge A and B are in the correct order and then
      // we compute edge normal directions from their assumed edge order
      double const ma_x =   ea_y;
      double const ma_y = - ea_x;
      double const mb_x = - eb_y;
      double const mb_y =   eb_x;

      // Now we simply test if the known true normal directions of
      // edge A and B are pointing in the same direction as the
      // assumed correct order indicates.
      //
      // If not our assumption must be wrong and we must
      // flip the assumed edge order.
      bool const a_normal_was_the_same = (ma_x*na_x + ma_y*na_y) > 0.0;
      bool const b_normal_was_the_same = (mb_x*nb_x + mb_y*nb_y) > 0.0;

      if (a_normal_was_the_same && b_normal_was_the_same)
        return false;

      if (!a_normal_was_the_same && !b_normal_was_the_same)
        return true;

      assert(false && !"should_flip_edge_order(): Internal error");

      return true;
    }

    /**
     * This function detect the proper outside normal direction of a given
     * edge by considering the adjacent faces. Knowing the inside label
     * of the adjacent faces helps uniquely determine the outward normal
     * direction indepedent of the vertex ordering of the given edge.
     *
     * This way we can avoid having to sort edges/vertices in say
     * counter-clock-wise order to determine proper inside/outside information.
     * Besides this way of determining normal directions is robust in cases of
     * non-manifold interface vertices. These would otherwise create cycles in
     * a ordering algorithm. Hence making such algoritms non-trivial and
     * non-practical.
     */
    inline void compute_edge_normal(
                                    grit::engine2d_type const & engine
                                    , unsigned int      const & inside_label
                                    , grit::Simplex1    const & edge
                                    , double                  & normal_x
                                    , double                  & normal_y
                                    )
    {
      using std::sqrt;

      typedef typename grit::engine2d_type::MT::vector3_type  V;

      assert( engine.mesh().is_interface(edge) || !"compute_edge_normal(): internal error");

      grit::SimplexSet vertices = engine.mesh().boundary(edge);
      grit::SimplexSet faces    = engine.mesh().star(edge);

      assert( vertices.size(0)==2 || !"compute_edge_normal(): internal error?");

      grit::SimplexSet::simplex0_const_iterator vertex = vertices.begin0();

      grit::Simplex0 const v0 = *vertex++;
      grit::Simplex0 const v1 = *vertex++;

      assert( faces.size(2)==2 || !"compute_edge_normal(): internal error?");

      grit::SimplexSet::simplex2_const_iterator face = faces.begin2();

      grit::Simplex2 const f0 = *face++;
      grit::Simplex2 const f1 = *face++;

      unsigned int const label_0 = engine.mesh().label(f0);
      unsigned int const label_1 = engine.mesh().label(f1);

      assert(label_0==inside_label || label_1 == inside_label || !"compute_edge_normal(): internal error");

      V const & a0 = engine.attributes().get_current_value( f0.get_idx0() );
      V const & a1 = engine.attributes().get_current_value( f0.get_idx1() );
      V const & a2 = engine.attributes().get_current_value( f0.get_idx2() );

      V const & b0 = engine.attributes().get_current_value( f1.get_idx0() );
      V const & b1 = engine.attributes().get_current_value( f1.get_idx1() );
      V const & b2 = engine.attributes().get_current_value( f1.get_idx2() );

      V      const c0          = (a0 + a1 + a2)/3.0;
      V      const c1          = (b0 + b1 + b2)/3.0;

      // A triangle is inside and B triangle is outside
      bool   const f0_is_A     = label_0 == inside_label;

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

      normal_x          = mx / sqrt(mx*mx + my*my);
      normal_y          = my / sqrt(mx*mx + my*my);
    }

    /**
     * Loops over all edges and computes the outward unit normal for each edge.
     */
    inline void compute_edge_normals(
                                     grit::engine2d_type        const & engine
                                     , unsigned int             const & inside_label
                                     , grit::SimplexSet         const & interface_set
                                     , std::vector<double>            & normal_x
                                     , std::vector<double>            & normal_y
                                     )
    {
      using std::sqrt;

      normal_x.clear();
      normal_y.clear();

      grit::SimplexSet::simplex1_const_iterator const edge_begin = interface_set.begin1();
      grit::SimplexSet::simplex1_const_iterator const edge_end   = interface_set.end1();

      for (grit::SimplexSet::simplex1_const_iterator edge = edge_begin; edge != edge_end; ++edge)
      {
        grit::Simplex1 const s = *edge;

        double nx = 0.0;
        double ny = 0.0;

        compute_edge_normal(engine, inside_label, s, nx, ny);

        normal_x.push_back(nx);
        normal_y.push_back(ny);
      }
    }

    /**
     * This function computes the outward unit normal of interface vertices
     * as the mean outward unit normal of incident interface edges. For
     * non-manifold vertices then normal is defined as the zero-vector.
     *
     * The method also computes the interior angles for each vertex
     * as well as whether the vertex is strict convex or not.
     */
    inline void compute_vertex_normals(
                                       grit::engine2d_type        const & engine
                                       , unsigned int             const & inside_label
                                       , grit::SimplexSet         const & interface_set
                                       , std::vector<double>            & vertex_normal_x
                                       , std::vector<double>            & vertex_normal_y
                                       , std::vector<bool>              & vertex_strict_convex
                                       , std::vector<double>            & vertex_theta
                                       , std::vector<double>            & curvature
                                       )
    {
      using std::sqrt;

      typedef typename grit::engine2d_type::MT::vector3_type  V;

      grit::SimplexSet::simplex0_const_iterator const vertex_begin = interface_set.begin0();
      grit::SimplexSet::simplex0_const_iterator const vertex_end   = interface_set.end0();
      grit::SimplexSet::simplex0_const_iterator       vertex       = vertex_begin;

      unsigned int const N = interface_set.size(0u);

      vertex_normal_x.resize(N, 0.0);
      vertex_normal_y.resize(N, 0.0);
      vertex_strict_convex.resize(N, false);
      vertex_theta.resize(N, 0.0);
      curvature.resize(N, 0.0);

      for( unsigned int idx=0u; vertex != vertex_end; ++vertex,++idx)
      {
        grit::Simplex0 const s = *vertex;

        grit::SimplexSet adjacent = engine.mesh().star(s);
        grit::SimplexSet edges    = grit::compute_interface(engine.mesh(), adjacent);

        assert(edges.size(1)>=2u || !"compute_vertex_normals(): internal error");

        if(edges.size(1)==2u)
        {
          grit::SimplexSet::simplex1_const_iterator edge = edges.begin1();

          grit::Simplex1 const edge_a = *edge++;
          grit::Simplex1 const edge_b = *edge++;

          double na_x = 0.0;
          double na_y = 0.0;

          compute_edge_normal(engine, inside_label, edge_a, na_x, na_y);

          double nb_x = 0.0;
          double nb_y = 0.0;

          compute_edge_normal(engine, inside_label, edge_b, nb_x, nb_y);

          double const m_x      = na_x + nb_x;
          double const m_y      = nb_y + nb_y;
          double const m_length = sqrt(m_x*m_x+m_y*m_y);

          assert(m_length > 0.0 || !"compute_vertex_normals(): internal error");

          double const n_x = m_x / m_length;
          double const n_y = m_y / m_length;

          unsigned int const a0 = edge_a.get_idx0();
          unsigned int const a1 = edge_a.get_idx1();

          unsigned int const b0 = edge_b.get_idx0();
          unsigned int const b1 = edge_b.get_idx1();

          unsigned int i = 0u;
          unsigned int j = 0u;
          unsigned int k = 0u;

          details::reduce_shared_index(a0,a1,b0,b1,i,j,k);

          V const & p_i = engine.attributes().get_current_value( i );
          V const & p_j = engine.attributes().get_current_value( j ); // shared vertex
          V const & p_k = engine.attributes().get_current_value( k );

          double const ax = p_i(0u) - p_j(0u);
          double const ay = p_i(1u) - p_j(1u);
          double const bx = p_k(0u) - p_j(0u);
          double const by = p_k(1u) - p_j(1u);
          double const cx = p_k(0u) - p_i(0u);
          double const cy = p_k(1u) - p_i(1u);

          double const a_length = sqrt(ax*ax + ay*ay);
          double const b_length = sqrt(bx*bx + by*by);
          double const c_length = sqrt(cx*cx + cy*cy);

          assert(a_length > 0.0 || !"compute_vertex_normals(): internal error");
          assert(b_length > 0.0 || !"compute_vertex_normals(): internal error");
          assert(c_length > 0.0 || !"compute_vertex_normals(): internal error");

          double const ea_x =  ax / a_length;
          double const ea_y =  ay / a_length;

          double const eb_x =  bx / b_length;
          double const eb_y =  by / b_length;

          bool   const flip = should_flip_edge_order(ea_x, ea_y, eb_x, eb_y, na_x, na_y, nb_x, nb_y);
          double const sign = flip ? -1.0 : 1.0;

          double const cross_product = sign*(ea_x*eb_y - eb_x*ea_y);   // sin theta
          double const dot_product   =      (ea_x*eb_x + ea_y*eb_y);   // cos theta
          double       theta         = atan2(cross_product,  dot_product);

          // Make sure angle is interval [0.0..2pi[, atan2 returns in [-pi..pi[
          while(  theta < 0.0)
          {
            theta += 2.0*PI();
          }

          curvature[idx]            = -cross_product/c_length;
          vertex_normal_x[idx]      = n_x;
          vertex_normal_y[idx]      = n_y;
          vertex_strict_convex[idx] = cross_product>10e-10;
          vertex_theta[idx]         = theta;
        }
      }
    }

  }// end namespace details


  /**
   *
   * @param use_normals      Boolean flag that indicates whether this
   *                         factory function should compute normal (geometry)
   *                         information. If turned off then the factory function
   *                         will only extract coordinate information.
   */
  inline Domain make_domain(
                            grit::engine2d_type const & engine
                            , unsigned int      const & label_liquid
                            , unsigned int      const & label_solid
                            , bool              const & use_normals = true
                            )
  {
    Domain domain;

    domain.m_label_liquid = label_liquid;
    domain.m_label_solid  = label_solid;

    grit::SimplexSet const liquid_set     = grit::compute_phase(     engine.mesh(), label_liquid );
    grit::SimplexSet const solid_set      = grit::compute_phase(     engine.mesh(), label_solid  );
    grit::SimplexSet const interface_set  = grit::compute_interface( engine.mesh(), liquid_set   );
    grit::SimplexSet const boundary_set   = grit::compute_boundary(  engine.mesh());

    domain.m_all       = glue::make_phase(engine);
    domain.m_liquid    = glue::make_phase(engine, liquid_set   );
    domain.m_solid     = glue::make_phase(engine, solid_set    );
    domain.m_interface = glue::make_phase(engine, interface_set);
    domain.m_boundary  = glue::make_phase(engine, boundary_set );

    domain.m_interface.m_labels.push_back(label_solid);
    domain.m_interface.m_labels.push_back(label_liquid);

    if(use_normals)
    {
      details::compute_edge_normals(
                                    engine
                                    , label_liquid
                                    , interface_set
                                    , domain.m_interface_edge_normals_x
                                    , domain.m_interface_edge_normals_y
                                    );

      details::compute_vertex_normals(
                                      engine
                                      , label_liquid
                                      , interface_set
                                      , domain.m_interface_vertex_normals_x
                                      , domain.m_interface_vertex_normals_y
                                      , domain.m_interface_vertex_strict_convex
                                      , domain.m_interface_vertex_theta
                                      , domain.m_curvature
                                      );
    }

    glue::get_sub_range_current(
                                engine
                                , domain.m_interface
                                , domain.m_interface_x
                                , domain.m_interface_y
                                );

    glue::get_sub_range_current(
                                engine
                                , domain.m_all
                                , domain.m_x
                                , domain.m_y
                                );

    return domain;
  }

} // end namespace stefan

// STEFAN_MAKE_DOMAIN_H
#endif
