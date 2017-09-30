#ifndef GRIT_LAPLACIAN_SMOOTHING_OPERATION
#define GRIT_LAPLACIAN_SMOOTHING_OPERATION

#include <grit_interface_mesh.h>
#include <grit_interface_mesh_operation.h>

#include <grit_logic_expressions.h>
#include <grit_simplex.h>
#include <grit_simplex_set.h>
#include <grit_simplex_set_functions.h>

#include <utilities/grit_accumulate_attributes.h>
#include <utilities/grit_is_any_inverted.h>

#include <util_log.h>
#include <util_barycentric.h>
#include <util_barycentric_inside.h>

#include <string>
#include <cassert>

namespace grit
{
  namespace details
  {
    /**
     * Implementation of the smart Laplacian smoothing operation with linear interpolation of nodal attributes.
     * Each non-interface, non-boundary vertex is moved towards the barycenter of its neighbors. The operation
     * is accepted if it does not invert any triangles.
     */
    template< typename types>
    class LaplacianSmoothingOperation
    : public InterfaceMeshOperation< Simplex0, types>
    {
    protected:

      typedef typename types::real_type               T;
      typedef typename types::vector3_type            V;
      typedef typename types::math_types              MT;
      typedef typename types::attributes_type         AT;
      typedef typename types::param_type              PT;

      typedef typename V::value_traits                VT;

      typedef typename AT::name_iterator              name_iterator;

      typedef Simplex0AttributeVector<V>              CT;

    protected:

      std::string  const & m_name;                  ///< Name used to retrieve parameters.

      unsigned int const   m_label;                 ///< Label of the submesh on which operation is performed.

      PT           const & m_parameters;            ///< Parameters container.

      T            const   m_strength;              ///< Strength of Laplacian smoothing (0 for no smoothing, 1 for for moving vertex to barycenter of neighbors in one. step)

      CT                   m_new_positions;         ///< precomputed new positions for the vertices

    public:

      LaplacianSmoothingOperation(
                                  std::string    const & operation_name
                                  , unsigned int const   label
                                  , PT           const & parameters
                                  )
      : m_name( operation_name)
      , m_label( label)
      , m_parameters( parameters)
      , m_strength( parameters.get_strength( operation_name, label))
      { }


      void init(
                InterfaceMesh const & mesh
                , AT                & attributes
                )
      {
        SimplexSet const phase = compute_phase( mesh, m_label);

        SimplexSet const bulk = filter( phase, !( IsSubmeshBoundary(mesh)
                                               || IsInterface(mesh)
                                               || IsBoundary(mesh) ));

        for (SimplexSet::simplex0_const_iterator it = bulk.begin0(); it != bulk.end0(); ++it)
        {
          Simplex0 const & simplex = *it;

          SimplexSet const link0 = filter( mesh.link(simplex), IsDimension( mesh, 0u));

          int const n = link0.size(0u);

          if (n == 0u)
            return;

          V const barycenter = accumulate_attributes( attributes.current(), link0)/n;

          m_new_positions[simplex] = m_strength*barycenter+(VT::one()-m_strength)*attributes.get_current_value(simplex);
        }
      } // end init()


      bool update_local_attributes(
                                    Simplex0      const & simplex
                                  , InterfaceMesh const & mesh
                                  , AT                  & attributes
                                  )
      {
        if(
           IsSubmeshBoundary(mesh)(simplex)
           || IsInterface(mesh)(simplex)
           || IsBoundary(mesh)(simplex)
           || !IsValid(mesh)(simplex)
           )
        {
          return false;
        }

        V const & new_pos = m_new_positions[simplex];

        SimplexSet const S = filter( mesh.star(simplex), IsDimension( mesh, 2u));

        if (is_any_inverted( simplex, S, attributes, new_pos))
        {
          return false;
        }

        //--- Attribute interpolation
        if ((!m_parameters.use_ambient()) || (m_label != m_parameters.ambient_label()))
        {
          for (SimplexSet::simplex2_const_iterator it = S.begin2(); it != S.end2(); ++it)
          {
            Simplex2 const s2 = *it;

            std::vector<Simplex0> vertices(3u);
            std::vector<V>        positions(3u);

            for (unsigned int i = 0u; i < 3u; ++i)
            {
              vertices[i]  = s2.get_simplex_from_local_index(i);
              positions[i] = attributes.get_current_value(vertices[i]);
            }

            std::vector<T> b_coords( 3u, VT::zero() );

            util::barycentric(
                              positions[0u], positions[1u], positions[2u]
                              , new_pos
                              , b_coords[0u], b_coords[1u], b_coords[2u]
                              );

            if (util::barycentric_inside( b_coords[0u], b_coords[1u], b_coords[2u]))
            {
              std::vector< std::string> const & attribute_names = attributes.simplex0_attribute_names();

              name_iterator name_it = attribute_names.begin();

              for ( ; name_it != attribute_names.end(); ++name_it)
              {
                std::string const & attribute_name = *name_it;

                T val = b_coords[0u] * attributes.get_attribute_value( attribute_name, vertices[0u], m_label)
                      + b_coords[1u] * attributes.get_attribute_value( attribute_name, vertices[1u], m_label)
                      + b_coords[2u] * attributes.get_attribute_value( attribute_name, vertices[2u], m_label);

                attributes.set_attribute_value( *name_it, simplex, m_label, val);
              }

              break;
            }
          }
        }

        attributes.set_current_value( simplex, new_pos);

        return true;
      }


      bool plan_local_connectivity_changes(
                                           Simplex0                 const & /* simplex */
                                           , InterfaceMesh                & /* mesh */
                                           , AT                     const & /* attributes */
                                           , SimplexSet                   & /* new_simplices */
                                           , SimplexSet                   & /* old_simplices */
                                           , std::map<Simplex1, Simplex1> & /* simplex1_lut */
                                           , std::map<Simplex2, Simplex2> & /* simplex2_lut */
                                           )
      {
        return false;
      }

    }; // class LaplacianSmoothingOperation

  } // end namespace details
} // end namespace grit


// GRIT_LAPLACIAN_SMOOTHING_OPERATION
#endif

