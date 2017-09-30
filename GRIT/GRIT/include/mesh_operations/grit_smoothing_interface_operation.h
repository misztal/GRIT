#ifndef GRIT_SMOOTHING_INTERFACE_OPERATION
#define GRIT_SMOOTHING_INTERFACE_OPERATION

#include <grit_interface_mesh_operation.h>
#include <grit_interface_mesh.h>

#include <grit_attribute_vector.h>

#include <grit_logic_expressions.h>
#include <grit_simplex.h>
#include <grit_simplex_set.h>
#include <grit_simplex_set_functions.h>

#include <utilities/grit_extract_simplices.h>
#include <utilities/grit_is_any_inverted.h>

#include <util_log.h>

#include <string>
#include <vector>
#include <map>

namespace grit
{

  namespace details
  {
    template< typename types>
    class SmoothingInterfaceOperation
    : public InterfaceMeshOperation< Simplex0, types>
    {
    protected:

      typedef typename types::real_type        T;
      typedef typename types::vector3_type     V;
      typedef typename types::math_types       MT;
      typedef typename types::attributes_type  AT;
      typedef typename types::param_type       PT;

      typedef typename V::value_traits         VT;

      typedef Simplex0AttributeVector<V>       CT;

    protected:

      std::string  const & m_name;          ///< name used to retrieve parameters

      unsigned int const   m_label;         ///< label of the submesh on which operation is performed

      PT           const & m_parameters;    ///< parameters container

      T            const   m_strength;      ///< strength of smoothing (0 for no smoothing, 1 for for moving vertex to barycenter of neighbors in one step)

      CT                   m_new_positions; ///< precomputed new positions for the vertices

    public:

      SmoothingInterfaceOperation(
                                  std::string    const & operation_name
                                  , unsigned int const   label
                                  , PT           const & parameters
                                  )
      : m_name( operation_name )
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

        SimplexSet const interface = filter(
                                            compute_interface( mesh, phase)
                                            , IsDimension(mesh,0u)
                                              && !IsSubmeshBoundary(mesh)
                                              && !IsBoundary(mesh)
                                            );

        for (SimplexSet::simplex0_const_iterator it = interface.begin0(); it != interface.end0(); ++it)
        {
          Simplex0   const & simplex = *it;
          V          const & v       = attributes.get_current_value(simplex);

          //--- find interface edges adjacent to simplex
          SimplexSet const S = filter( mesh.star(simplex), IsDimension( mesh, 1u) && IsInterface(mesh));

          //--- find neighboring vertices along the interface
          SimplexSet       C = filter( mesh.closure(S), IsDimension( mesh, 0u));

          C.remove(simplex);

          if (C.size(0u) != 2u)
          {
            m_new_positions[simplex] = v;
            continue;
          }

          Simplex0 p,q;
          extract_simplices(p,q,C);

          V const & u  = attributes.get_current_value(p);
          V const & w  = attributes.get_current_value(q);

          V const barycenter = VT::half()*(u+w);

          m_new_positions[simplex] = (VT::one()-m_strength)*v+m_strength*barycenter;
        }
      }


      bool update_local_attributes(
                                   Simplex0        const & simplex
                                   , InterfaceMesh const & mesh
                                   , AT                  & attributes
                                   )
      {
        if(
           IsSubmeshBoundary(mesh)(simplex)
           || !IsInterface(mesh)(simplex)
           || IsBoundary(mesh)(simplex)
           || !IsValid(mesh)(simplex)
           )
          return false;

        V const & new_pos = m_new_positions[simplex];

        SimplexSet const S = mesh.star(simplex);

        //--- extract faces
        SimplexSet const F = filter( S, IsDimension( mesh, 2u));

        //--- extract interface edges
        SimplexSet const E = filter( S, IsDimension( mesh, 1u) && IsInterface( mesh));

        if( is_any_inverted( simplex, F, attributes, new_pos))
        {
          return false;
        }

        //--- Attribute interpolation
        if( !m_parameters.use_ambient() || (m_label != m_parameters.ambient_label()))
        {
          for( SimplexSet::simplex1_const_iterator it = E.begin1(); it != E.end1(); ++it)
          {
            Simplex1 const & s1 = *it;

            std::vector<Simplex0> vertices(2u);
            std::vector<V>        positions(2u);

            for (unsigned int i = 0u; i < 2u; ++i)
            {
              vertices[i]  = s1.get_simplex_from_local_index(i);
              positions[i] = attributes.get_current_value(vertices[i]);
            }

            std::vector<T> b_coords( 2u, VT::zero());

            util::barycentric(
                              positions[0u], positions[1u]
                              , new_pos
                              , b_coords[0u], b_coords[1u]
                              );

            if (util::barycentric_inside( b_coords[0u], b_coords[1u]))
            {
              std::vector< std::string> const & attribute_names = attributes.simplex0_attribute_names();

              std::vector< std::string>::const_iterator name_it = attribute_names.begin();

              for ( ; name_it != attribute_names.end(); ++name_it)
              {
                std::string const & attribute_name = *name_it;

                T const val = b_coords[0u] * attributes.get_attribute_value( attribute_name, vertices[0u], m_label)
                            + b_coords[1u] * attributes.get_attribute_value( attribute_name, vertices[1u], m_label);

                attributes.set_attribute_value( attribute_name, simplex, m_label, val);
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

  };

  } // end namespace details

} // end namespace grit

// GRIT_SMOOTHING_INTERFACE_OPERATION
#endif
