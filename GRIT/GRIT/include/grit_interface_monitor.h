#ifndef GRIT_INTERFACE_MONITOR_H
#define GRIT_INTERFACE_MONITOR_H


#include <util_log.h>

#include <string>

namespace grit
{

  namespace details
  {

    template<typename types>
    class InterfaceMonitor
    {
    protected:

      typedef typename types::param_type     PT;

      std::string m_name;

    public:

      std::string const & name() const { return m_name; }
      std::string       & name()       { return m_name; }

    public:

      InterfaceMonitor()
      : m_name("")
      {}

    public:

      virtual bool next() = 0;
      virtual void reset() = 0;
      virtual void update() = 0;
      virtual void init(PT const & params) = 0;

    };

  } // namespace details

} // namespace grit

// GRIT_INTERFACE_MONITOR_H
#endif
