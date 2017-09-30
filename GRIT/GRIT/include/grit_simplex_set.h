#ifndef GRIT_SIMPLEX_SET_H
#define GRIT_SIMPLEX_SET_H

#include <grit_simplex.h>

#include <util_log.h>

#include <set>
#include <cassert>
#include <limits>

namespace grit
{

  class SimplexSetAccess;  ///< Forward declaration of SimplexSet backdoor.

  class SimplexSet
  {
  public:

    friend
    class SimplexSetAccess;

  public:

    typedef std::set<Simplex0>::const_iterator simplex0_const_iterator;
    typedef std::set<Simplex1>::const_iterator simplex1_const_iterator;
    typedef std::set<Simplex2>::const_iterator simplex2_const_iterator;

    typedef simplex0_const_iterator s0_iterator;
    typedef simplex1_const_iterator s1_iterator;
    typedef simplex2_const_iterator s2_iterator;

  protected:

    std::set<Simplex0> m_simplices0;
    std::set<Simplex1> m_simplices1;
    std::set<Simplex2> m_simplices2;

  public:

    std::set<Simplex0> const & simplices0() const { return m_simplices0; }
    std::set<Simplex1> const & simplices1() const { return m_simplices1; }
    std::set<Simplex2> const & simplices2() const { return m_simplices2; }

  public:

    simplex0_const_iterator begin0() const { return m_simplices0.begin(); }
    simplex0_const_iterator end0()   const { return m_simplices0.end();   }
    simplex1_const_iterator begin1() const { return m_simplices1.begin(); }
    simplex1_const_iterator end1()   const { return m_simplices1.end();   }
    simplex2_const_iterator begin2() const { return m_simplices2.begin(); }
    simplex2_const_iterator end2()   const { return m_simplices2.end();   }

  public:

    SimplexSet()
    : m_simplices0()
    , m_simplices1()
    , m_simplices2()
    {}

    virtual ~SimplexSet()
    {
      m_simplices0.clear();
      m_simplices1.clear();
      m_simplices2.clear();
    }

    SimplexSet(SimplexSet const & A)
    {
      *this = A;
    }

    SimplexSet const & operator=(SimplexSet const & A)
    {
      if( this != &A )
      {
        m_simplices0 = A.m_simplices0;
        m_simplices1 = A.m_simplices1;
        m_simplices2 = A.m_simplices2;
      }
      return *this;
    }

  protected:

    template<typename iterator>
    void range_insert(iterator const & begin, iterator const & end, Simplex0 const & /*tag*/)
    {
      m_simplices0.insert( begin, end );
    }

    template<typename iterator>
    void range_insert(iterator const & begin, iterator const & end, Simplex1 const & /*tag*/)
    {
      m_simplices1.insert( begin, end );
    }

    template<typename iterator>
    void range_insert(iterator const & begin, iterator const & end, Simplex2 const & /*tag*/)
    {
      m_simplices2.insert( begin, end );
    }

  public:

    template<typename iterator>
    void insert(iterator const & begin, iterator const & end)
    {
      typename iterator::value_type const tag;

      range_insert(begin,end, tag);
    }

    void insert(Simplex0 const & s) { m_simplices0.insert(s); }
    void insert(Simplex1 const & s) { m_simplices1.insert(s); }
    void insert(Simplex2 const & s) { m_simplices2.insert(s); }

    void insert(SimplexSet const & A)
    {
      m_simplices0.insert(A.begin0(), A.end0());
      m_simplices1.insert(A.begin1(), A.end1());
      m_simplices2.insert(A.begin2(), A.end2());
    }

    unsigned int size(unsigned int const & dimension) const
    {
      switch (dimension)
      {
        case 0: return m_simplices0.size();
        case 1: return m_simplices1.size();
        case 2: return m_simplices2.size();
        default: assert(false || !"SimplexSet::insert(): Unrecognized dimension");
      }

      return std::numeric_limits<unsigned int>::max();
    }

    unsigned int size() const
    {
      return m_simplices0.size() + m_simplices1.size() + m_simplices2.size();
    }

    bool contains(Simplex0 const & s) const
    {
      return m_simplices0.find(s) != end0();
    }

    bool contains(Simplex1 const & s) const
    {
      return m_simplices1.find(s) != end1();
    }

    bool contains(Simplex2 const & s) const
    {
      return m_simplices2.find(s) != end2();
    }

    bool remove(Simplex0 const & s)
    {
      util::Log log;

      if (m_simplices0.find(s) == end0())
      {
        return false;
      }

      m_simplices0.erase(m_simplices0.find(s));

      return true;
    }

    bool remove(Simplex1 const & s)
    {
      util::Log log;

      if (m_simplices1.find(s) == end1())
      {
        log << "1-simplex not found" << util::Log::newline();

        return false;
      }

      m_simplices1.erase( m_simplices1.find(s) );

      return true;
    }

    bool remove(Simplex2 const & s)
    {
      util::Log log;

      if (m_simplices2.find(s) == end2())
      {
        log << "2-simplex not found" << util::Log::newline();

        return false;
      }

      m_simplices2.erase(m_simplices2.find(s));

      return true;
    }

  };

  /**
   * Some SimplexSet functions/operations are more easily written if one has
   * direct access to the internal containers of the SimplexSet class. This
   * "Access" class provides a means ot get this access.
   *
   * Warning: This is considered unsafe. A SimplexSet is considered
   * to be a static snap shot of the mesh. It should be treated as a
   * "const" value by algorithms and applications. Hence, "Access" should
   * only be used by library developers that know what they are doing.
   *
   * Example of usage:
   *
   *     SimplexSet A = ....;
   *     SimplexSetAccess::simplices0(A).begin();
   *
   */
  class SimplexSetAccess
  {
  public:

    typedef std::set<Simplex0>::const_iterator simplex0_iterator;
    typedef std::set<Simplex1>::const_iterator simplex1_iterator;
    typedef std::set<Simplex2>::const_iterator simplex2_iterator;

  public:

    static std::set<Simplex0> & simplices0(SimplexSet & A) { return A.m_simplices0; }
    static std::set<Simplex1> & simplices1(SimplexSet & A) { return A.m_simplices1; }
    static std::set<Simplex2> & simplices2(SimplexSet & A) { return A.m_simplices2; }

  public:

    static simplex0_iterator begin0(SimplexSet & A) { return A.m_simplices0.begin(); }
    static simplex0_iterator end0(SimplexSet & A)   { return A.m_simplices0.end();   }
    static simplex1_iterator begin1(SimplexSet & A) { return A.m_simplices1.begin(); }
    static simplex1_iterator end1(SimplexSet & A)   { return A.m_simplices1.end();   }
    static simplex2_iterator begin2(SimplexSet & A) { return A.m_simplices2.begin(); }
    static simplex2_iterator end2(SimplexSet & A)   { return A.m_simplices2.end();   }

  };

  template<typename S>
  class SimplexSetTypeTraits;

  template<>
  class SimplexSetTypeTraits<Simplex0>
  {
  public:
    typedef SimplexSet::simplex0_const_iterator iterator_type;
  };

  template<>
  class SimplexSetTypeTraits<Simplex1>
  {
  public:
    typedef SimplexSet::simplex1_const_iterator iterator_type;
  };

  template<>
  class SimplexSetTypeTraits<Simplex2>
  {
  public:
    typedef SimplexSet::simplex2_const_iterator iterator_type;
  };


}//end namespace grit

// GRIT_SIMPLEX_SET_H
#endif
