#ifndef GRIT_EDGE_SPLIT_OPERATION_H
#define GRIT_EDGE_SPLIT_OPERATION_H

#include <grit_interface_mesh_operation.h>
#include <grit_interface_mesh.h>

#include <grit_logic_expressions.h>
#include <grit_simplex.h>
#include <grit_simplex_set.h>
#include <grit_simplex_set_functions.h>

#include <utilities/grit_extract_simplices.h>

#include <util_log.h>

#include <string>
#include <map>


namespace grit
{
  namespace details
  {
    ///
    template< typename types>
    class EdgeSplitOperation
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

      EdgeSplitOperation(
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
                                           Simplex1                 const & simplex
                                           , InterfaceMesh                & mesh
                                           , AMT                    const & attributes
                                           , SimplexSet                   & new_simplices
                                           , SimplexSet                   & old_simplices
                                           , std::map<Simplex1, Simplex1> & simplex1_lut
                                           , std::map<Simplex2, Simplex2> & simplex2_lut
                                           )
      {
        if( IsSubmeshBoundary(mesh)(simplex) || !IsValid(mesh)(simplex))
        {
          return false;
        }

        SimplexSet const C = mesh.closure(simplex);
        SimplexSet const S = mesh.star(simplex);

        //--- Insert a new vertex
        Simplex0   const m = mesh.insert();

        //--- Find the vertices of simplex
        Simplex0 v1, v2;
        extract_simplices(v1, v2, C);

        //--- Entire star of simplex is to be removed.
        old_simplices.insert(S);
        new_simplices.insert(m);

        SimplexSet::simplex2_const_iterator it = S.begin2();

        for( ; it != S.end2(); ++it)
        {
          Simplex2 const & t = *it;

          SimplexSet A = filter( mesh.full_boundary(t), 0u);

          A = difference( A, C);

          assert( A.size(0u) == 1u || "EdgeSplitOperation::plan_local_connectivity_changes(): incorrect number of vertices.");

          Simplex0 const vo = *A.begin0();

          Simplex2 n1(vo, m, v1);
          new_simplices.insert(n1);
          simplex2_lut[n1] = t;

          Simplex2 n2(vo, m, v2);
          new_simplices.insert(n2);
          simplex2_lut[n2] = t;

          new_simplices.insert(Simplex1(m, vo));

          if( !m_parameters.use_sparse_edge_attributes())
          {
            simplex1_lut[Simplex1(m, vo)] = simplex;
          }

          new_simplices.insert(Simplex1(m, v1));
          simplex1_lut[Simplex1(m, v1)] = simplex;

          new_simplices.insert(Simplex1(m, v2));
          simplex1_lut[Simplex1(m, v2)] = simplex;
        }

        return true;
      }

    }; // class EdgeSplitOperation
  } // end namespace details
} // end namespace grit

// GRIT_EDGE_SPLIT_OPERATION_H
#endif
