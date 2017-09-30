#ifndef GRIT_LOGIC_EXPRESSIONS_H
#define GRIT_LOGIC_EXPRESSIONS_H

#include <grit_interface_mesh.h>
#include <grit_simplex_set.h>

namespace grit
{

  template<typename E>
  class LogicExpression
  {
  public:

    template<typename S>
    bool operator()(S const & s) const
    {
      E const & self = static_cast<E const &>( *this );

      return self(s);
    }

    operator E &()             { return static_cast<      E&>(*this); }
    operator E const &() const { return static_cast<const E&>(*this); }

  };

  template<typename E1,typename E2>
  class And
  : public LogicExpression< And<E1,E2>  >
  {
  public:

    E1 const & m_A;
    E2 const & m_B;

  public:

    And( LogicExpression<E1> const & A, LogicExpression<E2> const & B)
    : m_A(A)
    , m_B(B)
    {}

    virtual ~And(){}

  public:

    template<typename S>
    bool operator()(S const & s) const
    {
      return (this->m_A(s)) && (this->m_B(s));
    }

  };

  template<typename E1,typename E2>
  class Or
  : public LogicExpression< Or<E1,E2>  >
  {
  public:

    E1 const & m_A;
    E2 const & m_B;

  public:

    Or( LogicExpression<E1> const & A, LogicExpression<E2> const & B)
    : m_A(A)
    , m_B(B)
    {}


    virtual ~Or(){}

  public:

    template<typename S>
    bool operator()(S const & s) const
    {
      return (this->m_A(s)) || (this->m_B(s));
    }

  };


  template<typename E>
  class Not
  : public LogicExpression< Not<E>  >
  {
  public:

    E const & m_A;

  public:

    Not( LogicExpression<E> const & A)
    : m_A(A)
    {}

    virtual ~Not(){}

  public:

    template<typename S>
    bool operator()(S const & s) const
    {
      return ! this->m_A(s);
    }

  };

  template <typename E1, typename E2>
  inline And<E1,E2> operator&&(LogicExpression<E1> const & A, LogicExpression<E2> const & B)
  {
    return And<E1,E2>(A,B);
  }

  template <typename E1, typename E2>
  inline Or<E1,E2> operator||(LogicExpression<E1> const& A, LogicExpression<E2> const& B)
  {
    return Or<E1,E2>(A,B);
  }

  template <typename E>
  inline Not<E> operator!(LogicExpression<E> const& A)
  {
    return Not<E>(A);
  }

  class Bool
  : public LogicExpression<Bool>
  {
  public:

    bool m_val;

  public:

    Bool()
    : m_val(false)
    {
    }

    Bool(bool const & val)
    : m_val(val)
    {
    }

    template<typename S>
    bool operator()(S const & s) const
    {
      return this->m_val;
    }

  };

  class InPhase
  : public LogicExpression<InPhase>
  {
  protected:

    InterfaceMesh             const & m_mesh;
    unsigned int                m_label;

  public:

    InPhase(InterfaceMesh const & mesh, unsigned int const & label )
    : m_mesh(mesh)
    , m_label(label)
    {
    }

    template<typename S>
    bool operator()(S const & s) const;

    bool operator()(Simplex0 const & s) const
    {
      SimplexSet const S = this->m_mesh.star(s);

      for( SimplexSet::simplex2_const_iterator it = S.begin2(); it != S.end2(); ++it)
      {
        Simplex2 const & t = *it;

        if( this->m_mesh.label(t) == this->m_label)
          return true;
      }

      return false;
    }

    bool operator()(Simplex1 const & s) const
    {
      SimplexSet const S = this->m_mesh.star(s);

      for( SimplexSet::simplex2_const_iterator it = S.begin2(); it != S.end2(); ++it)
      {
        Simplex2 const & t = *it;

        if( this->m_mesh.label(t) == this->m_label)
          return true;
      }

      return false;
    }

    bool operator()(Simplex2 const & s) const
    {
      return this->m_mesh.label(s) == this->m_label;
    }

  };

  class IsDimension
  : public LogicExpression<IsDimension>
  {
  protected:

    InterfaceMesh             const & m_mesh;
    unsigned int                m_dim;

  public:

    IsDimension(InterfaceMesh const & mesh, unsigned int const & dim )
    : m_mesh(mesh)
    , m_dim(dim)
    {
    }

    template<typename S>
    bool operator()(S const & s) const;

    bool operator()(Simplex0 const & s) const    {      return this->m_dim == 0;    }

    bool operator()(Simplex1 const & s) const    {      return this->m_dim == 1;    }

    bool operator()(Simplex2 const & s) const    {      return this->m_dim == 2;    }

  };

  class IsBoundary
  : public LogicExpression<IsBoundary>
  {
  protected:

    InterfaceMesh             const & m_mesh;

  public:

    IsBoundary(InterfaceMesh const & mesh)
    : m_mesh(mesh)
    {
    }

    template<typename S>
    bool operator()(S const & s) const
    {
      return this->m_mesh.is_boundary(s);
    }

  };

  class IsInterface
  : public LogicExpression<IsInterface>
  {
  protected:

    InterfaceMesh             const & m_mesh;

  public:

    IsInterface(InterfaceMesh const & mesh)
    : m_mesh(mesh)
    {
    }

    template<typename S>
    bool operator()(S const & s) const
    {
      return this->m_mesh.is_interface(s);
    }

  };

  class IsSubmeshBoundary
  : public LogicExpression<IsSubmeshBoundary>
  {
  protected:

    InterfaceMesh             const & m_mesh;

  public:

    IsSubmeshBoundary(InterfaceMesh const & mesh)
    : m_mesh(mesh)
    {
    }

    template<typename S>
    bool operator()(S const & s) const
    {
      return this->m_mesh.is_submesh_boundary(s);
    }

  };

  class IsManifold
  : public LogicExpression<IsManifold>
  {
  protected:

    InterfaceMesh             const & m_mesh;

  public:

    IsManifold(InterfaceMesh const & mesh)
    : m_mesh(mesh)
    {
    }

    template<typename S>
    bool operator()(S const & s) const
    {
      return this->m_mesh.is_manifold(s);
    }

  };

  class IsValid
  : public LogicExpression<IsValid>
  {
  protected:

    InterfaceMesh             const & m_mesh;

  public:

    IsValid(InterfaceMesh const & mesh)
    : m_mesh(mesh)
    {
    }

    template<typename S>
    bool operator()(S const & s) const
    {
      return this->m_mesh.is_valid_simplex(s);
    }

  };

  class IsNull
  : public LogicExpression<IsNull>
  {
  public:

    IsNull() {}

    template<typename S>
    bool operator()(S const & s) const
    {
      return s.is_null();
    }
  };

} // namespace grit

// GRIT_LOGIC_EXPRESSIONS_H
#endif

