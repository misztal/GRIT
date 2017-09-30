#ifndef GRIT_FIND_COLLAPSABLE_VERTICES_H
#define GRIT_FIND_COLLAPSABLE_VERTICES_H

#include <grit_simplex.h>
#include <grit_simplex_set.h>
#include <grit_interface_mesh.h>
#include <grit_logic_expressions.h>

#include <utilities/grit_extract_simplices.h>

namespace grit
{
  namespace details
  {
    /**
     * Returns false if
     * a) vertex v is on boundary, and e is non-boundary edge
     * b) if either vertex v or vo of edge e are on submesh boundary
     * c) if collapsing v onto vo would produce ears (if the parameters.no_ears() flag is on)
     */
    bool preserves_boundary(
                            Simplex0        const & v  ///< a vertex of edge e
                            , Simplex0      const & vo ///< simplex0 opposite to v in e
                            , Simplex1      const & e  ///< simplex1 to be collapsed
                            , SimplexSet    const & C  ///< closure of an edge containing v
                            , SimplexSet    const & L  ///< link of v
                            , InterfaceMesh const & mesh
                            , Parameters    const & parameters
                            )
    {
      //--- Collapsing submesh boundary vertices, as well as collapsing onto
      //--- submesh boundary vertices can modify attribute values at those vertices
      //--- an hence is disallowed.
      if( IsSubmeshBoundary(mesh)(v) || IsSubmeshBoundary(mesh)(vo))
      {
        return false;
      }
      else if( IsBoundary(mesh)(v) && !IsBoundary(mesh)(e))
      {
        return false;
      }

      return true;
    }

    /**
     * Returns true if
     * a) vertex v of edge e is non-interface
     * b) vertex v is on the interface, e is also an interface edge, and v is 1-manifold.
     * Otherwise returns false.
     */
    bool preserves_interface(
                             Simplex0        const & v  ///< a vertex of e
                             , Simplex1      const & e  ///< simplex1 to be collapsed
                             , InterfaceMesh const & mesh
                             )
    {
      if( IsInterface(mesh)(v))
      {
        if( IsInterface(mesh)(e))
        {
          //--- Checking if the interface is 1-manifold around v
          SimplexSet const interface_edges = filter(
                                                    mesh.star(v)
                                                    , IsDimension( mesh, 1u)
                                                      && IsInterface( mesh)
                                                    );

          if( interface_edges.size(1u) != 2u)
          {
            return false;
          }

          return true;
        }

        return false;
      }

      return true;
    }

    /**
     * This function finds the set of vertices in the boundary
     * of simplex which can be collapsed without destroying mesh
     * topology and without deforming the interface or mesh boundary.
     */
    SimplexSet find_collapsable_vertices(
                                         Simplex1        const & simplex             ///< an edge to be collapsed
                                         , InterfaceMesh const & mesh
                                         , Parameters    const & parameters
                                         , bool          const   preserve_interface  ///< a flag specifying whether to perform
                                                                                     ///< interface preserving test (yes if true)
                                         )
    {
      SimplexSet result;

      //--- We can never change submesh boundary
      if( IsSubmeshBoundary(mesh)(simplex) || !IsValid(mesh)(simplex))
      {
        return result;
      }

      //--- Checking the condition for edge collapse, which ensures the topological
      //--- validity of the operation link(simplex) = intersection(link(v1), link(v2))
      SimplexSet const C = mesh.closure(simplex);
      SimplexSet const S = mesh.star(simplex);

      Simplex0 v1, v2;
      extract_simplices(v1, v2, C);

      SimplexSet const L1 = mesh.link(v1);
      SimplexSet const L2 = mesh.link(v2);
      SimplexSet const X  = intersection(L1,L2);
      SimplexSet const SC = mesh.star(C);
      SimplexSet const CS = mesh.closure(S);
      SimplexSet const L  = difference( CS, SC);
      SimplexSet const D  = difference(X, L);

      if( D.size(0u) != 0u)
      {
        return result;
      }

      //--- Finding the subset of vertices in C, which can be collapsed
      result.insert(C);

      if( !preserves_boundary( v1, v2, simplex, C, L1, mesh, parameters)) result.remove(v1);
      if( !preserves_boundary( v2, v1, simplex, C, L2, mesh, parameters)) result.remove(v2);

      if( preserve_interface)
      {
        if( !preserves_interface( v1, simplex, mesh)) result.remove(v1);
        if( !preserves_interface( v2, simplex, mesh)) result.remove(v2);
      }

      return result;
    }

  } // namespace details

} // namespace grit

// GRIT_FIND_COLLAPSABLE_VERTICES_H
#endif
