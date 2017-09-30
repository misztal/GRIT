#ifndef GRIT_COUNT_MONITOR
#define GRIT_COUNT_MONITOR

#include <grit_interface_monitor.h>

#include <util_log.h>

namespace grit
{
  namespace details
  {

    template<typename types>
    class CountMonitor
    : public InterfaceMonitor<types>
    {
    protected:

      typedef InterfaceMonitor<types> base_class;

      typedef typename types::param_type  PT;

    protected:

      unsigned int  m_max_iterations;
      unsigned int  m_iterations;
      bool          m_verbose;

    public:

      unsigned int const & max_iterations() const { return m_max_iterations; }

    public:

      unsigned int & max_iterations() { return m_max_iterations; }

    public:

      CountMonitor()
      : m_max_iterations(0u)
      , m_iterations(0u)
      , m_verbose(true)
      { }

      CountMonitor(unsigned int const & max_iterations, bool verbose = true)
      : m_max_iterations(max_iterations)
      , m_iterations(0u)
      , m_verbose(verbose)
      { }

      bool next()
      {
        if (m_iterations >= m_max_iterations)
        {
          if (m_verbose)
          {
            util::Log log;

            log << "Max iterations reached for " << this->m_name << util::Log::newline();
          }
          return false;
        }

        return true;
      }

      void reset()
      {
        m_iterations = 0u;
      }

      void update()
      {
        ++m_iterations;
      }

      void init(PT const & params)
      {
        m_max_iterations = params.max_iterations(base_class::m_name);
        m_verbose        = params.verbose();
      }

    }; // class CountMonitor

  } // end namespace details
} // end namespace grit

// GRIT_COUNT_MONITOR
#endif

