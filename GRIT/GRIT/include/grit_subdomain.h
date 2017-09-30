#ifndef GRIT_SUBDOMAIN_H
#define GRIT_SUBDOMAIN_H

namespace grit
{

  namespace details
  {

    template<typename types>
    class SubDomain
    {
    public:

      typedef typename types::mesh_impl              MI;
      typedef typename types::attributes_type        AMT;
      typedef typename types::algorithm_type         AT;
      typedef typename types::param_type             PT;

    protected:

      unsigned int     m_id;               /// Unique domain identifier (=thread idx)

      unsigned int     m_operations_done;  /// Number of changes performed on the subdomain by an algorithm

      MI               m_mesh;             /// (Sub)mesh associated with the subdomain.

      PT               m_parameters;

      AT               m_algorithm;

      AMT              m_attributes;

    public:

      //Subdomain()

    public:

      unsigned int const & id()              const { return m_id; }

      unsigned int const & operations_done() const { return m_operations_done; }

      MI           const & mesh()            const { return m_mesh; }

      PT           const & parameters()      const { return m_parameters; }

      AT           const & algorithm()       const { return m_algorithm; }

      AMT          const & attributes()      const { return m_attributes; }

    public:

      unsigned int & id()              { return m_id; }

      unsigned int & operations_done() { return m_operations_done; }

      MI           & mesh()            { return m_mesh; }

      PT           & parameters()      { return m_parameters; }

      AT           & algorithm()       { return m_algorithm; }

      AMT          & attributes()      { return m_attributes; }

    }; // class Subdomain

  } // namespace details

} // namespace grit

// GRIT_SUBDOMAIN_H
#endif

