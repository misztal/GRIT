#ifndef GRIT_INTERFACE_ALGORITHM_H
#define GRIT_INTERFACE_ALGORITHM_H

#include <util_log.h>

#include <vector>

namespace grit
{
  namespace details
  {
    template<typename types>
    class InterfaceAlgorithm
    {
    public:

      typedef typename types::mesh_impl                  MI;
      typedef typename types::attributes_type            AMT;
      typedef typename types::param_type                 PT;
      typedef          std::vector<unsigned int>         LT;

    public:

      unsigned int     m_id; ///< Identifier of the subdomain to which this Algorithm is assigned.

    public:

      unsigned int const & id()        const { return m_id; }

    public:

      unsigned int       & id()              { return m_id; }

    public:

      virtual unsigned int run(
                                 MI                & mesh       ///< mesh on which GRIT algorithm is run
                               , AMT               & attributes ///< attribute manager
                               , PT          const & parameters ///< simulation parameters
                               , LT          const & labels     //< vector of labels in the mesh
                              ) = 0;

    }; // class Algorithm


    template<typename types>
    class EmptyAlgorithm
    : public InterfaceAlgorithm<types>
    {
    public:

      typedef typename types::mesh_impl                  MI;
      typedef typename types::attribute_manager_type     AMT;
      typedef typename types::param_type                 PT;
      typedef          std::vector<unsigned int>         LT;

    public:

      unsigned int run(
                        MI                & mesh
                      , AMT               & attributes
                      , PT          const & parameters
                       , LT          const & labels     ///< vector of labels in the mesh
                      )
      {
        util::Log logging;

        if (parameters.verbose() && !(parameters.silent_threads()))
        {
          logging << "EmptyAlgorithm::run(): Inside EmptyAlgorithm" << util::Log::newline();
        }

        return 0;
      }

    };

  } // namespace details

} // namespace grit

// GRIT_INTERFACE_ALGORITHM_H
#endif

