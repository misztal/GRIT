#ifndef GRIT_DELAUNAY_QUALITY_MESAURE_H
#define GRIT_DELAUNAY_QUALITY_MESAURE_H

#include <grit_interface_mesh.h>
#include <grit_interface_quality_measure.h>

#include <grit_logic_expressions.h>
#include <grit_simplex.h>
#include <grit_simplex_set.h>
#include <grit_simplex_set_functions.h>

#include <utilities/grit_extract_simplices.h>

#include <util_is_ccw.h>
#include <util_determinant.h>
#include <util_triangle_area.h>

namespace grit
{
  namespace details
  {

    template <typename types>
    class DelaunayQualityMeasure
    : public InterfaceQualityMeasure<Simplex1>
    {
    protected:

      typedef typename types::attributes_type  AMT;
      typedef typename types::param_type       PT;
      typedef typename types::real_type        T;
      typedef typename types::vector3_type     V;

    protected:

      InterfaceMesh const & m_mesh;
      AMT     const & m_attributes;

    protected:

      V const & get_coordinates( Simplex0 const & s) const
      {
        return this->m_attributes.get_current_value( s );
      }

      bool should_flip(
                   Simplex0 const & A
                   , Simplex0 const & B
                   , Simplex0 const & C
                   , Simplex0 const & D
                   ) const
      {
        V       a = get_coordinates(A);
        V       b = get_coordinates(B);
        V       c = get_coordinates(C);
        V const d = get_coordinates(D);

        if (!util::is_ccw(a, b, c))
        {
          V const temp = c;
          c = b;
          b = temp;
        }

        T const dxdx = d[0] * d[0];
        T const dydy = d[1] * d[1];

        T const value = util::determinant(
                            a[0] - d[0], a[1] - d[1], (a[0]*a[0] - dxdx) + (a[1]*a[1] - dydy),
                            b[0] - d[0], b[1] - d[1], (b[0]*b[0] - dxdx) + (b[1]*b[1] - dydy),
                            c[0] - d[0], c[1] - d[1], (c[0]*c[0] - dxdx) + (c[1]*c[1] - dydy)
                            );

		return value > 0;
      }

    public:

      DelaunayQualityMeasure(
                             std::string    const & name
                             , unsigned int const & label
                             , PT           const & parameters
                             , InterfaceMesh      const & mesh
                             , AMT          const & attributes
                             )
      : m_mesh(mesh)
      , m_attributes(attributes)
      {}

      bool is_bad(Simplex1 const & s) const
      {


        SimplexSet const Ts = m_mesh.star(s);

        if (Ts.size(2u) < 2u)
          return false;

		SimplexSet::simplex2_const_iterator it = Ts.begin2();
		Simplex2 t1 = *it;
		++it;
		Simplex2 t2 = *it;

        //--- 2018-04-09 Marek: this helps preventing the polymesh assertion fail.
		if (util::SignedTriangleArea()(
		                               get_coordinates(t1.get_idx0())
			                         , get_coordinates(t1.get_idx1())
			                         , get_coordinates(t1.get_idx2())
		                             ) <= 0.)
		{
          return false;
		}

        if (util::SignedTriangleArea()(
		                               get_coordinates(t2.get_idx0())
			                         , get_coordinates(t2.get_idx1())
			                         , get_coordinates(t2.get_idx2())
		                             ) <= 0.)
		{
          return false;
		}

        SimplexSet Vs = m_mesh.closure(Ts);

        Vs = difference(Vs, m_mesh.boundary(s)); // V is two opposite vertices

        Simplex0 v1;
        Simplex0 v2;

        extract_simplices(v1, v2, Vs);

        return should_flip(s.get_simplex0_idx0(), s.get_simplex0_idx1(), v1, v2);
      }
    };

  } // end namespace details
}//end namespace grit

// GRIT_DELAUNAY_QUALITY_MESAURE_H
#endif
