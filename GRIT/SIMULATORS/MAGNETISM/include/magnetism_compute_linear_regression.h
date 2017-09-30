#ifndef MAGNETISM_COMPUTE_LINEAR_REGRESSION_H
#define MAGNETISM_COMPUTE_LINEAR_REGRESSION_H

#include <util_solve3x3.h>

#include <vector>

namespace magnetism
{

  inline void compute_linear_regression(
                                        unsigned int              const & m
                                        , std::vector<unsigned int> const & ring
                                        , std::vector<double>       const & x
                                        , std::vector<double>       const & y
                                        , std::vector<double>       const & phi
                                        , double                          & error
                                        )
  {
    double const D = ring.size();

    assert(D>3u || !"compute_linear_regression(): Bad data");

    //
    //  Given the linear model
    //
    //    L(x,y,c)  =   c0 + x c1 + y c2
    //
    //  where c = [c0, c1, c2] we wish to find c^* such that
    //
    //    c^* = argmin_c  sum_{i \in ring} 1/2 ( L(x_i,y_i,c)-phi_i )^2
    //
    // Let
    //
    //  B = | 1 x_1 y_1 |  and  Phi  =  | phi_1 |
    //      | 1 x_2 y_2 |               | phi_2 |
    //      |  ...      |               | ...   |
    //      | 1 x_D y_D |               | phi_D |
    //
    // Then the first order optimality conditions give that we must
    // solve for c^* such that
    //
    //    (B^T B) c^* = (B^T Phi)
    //

    util::Matrix3x3<double> A;
    util::Vector3<double> b;

    for(unsigned int d=0; d<D;++d)
    {
      unsigned int const i = ring[d];

      A(0,0) += 1;
      A(0,1) += x[i];
      A(0,2) += y[i];

      A(1,0) += x[i];
      A(1,1) += x[i]*x[i];
      A(1,2) += x[i]*y[i];

      A(2,0) += y[i];
      A(2,1) += y[i]*x[i];
      A(2,2) += y[i]*y[i];

      b(0) += phi[i];
      b(1) += x[i]*phi[i];
      b(2) += y[i]*phi[i];
    }

    util::Vector3<double> z;

    util::solve3x3(A,b,z);

    std::vector<double> c(3);

    c[0] = z(0);
    c[1] = z(1);
    c[2] = z(2);

    error = 0.0;
    for (unsigned int d = 0u; d<D; ++d)
    {
      unsigned int const i = ring[d];

      double const tmp = (c[0] + c[1]*x[i] + c[2]*y[i]) - phi[i];
      
      error += tmp*tmp;
    }
    error /= 2.0;
  }
  
} // namespace magnetism

// MAGNETISM_COMPUTE_LINEAR_REGRESSION_H
#endif
