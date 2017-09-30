#ifndef GRIT_QUALITY_ANALYZER_H
#define GRIT_QUALITY_ANALYZER_H

#include <grit_simplex.h>
#include <grit_simplex_set.h>
#include <grit_simplex_set_functions.h>

#include <grit_interface_quality_measure.h>

namespace grit
{

  template<typename S>
  class QualityAnalyzer
  {
  public:

    typedef typename SimplexSetTypeTraits<S>::iterator_type  iterator_type;

  protected:

    SimplexSet                  const & m_simplex_set;
    InterfaceQualityMeasure<S>  const & m_quality_measure;
    iterator_type                       m_iterator;
    unsigned int                        m_count;

  protected:

    iterator_type get_bad_from(iterator_type & iterator, bool const & include_first = false)
    {
      iterator_type const _end = end<iterator_type>( m_simplex_set );

      if (iterator == _end )
        return _end;

      if (!include_first)
        ++iterator;

      for (; iterator != _end; ++iterator)
      {
        if (m_quality_measure.is_bad(*iterator))
        {
          return iterator;
        }
      }

      return _end;
    }

  public:

    QualityAnalyzer(
                    SimplexSet                   const & simplex_set
                    , InterfaceQualityMeasure<S> const & quality_measure
                    )
    : m_simplex_set(simplex_set)
    , m_quality_measure(quality_measure)
    , m_iterator()
    , m_count(0u)
    {
      iterator_type it = begin<iterator_type>(m_simplex_set);
      m_iterator = get_bad_from(it,true);
    }

    bool has_next()
    {
      return m_iterator != end<iterator_type>(m_simplex_set);
    }

    S pop()
    {
      S const s = *m_iterator;
      m_iterator = get_bad_from(m_iterator);

      ++m_count;

      return s;
    }

    void set_simplex_set(SimplexSet & A)
    {
      m_simplex_set = A;

      iterator_type it = begin<iterator_type>(m_simplex_set);
      m_iterator = get_bad_from(it,true);
    }

    int get_count()
    {
      return m_count;
    }

  };

}//end namespace grit

// GRIT_QUALITY_ANALYZER_H
#endif
