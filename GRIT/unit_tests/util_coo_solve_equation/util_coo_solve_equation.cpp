#include <util_coo_matrix.h>
#include <util_coo_pcg.h>
#include <util_coo_bicg.h>
#include <util_coo_cr.h>
#include <util_coo_bicgstab.h>
#include <util_coo_gmres.h>


#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/test_tools.hpp>


#include <boost/random.hpp>
#include <boost/cast.hpp>


template<typename T>
class Random
{
public:

  typedef boost::minstd_rand  generator_type;

protected:

  static generator_type &  generator()
  {
    static generator_type tmp(static_cast<unsigned int>(std::time(0)));
    return tmp;
  }

public:

  typedef boost::uniform_real<T>                                         distribution_type;
  typedef boost::variate_generator<generator_type&, distribution_type >  random_type;

  distribution_type       m_distribution;
  random_type             m_random;

public:

  Random()
  : m_distribution(  boost::numeric_cast<T>(0.0)
                   , boost::numeric_cast<T>(1.0)
                   )
  , m_random(generator(), m_distribution)
  {}

  Random(T const & lower,T const & upper)
  : m_distribution(lower,upper)
  , m_random(generator(), m_distribution)
  {}

private:

  Random(Random const & rnd){}

  Random & operator=(Random const & rnd){return *this;}

public:

  T operator()() { return m_random();  }

  bool operator==(Random const & rnd) const { return m_distribution == rnd.m_distribution; }

};

template<typename T>
inline util::COOMatrix<T>  make_random_matrix( unsigned int const & m, unsigned int const & n )
{
  typedef util::COOMatrix<T>   matrix_type;

  matrix_type A;

  Random<T>  value( T(0.0), T(1.0));

  A.init(m,n,n);

  for(unsigned int i=0u;i<m;++i)
  {
    for(unsigned int j=0u;j<n;++j)
    {
      T const val = value();
      A.insert(i,j,val);
    }
  }
  return A;

}

template<typename T>
inline std::vector<T> make_random_vector( unsigned int const & n)
{
  Random<T>  value( T(0.0), T(1.0));

  std::vector<T> v(n, T(0.0));

  for(unsigned int i=0u; i<n; ++i)
  {
    v[i] = value();
  }

  return v;
}

template<typename T>
inline util::COOMatrix<T>  make_hilbert_matrix( unsigned int const & n )
{
  util::COOMatrix<T> A;

  A.init(n,n,n);

  for(unsigned int i=0u;i<n;++i)
  {
    for(unsigned int j=0u;j<n;++j)
    {
      T const val = T(1.0) / (   T(i+j+1u) );
      A.insert(i,j,val);
    }
  }
  return A;

}

/**
 * Factory function for creating a tri diagonal matrix (special case of a banded matrix).
 *
 * @param n       The size of the matrix to make. The resulting matrix will be of size (n X n)
 * @param a       The value to store on the diagonal of the matrix
 * @param b       The value to store above the diagonal.
 * @param c       The value to store below the diagonal.
 *
 */
template<typename T>
inline util::COOMatrix<T>  make_triadiagonal_matrix( unsigned int const & n
                                                    , T const & a = T(4.0)
                                                    , T const & b = T(1.0)
                                                    , T const & c = T(1.0)
                                                    )
{
  util::COOMatrix<T> A;

  A.init(n,n,3u);

  for(unsigned int i=0u;i<n;++i)
  {
    for(unsigned int j=0u;j<n;++j)
    {
      T val = 0.0;

      if( i==j)
      {
        val = a;
      }
      if( i==j+1u)
      {
        val = b;
      }
      if( i==j-1u)
      {
        val = c;
      }
      if (fabs(val)> T(0.0))
      {
        A.insert(i,j,val);
      }

    }
  }
  return A;
}

template<typename T>
inline util::COOMatrix<T>  make_diagonal_dominant_matrix( unsigned int const & n )
{
  util::COOMatrix<T> A;

  A.init(n,n,n);

  for(unsigned int i=0u;i<n;++i)
  {
    for(unsigned int j=0u;j<n;++j)
    {
      if(i==j)
      {
        A.insert(i,j,T(n*n) );
      }
      else if (fabs( T(j) - T(i) ) <= 2.0 )
      {
        T const val = fabs(  T(j) - T(i) );
        A.insert(i,j,val);
      }
    }
  }
  return A;
}

