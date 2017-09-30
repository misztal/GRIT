#ifndef UTIL_TENSORS_H
#define UTIL_TENSORS_H

#include <util_is_finite.h>
#include <util_is_number.h>

#include <cassert>
#include <cmath>

namespace util
{

  typedef double Tensor0;

  class Tensor2
  {
  public:

    double m_00;
    double m_01;
    double m_10;
    double m_11;


  public:

    Tensor2()
    : m_00(0.0)
    , m_01(0.0)
    , m_10(0.0)
    , m_11(0.0)
    {}

    Tensor2(Tensor2 const & T)
    : m_00(T.m_00)
    , m_01(T.m_01)
    , m_10(T.m_10)
    , m_11(T.m_11)
    {}

  };

  inline Tensor2 identity()
  {
    Tensor2 C;

    C.m_00 = 1.0;  C.m_01 = 0.0;
    C.m_10 = 0.0;  C.m_11 = 1.0;

    assert( is_finite(C.m_00) || !"identity(): C.m_00 was not a finite number");
    assert( is_finite(C.m_01) || !"identity(): C.m_01 was not a finite number");
    assert( is_finite(C.m_10) || !"identity(): C.m_10 was not a finite number");
    assert( is_finite(C.m_11) || !"identity(): C.m_11 was not a finite number");

    return C;
  }

  inline Tensor2 trans(Tensor2 const & A)
  {
    assert( is_finite(A.m_00) || !"trans(): A.m_00 was not a finite number");
    assert( is_finite(A.m_01) || !"trans(): A.m_01 was not a finite number");
    assert( is_finite(A.m_10) || !"trans(): A.m_10 was not a finite number");
    assert( is_finite(A.m_11) || !"trans(): A.m_11 was not a finite number");

    Tensor2 B;
    B.m_00 = A.m_00;  B.m_01 = A.m_10;
    B.m_10 = A.m_01;  B.m_11 = A.m_11;

    assert( is_finite(B.m_00) || !"trans(): B.m_00 was not a finite number");
    assert( is_finite(B.m_01) || !"trans(): B.m_01 was not a finite number");
    assert( is_finite(B.m_10) || !"trans(): B.m_10 was not a finite number");
    assert( is_finite(B.m_11) || !"trans(): B.m_11 was not a finite number");

    return B;
  }

  inline Tensor2 mul(double const & a,Tensor2 const & B)
  {
    Tensor2 C;

    assert( is_finite(a)      || !"mul(): a was not a finite number");
    assert( is_finite(B.m_00) || !"mul(): B.m_00 was not a finite number");
    assert( is_finite(B.m_01) || !"mul(): B.m_01 was not a finite number");
    assert( is_finite(B.m_10) || !"mul(): B.m_10 was not a finite number");
    assert( is_finite(B.m_11) || !"mul(): B.m_11 was not a finite number");

    C.m_00 = a*B.m_00;  C.m_01 = a*B.m_01;
    C.m_10 = a*B.m_10;  C.m_11 = a*B.m_11;

    assert( is_finite(C.m_00) || !"mul(): C.m_00 was not a finite number");
    assert( is_finite(C.m_01) || !"mul(): C.m_01 was not a finite number");
    assert( is_finite(C.m_10) || !"mul(): C.m_10 was not a finite number");
    assert( is_finite(C.m_11) || !"mul(): C.m_11 was not a finite number");

    return C;
  }

