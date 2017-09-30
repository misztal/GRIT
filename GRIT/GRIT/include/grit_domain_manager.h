#ifndef GRIT_DOMAIN_MANAGER_H
#define GRIT_DOMAIN_MANAGER_H

#include <grit_subdomain.h>

#include <vector>

namespace grit
{

  namespace details
  {

    template<typename types>
    class DomainManager
    {
    public:

      typedef typename types::subdomain_type  SD;
 
    protected:

      std::vector<SD>  m_subdomains;

    public:

      unsigned int number_of_domains() const { return this->m_subdomains.size(); }
      
      SD const & subdomain(unsigned int const & idx) const { return this->m_subdomains[idx]; }
      SD       & subdomain(unsigned int const & idx)       { return this->m_subdomains[idx]; }
      
    public:

      DomainManager()
      : m_subdomains( 0 )
      {}

      DomainManager( unsigned int const & number_of_subdomains )
      : m_subdomains( number_of_subdomains )
      {}

    }; // class DomainManager

  } // namespace details

} // namespace grit

// GRIT_DOMAIN_MANAGER_H
#endif
