#ifndef UTIL_DISTANCE_FIELDS_H
#define UTIL_DISTANCE_FIELDS_H

#include <cmath>
#include <algorithm>

namespace util
{

  template<typename F>
  class DistanceFieldExp
  {
  public:

    double operator()(double const & x, double const & y) const
    {
      F const & self = static_cast<F const &>( *this );
      return self(x,y);
    }

    operator F &()             { return static_cast<      F&>(*this); }
    operator F const &() const { return static_cast<const F&>(*this); }

  };

  class Plane
  : public DistanceFieldExp<Plane>
  {
  public:

    double m_nx;
    double m_ny;
    double m_w;

  public:

    Plane()
    : m_nx(1.0)
    , m_ny(0.0)
    , m_w(0.5)
    {
    }

    Plane(double const nx, double const ny,double const w)
    {
      using std::sqrt;

      m_nx = nx / sqrt(nx*nx + ny*ny);
      m_ny = ny / sqrt(nx*nx + ny*ny);
      m_w  = w;
    }

    double operator()(double const & x, double const & y) const
    {
      return (x*m_nx + y*m_ny - m_w);
    }

  };

  class Box
	: public DistanceFieldExp<Box>
  {
  public:

    double m_min_x;
    double m_max_x;
    double m_min_y;
    double m_max_y;

  public:

    Box()
    : m_min_x(0.1)
    , m_max_x(0.9)
    , m_min_y(0.1)
    , m_max_y(0.9)
    {
    }

    Box(double const min_x,double const max_x,double const min_y,double const max_y)
    : m_min_x(min_x)
    , m_max_x(max_x)
    , m_min_y(min_y)
    , m_max_y(max_y)
    {
    }

    virtual ~Box()
    {
    }

    double operator()(double const & x, double const & y) const
    {
      using std::fabs;
      using std::sqrt;

      double const cx = (m_min_x+m_max_x)/2.0f;  // box center
      double const cy = (m_min_y+m_max_y)/2.0f;
      double const w  = (m_max_x-m_min_x)/2.0f;  // half widht and height
      double const h  = (m_max_y-m_min_y)/2.0f;

      double const dx = fabs(x-cx);              // point in positive ortant
      double const dy = fabs(y-cy);
      double const qx = (dx>w) ? w : dx;         // project onto box if point is outside
      double const qy = (dy>h) ? h : dy;

      double const dist2 = (qx-dx)*(qx-dx) + (qy-dy)*(qy-dy) ;  // squared distance to closest point on surface

      double const tx =  w-qx;  // if inside then distance to box wall
      double const ty =  h-qy;

      return dist2 > 0 ? - sqrt(dist2) : (tx<ty ? tx : ty);
    }

  };

  class Circle
  : public DistanceFieldExp<Circle>
  {
  public:

    double m_x;
    double m_y;
    double m_r;

  public:

    Circle()
    : m_x(0.5)
    , m_y(0.5)
    , m_r(0.4)
    {
    }

    Circle(double const x,double const y,double const r)
    : m_x(x)
    , m_y(y)
    , m_r(r)
    {
    }

    virtual ~Circle()
    {
    }

    double operator()(double const & x, double const & y) const
    {
      using std::sqrt;

      double const dx   = x - m_x;
      double const dy   = y - m_y;
      double const dist = sqrt(dx*dx+dy*dy);

      return - (dist - m_r);
    }
  };

  template<typename F1, typename F2>
  class Intersection
  : public DistanceFieldExp< Intersection<F1,F2> >
  {
  public:

    F1 const & m_A;
    F2 const & m_B;

  public:

    Intersection( DistanceFieldExp<F1> const & A, DistanceFieldExp<F2> const & B)
    : m_A(A)
    , m_B(B)
    {
    }

    virtual ~Intersection()
    {
    }

  public:

    double operator()(double const & x, double const & y) const
    {
      using std::min;

      return min( m_A(x,y), m_B(x,y) );
    }

  };

  template<typename F>
  class Invert
  : public DistanceFieldExp< Invert<F> >
  {
  public:

    F const & m_A;

  public:

    Invert(DistanceFieldExp<F> const & A)
    : m_A(A)
    {
    }

    virtual ~Invert()
    {
    }

  public:

    double operator()(double const & x, double const & y) const
    {
      return - m_A(x,y);
    }

  };

  template<typename F1, typename F2>
  class Union
  : public DistanceFieldExp< Union<F1,F2> >
  {
  public:

    F1 const & m_A;
    F2 const & m_B;

  public:

    Union(DistanceFieldExp<F1> const & A, DistanceFieldExp<F2> const & B)
    : m_A(A)
    , m_B(B)
    {
    }

  public:

    double operator()(double const & x, double const & y) const
    {
      using std::max;

      return max( m_A(x,y), m_B(x,y) );
    }

  };

  class Const
  : public DistanceFieldExp<Const>
  {
  public:

    double m_val;

  public:

    Const()
    : m_val(0.0f)
    {
    }

    Const(double const val)
    : m_val(val)
    {
    }

    double operator()(double const & , double const & ) const
    {
      return m_val;
    }

  };

  template<typename F>
  class Dilation
  : public DistanceFieldExp< Dilation<F> >
  {
  public:

    F     const & m_A;
    Const         m_B;

  public:

    Dilation(DistanceFieldExp<F> const & A, Const const & B)
    : m_A(A)
    , m_B(B)
    {
    }

    double operator()(double const & x, double const & y) const
    {
      return m_A(x,y) + m_B(x,y);
    }

  };

  template <typename F1, typename F2>
  inline Union<F1,F2> const operator+(DistanceFieldExp<F1> const& A, DistanceFieldExp<F2> const& B)
  {
    return Union<F1,F2>(A,B);
  }

  template <typename F1, typename F2>
  inline Intersection<F1,F2> const operator%(DistanceFieldExp<F1> const& A, DistanceFieldExp<F2> const& B)
  {
    return Intersection<F1,F2>(A,B);
  }
  
  template <typename F>
  inline Invert<F> const operator-(DistanceFieldExp<F> const& A)
  {
    return Invert<F>(A);
  }
  
  template <typename F>
  inline Dilation<F> const dilation(DistanceFieldExp<F> const& A, double const & val)
  {
    return Dilation<F>(A,Const(val));
  }
  
  template <typename F>
  inline Dilation<F> const dilation(double const & val,DistanceFieldExp<F> const& A)
  {
    return A+val;
  }
  
}//namespace util


// UTIL_DISTANCE_FIELDS_H
#endif
