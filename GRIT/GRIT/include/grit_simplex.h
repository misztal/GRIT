#ifndef GRIT_SIMPLEX_H
#define GRIT_SIMPLEX_H

#include <util_sort.h>

#include <vector>
#include <limits>      // Needed for std::numeric_limits
#include <iostream>    // Needed to support output of string literals in streams

namespace grit
{
  class Simplex
  {
  public:

    typedef unsigned long index_type;

  protected:

    static index_type UNSET()
    {
      return std::numeric_limits<index_type>::max();
    }

  };

  class Simplex0
  : public Simplex
  {
  protected:

    index_type m_idx0;

  public:

    Simplex0()
    : m_idx0( Simplex::UNSET() )
    {}

    Simplex0(index_type const & idx0)
    : m_idx0(idx0)
    {}

    friend bool operator>  (Simplex0 const & s0, Simplex0 const & s1) { return s0.m_idx0 > s1.m_idx0; }
    friend bool operator<  (Simplex0 const & s0, Simplex0 const & s1) { return s0.m_idx0 < s1.m_idx0; }
    friend bool operator== (Simplex0 const & s0, Simplex0 const & s1) { return s0.m_idx0 == s1.m_idx0; }
    friend bool operator!= (Simplex0 const & s0, Simplex0 const & s1) { return s0.m_idx0 != s1.m_idx0; }

    index_type const & get_idx0() const { return m_idx0; }

    bool is_null() const
    {
      return m_idx0 == Simplex::UNSET();
    }

  };

  class Simplex1
  : public Simplex
  {
  protected:

    index_type m_idx0;
    index_type m_idx1;

    static Simplex1 order(Simplex1 const & s)
    {
      if (s.get_idx0() <= s.get_idx1())
        return s;
      else
        return Simplex1(s.get_idx1(), s.get_idx0());
    }

  public:

    Simplex1()
    : m_idx0(Simplex::UNSET())
    , m_idx1(Simplex::UNSET())
    { }

    Simplex1(Simplex0 const & s0, Simplex0 const & s1)
    : m_idx0(s0.get_idx0())
    , m_idx1(s1.get_idx0())
    { }

    friend bool operator> (Simplex1 const & s0, Simplex1 const & s1)
    {
      Simplex1 const ns0 = order(s0);
      Simplex1 const ns1 = order(s1);

      return (ns0.m_idx0 > ns1.m_idx0) ||
             (ns0.m_idx0 == ns1.m_idx0 && ns0.m_idx1 > ns1.m_idx1);
    }

    friend bool operator< (Simplex1 const & s0, Simplex1 const & s1)
    {
      Simplex1 const ns0 = order(s0);
      Simplex1 const ns1 = order(s1);

      return (ns0.m_idx0 < ns1.m_idx0) ||
             (ns0.m_idx0 == ns1.m_idx0 && ns0.m_idx1 < ns1.m_idx1);
    }

    friend bool operator== (Simplex1 const & s0, Simplex1 const & s1)
    {
      Simplex1 const ns0 = order(s0);
      Simplex1 const ns1 = order(s1);

      return ns0.m_idx0 == ns1.m_idx0 && ns0.m_idx1 == ns1.m_idx1;
    }

    index_type const & get_idx0() const { return m_idx0; }
    index_type const & get_idx1() const { return m_idx1; }

    Simplex0 get_simplex0_idx0() const { return Simplex0(m_idx0); }
    Simplex0 get_simplex0_idx1() const { return Simplex0(m_idx1); }

    bool is_null() const
    {
      return m_idx0 == Simplex::UNSET() || m_idx1 == Simplex::UNSET();
    }

    void reverse()
    {
      index_type const temp = m_idx0;
      m_idx0 = m_idx1;
      m_idx1 = temp;
    }

    Simplex1 get_reversed() const
    {
      return Simplex1(m_idx1, m_idx0);
    }

    unsigned int get_local_index(Simplex0 const & s) const
    {
      if( s.get_idx0() == this->m_idx0)
        return 0u;
      if( s.get_idx0() == this->m_idx1)
        return 1u;

      return std::numeric_limits<unsigned int>::max();
    }

    Simplex0 get_simplex_from_local_index(unsigned int const & idx) const
    {
      if( idx == 0u )
        return Simplex0(this->m_idx0);
      if( idx == 1u )
        return Simplex0(this->m_idx1);
      return Simplex0();
    }

  };