template<typename T>
class Problem
{
public:

  util::COOMatrix<T>    m_A;   ///< System matrix (assumed non-singular at least)
  std::vector<T>        m_x;   ///< The known solution
  std::vector<T>        m_b;   ///< Right hand side vector

};

template<typename T>
inline Problem<T> make_problem(unsigned int const & n)
{
  Problem<T> problem;

  // problem.m_A = make_hilbert_matrix<T>(n); // Hilbert matrices seems to be badly conditioned as n goes up
  problem.m_A = make_triadiagonal_matrix<T>(n);
  // problem.m_A = make_diagonal_dominant_matrix<T>(n);
  problem.m_x = make_random_vector<T>(n);
  problem.m_b = util::mul(problem.m_A, problem.m_x);

  return problem;
}

BOOST_AUTO_TEST_SUITE(coo_solve_equation);

BOOST_AUTO_TEST_CASE(pcg_testing)
{
  typedef double T;

  util::COOMatrix<T> A  = util::make_identity<T>(4u);
  util::COOMatrix<T> M  = util::make_identity<T>(4u);

  unsigned int    const   max_iterations = 10u;
  unsigned int    const   restart        = 5u;
  T               const   tolerance      = 1e-10;
  bool            const   verbose        = true;
  std::vector<T>          residual;

  std::vector<T> b(4u);

  b[0u] = 1.0;
  b[1u] = 2.0;
  b[2u] = 3.0;
  b[3u] = 4.0;

  std::vector<T> x(4u, 0.0);

  bool success = util::pcg(M, A, x, b, max_iterations, restart, tolerance, verbose, &residual);

  BOOST_CHECK(success);

  BOOST_CHECK_CLOSE(x[0u], 1.0, 0.01);
  BOOST_CHECK_CLOSE(x[1u], 2.0, 0.01);
  BOOST_CHECK_CLOSE(x[2u], 3.0, 0.01);
  BOOST_CHECK_CLOSE(x[3u], 4.0, 0.01);
}

BOOST_AUTO_TEST_CASE(pcg_random_testing)
{
  typedef double T;

  unsigned int    const   max_samples    = 10u;      ///< Maximum number of random problems to solve.
  unsigned int    const   n              = 10u;      ///< Problem size
  unsigned int    const   max_iterations = 100u;
  unsigned int    const   restart        = 1u;
  T               const   tolerance      = 1e-10;
  bool            const   verbose        = true;
  std::vector<T>          residual;

  for (unsigned int sample=0u; sample < max_samples; ++sample)
  {
    Problem<T> problem = make_problem<T>(n);

    util::COOMatrix<T> M  = util::make_identity<T>(n);

    std::vector<T> x(n, 0.0);

    bool success = util::pcg(
                             M
                             , problem.m_A
                             , x
                             , problem.m_b
                             , max_iterations
                             , restart
                             , tolerance
                             , verbose
                             , &residual
                             );

    BOOST_CHECK(success);

    if(success)
    {
      for (unsigned int i=0u; i < n; ++i)
      {
        BOOST_CHECK_CLOSE(x[i], problem.m_x[i], 0.1);
      }
    }

  }
}

BOOST_AUTO_TEST_CASE(bicg_testing)
{
  typedef double T;

  util::COOMatrix<T> A  = util::make_identity<T>(4u);
  util::COOMatrix<T> At = util::transpose(A);

  util::COOMatrix<T> M  = util::make_identity<T>(4u);
  util::COOMatrix<T> Mt = util::transpose(M);

  unsigned int    const   max_iterations = 10u;
  T               const   tolerance      = 1e-10;
  bool            const   verbose        = true;
  std::vector<T>          residual;

  std::vector<T> b(4u);

  b[0u] = 1.0;
  b[1u] = 2.0;
  b[2u] = 3.0;
  b[3u] = 4.0;

  std::vector<T> x(4u, 0.0);

  bool success = util::bicg(A, At, x, b, M, Mt, max_iterations, tolerance, verbose, &residual);

  BOOST_CHECK(success);

  BOOST_CHECK_CLOSE(x[0u], 1.0, 0.01);
  BOOST_CHECK_CLOSE(x[1u], 2.0, 0.01);
  BOOST_CHECK_CLOSE(x[2u], 3.0, 0.01);
  BOOST_CHECK_CLOSE(x[3u], 4.0, 0.01);
}