  inline Tensor2 mul(Tensor2 const & A,Tensor2 const & B)
  {
    assert( is_finite(A.m_00) || !"mul(): A.m_00 was not a finite number");
    assert( is_finite(A.m_01) || !"mul(): A.m_01 was not a finite number");
    assert( is_finite(A.m_10) || !"mul(): A.m_10 was not a finite number");
    assert( is_finite(A.m_11) || !"mul(): A.m_11 was not a finite number");

    assert( is_finite(B.m_00) || !"mul(): B.m_00 was not a finite number");
    assert( is_finite(B.m_01) || !"mul(): B.m_01 was not a finite number");
    assert( is_finite(B.m_10) || !"mul(): B.m_10 was not a finite number");
    assert( is_finite(B.m_11) || !"mul(): B.m_11 was not a finite number");

    Tensor2 C;

    C.m_00 = A.m_00*B.m_00 + A.m_01*B.m_10;
    C.m_01 = A.m_00*B.m_01 + A.m_01*B.m_11;
    C.m_10 = A.m_10*B.m_00 + A.m_11*B.m_10;
    C.m_11 = A.m_10*B.m_01 + A.m_11*B.m_11;

    assert( is_finite(C.m_00) || !"mul(): C.m_00 was not a finite number");
    assert( is_finite(C.m_01) || !"mul(): C.m_01 was not a finite number");
    assert( is_finite(C.m_10) || !"mul(): C.m_10 was not a finite number");
    assert( is_finite(C.m_11) || !"mul(): C.m_11 was not a finite number");

    return C;
  }

  inline double det(Tensor2 const & A)
  {
    assert( is_finite(A.m_00) || !"det(): A.m_00 was not a finite number");
    assert( is_finite(A.m_01) || !"det(): A.m_01 was not a finite number");
    assert( is_finite(A.m_10) || !"det(): A.m_10 was not a finite number");
    assert( is_finite(A.m_11) || !"det(): A.m_11 was not a finite number");

    double const value = A.m_00*A.m_11 - A.m_01*A.m_10;

    assert( is_finite(value) || !"det(): value was not a finite number");

    return value;
  }

  inline double trace(Tensor2 const & A)
  {
    assert( is_finite(A.m_00) || !"trace(): A.m_00 was not a finite number");
    assert( is_finite(A.m_01) || !"trace(): A.m_01 was not a finite number");
    assert( is_finite(A.m_10) || !"trace(): A.m_10 was not a finite number");
    assert( is_finite(A.m_11) || !"trace(): A.m_11 was not a finite number");

    double const value = A.m_00 + A.m_11;

    assert( is_finite(value) || !"trace(): value was not a finite number");

    return value;
  }

  inline double concat(Tensor2 const & A,Tensor2 const & B)
  {
    assert( is_finite(A.m_00) || !"concat(): A.m_00 was not a finite number");
    assert( is_finite(A.m_01) || !"concat(): A.m_01 was not a finite number");
    assert( is_finite(A.m_10) || !"concat(): A.m_10 was not a finite number");
    assert( is_finite(A.m_11) || !"concat(): A.m_11 was not a finite number");

    assert( is_finite(B.m_00) || !"concat(): B.m_00 was not a finite number");
    assert( is_finite(B.m_01) || !"concat(): B.m_01 was not a finite number");
    assert( is_finite(B.m_10) || !"concat(): B.m_10 was not a finite number");
    assert( is_finite(B.m_11) || !"concat(): B.m_11 was not a finite number");


    double const dotdot = A.m_00*B.m_00 + A.m_01*B.m_01 + A.m_10*B.m_10 + A.m_11*B.m_11;

    assert( is_finite(dotdot) || !"concat(): dotdot was not a finite number");

    return dotdot;
  }

  inline Tensor2 add(Tensor2 const & A,Tensor2 const & B)
  {
    assert( is_finite(A.m_00) || !"add(): A.m_00 was not a finite number");
    assert( is_finite(A.m_01) || !"add(): A.m_01 was not a finite number");
    assert( is_finite(A.m_10) || !"add(): A.m_10 was not a finite number");
    assert( is_finite(A.m_11) || !"add(): A.m_11 was not a finite number");

    assert( is_finite(B.m_00) || !"add(): B.m_00 was not a finite number");
    assert( is_finite(B.m_01) || !"add(): B.m_01 was not a finite number");
    assert( is_finite(B.m_10) || !"add(): B.m_10 was not a finite number");
    assert( is_finite(B.m_11) || !"add(): B.m_11 was not a finite number");

    Tensor2 C;

    C.m_00 = A.m_00 + B.m_00;
    C.m_01 = A.m_01 + B.m_01;
    C.m_10 = A.m_10 + B.m_10;
    C.m_11 = A.m_11 + B.m_11;

    assert( is_finite(C.m_00) || !"add(): C.m_00 was not a finite number");
    assert( is_finite(C.m_01) || !"add(): C.m_01 was not a finite number");
    assert( is_finite(C.m_10) || !"add(): C.m_10 was not a finite number");
    assert( is_finite(C.m_11) || !"add(): C.m_11 was not a finite number");

    return C;
  }