  class Simplex2
  : public Simplex
  {
  protected:

    index_type m_idx0;
    index_type m_idx1;
    index_type m_idx2;

    static Simplex2 order(Simplex2 const & s)
    {
      index_type v0 = Simplex::UNSET();
      index_type v1 = Simplex::UNSET();
      index_type v2 = Simplex::UNSET();

      util::sort<index_type>(s.m_idx0, s.m_idx1, s.m_idx2, v0, v1, v2);

      return Simplex2(v0, v1, v2);
    }

  public:

    Simplex2()
    : m_idx0(Simplex::UNSET())
    , m_idx1(Simplex::UNSET())
    , m_idx2(Simplex::UNSET())
    { }

    Simplex2(Simplex0 const & s0, Simplex0 const & s1, Simplex0 const & s2)
    : m_idx0(s0.get_idx0())
    , m_idx1(s1.get_idx0())
    , m_idx2(s2.get_idx0())
    { }

    friend bool operator> (Simplex2 const & s0, Simplex2 const & s1)
    {
      Simplex2 const ns0 = order(s0);
      Simplex2 const ns1 = order(s1);

      return (ns0.m_idx0 > ns1.m_idx0) ||
             (ns0.m_idx0 == ns1.m_idx0 && ns0.m_idx1 > ns1.m_idx1) ||
             (ns0.m_idx0 == ns1.m_idx0 && ns0.m_idx1 == ns1.m_idx1 && ns0.m_idx2 > ns1.m_idx2);
    }

    friend bool operator< (Simplex2 const & s0, Simplex2 const & s1)
    {
      Simplex2 const ns0 = order(s0);
      Simplex2 const ns1 = order(s1);

      return (ns0.m_idx0 < ns1.m_idx0) ||
            (ns0.m_idx0 == ns1.m_idx0 && ns0.m_idx1 < ns1.m_idx1) ||
            (ns0.m_idx0 == ns1.m_idx0 && ns0.m_idx1 == ns1.m_idx1 && ns0.m_idx2 < ns1.m_idx2);
    }

    friend bool operator== (Simplex2 const & s0, Simplex2 const & s1)
    {
      Simplex2 const ns0 = order(s0);
      Simplex2 const ns1 = order(s1);

      return ns0.m_idx0 == ns1.m_idx0 &&
             ns0.m_idx1 == ns1.m_idx1 &&
             ns0.m_idx2 == ns1.m_idx2;
    }

    index_type const & get_idx0() const { return m_idx0; }
    index_type const & get_idx1() const { return m_idx1; }
    index_type const & get_idx2() const { return m_idx2; }

    Simplex0 get_simplex0_idx0() const { return Simplex0(m_idx0); }
    Simplex0 get_simplex0_idx1() const { return Simplex0(m_idx1); }
    Simplex0 get_simplex0_idx2() const { return Simplex0(m_idx2); }

    bool is_null() const
    {
      return m_idx0 == Simplex::UNSET() ||
             m_idx1 == Simplex::UNSET() ||
             m_idx2 == Simplex::UNSET();
    }

    unsigned int get_local_index(Simplex0 const & s) const
    {
      if( s.get_idx0() == this->m_idx0)
        return 0u;
      if( s.get_idx0() == this->m_idx1)
        return 1u;
      if( s.get_idx0() == this->m_idx2)
        return 2u;

      return std::numeric_limits<unsigned int>::max();
    }

    Simplex0 get_simplex_from_local_index(unsigned int const & idx) const
    {
      if( idx == 0u )
        return Simplex0(this->m_idx0);
      if( idx == 1u )
        return Simplex0(this->m_idx1);
      if( idx == 2u )
        return Simplex0(this->m_idx2);
      return Simplex0();
    }
  };

  inline std::ostream & operator<<(std::ostream & stream, Simplex0 const & s)
  {
    stream << s.get_idx0();

    return stream;
  }

  inline std::ostream & operator<<(std::ostream & stream, Simplex1 const & s)
  {
    stream << s.get_idx0()
           << ","
           << s.get_idx1();

    return stream;
  }

  inline std::ostream & operator<<(std::ostream & stream, Simplex2 const & s)
  {
    stream << s.get_idx0()
           << ","
           << s.get_idx1()
           << ","
           << s.get_idx2();

    return stream;
  }

  inline Simplex0 make_simplex0(Simplex::index_type const & idx)
  {
    return Simplex0(idx);
  }

  inline Simplex1 make_simplex1(Simplex0 const & a, Simplex0 const & b)
  {
    return Simplex1(a,b);
  }

  inline Simplex2 make_simplex2(Simplex0 const & a, Simplex0 const & b, Simplex0 const & c)
  {
    return Simplex2(a,b,c);
  }

  /**
   * This function retrives the values of indices as a std::vector. This
   * is convenient for certain book-keeping operations.
   *
   * @param  s   The simplex to retrieve indices from.
   * @return     A std::vector of all indices
   */
  inline std::vector<Simplex::index_type> get_indices(Simplex0 const & s)
  {
    std::vector<Simplex::index_type> indices(1);
    indices[0] = s.get_idx0();
    return indices;
  }

  /**
   * This function retrives the values of indices as a std::vector. This
   * is convenient for certain book-keeping operations.
   *
   * @param  s   The simplex to retrieve indices from.
   * @return     A std::vector of all indices
   */
  inline std::vector<Simplex::index_type> get_indices(Simplex1 const & s)
  {
    std::vector<Simplex::index_type> indices(2);
    indices[0] = s.get_idx0();
    indices[1] = s.get_idx1();
    return indices;
  }

  /**
   * This function retrives the values of indices as a std::vector. This
   * is convenient for certain book-keeping operations.
   *
   * @param  s   The simplex to retrieve indices from.
   * @return     A std::vector of all indices
   */
  inline std::vector<Simplex::index_type> get_indices(Simplex2 const & s)
  {
    std::vector<Simplex::index_type> indices(3);
    indices[0] = s.get_idx0();
    indices[1] = s.get_idx1();
    indices[2] = s.get_idx2();
    return indices;
  }

}//end namespace grit

// GRIT_SIMPLEX_H
#endif
