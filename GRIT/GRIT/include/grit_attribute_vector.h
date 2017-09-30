#ifndef GRIT_ATTRIBUTE_VECTOR_H
#define GRIT_ATTRIBUTE_VECTOR_H

#include <grit_simplex.h>
#include <grit_simplex_set.h>

#include <map>
#include <vector>
#include <algorithm> // Needed for std::min and std::max
#include <limits>    // Needed for std::numeric_limits

namespace grit
{

  template<typename T>
  class SimplexAttributeVector
  {
  };

  template<typename T>
  class Simplex0AttributeVector
    : public SimplexAttributeVector<T>
  {
  protected:

    std::vector<T> m_data;

  public:

    typedef typename std::vector<T>::iterator        iterator;
    typedef typename std::vector<T>::const_iterator  const_iterator;

    iterator       begin()       { return this->m_data.begin(); }
    const_iterator begin() const { return this->m_data.begin(); }
    iterator       end()         { return this->m_data.end();   }
    const_iterator end()   const { return this->m_data.end();   }

  public:

    Simplex0AttributeVector()
    : m_data()
    {}

    virtual ~Simplex0AttributeVector(){}

    Simplex0AttributeVector(    Simplex0AttributeVector const & v)
    {
      *this = v;
    }

    Simplex0AttributeVector & operator=( Simplex0AttributeVector const & v )
    {
      if(this!=&v)
      {
        this->m_data = v.m_data;
      }
      return *this;
    }

  public:

    T & operator[](Simplex0 const & s)
    {
      this->m_data.resize((s.get_idx0() + 1 > size()) ? s.get_idx0() + 1 : size());

      return this->m_data[s.get_idx0()];
    }

    T const & operator[](Simplex0 const & s) const
    {
      return this->m_data[s.get_idx0()];
    }

    size_t size() const
    {
      return this->m_data.size();
    }

    void insert_from_simplex_set(SimplexSet const & A, Simplex0AttributeVector<T> const & data)
    {
      typedef typename SimplexSet::simplex0_const_iterator iterator0;

      //--- Make sure we have space enough for inserting data
      using std::min;
      using std::max;

      typename Simplex::index_type idx_min = std::numeric_limits<typename Simplex::index_type>::max();
      typename Simplex::index_type idx_max = std::numeric_limits<typename Simplex::index_type>::min();

      for (iterator0 it = A.begin0(); it != A.end0(); ++it)
      {
        Simplex0 const s = *it;

        idx_min = min( s.get_idx0(), idx_min);
        idx_max = max( s.get_idx0(), idx_max);
      }
      if (this->m_data.size() != idx_max+1u )
      {
        this->m_data.resize( idx_max+1u );
      }

      //--- Now we are sure we got memory allocated to now we can insert stuff
      for (iterator0 it = A.begin0(); it != A.end0(); ++it)
      {
        Simplex0 const s = *it;

        this->m_data[s.get_idx0()] = data[s];
      }
    }

    void clear()
    {
      this->m_data.clear();
    }

  };

  template<typename T1, typename T2>
  inline void scale(Simplex0AttributeVector<T1> & data, T2 const & value)
  {
    typedef typename Simplex0AttributeVector<T1>::iterator iterator;

    iterator begin = data.begin();
    iterator end   = data.end();

    for(iterator it = begin; it != end; ++it)
      *it *= value;
  }

  template<typename T1, typename T2>
  inline void scale(Simplex0AttributeVector<T1> & data, T2 const & sx, T2 const & sy)
  {
    typedef typename Simplex0AttributeVector<T1>::iterator iterator;

    iterator begin = data.begin();
    iterator end   = data.end();

    for(iterator it = begin; it != end; ++it)
    {
      (*it)[0] *= sx;
      (*it)[1] *= sy;
    }
  }

  template<typename T>
  class Simplex1AttributeVector
    : public SimplexAttributeVector<T>
  {
  protected:

    std::map<Simplex1, T> m_data;

  public:

    T & operator[](Simplex1 const & s)
    {
      return m_data[s];
    }

    T const & operator[](Simplex1 const & s) const
    {
      return m_data.at(s);//C++11 feature
    }

    size_t size() const
    {
      return m_data.size();
    }

    void erase(Simplex1 const & s)
    {
      m_data.erase(s);
    }

    void clear()
    {
      m_data.clear();
    }

    bool has_value(Simplex1 const & s) const
    {
      return (m_data.find(s) != m_data.end());
    }

  };

  template<typename T>
  class Simplex2AttributeVector
    : public SimplexAttributeVector<T>
  {
  protected:

    std::map<Simplex2, T> m_data;

  public:

    T const & operator[](Simplex2 const & s) const
    {
      return m_data.at(s);//C++11 feature
    }

    T       & operator[](Simplex2 const & s)
    {
      return m_data[s];
    }

    size_t size() const
    {
      return m_data.size();
    }

    void erase(Simplex2 const & s)
    {
      m_data.erase(s);
    }

    void clear()
    {
      m_data.clear();
    }

    bool has_value(Simplex2 const & s) const
    {
      return (m_data.find(s) != m_data.end());
    }

  };

}// end namespace grit

// GRIT_ATTRIBUTE_VECTOR_H
#endif