  inline Tensor2 sub(Tensor2 const & A,Tensor2 const & B)
  {
    assert( is_finite(A.m_00) || !"sub(): A.m_00 was not a finite number");
    assert( is_finite(A.m_01) || !"sub(): A.m_01 was not a finite number");
    assert( is_finite(A.m_10) || !"sub(): A.m_10 was not a finite number");
    assert( is_finite(A.m_11) || !"sub(): A.m_11 was not a finite number");

    assert( is_finite(B.m_00) || !"sub(): B.m_00 was not a finite number");
    assert( is_finite(B.m_01) || !"sub(): B.m_01 was not a finite number");
    assert( is_finite(B.m_10) || !"sub(): B.m_10 was not a finite number");
    assert( is_finite(B.m_11) || !"sub(): B.m_11 was not a finite number");

    Tensor2 C;

    C.m_00 = A.m_00-B.m_00;
    C.m_01 = A.m_01-B.m_01;
    C.m_10 = A.m_10-B.m_10;
    C.m_11 = A.m_11-B.m_11;

    assert( is_finite(C.m_00) || !"sub(): C.m_00 was not a finite number");
    assert( is_finite(C.m_01) || !"sub(): C.m_01 was not a finite number");
    assert( is_finite(C.m_10) || !"sub(): C.m_10 was not a finite number");
    assert( is_finite(C.m_11) || !"sub(): C.m_11 was not a finite number");

    return C;
  }

  inline Tensor2 inverse(Tensor2 const & A)
  {
    Tensor2 C;

    assert( is_finite(A.m_00) || !"inverse(): A.m_00 was not a finite number");
    assert( is_finite(A.m_01) || !"inverse(): A.m_01 was not a finite number");
    assert( is_finite(A.m_10) || !"inverse(): A.m_10 was not a finite number");
    assert( is_finite(A.m_11) || !"inverse(): A.m_11 was not a finite number");

    double const D = det(A);

    assert( fabs(D)>0 || !"inverse(): A was singular");

    C.m_00 =  A.m_11/D;
    C.m_01 = -A.m_01/D;
    C.m_10 = -A.m_10/D;
    C.m_11 =  A.m_00/D;

    assert( is_finite(C.m_00) || !"inverse(): C.m_00 was not a finite number");
    assert( is_finite(C.m_01) || !"inverse(): C.m_01 was not a finite number");
    assert( is_finite(C.m_10) || !"inverse(): C.m_10 was not a finite number");
    assert( is_finite(C.m_11) || !"inverse(): C.m_11 was not a finite number");
    
    {
      // Only for debugging
      Tensor2 const I = mul(A,C);
      
      assert( fabs(1-I.m_00)<10e-7 || !"inverse(): Could not compute inverse");
      assert( fabs(I.m_01)<10e-7   || !"inverse(): Could not compute inverse");
      assert( fabs(I.m_10)<10e-7   || !"inverse(): Could not compute inverse");
      assert( fabs(1-I.m_11)<10e-7 || !"inverse(): Could not compute inverse");
    }
    
    return C;
  }


  class Tensor1
  {
  public:

    double m_0;
    double m_1;

  };

  inline Tensor1 make(double a, double b)
  {
    Tensor1 T;
    T.m_0 = a;
    T.m_1 = b;

    assert( is_finite(T.m_0) || !"make(): m_0 was not a finite number");
    assert( is_finite(T.m_1) || !"make(): m_1 was not a finite number");

    return T;
  }