BOOST_AUTO_TEST_CASE(bicg_random_testing)
{
  typedef double T;

  unsigned int    const   max_samples    = 10u;   ///< Maximum number of random problems to solve.
  unsigned int    const   n              = 10u;   ///< Problem size
  unsigned int    const   max_iterations = 100u;
  T               const   tolerance      = 1e-10;
  bool            const   verbose        = true;
  std::vector<T>          residual;

  for (unsigned int sample=0u; sample < max_samples; ++sample)
  {
    Problem<T> problem = make_problem<T>(n);

    util::COOMatrix<T> At = util::transpose(problem.m_A);
    util::COOMatrix<T> M  = util::make_identity<T>(n);
    util::COOMatrix<T> Mt = util::transpose(M);

    std::vector<T> x(n, 0.0);

    bool success = util::bicg(
                              problem.m_A
                              , At
                              , x
                              , problem.m_b
                              , M
                              , Mt
                              , max_iterations
                              , tolerance
                              , verbose
                              , &residual
                              );

    BOOST_CHECK(success);

    if(success)
    {
      for (unsigned int i=0u; i < n; ++i)
      {
        BOOST_CHECK_CLOSE(x[i], problem.m_x[i], 0.1);
      }
    }

  }
}

BOOST_AUTO_TEST_CASE(cr_testing)
{
  typedef double T;

  util::COOMatrix<T> A  = util::make_identity<T>(4u);
  util::COOMatrix<T> M  = util::make_identity<T>(4u);

  unsigned int    const   max_iterations = 10u;
  unsigned int    const   restart       = 5u;
  T               const   tolerance      = 1e-10;
  bool            const   verbose        = true;
  std::vector<T>          residual;

  std::vector<T> b(4u);

  b[0u] = 1.0;
  b[1u] = 2.0;
  b[2u] = 3.0;
  b[3u] = 4.0;

  std::vector<T> x(4u, 0.0);

  bool success = util::cr(M, A, x, b, max_iterations, restart, tolerance, verbose, &residual);

  BOOST_CHECK(success);

  BOOST_CHECK_CLOSE(x[0u], 1.0, 0.01);
  BOOST_CHECK_CLOSE(x[1u], 2.0, 0.01);
  BOOST_CHECK_CLOSE(x[2u], 3.0, 0.01);
  BOOST_CHECK_CLOSE(x[3u], 4.0, 0.01);
}

BOOST_AUTO_TEST_CASE(cr_random_testing)
{
  typedef double T;

  unsigned int    const   max_samples    = 10u;    ///< Maximum number of random problems to solve.
  unsigned int    const   n              = 10u;    ///< Problem size
  unsigned int    const   max_iterations = 100u;
  unsigned int    const   restart        = 1u;
  T               const   tolerance      = 1e-10;
  bool            const   verbose        = true;
  std::vector<T>          residual;

  for (unsigned int sample=0u; sample < max_samples; ++sample)
  {
    Problem<T> problem = make_problem<T>(n);

    util::COOMatrix<T> M  = util::make_identity<T>(n);

    std::vector<T> x(n, 0.0);

    bool success = util::cr(
                             M
                             , problem.m_A
                             , x
                             , problem.m_b
                             , max_iterations
                             , restart
                             , tolerance
                             , verbose
                             , &residual
                             );

    BOOST_CHECK(success);

    if(success)
    {
      for (unsigned int i=0u; i < n; ++i)
      {
        BOOST_CHECK_CLOSE(x[i], problem.m_x[i], 0.1);
      }
    }

  }
}

