#ifndef GRIT_EDGE_FLIP_OPERATION_H
#define GRIT_EDGE_FLIP_OPERATION_H

#include <grit_interface_mesh_operation.h>
#include <grit_interface_mesh.h>

#include <grit_logic_expressions.h>
#include <grit_simplex.h>
#include <grit_simplex_set.h>
#include <grit_simplex_set_functions.h>

#include <utilities/grit_extract_simplices.h>
#include <utilities/grit_is_any_inverted.h>

#include <string>
#include <map>

namespace grit
{
  namespace details
  {
    ///
    template< typename types>
    class EdgeFlipOperation
    : public InterfaceMeshOperation< Simplex1, types>
    {
    protected:

      typedef typename types::real_type               T;
      typedef typename types::vector3_type            V;
      typedef typename V::value_traits                VT;

      typedef typename types::math_types              MT;

      typedef typename types::attributes_type         AMT;
      typedef typename types::param_type              PT;

    protected:

      std::string  const & m_name;                  //!< Name used to retrieve parameters.
      unsigned int const   m_label;                 //!< Label of the phase on which operation is performed.
      PT           const & m_parameters;            //!< Parameters container.

    public:

      EdgeFlipOperation(
                    std::string    const & name
                    , unsigned int const   label
                    , PT           const & parameters
                    )
      : m_name( name)
      , m_label( label)
      , m_parameters( parameters)
      { }

      void init(
                 InterfaceMesh  const & mesh
               , AMT  & attributes
               )
      { }

      bool update_local_attributes(
                                    Simplex1 const & simplex
                                  , InterfaceMesh const & mesh
                                  , AMT & attributes
                                  )
      {
        return false;
      }

      bool plan_local_connectivity_changes(
                                           Simplex1               const & simplex
                                           , InterfaceMesh                & mesh
                                           , AMT                    const & attributes
                                           , SimplexSet                   & new_simplices
                                           , SimplexSet                   & old_simplices
                                           , std::map<Simplex1, Simplex1> & simplex1_lut
                                           , std::map<Simplex2, Simplex2> & simplex2_lut
                                           )
      {
        if( IsInterface(mesh)(simplex) || IsSubmeshBoundary(mesh)(simplex) || IsBoundary(mesh)(simplex) || !IsValid(mesh)(simplex))
          return false;

        SimplexSet Vs = filter(mesh.boundary(simplex), IsDimension( mesh, 0u)); // Vs: vertices of input edge
        SimplexSet T  = mesh.star(simplex); // T: triangles adjacent to edge

        assert(T.size(2u) == 2u || !"EdgeFlipOperation::plan_local_connectivity_changes(): Edge must be shared between exactly two triangles (is edge on boundary?)");

        // 2016-03-07 Marek: this is probably not needed anymore - if something gets inverted, the code will fail anyway
        //                   No need to spend CPU time on this test.
        // If there's an inverted triangle in the original mesh, no topological operations should be performed.
        //if (is_any_inverted<V>( T, attributes)) return false;

        SimplexSet V2 = filter( mesh.closure(T), IsDimension( mesh, 0u)); // V2: boundary of triangles in T.

        SimplexSet W = difference(V2, Vs); // W: opposite vertices

        assert(W.size(0u) == 2u || !"EdgeFlipOperation::plan_local_connectivity_changes(): looking for exactly two opposite vertices");

        assert( old_simplices.size(0u) == 0u || !"EdgeFlipOperation::plan_local_connectivity_changes(): used with not-empty simplex set.");
        assert( old_simplices.size(1u) == 0u || !"EdgeFlipOperation::plan_local_connectivity_changes(): used with not-empty simplex set.");
        assert( old_simplices.size(2u) == 0u || !"EdgeFlipOperation::plan_local_connectivity_changes(): used with not-empty simplex set.");

        assert( new_simplices.size(0u) == 0u || !"EdgeFlipOperation::plan_local_connectivity_changes(): used with not-empty simplex set.");
        assert( new_simplices.size(1u) == 0u || !"EdgeFlipOperation::plan_local_connectivity_changes(): used with not-empty simplex set.");
        assert( new_simplices.size(2u) == 0u || !"EdgeFlipOperation::plan_local_connectivity_changes(): used with not-empty simplex set.");

        Simplex0 v1, v2, w1, w2;
        extract_simplices(v1, v2, Vs);
        extract_simplices(w1, w2, W);

        Simplex2 t1, t2;
        extract_simplices( t1, t2, T);

        old_simplices.insert(T);

        Simplex2 n1 = Simplex2(w1, w2, v1);
        new_simplices.insert(n1);
        // 2015-11-04 Marek: todo, make the assignment less arbitrary, based on overlap of smth like that
        simplex2_lut[n1] = t1;

        Simplex2 n2 = Simplex2(w1, w2, v2);
        new_simplices.insert(n2);
        // 2015-11-04 Marek: todo, make the assignment less arbitrary, based on overlap of smth like that
        simplex2_lut[n2] = t2;

        new_simplices.insert(Simplex1(w1, w2));
        simplex1_lut[Simplex1(w1, w2)] = simplex;

        // 2015-11-04 Marek: to prevent the OpenTissueMesh error.
        // 2016-08-08 Marek: this doesn't seem to be sufficient - we probably need a threshold, but it's a bit risky
        //                   (bounds the algorithm to a certain scale).
        if (util::TriangleArea()(
                                   attributes.get_current_value(w1)
                                 , attributes.get_current_value(w2)
                                 , attributes.get_current_value(v1)
                                 ) <= VT::zero()
            || util::TriangleArea()(
                                    attributes.get_current_value(w1)
                                    , attributes.get_current_value(w2)
                                    , attributes.get_current_value(v2)
                                    ) <= VT::zero())
        {
          return false;
        }

        return true;
      }

    }; // class EdgeFlipOperation

  } // end namespace details
} // end namespace grit

// GRIT_EDGE_FLIP_OPERATION_H
#endif
