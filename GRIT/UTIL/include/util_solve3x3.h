#ifndef UTIL_SOLVE3X3_H
#define UTIL_SOLVE3X3_H

#include <cmath>
#include <cassert>

namespace util
{

  template<typename T>
  class Matrix3x3
  {
  protected:

    T m_values[3][3];

  public:

    Matrix3x3()
    {
      m_values[0][0] = 0.0;      m_values[0][1] = 0.0;      m_values[0][2] = 0.0;
      m_values[1][0] = 0.0;      m_values[1][1] = 0.0;      m_values[1][2] = 0.0;
      m_values[2][0] = 0.0;      m_values[2][1] = 0.0;      m_values[2][2] = 0.0;
    }

    T const & operator()(unsigned int const & i,unsigned int const & j) const { return m_values[i][j]; }
    T       & operator()(unsigned int const & i,unsigned int const & j)       { return m_values[i][j]; }

  };

  template<typename T>
  class Vector3
  {
  protected:

    T m_values[3];

  public:

    Vector3()
    {
      m_values[0] = 0.0;
      m_values[1] = 0.0;
      m_values[2] = 0.0;
    }

    T const & operator()(unsigned int const & i) const { return m_values[i]; }
    T       & operator()(unsigned int const & i)       { return m_values[i]; }
    
  };


  template<typename T>
  inline Matrix3x3<T> trans( Matrix3x3<T> const & A)
  {
    Matrix3x3<T> B;

    B(0,0) = A(0,0);    B(0,1) = A(1,0);    B(0,2) = A(2,0);
    B(1,0) = A(0,1);    B(1,1) = A(1,1);    B(1,2) = A(2,1);
    B(2,0) = A(0,2);    B(2,1) = A(1,2);    B(2,2) = A(2,2);

    return B;
  }

  template<typename T>
  inline Matrix3x3<T> identity( )
  {
    Matrix3x3<T> B;

    B(0,0) = 1.0;    B(0,1) = 0.0;    B(0,2) = 0.0;
    B(1,0) = 0.0;    B(1,1) = 1.0;    B(1,2) = 0.0;
    B(2,0) = 0.0;    B(2,1) = 0.0;    B(2,2) = 1.0;

    return B;
  }

  template<typename T>
  inline Matrix3x3<T> operator/( Matrix3x3<T> const & A, T const & b)
  {
    Matrix3x3<T> B;

    B(0,0) = A(0,0)/b;    B(0,1) = A(0,1)/b;    B(0,2) = A(0,2)/b;
    B(1,0) = A(1,0)/b;    B(1,1) = A(1,1)/b;    B(1,2) = A(1,2)/b;
    B(2,0) = A(2,0)/b;    B(2,1) = A(2,1)/b;    B(2,2) = A(2,2)/b;

    return B;
  }

  template<typename T>
  inline Vector3<T> operator*( Matrix3x3<T> const & A, Vector3<T> const & b)
  {
    Vector3<T> x;

    x(0) = A(0,0)*b(0) + A(0,1)*b(1) + A(0,2)*b(2);
    x(1) = A(1,0)*b(0) + A(1,1)*b(1) + A(1,2)*b(2);
    x(2) = A(2,0)*b(0) + A(2,1)*b(1) + A(2,2)*b(2);

    return x;
  }

  template<typename T>
  inline Matrix3x3<T> inverse( Matrix3x3<T> const & A)
  {
    //From messer p.283 we know
    //
    //  -1     1
    // A   = -----  adj A
    //       det A
    //
    //                      i+j
    // ij-cofactor of A = -1    det A
    //                               ij
    //
    // i,j entry of the adjoint.
    //                                   i+j
    // adjoint A   = ji-cofactor = A = -1    det A
    //          ij                                ji
    //
    // As it can be seen the only numerical error
    // in these calculations is from the resolution
    // of the scalars. So it is a very accurate method.
    //
    Matrix3x3<T> adj;

    adj(0,0) = A(1,1)*A(2,2) - A(2,1)*A(1,2);
    adj(1,1) = A(0,0)*A(2,2) - A(2,0)*A(0,2);
    adj(2,2) = A(0,0)*A(1,1) - A(1,0)*A(0,1);
    adj(0,1) = A(1,0)*A(2,2) - A(2,0)*A(1,2);
    adj(0,2) = A(1,0)*A(2,1) - A(2,0)*A(1,1);
    adj(1,0) = A(0,1)*A(2,2) - A(2,1)*A(0,2);
    adj(1,2) = A(0,0)*A(2,1) - A(2,0)*A(0,1);
    adj(2,0) = A(0,1)*A(1,2) - A(1,1)*A(0,2);
    adj(2,1) = A(0,0)*A(1,2) - A(1,0)*A(0,2);

    T const  det = A(0,0)*adj(0,0) -  A(0,1)*adj(0,1) +   A(0,2)*adj(0,2);

    if(det)
    {
      adj(0,1) = -adj(0,1);
      adj(1,0) = -adj(1,0);
      adj(1,2) = -adj(1,2);
      adj(2,1) = -adj(2,1);

      return trans(adj)/det;
    }

    return identity<T>();
  }

  template<typename T>
  inline void solve3x3(Matrix3x3<T> const & A, Vector3<T> const & b, Vector3<T> & x)
  {
    Matrix3x3<T> invA = inverse(A);
    x = invA*b;
  }

}//namespace util

// UTIL_SOLVE3X3_H
#endif