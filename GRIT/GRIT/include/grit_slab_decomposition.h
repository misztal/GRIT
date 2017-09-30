#ifndef GRIT_SLAB_DECOMPOSITION_H
#define GRIT_SLAB_DECOMPOSITION_H

#include <grit_attribute_vector.h>
#include <grit_logic_expressions.h>
#include <grit_simplex.h>
#include <grit_simplex_set.h>
#include <grit_simplex_set_functions.h>

#include <grit_interface_domain_decomposition.h>

#include <util_integer.h>
#include <util_log.h>

#include <vector>
#include <cassert>

namespace grit
{

  namespace details
  {

    /**
     * Structure used by slab decomposition in order to sort the vertices
     * according to their 0-coordinate.
     */
    template <typename types>
    struct less_than
    {
      typedef typename types::attributes_type AT;

      AT const & m_attributes;

      less_than(AT const & attributes) : m_attributes(attributes) { }

      bool operator() (Simplex0 s0, Simplex0 s1)
      {
        return m_attributes.get_current_value(s0)[0] < m_attributes.get_current_value(s1)[0];
      }

      bool operator() (Simplex2 s0, Simplex2 s1)
      {
        Simplex0 v0 = s0.get_simplex0_idx0();
        Simplex0 v1 = s0.get_simplex0_idx1();
        Simplex0 v2 = s0.get_simplex0_idx2();

        Simplex0 smallest;
        smallest = (*this)(v0, v1) ? v0 : v1;
        smallest = (*this)(smallest, v2) ? smallest : v2;

        v0 = s1.get_simplex0_idx0();
        v1 = s1.get_simplex0_idx1();
        v2 = s1.get_simplex0_idx2();

        return (*this)(smallest, v0) && (*this)(smallest, v1) && (*this)(smallest, v2);
      }
    };


