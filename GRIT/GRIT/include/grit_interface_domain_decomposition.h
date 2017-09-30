#ifndef GRIT_INTERFACE_DOMAIN_DECOMPOSITION_H
#define GRIT_INTERFACE_DOMAIN_DECOMPOSITION_H

namespace grit
{

  namespace details
  {
    /**
     * An interface for domain decomposition classes, responsible for
     * creating subdomains for parallel processing from a global mesh,
     * and merging them into a single mesh.
     */
    template<typename types>
    class InterfaceDomainDecomposition
    {
    public:

      typedef typename types::mesh_impl               MI;
      typedef typename types::attributes_type         AMT;
      typedef typename types::domain_manager_type     DMT;
      typedef typename types::param_type              PT;

    public:

      /**
       * Performs a domain decomposition of @mesh into a number of subdomains,
       * specified by @parameters.number_of_subdomains Returns DomainManager
       * object, containing all subdomains with separate attribute vectors.
       */
      virtual DMT create_subdomains(
                                      PT          const & parameters ///< simulation parameters (for number_of_subdomains)
                                    , MI          const & mesh       ///< global mesh
                                    , AMT         const & attributes ///< global attribute manager
                                    ) = 0;

      /**
       * Merges processed subdomains (@domains) into a single,
       * global @mesh, and updates global @attributes.
       */
      virtual void merge_subdomains(
                                    DMT         const & domains    ///< domain manager, containing the subdomains
                                    , PT          const & parameters ///< simulation parameters (for number_of_subdomains)
                                    , MI                & mesh       ///< global mesh
                                    , AMT               & attributes ///< global attribute manager
      ) = 0;

    }; // class InterfaceDomainDecomposition

  } // namespace details

} // namespace grit

// GRIT_INTERFACE_DOMAIN_DECOMPOSITION_H
#endif

