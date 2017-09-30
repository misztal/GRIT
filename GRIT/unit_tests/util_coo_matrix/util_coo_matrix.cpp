#include <util_coo_matrix.h>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/test_tools.hpp>

BOOST_AUTO_TEST_SUITE(coo_matrix);

BOOST_AUTO_TEST_CASE(incremental_insert)
{
  util::COOMatrix<float> A;

  BOOST_CHECK_EQUAL(A.nzeros(), 0u );
  BOOST_CHECK_EQUAL(A.rows(), 0u );
  BOOST_CHECK_EQUAL(A.cols(), 0u );

  BOOST_CHECK_NO_THROW( A.init(5u, 5u, 3u) );
  BOOST_CHECK_EQUAL(A.nzeros(), 0u );
  BOOST_CHECK_EQUAL(A.rows(), 5u );
  BOOST_CHECK_EQUAL(A.cols(), 5u );

  BOOST_CHECK( A.insert(0u, 0u, 1.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 1u );

  BOOST_CHECK( A.insert(0u, 1u, 2.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 2u );

  BOOST_CHECK( A.insert(0u, 2u, 3.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 3u );

  BOOST_CHECK( A.insert(1u, 0u, 4.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 4u );

  BOOST_CHECK( A.insert(1u, 1u, 5.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 5u );

  BOOST_CHECK( A.insert(1u, 2u, 6.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 6u );

  BOOST_CHECK( A.insert(2u, 0u, 7.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 7u );

  BOOST_CHECK( A.insert(2u, 1u, 8.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 8u );

  BOOST_CHECK( A.insert(2u, 2u, 9.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 9u );

  BOOST_CHECK( A.insert(3u, 0u, 10.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 10u );

  BOOST_CHECK( A.insert(3u, 1u, 11.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 11u );

  BOOST_CHECK( A.insert(3u, 2u, 12.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 12u );

  BOOST_CHECK( A.insert(4u, 0u, 13.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 13u );

  BOOST_CHECK( A.insert(4u, 1u, 14.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 14u );

  BOOST_CHECK( A.insert(4u, 2u, 15.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 15u );

  std::cout << util::to_string(A) << std::endl;
}

BOOST_AUTO_TEST_CASE(backward_insert)
{
  util::COOMatrix<float> A;

  BOOST_CHECK_EQUAL(A.nzeros(), 0u );
  BOOST_CHECK_EQUAL(A.rows(), 0u );
  BOOST_CHECK_EQUAL(A.cols(), 0u );

  BOOST_CHECK_NO_THROW( A.init(5u,5u, 3u) );
  BOOST_CHECK_EQUAL(A.nzeros(), 0u );
  BOOST_CHECK_EQUAL(A.rows(), 5u );
  BOOST_CHECK_EQUAL(A.cols(), 5u );

  BOOST_CHECK( A.insert(4u, 2u, 15.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 1u );

  BOOST_CHECK( A.insert(4u, 1u, 14.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 2u );

  BOOST_CHECK( A.insert(4u, 0u, 13.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 3u );

  BOOST_CHECK( A.insert(3u, 2u, 12.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 4u );

  BOOST_CHECK( A.insert(3u, 1u, 11.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 5u );

  BOOST_CHECK( A.insert(3u, 0u, 10.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 6u );

  BOOST_CHECK( A.insert(2u, 2u, 9.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 7u );

  BOOST_CHECK( A.insert(2u, 1u, 8.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 8u );

  BOOST_CHECK( A.insert(2u, 0u, 7.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 9u );

  BOOST_CHECK( A.insert(1u, 2u, 6.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 10u );

  BOOST_CHECK( A.insert(1u, 1u, 5.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 11u );

  BOOST_CHECK( A.insert(1u, 0u, 4.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 12u );

  BOOST_CHECK( A.insert(0u, 2u, 3.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 13u );

  BOOST_CHECK( A.insert(0u, 1u, 2.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 14u );

  BOOST_CHECK( A.insert(0u, 0u, 1.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 15u );

  std::cout << util::to_string(A) << std::endl;

}

BOOST_AUTO_TEST_CASE(inside_insert)
{
  util::COOMatrix<float> A;

  BOOST_CHECK_NO_THROW( A.init(5u,5u, 3u) );

  BOOST_CHECK( A.insert(0u, 0u, 1.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 1u );

  BOOST_CHECK( A.insert(0u, 1u, 2.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 2u );

  BOOST_CHECK( A.insert(4u, 1u, 14.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 3u );

  BOOST_CHECK( A.insert(4u, 2u, 15.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 4u );

  BOOST_CHECK( A.insert(0u, 2u, 3.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 5u );

  BOOST_CHECK( A.insert(1u, 0u, 4.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 6u );

  BOOST_CHECK( A.insert(1u, 1u, 5.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 7u );

  BOOST_CHECK( A.insert(3u, 0u, 10.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 8u );

  BOOST_CHECK( A.insert(3u, 1u, 11.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 9u );

  BOOST_CHECK( A.insert(3u, 2u, 12.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 10u );

  BOOST_CHECK( A.insert(4u, 0u, 13.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 11u );

  BOOST_CHECK( A.insert(2u, 2u, 9.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 12u );

  BOOST_CHECK( A.insert(2u, 1u, 8.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 13u );

  BOOST_CHECK( A.insert(2u, 0u, 7.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 14u );

  BOOST_CHECK( A.insert(1u, 2u, 6.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 15u );

  std::cout << util::to_string(A) << std::endl;
}

BOOST_AUTO_TEST_CASE(auto_growth)
{
  util::COOMatrix<float> A;

  BOOST_CHECK_NO_THROW( A.init(5u, 5u, 1u) );
  BOOST_CHECK_EQUAL(A.nzeros(), 0u );
  BOOST_CHECK_EQUAL(A.rows(), 5u );
  BOOST_CHECK_EQUAL(A.cols(), 5u );

  BOOST_CHECK( A.insert(0u, 0u, 1.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 1u );

  BOOST_CHECK( A.insert(0u, 1u, 2.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 2u );

  BOOST_CHECK( A.insert(0u, 2u, 3.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 3u );

  BOOST_CHECK( A.insert(1u, 0u, 4.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 4u );

  BOOST_CHECK( A.insert(1u, 1u, 5.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 5u );

  BOOST_CHECK( A.insert(1u, 2u, 6.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 6u );

  BOOST_CHECK( A.insert(2u, 0u, 7.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 7u );

  BOOST_CHECK( A.insert(2u, 1u, 8.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 8u );

  BOOST_CHECK( A.insert(2u, 2u, 9.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 9u );

  BOOST_CHECK( A.insert(3u, 0u, 10.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 10u );

  BOOST_CHECK( A.insert(3u, 1u, 11.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 11u );

  BOOST_CHECK( A.insert(3u, 2u, 12.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 12u );

  BOOST_CHECK( A.insert(4u, 0u, 13.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 13u );

  BOOST_CHECK( A.insert(4u, 1u, 14.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 14u );

  BOOST_CHECK( A.insert(4u, 2u, 15.0f) );
  BOOST_CHECK_EQUAL(A.nzeros(), 15u );

  std::cout << util::to_string(A) << std::endl;
}

BOOST_AUTO_TEST_CASE(transpose)
{
  util::COOMatrix<float> A;

  BOOST_CHECK_NO_THROW( A.init(5u, 5u, 3u) );
  BOOST_CHECK( A.insert(0u, 0u, 1.0f) );
  BOOST_CHECK( A.insert(0u, 1u, 2.0f) );
  BOOST_CHECK( A.insert(0u, 2u, 3.0f) );
  BOOST_CHECK( A.insert(1u, 0u, 4.0f) );
  BOOST_CHECK( A.insert(1u, 1u, 5.0f) );
  BOOST_CHECK( A.insert(1u, 2u, 6.0f) );
  BOOST_CHECK( A.insert(2u, 0u, 7.0f) );
  BOOST_CHECK( A.insert(2u, 1u, 8.0f) );
  BOOST_CHECK( A.insert(2u, 2u, 9.0f) );
  BOOST_CHECK( A.insert(3u, 0u, 10.0f) );
  BOOST_CHECK( A.insert(3u, 1u, 11.0f) );
  BOOST_CHECK( A.insert(3u, 2u, 12.0f) );
  BOOST_CHECK( A.insert(4u, 0u, 13.0f) );
  BOOST_CHECK( A.insert(4u, 1u, 14.0f) );
  BOOST_CHECK( A.insert(4u, 2u, 15.0f) );

  std::cout << util::to_string(A) << std::endl;

  util::COOMatrix<float> At = util::transpose(A);

  std::cout << util::to_string(At) << std::endl;

  BOOST_CHECK_EQUAL(At.row_indices()[0],  0u );
  BOOST_CHECK_EQUAL(At.row_indices()[1],  0u );
  BOOST_CHECK_EQUAL(At.row_indices()[2],  0u );
  BOOST_CHECK_EQUAL(At.row_indices()[3],  0u );
  BOOST_CHECK_EQUAL(At.row_indices()[4],  0u );
  BOOST_CHECK_EQUAL(At.row_indices()[5],  1u );
  BOOST_CHECK_EQUAL(At.row_indices()[6],  1u );
  BOOST_CHECK_EQUAL(At.row_indices()[7],  1u );
  BOOST_CHECK_EQUAL(At.row_indices()[8],  1u );
  BOOST_CHECK_EQUAL(At.row_indices()[9],  1u );
  BOOST_CHECK_EQUAL(At.row_indices()[10], 2u );
  BOOST_CHECK_EQUAL(At.row_indices()[11], 2u );
  BOOST_CHECK_EQUAL(At.row_indices()[12], 2u );
  BOOST_CHECK_EQUAL(At.row_indices()[13], 2u );
  BOOST_CHECK_EQUAL(At.row_indices()[14], 2u );

  BOOST_CHECK_EQUAL(At.column_indices()[0],  0u );
  BOOST_CHECK_EQUAL(At.column_indices()[1],  1u );
  BOOST_CHECK_EQUAL(At.column_indices()[2],  2u );
  BOOST_CHECK_EQUAL(At.column_indices()[3],  3u );
  BOOST_CHECK_EQUAL(At.column_indices()[4],  4u );
  BOOST_CHECK_EQUAL(At.column_indices()[5],  0u );
  BOOST_CHECK_EQUAL(At.column_indices()[6],  1u );
  BOOST_CHECK_EQUAL(At.column_indices()[7],  2u );
  BOOST_CHECK_EQUAL(At.column_indices()[8],  3u );
  BOOST_CHECK_EQUAL(At.column_indices()[9],  4u );
  BOOST_CHECK_EQUAL(At.column_indices()[10], 0u );
  BOOST_CHECK_EQUAL(At.column_indices()[11], 1u );
  BOOST_CHECK_EQUAL(At.column_indices()[12], 2u );
  BOOST_CHECK_EQUAL(At.column_indices()[13], 3u );
  BOOST_CHECK_EQUAL(At.column_indices()[14], 4u );

  BOOST_CHECK_CLOSE(At.values()[0],  1.0, 0.01 );
  BOOST_CHECK_CLOSE(At.values()[1],  4.0, 0.01 );
  BOOST_CHECK_CLOSE(At.values()[2],  7.0, 0.01 );
  BOOST_CHECK_CLOSE(At.values()[3],  10.0, 0.01 );
  BOOST_CHECK_CLOSE(At.values()[4],  13.0, 0.01 );
  BOOST_CHECK_CLOSE(At.values()[5],  2.0, 0.01 );
  BOOST_CHECK_CLOSE(At.values()[6],  5.0, 0.01 );
  BOOST_CHECK_CLOSE(At.values()[7],  8.0, 0.01 );
  BOOST_CHECK_CLOSE(At.values()[8],  11.0, 0.01 );
  BOOST_CHECK_CLOSE(At.values()[9],  14.0, 0.01 );
  BOOST_CHECK_CLOSE(At.values()[10], 3.0, 0.01 );
  BOOST_CHECK_CLOSE(At.values()[11], 6.0, 0.01 );
  BOOST_CHECK_CLOSE(At.values()[12], 9.0, 0.01 );
  BOOST_CHECK_CLOSE(At.values()[13], 12.0, 0.01 );
  BOOST_CHECK_CLOSE(At.values()[14], 15.0, 0.01 );
}

BOOST_AUTO_TEST_CASE(zero_matrix)
{
  util::COOMatrix<float> A = util::make_zero<float>(5u);

  std::cout << util::to_string(A) << std::endl;

  BOOST_CHECK_EQUAL(A.is_zero(), true);
}

BOOST_AUTO_TEST_CASE(identity_matrix)
{
  util::COOMatrix<float> A = util::make_identity<float>(5u);

  std::cout << util::to_string(A) << std::endl;

  BOOST_CHECK_EQUAL(A.row_indices()[0],  0u );
  BOOST_CHECK_EQUAL(A.row_indices()[1],  1u );
  BOOST_CHECK_EQUAL(A.row_indices()[2],  2u );
  BOOST_CHECK_EQUAL(A.row_indices()[3],  3u );
  BOOST_CHECK_EQUAL(A.row_indices()[4],  4u );

  BOOST_CHECK_EQUAL(A.column_indices()[0],  0u );
  BOOST_CHECK_EQUAL(A.column_indices()[1],  1u );
  BOOST_CHECK_EQUAL(A.column_indices()[2],  2u );
  BOOST_CHECK_EQUAL(A.column_indices()[3],  3u );
  BOOST_CHECK_EQUAL(A.column_indices()[4],  4u );

  BOOST_CHECK_CLOSE(A.values()[0],  1.0, 0.01 );
  BOOST_CHECK_CLOSE(A.values()[1],  1.0, 0.01 );
  BOOST_CHECK_CLOSE(A.values()[2],  1.0, 0.01 );
  BOOST_CHECK_CLOSE(A.values()[3],  1.0, 0.01 );
  BOOST_CHECK_CLOSE(A.values()[4],  1.0, 0.01 );
}


BOOST_AUTO_TEST_CASE(make_from_blocks)
{
  util::COOMatrix<float> A = util::make_identity<float>(2u);
  util::COOMatrix<float> C = util::make_identity<float>(3u);


  util::COOMatrix<float> B;

  B.init(2u, 3u, 2u);

  B.insert(0u, 0u, 2.0f);
  B.insert(0u, 1u, 3.0f);
  B.insert(1u, 1u, 4.0f);
  B.insert(1u, 2u, 5.0f);

  util::COOMatrix<float> K = util::make_from_blocks(A, B, C);

  std::cout << util::to_string(K) << std::endl;

  //   1  0   2  3  0
  //   0  1   0  4  5
  //   2  0   1  0  0
  //   3  4   0  1  0
  //   0  5   0  0  1

  BOOST_CHECK_EQUAL(K.row_indices()[0],  0u );
  BOOST_CHECK_EQUAL(K.row_indices()[1],  0u );
  BOOST_CHECK_EQUAL(K.row_indices()[2],  0u );
  BOOST_CHECK_EQUAL(K.row_indices()[3],  1u );
  BOOST_CHECK_EQUAL(K.row_indices()[4],  1u );
  BOOST_CHECK_EQUAL(K.row_indices()[5],  1u );
  BOOST_CHECK_EQUAL(K.row_indices()[6],  2u );
  BOOST_CHECK_EQUAL(K.row_indices()[7],  2u );
  BOOST_CHECK_EQUAL(K.row_indices()[8],  3u );
  BOOST_CHECK_EQUAL(K.row_indices()[9],  3u );
  BOOST_CHECK_EQUAL(K.row_indices()[10],  3u );
  BOOST_CHECK_EQUAL(K.row_indices()[11],  4u );
  BOOST_CHECK_EQUAL(K.row_indices()[12],  4u );

  BOOST_CHECK_EQUAL(K.column_indices()[0],  0u );
  BOOST_CHECK_EQUAL(K.column_indices()[1],  2u );
  BOOST_CHECK_EQUAL(K.column_indices()[2],  3u );
  BOOST_CHECK_EQUAL(K.column_indices()[3],  1u );
  BOOST_CHECK_EQUAL(K.column_indices()[4],  3u );
  BOOST_CHECK_EQUAL(K.column_indices()[5],  4u );
  BOOST_CHECK_EQUAL(K.column_indices()[6],  0u );
  BOOST_CHECK_EQUAL(K.column_indices()[7],  2u );
  BOOST_CHECK_EQUAL(K.column_indices()[8],  0u );
  BOOST_CHECK_EQUAL(K.column_indices()[9],  1u );
  BOOST_CHECK_EQUAL(K.column_indices()[10],  3u );
  BOOST_CHECK_EQUAL(K.column_indices()[11],  1u );
  BOOST_CHECK_EQUAL(K.column_indices()[12],  4u );

  BOOST_CHECK_CLOSE(K.values()[0],  1.0, 0.01 );
  BOOST_CHECK_CLOSE(K.values()[1],  2.0, 0.01 );
  BOOST_CHECK_CLOSE(K.values()[2],  3.0, 0.01 );
  BOOST_CHECK_CLOSE(K.values()[3],  1.0, 0.01 );
  BOOST_CHECK_CLOSE(K.values()[4],  4.0, 0.01 );
  BOOST_CHECK_CLOSE(K.values()[5],  5.0, 0.01 );
  BOOST_CHECK_CLOSE(K.values()[6],  2.0, 0.01 );
  BOOST_CHECK_CLOSE(K.values()[7],  1.0, 0.01);
  BOOST_CHECK_CLOSE(K.values()[8],  3.0, 0.01);
  BOOST_CHECK_CLOSE(K.values()[9],  4.0, 0.01);
  BOOST_CHECK_CLOSE(K.values()[10],  1.0, 0.01);
  BOOST_CHECK_CLOSE(K.values()[11],  5.0, 0.01);
  BOOST_CHECK_CLOSE(K.values()[12],  1.0, 0.01);


  std::vector<double> a;
  std::vector<double> b;

  a.resize(2,1.0);
  b.resize(2,2.0);
  std::vector<double> c = util::make_from_blocks(a, b);

  BOOST_CHECK_EQUAL(c.size(), 4u );

  BOOST_CHECK_CLOSE(c[0],  1.0, 0.01 );
  BOOST_CHECK_CLOSE(c[1],  1.0, 0.01 );
  BOOST_CHECK_CLOSE(c[2],  2.0, 0.01 );
  BOOST_CHECK_CLOSE(c[3],  2.0, 0.01 );
}

BOOST_AUTO_TEST_CASE(split_into_blocks)
{
  std::vector<double> c;
  c.resize(6);

  c[0] = 1.0;
  c[1] = 2.0;
  c[2] = 3.0;
  c[3] = 4.0;
  c[4] = 5.0;
  c[5] = 6.0;

  std::vector<double> a;
  std::vector<double> b;

  util::split_into_blocks(c, a, b, false);

  BOOST_CHECK_EQUAL(a.size(), 3u );
  BOOST_CHECK_CLOSE(a[0],  1.0, 0.01 );
  BOOST_CHECK_CLOSE(a[1],  2.0, 0.01 );
  BOOST_CHECK_CLOSE(a[2],  3.0, 0.01 );

  BOOST_CHECK_EQUAL(b.size(), 3u );
  BOOST_CHECK_CLOSE(b[0],  4.0, 0.01 );
  BOOST_CHECK_CLOSE(b[1],  5.0, 0.01 );
  BOOST_CHECK_CLOSE(b[2],  6.0, 0.01 );

  util::split_into_blocks(c, a, b, true);

  BOOST_CHECK_EQUAL(a.size(), 3u );
  BOOST_CHECK_CLOSE(a[0],  1.0, 0.01 );
  BOOST_CHECK_CLOSE(a[1],  3.0, 0.01 );
  BOOST_CHECK_CLOSE(a[2],  5.0, 0.01 );

  BOOST_CHECK_EQUAL(b.size(), 3u );
  BOOST_CHECK_CLOSE(b[0],  2.0, 0.01 );
  BOOST_CHECK_CLOSE(b[1],  4.0, 0.01 );
  BOOST_CHECK_CLOSE(b[2],  6.0, 0.01 );

  util::split_into_blocks(c, 2, a, b);

  BOOST_CHECK_EQUAL(a.size(), 2u );
  BOOST_CHECK_CLOSE(a[0],  1.0, 0.01 );
  BOOST_CHECK_CLOSE(a[1],  2.0, 0.01 );

  BOOST_CHECK_EQUAL(b.size(), 4u );
  BOOST_CHECK_CLOSE(b[0],  3.0, 0.01 );
  BOOST_CHECK_CLOSE(b[1],  4.0, 0.01 );
  BOOST_CHECK_CLOSE(b[2],  5.0, 0.01 );
  BOOST_CHECK_CLOSE(b[3],  6.0, 0.01 );
}


BOOST_AUTO_TEST_CASE(add_sub_add_to)
{
  std::vector<double> a;
  a.resize(6);

  a[0] = 1.0;
  a[1] = 2.0;
  a[2] = 3.0;
  a[3] = 4.0;
  a[4] = 5.0;
  a[5] = 6.0;

  std::vector<double> b;
  b.resize(6);

  b[5] = 1.0;
  b[4] = 2.0;
  b[3] = 3.0;
  b[2] = 4.0;
  b[1] = 5.0;
  b[0] = 6.0;

  std::vector<double> c = util::add(a, b);
  std::vector<double> d = util::sub(a, b);

  BOOST_CHECK_EQUAL(c.size(), 6u );
  BOOST_CHECK_CLOSE(c[0],  7.0, 0.01 );
  BOOST_CHECK_CLOSE(c[1],  7.0, 0.01 );
  BOOST_CHECK_CLOSE(c[2],  7.0, 0.01 );
  BOOST_CHECK_CLOSE(c[3],  7.0, 0.01 );
  BOOST_CHECK_CLOSE(c[4],  7.0, 0.01 );
  BOOST_CHECK_CLOSE(c[5],  7.0, 0.01 );

  BOOST_CHECK_EQUAL(d.size(), 6u );
  BOOST_CHECK_CLOSE(d[0],  -5.0, 0.01 );
  BOOST_CHECK_CLOSE(d[1],  -3.0, 0.01 );
  BOOST_CHECK_CLOSE(d[2],  -1.0, 0.01 );
  BOOST_CHECK_CLOSE(d[3],   1.0, 0.01 );
  BOOST_CHECK_CLOSE(d[4],   3.0, 0.01 );
  BOOST_CHECK_CLOSE(d[5],   5.0, 0.01 );

  util::add_to(d, b);

  BOOST_CHECK_EQUAL(d.size(), 6u );
  BOOST_CHECK_CLOSE(d[0],  1.0, 0.01 );
  BOOST_CHECK_CLOSE(d[1],  2.0, 0.01 );
  BOOST_CHECK_CLOSE(d[2],  3.0, 0.01 );
  BOOST_CHECK_CLOSE(d[3],  4.0, 0.01 );
  BOOST_CHECK_CLOSE(d[4],  5.0, 0.01 );
  BOOST_CHECK_CLOSE(d[5],  6.0, 0.01 );


  std::vector<double> e = util::neg(a);
  BOOST_CHECK_EQUAL(e.size(), 6u );
  BOOST_CHECK_CLOSE(e[0],  -1.0, 0.01 );
  BOOST_CHECK_CLOSE(e[1],  -2.0, 0.01 );
  BOOST_CHECK_CLOSE(e[2],  -3.0, 0.01 );
  BOOST_CHECK_CLOSE(e[3],  -4.0, 0.01 );
  BOOST_CHECK_CLOSE(e[4],  -5.0, 0.01 );
  BOOST_CHECK_CLOSE(e[5],  -6.0, 0.01 );

}



BOOST_AUTO_TEST_CASE(dot_test)
{
  std::vector<double> a;
  a.resize(6);


  std::vector<double> b;
  b.resize(6);

  a[0] = 1.0;
  a[1] = 2.0;
  a[2] = 3.0;
  a[3] = 4.0;
  a[4] = 5.0;
  a[5] = 6.0;

  b[0] = 6.0;
  b[1] = 5.0;
  b[2] = 4.0;
  b[3] = 3.0;
  b[4] = 2.0;
  b[5] = 1.0;

  double test = util::dot(a,b);

  BOOST_CHECK_CLOSE(test,  56.0, 0.01 );
}


BOOST_AUTO_TEST_CASE(mul_mul_add)
{
  util::COOMatrix<double> A;

  A.init(5u, 5u, 3u);


//   1  2  3  0  0
//   0  4  5  6  0
//   0  0  7  8  9
//  10 11 12  0  0
//   0 13 14 15  0


  A.insert(0u, 0u, 1.0f);
  A.insert(0u, 1u, 2.0f);
  A.insert(0u, 2u, 3.0f);

  A.insert(1u, 1u, 4.0f);
  A.insert(1u, 2u, 5.0f);
  A.insert(1u, 3u, 6.0f);

  A.insert(2u, 2u, 7.0f);
  A.insert(2u, 3u, 8.0f);
  A.insert(2u, 4u, 9.0f);

  A.insert(3u, 0u, 10.0f);
  A.insert(3u, 1u, 11.0f);
  A.insert(3u, 2u, 12.0f);

  A.insert(4u, 1u, 13.0f);
  A.insert(4u, 2u, 14.0f);
  A.insert(4u, 3u, 15.0f);

  util::COOMatrix<double> B = util::mul(A,2.0);

  BOOST_CHECK_EQUAL(B.row_indices()[0],  0u );
  BOOST_CHECK_EQUAL(B.row_indices()[1],  0u );
  BOOST_CHECK_EQUAL(B.row_indices()[2],  0u );
  BOOST_CHECK_EQUAL(B.row_indices()[3],  1u );
  BOOST_CHECK_EQUAL(B.row_indices()[4],  1u );
  BOOST_CHECK_EQUAL(B.row_indices()[5],  1u );
  BOOST_CHECK_EQUAL(B.row_indices()[6],  2u );
  BOOST_CHECK_EQUAL(B.row_indices()[7],  2u );
  BOOST_CHECK_EQUAL(B.row_indices()[8],  2u );
  BOOST_CHECK_EQUAL(B.row_indices()[9],  3u );
  BOOST_CHECK_EQUAL(B.row_indices()[10], 3u );
  BOOST_CHECK_EQUAL(B.row_indices()[11], 3u );
  BOOST_CHECK_EQUAL(B.row_indices()[12], 4u );
  BOOST_CHECK_EQUAL(B.row_indices()[13], 4u );
  BOOST_CHECK_EQUAL(B.row_indices()[14], 4u );

  BOOST_CHECK_EQUAL(B.column_indices()[0],  0u );
  BOOST_CHECK_EQUAL(B.column_indices()[1],  1u );
  BOOST_CHECK_EQUAL(B.column_indices()[2],  2u );
  BOOST_CHECK_EQUAL(B.column_indices()[3],  1u );
  BOOST_CHECK_EQUAL(B.column_indices()[4],  2u );
  BOOST_CHECK_EQUAL(B.column_indices()[5],  3u );
  BOOST_CHECK_EQUAL(B.column_indices()[6],  2u );
  BOOST_CHECK_EQUAL(B.column_indices()[7],  3u );
  BOOST_CHECK_EQUAL(B.column_indices()[8],  4u );
  BOOST_CHECK_EQUAL(B.column_indices()[9],  0u );
  BOOST_CHECK_EQUAL(B.column_indices()[10], 1u );
  BOOST_CHECK_EQUAL(B.column_indices()[11], 2u );
  BOOST_CHECK_EQUAL(B.column_indices()[12], 1u );
  BOOST_CHECK_EQUAL(B.column_indices()[13], 2u );
  BOOST_CHECK_EQUAL(B.column_indices()[14], 3u );

  BOOST_CHECK_CLOSE(B.values()[0],  2.0*1.0, 0.01 );
  BOOST_CHECK_CLOSE(B.values()[1],  2.0*2.0, 0.01 );
  BOOST_CHECK_CLOSE(B.values()[2],  2.0*3.0, 0.01 );
  BOOST_CHECK_CLOSE(B.values()[3],  2.0*4.0, 0.01 );
  BOOST_CHECK_CLOSE(B.values()[4],  2.0*5.0, 0.01 );
  BOOST_CHECK_CLOSE(B.values()[5],  2.0*6.0, 0.01 );
  BOOST_CHECK_CLOSE(B.values()[6],  2.0*7.0, 0.01 );
  BOOST_CHECK_CLOSE(B.values()[7],  2.0*8.0, 0.01 );
  BOOST_CHECK_CLOSE(B.values()[8],  2.0*9.0, 0.01 );
  BOOST_CHECK_CLOSE(B.values()[9],  2.0*10.0, 0.01 );
  BOOST_CHECK_CLOSE(B.values()[10], 2.0*11.0, 0.01 );
  BOOST_CHECK_CLOSE(B.values()[11], 2.0*12.0, 0.01 );
  BOOST_CHECK_CLOSE(B.values()[12], 2.0*13.0, 0.01 );
  BOOST_CHECK_CLOSE(B.values()[13], 2.0*14.0, 0.01 );
  BOOST_CHECK_CLOSE(B.values()[14], 2.0*15.0, 0.01 );

  util::COOMatrix<double> C = util::add(A,B);

  BOOST_CHECK_EQUAL(C.row_indices()[0],  0u );
  BOOST_CHECK_EQUAL(C.row_indices()[1],  0u );
  BOOST_CHECK_EQUAL(C.row_indices()[2],  0u );
  BOOST_CHECK_EQUAL(C.row_indices()[3],  1u );
  BOOST_CHECK_EQUAL(C.row_indices()[4],  1u );
  BOOST_CHECK_EQUAL(C.row_indices()[5],  1u );
  BOOST_CHECK_EQUAL(C.row_indices()[6],  2u );
  BOOST_CHECK_EQUAL(C.row_indices()[7],  2u );
  BOOST_CHECK_EQUAL(C.row_indices()[8],  2u );
  BOOST_CHECK_EQUAL(C.row_indices()[9],  3u );
  BOOST_CHECK_EQUAL(C.row_indices()[10], 3u );
  BOOST_CHECK_EQUAL(C.row_indices()[11], 3u );
  BOOST_CHECK_EQUAL(C.row_indices()[12], 4u );
  BOOST_CHECK_EQUAL(C.row_indices()[13], 4u );
  BOOST_CHECK_EQUAL(C.row_indices()[14], 4u );

  BOOST_CHECK_EQUAL(C.column_indices()[0],  0u );
  BOOST_CHECK_EQUAL(C.column_indices()[1],  1u );
  BOOST_CHECK_EQUAL(C.column_indices()[2],  2u );
  BOOST_CHECK_EQUAL(C.column_indices()[3],  1u );
  BOOST_CHECK_EQUAL(C.column_indices()[4],  2u );
  BOOST_CHECK_EQUAL(C.column_indices()[5],  3u );
  BOOST_CHECK_EQUAL(C.column_indices()[6],  2u );
  BOOST_CHECK_EQUAL(C.column_indices()[7],  3u );
  BOOST_CHECK_EQUAL(C.column_indices()[8],  4u );
  BOOST_CHECK_EQUAL(C.column_indices()[9],  0u );
  BOOST_CHECK_EQUAL(C.column_indices()[10], 1u );
  BOOST_CHECK_EQUAL(C.column_indices()[11], 2u );
  BOOST_CHECK_EQUAL(C.column_indices()[12], 1u );
  BOOST_CHECK_EQUAL(C.column_indices()[13], 2u );
  BOOST_CHECK_EQUAL(C.column_indices()[14], 3u );

  BOOST_CHECK_CLOSE(C.values()[0],  3.0*1.0, 0.01 );
  BOOST_CHECK_CLOSE(C.values()[1],  3.0*2.0, 0.01 );
  BOOST_CHECK_CLOSE(C.values()[2],  3.0*3.0, 0.01 );
  BOOST_CHECK_CLOSE(C.values()[3],  3.0*4.0, 0.01 );
  BOOST_CHECK_CLOSE(C.values()[4],  3.0*5.0, 0.01 );
  BOOST_CHECK_CLOSE(C.values()[5],  3.0*6.0, 0.01 );
  BOOST_CHECK_CLOSE(C.values()[6],  3.0*7.0, 0.01 );
  BOOST_CHECK_CLOSE(C.values()[7],  3.0*8.0, 0.01 );
  BOOST_CHECK_CLOSE(C.values()[8],  3.0*9.0, 0.01 );
  BOOST_CHECK_CLOSE(C.values()[9],  3.0*10.0, 0.01 );
  BOOST_CHECK_CLOSE(C.values()[10], 3.0*11.0, 0.01 );
  BOOST_CHECK_CLOSE(C.values()[11], 3.0*12.0, 0.01 );
  BOOST_CHECK_CLOSE(C.values()[12], 3.0*13.0, 0.01 );
  BOOST_CHECK_CLOSE(C.values()[13], 3.0*14.0, 0.01 );
  BOOST_CHECK_CLOSE(C.values()[14], 3.0*15.0, 0.01 );


  std::vector<double> x;
  x.resize(5u,0.0);

  x[1] = 1.0;

  std::vector<double> y = util::mul(A,x);

  BOOST_CHECK_CLOSE(y[0],   2.0, 0.01 );
  BOOST_CHECK_CLOSE(y[1],   4.0, 0.01 );
  BOOST_CHECK_CLOSE(y[2],   0.0, 0.01 );
  BOOST_CHECK_CLOSE(y[3],  11.0, 0.01 );
  BOOST_CHECK_CLOSE(y[4],  13.0, 0.01 );
}

BOOST_AUTO_TEST_SUITE_END();
