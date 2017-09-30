#ifndef GRIT_VERTEX_SPLIT_OPERATION_H
#define GRIT_VERTEX_SPLIT_OPERATION_H

#include <grit_interface_mesh_operation.h>

#include <grit_simplex_set.h>

#include <utilities/grit_extract_simplices.h>

namespace grit
{
  namespace details
  {
    template< typename types>
    class VertexSplitOperation
    : public InterfaceMeshOperation< Simplex0, types>
    {
    protected:

      typedef          InterfaceMeshOperation<
                                              Simplex0
                                              , types
                                              >        base_class;

      typedef typename types::real_type                T;
      typedef typename types::vector3_type             V;
      typedef typename types::math_types               MT;
      typedef typename types::attributes_type          AT;
      typedef typename types::param_type               PT;

      typedef typename V::value_traits                 VT;

      typedef typename base_class::simplex1_lut_type   simplex1_lut_type;
      typedef typename base_class::simplex2_lut_type   simplex2_lut_type;

    protected:

      std::string  const & m_name;                  ///< Name used to retrieve parameters.

      unsigned int const   m_label;                 ///< Label of the phase on which operation is performed.

      PT           const & m_parameters;            ///< Parameters container.

    public:

      VertexSplitOperation(
                           std::string    const & name
                           , unsigned int const   label
                           , PT           const & parameters
                           )
      : m_name(name)
      , m_label(label)
      , m_parameters(parameters)
      { }

    public:

      void init(
                InterfaceMesh const & mesh
                , AT                & attributes
                )
      { }

    public:

      bool update_local_attributes(
                                   Simplex0        const & simplex
                                   , InterfaceMesh const & mesh
                                   , AT                  & attributes
                                   )
      {
        return false;
      }

    public:

      bool plan_local_connectivity_changes(
                                           Simplex0            const & simplex
                                           , InterfaceMesh           & mesh
                                           , AT                const & attributes
                                           , SimplexSet              & new_simplices
                                           , SimplexSet              & old_simplices
                                           , simplex1_lut_type       & simplex1_lut
                                           , simplex2_lut_type       & simplex2_lut
                                           )
      {
        util::Log           log;
        std::string const & newline = util::Log::newline();

        //--- Vertex split is only performed on interface vertices which are not
        //--- on submesh boundary.
        if( IsSubmeshBoundary(mesh)(simplex) || !IsInterface(mesh)(simplex))
        {
          return false;
        }

        //--- Get all labels from the 1-ring of simplex.
        std::vector<unsigned int> const & labels = attributes.get_simplex0_labels(simplex);

        //--- Only one phase assigned (the other phase is ambient).
        if( labels.size() < 2u)
        {
          return false;
        }

        SimplexSet const S  = mesh.star(simplex);
        //--- All edges in the 1-ring
        SimplexSet const E  = filter( S, IsDimension( mesh, 1u));
        //--- All interface edges in the 1-ring, in phase specified by m_label
        SimplexSet const PE = filter( E, InPhase( mesh, m_label) && IsInterface(mesh));

        if( PE.size(1u) != 2u)
        {
          log << "plan_local_connectivity_changes(): non-manifold configuration detected, vertex split not performed" << newline;
          return false;
        }

        //-- An object with phase m_label here detaches from the surrounding phases.
        //-- @param simplex remains to be "attached" to all other phases than m_label.
        //-- A new simplex0 is inserted, which will belong to phase m_label.
        Simplex0 w = mesh.insert();

        new_simplices.insert(w);

        mesh.submesh_boundary(w) = false;

        //--- We need it to fix the attributes for this vertex
        new_simplices.insert(simplex);

        SimplexSet::simplex1_const_iterator it=S.begin1();

        for( ; it!=S.end1(); ++it)
        {
          Simplex1 const & e = *it;

          if( InPhase( mesh, m_label)(e))
          {
            replace_edge( simplex, w, e, mesh, new_simplices, old_simplices, simplex1_lut);
          }
        }

        Simplex1 const edge( w, simplex);

        new_simplices.insert(edge);

        for( SimplexSet::simplex2_const_iterator it=S.begin2(); it!=S.end2(); ++it)
        {
          Simplex2 const & t = *it;

          if( InPhase(mesh, m_label)(t))
          {
            replace_triangle( simplex, w, t, mesh, new_simplices, old_simplices, simplex2_lut);
          }
        }

        return true;
      }

    protected:

      void replace_edge(
                        Simplex0            const & simplex
                        , Simplex0          const & w
                        , Simplex1          const & e
                        , InterfaceMesh           & mesh
                        , SimplexSet              & new_simplices
                        , SimplexSet              & old_simplices
                        , simplex1_lut_type       & simplex1_lut
                        )
      {
        Simplex0         v1, v2;

        SimplexSet const C = mesh.closure(e);

        extract_simplices(v1, v2, C);

        Simplex0 const v = (v1==simplex)?(v2):(v1);

        new_simplices.insert(Simplex1(v,w));
        simplex1_lut[ Simplex1(v,w) ] = e;

        if( IsInterface(mesh)(e))
        {
          Simplex2 const air(v1,v2,w);
          new_simplices.insert( air );

          //--- Leaving assignment empty, making sure that Mesh and AttributeAssignment can handle it
          //--- We can't pick a parent for this Simplex2
          //simplex2_lut[ air ] = ???

          Simplex1 const edge(w,simplex);
          new_simplices.insert( edge );
          simplex1_lut[ edge ] = e;
        }
        else
        {
          old_simplices.insert( e );
        }
      }

    protected:

      void replace_triangle(
                            Simplex0            const & simplex
                            , Simplex0          const & w
                            , Simplex2          const & t
                            , InterfaceMesh           & mesh
                            , SimplexSet              & new_simplices
                            , SimplexSet              & old_simplices
                            , simplex2_lut_type       & simplex2_lut
                            )
      {
        Simplex0 v1, v2, v3;

        SimplexSet const C = mesh.closure(t);

        extract_simplices( v1, v2, v3, C);

        if( v1 == simplex) v1 = w;
        if( v2 == simplex) v2 = w;
        if( v3 == simplex) v3 = w;

        Simplex2 const n(v1, v2, v3);

        old_simplices.insert(t);
        new_simplices.insert(n);

        simplex2_lut[ n ] = t;
      }

    }; // class VertexSplitOperation

  } // namespace details

} // namespace grit

// GRIT_VERTEX_SPLIT_H
#endif