  inline double inner_prod( Tensor1 const & A, Tensor1 const & B)
  {
    return A.m_0*B.m_0 +  A.m_1*B.m_1;
  }

  inline Tensor1 mul(double const & a,Tensor1 const & B)
  {
    Tensor1 C;
    C.m_0 = a*B.m_0;
    C.m_1 = a*B.m_1;
    return C;
  }

  inline Tensor2 outer_prod( Tensor1 const & A, Tensor1 const & B)
  {
    Tensor2 C;

    assert( is_finite(A.m_0) || !"outer_prod(): A.m_0 was not a finite number");
    assert( is_finite(A.m_1) || !"outer_prod(): A.m_1 was not a finite number");
    assert( is_finite(B.m_0) || !"outer_prod(): B.m_0 was not a finite number");
    assert( is_finite(B.m_1) || !"outer_prod(): B.m_1 was not a finite number");

    C.m_00 = A.m_0*B.m_0;
    C.m_01 = A.m_0*B.m_1;
    C.m_10 = A.m_1*B.m_0;
    C.m_11 = A.m_1*B.m_1;

    assert( is_finite(C.m_00) || !"outer_prod(): C.m_00 was not a finite number");
    assert( is_finite(C.m_01) || !"outer_prod(): C.m_01 was not a finite number");
    assert( is_finite(C.m_10) || !"outer_prod(): C.m_10 was not a finite number");
    assert( is_finite(C.m_11) || !"outer_prod(): C.m_11 was not a finite number");

    return C;
  }

  inline bool is_valid(util::Tensor2 const & A)
  {
    assert( is_finite(A.m_00) || !"is_valid() not finite number");
    assert( is_number(A.m_00) || !"is_valid() not a number"     );
    assert( is_finite(A.m_01) || !"is_valid() not finite number");
    assert( is_number(A.m_01) || !"is_valid() not a number"     );
    assert( is_finite(A.m_10) || !"is_valid() not finite number");
    assert( is_number(A.m_10) || !"is_valid() not a number"     );
    assert( is_finite(A.m_11) || !"is_valid() not finite number");
    assert( is_number(A.m_11) || !"is_valid() not a number"     );

    return true;
  }

  inline bool is_valid(util::Tensor1 const & a)
  {
    assert(is_finite(a.m_0) || !"is_valid() not finite number");
    assert(is_number(a.m_0) || !"is_valid() not a number"     );
    assert(is_finite(a.m_1) || !"is_valid() not finite number");
    assert(is_number(a.m_1) || !"is_valid() not a number"     );

    return true;
  }

  inline bool is_valid(util::Tensor0 const & a)
  {
    assert(is_finite(a) || !"is_valid() not finite number");
    assert(is_number(a) || !"is_valid() not a number"     );

    return true;
  }

  class Block3x3Tensor2
  {
  public:

    util::Tensor2 m_block[3][3];

  };

  inline Block3x3Tensor2 mul(double const & a, Block3x3Tensor2 const & B)
  {
    Block3x3Tensor2 A;

    A.m_block[0][0] = mul(a, B.m_block[0][0]);
    A.m_block[0][1] = mul(a, B.m_block[0][1]);
    A.m_block[0][2] = mul(a, B.m_block[0][2]);

    A.m_block[1][0] = mul(a, B.m_block[1][0]);
    A.m_block[1][1] = mul(a, B.m_block[1][1]);
    A.m_block[1][2] = mul(a, B.m_block[1][2]);

    A.m_block[2][0] = mul(a, B.m_block[2][0]);
    A.m_block[2][1] = mul(a, B.m_block[2][1]);
    A.m_block[2][2] = mul(a, B.m_block[2][2]);
    
    return A;
  }


  class Block3x1Tensor1
  {
  public:

    util::Tensor1 m_block[3];

  };

  class Block2x1Tensor1
  {
  public:

    util::Tensor1 m_block[2];

  };

  class Block3x3Tensor0
  {
  public:

    double m_block[3][3];

  };

}//namespace util

// UTIL_TENSORS_H
#endif