    /*! Implementation of slab decomposition.
     *
     */
    template <typename types>
    class SlabDecomposition
    : public InterfaceDomainDecomposition<types>
    {
    public:

      typedef typename types::mesh_impl               MI;
      typedef typename types::attributes_type         AT;
      typedef typename types::attribute_manager_type  AMT;
      typedef typename types::domain_manager_type     DMT;
      typedef typename types::param_type              PT;

      typedef typename types::vector3_type            V;
      typedef typename types::real_type               T;

    protected:

      std::vector<SimplexSet>                          m_simplex_sets;    ///< container used to store simplices in each of the slabs
      std::vector<Simplex0AttributeVector<Simplex0> >  m_submesh_to_mesh; ///< maps vertex indices from each slab back to corresponding indices in global mesh
      Simplex2AttributeVector<util::Integer >          m_submesh_index;   ///< maps each 2-simplex to the index of a submesh it's been assigned to

    public:

      SlabDecomposition()
      {}

    protected:

      /**
       * Helper function for on_submesh_boundary( Simplex0 const &, MI & const ).
       *  If edge @s lies on a submesh boundary returns the maximum index of
       * the adjacent subdomains. Otherwise returns -1.
       */
      int on_submesh_boundary(
                   Simplex1 const & s
                 , MI       const & mesh
                 )
      {
        SimplexSet simplex_set = mesh.star(s);

        if (simplex_set.size(2) == 1)
          return -1;

        SimplexSet::simplex2_const_iterator it = simplex_set.begin2();
        Simplex2 s1 = *it; ++it;
        Simplex2 s2 = *it;

        if (m_submesh_index[s1].value == m_submesh_index[s2].value)
          return -1;
        else
          // 2015-09-21 Marek: changed to std::max for convenience below (does not affect performance anyhow).
          return std::max(m_submesh_index[s1].value, m_submesh_index[s2].value);
          //return std::min(m_submesh_index[s1].value, m_submesh_index[s2].value);
      }

      /*! Helper function for create_subdomains(...) and merge_subdomains(...).
       *  If edge @s lies on a submesh boundary returns the maximum index of the adjacent subdomains.
       *  Otherwise returns -1.
       */
      int on_submesh_boundary(
                   Simplex0 const & s
                 , MI       const & mesh
                 )
      {
        SimplexSet simplex_set = filter(mesh.star(s), 1);

        int idx = -1;
        for (SimplexSet::simplex1_const_iterator it = simplex_set.begin1(); it != simplex_set.end1(); ++it)
        {
          idx = std::max(on_submesh_boundary(*it,mesh), idx);
        }

        return idx;
      }


    public:

      /*! Performs slab decomposition of @mesh into a number of subdomains, specified by @parameters.number_of_subdomains
       *  Returns DomainManager object, containing all subdomains with separate attribute vectors.
       */
      DMT create_subdomains(
                             PT const & parameters //!< simulation parameters (for number_of_subdomains)
                           , MI const & mesh       //!< global mesh
                           , AT const & attributes //!< global attribute manager
                           )
      {
        static bool is_black = true;

        unsigned int const number_of_subdomains = parameters.number_of_subdomains();

        // Clean up old utility containers
        m_simplex_sets.clear();
        m_submesh_to_mesh.clear();

        SimplexSet simplex_set = filter(mesh.get_all_simplices(), IsDimension(mesh,2));

        std::vector<Simplex2> sorted_triangles;
        sorted_triangles.reserve(simplex_set.size(2));

        std::copy(simplex_set.begin2(), simplex_set.end2(), std::back_inserter(sorted_triangles));

        const less_than<types> lt = less_than<types>(attributes);
        std::sort(sorted_triangles.begin(), sorted_triangles.end(), lt);

        const unsigned int triangles_total = sorted_triangles.size();
        assert(triangles_total > 0 || !"SlabDecomposition::create_submeshes(): Mesh does not contain any triangles");

        const unsigned int subdomain_size  = triangles_total / number_of_subdomains;
        assert(subdomain_size > 0 || !"SlabDecomposition::create_submeshes(): Subdomain size < 1");

        unsigned int remainder       = sorted_triangles.size() % number_of_subdomains;
        assert(remainder < number_of_subdomains || !"SlabDecomposition::create_submeshes(): Wrong remainder size");

        typedef typename std::vector<Simplex2>::iterator iterator;

        std::vector< std::vector<Simplex2> > slabs;

        unsigned int half_begin = subdomain_size / 2;

        iterator begin = sorted_triangles.begin();
        iterator end = begin;

        unsigned int triangles_remaining = triangles_total;

        unsigned int next = is_black ? subdomain_size : half_begin;

        while (triangles_remaining > 0)
        {
          if (remainder > 0)
          {
            ++next;
            --remainder;
          }

          end += next;

          // Create a new simplex set for slab
          SimplexSet simplex_set;

          simplex_set.insert(begin,end);

          if (triangles_remaining - next <= 0 && !is_black) // the last simplex set
          {
            m_simplex_sets[0] = set_union(m_simplex_sets[0], mesh.closure(simplex_set));
          }
          else
          {
            m_simplex_sets.push_back(mesh.closure(simplex_set));
          }

          begin = end;
          triangles_remaining -= next;
          next = std::min(subdomain_size, triangles_remaining);
        }

        assert((number_of_subdomains == m_simplex_sets.size()) || !"SlabDecomposition::create_submeshes(): Wrong number of simplex sets created");

        for (unsigned int i = 0u; i < number_of_subdomains; ++i)
        {
          for (SimplexSet::simplex2_const_iterator it = m_simplex_sets[i].begin2(); it != m_simplex_sets[i].end2(); ++it)
          {
            m_submesh_index[*it].value = i;
          }
        }

        Simplex0AttributeVector<Simplex0> mesh_to_submesh;

        DMT domains(number_of_subdomains);

        for (unsigned int i = 0u; i < number_of_subdomains; ++i)
        {
          m_submesh_to_mesh.push_back(Simplex0AttributeVector<Simplex0>());

          domains.subdomain(i).id()            = i;
          domains.subdomain(i).parameters()    = parameters;
          domains.subdomain(i).mesh().m_coords = &(domains.subdomain(i).attributes().current());

          AMT::copy_attribute_names( attributes, domains.subdomain(i).attributes());

          for (SimplexSet::simplex0_const_iterator it = m_simplex_sets[i].begin0(); it != m_simplex_sets[i].end0(); ++it)
          {
            Simplex0 s = *it;
            Simplex0 s0 = domains.subdomain(i).mesh().insert();
            mesh_to_submesh[s] = s0;

            AMT::copy_attribute_values( s, s0, attributes, domains.subdomain(i).attributes());

            if (on_submesh_boundary(s,mesh) >= 0)
            {
              domains.subdomain(i).mesh().submesh_boundary(s0) = true;
              m_submesh_to_mesh[i][s0] = s;
            }
            else
            {
              // 2015-10-08 Marek: otherwise we can't be sure that OpenTissueMesh::m_submesh_boundary is the correct size
              domains.subdomain(i).mesh().submesh_boundary(s0) = false;
            }
          }

          // Insert triangles
          for (SimplexSet::simplex2_const_iterator it = m_simplex_sets[i].begin2(); it != m_simplex_sets[i].end2(); ++it)
          {
            Simplex2 s = *it;
            Simplex0 s0 = mesh_to_submesh[s.get_simplex0_idx0()];
            Simplex0 s1 = mesh_to_submesh[s.get_simplex0_idx1()];
            Simplex0 s2 = mesh_to_submesh[s.get_simplex0_idx2()];

            Simplex2 s_new = domains.subdomain(i).mesh().insert(s0, s1, s2);

            AMT::copy_attribute_values( s, s_new, attributes, domains.subdomain(i).attributes());
            domains.subdomain(i).mesh().label(s_new) = mesh.label(s);
          }

          // Insert edges
          for (SimplexSet::simplex1_const_iterator it = m_simplex_sets[i].begin1(); it != m_simplex_sets[i].end1(); ++it)
          {
            Simplex1 s = *it;
            Simplex0 s0 = mesh_to_submesh[s.get_simplex0_idx0()];
            Simplex0 s1 = mesh_to_submesh[s.get_simplex0_idx1()];

            Simplex1 s_new( s0, s1);

            AMT::copy_attribute_values( s, s_new, attributes, domains.subdomain(i).attributes());
          }

        }

        is_black = !is_black;

        return domains;
      }

      /*! Merges processed subdomains (@domains) into a single, global @mesh, and updates global @attributes.
       *
       */
      void merge_subdomains(
                             DMT const & domains    //!< domain manager, containing the subdomains
                           , PT  const & parameters //!< simulation parameters (for number_of_subdomains)
                           , MI        & mesh       //!< global mesh
                           , AT        & attributes //!< global attribute manager
                           )
      {
        Simplex0AttributeVector<Simplex0> mesh_to_new_mesh; //!< map from old mesh to new mesh (for boundary vertices)

        MI new_mesh;

        new_mesh.m_coords = &(attributes.current());

        // 2015-12-11 Marek: the line below is not necessary in the current set-up (i.e. as the PThreadScheduler is
        //                   implemented, however, if someone passes empty attributes as an argument, the code will fail.
        //                   Perhaps an assertion's neeeded, otherwise uncommenting this line makes the code copy the
        //                   Algorithm's internal attribute vectors back to the global structure.
        //attributes.copy_attribute_names( domains.subdomain(0u).attributes());

        // 2015-09-21 Marek: for reasons explained in on_submesh_boundary(Simplex0 &, ...)
        for (int i = (int)m_simplex_sets.size()-1; i >= 0; --i)
        {
          util::Log logging;
          if (parameters.verbose())
          {
            logging << "merge_subdomains(...): inside loop, i = " << i << util::Log::newline();
          }

          Simplex0AttributeVector<Simplex0> submesh_to_new_mesh; //!< map from submesh to new mesh
          SimplexSet simplex_set = domains.subdomain(i).mesh().get_all_simplices();
          for (SimplexSet::simplex0_const_iterator it = simplex_set.begin0(); it != simplex_set.end0(); ++it)
          {
            Simplex0 s = *it;
            Simplex0 s0 = m_submesh_to_mesh[i][s];
            if (!IsNull()(s0)) // does the vertex exist in the old mesh?
            {
              if (on_submesh_boundary(s0,mesh) == i) // only insert the vertices once
              {
                Simplex0 new_s0 = new_mesh.insert(); // insert
                mesh_to_new_mesh[s0] = new_s0; // insert in map from old mesh to new mesh
                AMT::copy_attribute_values( s, new_s0, domains.subdomain(i).attributes(), attributes);
              }
            }
            else
            {
              Simplex0 s0 = new_mesh.insert();
              submesh_to_new_mesh[s] = s0;
              AMT::copy_attribute_values( s, s0, domains.subdomain(i).attributes(), attributes);
            }
          }

          for (SimplexSet::simplex2_const_iterator it = simplex_set.begin2(); it != simplex_set.end2(); ++it)
          {
            Simplex0 s0 = it->get_simplex0_idx0();
            Simplex0 s1 = it->get_simplex0_idx1();
            Simplex0 s2 = it->get_simplex0_idx2();

            bool s0_boundary = !IsNull()(m_submesh_to_mesh[i][s0]);
            bool s1_boundary = !IsNull()(m_submesh_to_mesh[i][s1]);
            bool s2_boundary = !IsNull()(m_submesh_to_mesh[i][s2]);

            // If a simplex is on the boundary, don't use submesh simplex
            Simplex0 _s0 = s0_boundary ? mesh_to_new_mesh[m_submesh_to_mesh[i][s0]] : submesh_to_new_mesh[s0];
            Simplex0 _s1 = s1_boundary ? mesh_to_new_mesh[m_submesh_to_mesh[i][s1]] : submesh_to_new_mesh[s1];
            Simplex0 _s2 = s2_boundary ? mesh_to_new_mesh[m_submesh_to_mesh[i][s2]] : submesh_to_new_mesh[s2];

            Simplex2 s = new_mesh.insert(_s0, _s1, _s2);

            AMT::copy_attribute_values( *it, s, domains.subdomain(i).attributes(), attributes);

            new_mesh.label(s) = domains.subdomain(i).mesh().label(*it);
          }

          //--- Note that the attributes for 1-simplices on the subdomain boundary are added twice.
          //--- The code does not touch the subdomain boundary, so it should not be an issue.
          for (SimplexSet::simplex1_const_iterator it = simplex_set.begin1(); it != simplex_set.end1(); ++it)
          {
            Simplex0 s0 = it->get_simplex0_idx0();
            Simplex0 s1 = it->get_simplex0_idx1();

            bool s0_boundary = !IsNull()(m_submesh_to_mesh[i][s0]);
            bool s1_boundary = !IsNull()(m_submesh_to_mesh[i][s1]);

            // If a simplex is on the boundary, don't use submesh simplex
            Simplex0 _s0 = s0_boundary ? mesh_to_new_mesh[m_submesh_to_mesh[i][s0]] : submesh_to_new_mesh[s0];
            Simplex0 _s1 = s1_boundary ? mesh_to_new_mesh[m_submesh_to_mesh[i][s1]] : submesh_to_new_mesh[s1];

            Simplex1 s(_s0, _s1);

            AMT::copy_attribute_values( *it, s, domains.subdomain(i).attributes(), attributes);
          }
        }

        mesh = new_mesh;
      }

    }; // class SlabDecomposition

  } // namespace details

} // namespace grit

// GRIT_SLAB_DECOMPOSITION_H
#endif

