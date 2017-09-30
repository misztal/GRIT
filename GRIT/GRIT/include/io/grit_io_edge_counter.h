#ifndef GRIT_IO_EDGE_COUNTER_H
#define GRIT_IO_EDGE_COUNTER_H

#include <grit_simplex.h>

#include <map>

namespace grit
{

  namespace io
  {

    /**
     * This is a convenience class that is used for counting
     * the number of times an edge is seen in an input mesh file.
     *
     * In order for the input mesh to be a discrete manifold no edge
     * can be seen more than twice. If seen three times or more it
     * will imply a fan-like connectivity in the input mesh. This
     * would imply non-manifoldness.
     */
    class EdgeCounter
    {
    public:

      std::map<Simplex0, std::map<Simplex0, unsigned int> > m_counters;

      unsigned int count(Simplex0 const & a, Simplex0 const & b)
      {
        unsigned int seen = 0u;
        if(a < b)
        {
          seen =  m_counters[a][b] + 1u;
          m_counters[a][b] = seen;
        }
        else
        {
          seen =  m_counters[b][a] + 1u;
          m_counters[b][a] = seen;
        }
        return seen;
      }

    };

  }// end namespace io

}// end namespace grit

// GRIT_IO_EDGE_COUNTER_H
#endif