BOOST_AUTO_TEST_CASE(bicgstab_testing)
{
  typedef double T;

  util::COOMatrix<T> A  = util::make_identity<T>(4u);
  util::COOMatrix<T> M  = util::make_identity<T>(4u);

  unsigned int    const   max_iterations = 10u;
  T               const   tolerance      = 1e-10;
  bool            const   verbose        = true;
  std::vector<T>          residual;

  std::vector<T> b(4u);

  b[0u] = 1.0;
  b[1u] = 2.0;
  b[2u] = 3.0;
  b[3u] = 4.0;

  std::vector<T> x(4u, 0.0);

  bool success = util::bicgstab(M, A, x, b, max_iterations, tolerance, verbose, &residual);

  BOOST_CHECK(success);

  BOOST_CHECK_CLOSE(x[0u], 1.0, 0.01);
  BOOST_CHECK_CLOSE(x[1u], 2.0, 0.01);
  BOOST_CHECK_CLOSE(x[2u], 3.0, 0.01);
  BOOST_CHECK_CLOSE(x[3u], 4.0, 0.01);
}

BOOST_AUTO_TEST_CASE(bicgstab_random_testing)
{
  typedef double T;

  unsigned int    const   max_samples    = 10u;      ///< Maximum number of random problems to solve.
  unsigned int    const   n              = 10u;      ///< Problem size
  unsigned int    const   max_iterations = 100u;
  T               const   tolerance      = 1e-10;
  bool            const   verbose        = true;
  std::vector<T>          residual;

  for (unsigned int sample=0u; sample < max_samples; ++sample)
  {
    Problem<T> problem = make_problem<T>(n);

    util::COOMatrix<T> M  = util::make_identity<T>(n);

    std::vector<T> x(n, 0.0);

    bool success = util::bicgstab(
                            M
                            , problem.m_A
                            , x
                            , problem.m_b
                            , max_iterations
                            , tolerance
                            , verbose
                            , &residual
                            );

    BOOST_CHECK(success);

    if(success)
    {
      for (unsigned int i=0u; i < n; ++i)
      {
        BOOST_CHECK_CLOSE(x[i], problem.m_x[i], 0.1);
      }
    }

  }
}

BOOST_AUTO_TEST_CASE(gmres_testing)
{
  typedef double T;

  util::COOMatrix<T> A  = util::make_identity<T>(4u);
  util::COOMatrix<T> M  = util::make_identity<T>(4u);

  unsigned int    const   max_iterations = 10u;
  unsigned int    const   restart        = 5u;
  T               const   tolerance      = 1e-10;
  bool            const   verbose        = true;
  std::vector<T>          residual;

  std::vector<T> b(4u);

  b[0u] = 1.0;
  b[1u] = 2.0;
  b[2u] = 3.0;
  b[3u] = 4.0;

  std::vector<T> x(4u, 0.0);

  bool success = util::gmres(M, A, x, b, max_iterations, restart, tolerance, verbose, &residual);

  BOOST_CHECK(success);

  BOOST_CHECK_CLOSE(x[0u], 1.0, 0.01);
  BOOST_CHECK_CLOSE(x[1u], 2.0, 0.01);
  BOOST_CHECK_CLOSE(x[2u], 3.0, 0.01);
  BOOST_CHECK_CLOSE(x[3u], 4.0, 0.01);
}

BOOST_AUTO_TEST_CASE(gmres_random_testing)
{
  typedef double T;

  unsigned int    const   max_samples    = 10u;      ///< Maximum number of random problems to solve.
  unsigned int    const   n              = 10u;      ///< Problem size
  unsigned int    const   max_iterations = 100u;
  unsigned int    const   restart        = 1u;

  // 2015-05-08 Marek: the tolerance 1e-10 was probably a bit too much for gmres.
  //                   It seems to have a tendency to get "stuck" at a certain residual value.
  T               const   tolerance      = 1e-7;
  bool            const   verbose        = true;
  std::vector<T>          residual;


  for (unsigned int sample=0u; sample < max_samples; ++sample)
  {
    Problem<T> problem = make_problem<T>(n);

    util::COOMatrix<T> M  = util::make_identity<T>(n);

    std::vector<T> x(n, 0.0);

    bool success = util::gmres(
                             M
                             , problem.m_A
                             , x
                             , problem.m_b
                             , max_iterations
                             , restart
                             , tolerance
                             , verbose
                             , &residual
                             );

    BOOST_CHECK(success);

    if(success)
    {
      for (unsigned int i=0u; i < n; ++i)
      {
        BOOST_CHECK_CLOSE(x[i], problem.m_x[i], 0.1);
      }
    }

  }
}

BOOST_AUTO_TEST_SUITE_END();
