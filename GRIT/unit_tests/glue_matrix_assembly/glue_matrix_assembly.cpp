#include <util_tensors.h>
#include <util_coo_matrix.h>
#include <glue_triplet.h>
#include <glue_matrix_assembly.h>

#include <vector>

#define BOOST_AUTO_TEST_MAIN
#include <boost/test/auto_unit_test.hpp>
#include <boost/test/unit_test_suite.hpp>
#include <boost/test/floating_point_comparison.hpp>
#include <boost/test/test_tools.hpp>


/**
 * This assembles a test problem on a small example mesh. The mesh consist
 * of four vertices and three triangles. The element matrices is chosen such
 * that it is quite simple to verify if the assembled matrix is correct or not.
 */
inline util::COOMatrix<double> do_assembly_of_tensor2_test_problem( bool const & interlaced )
{
  std::vector<glue::Triplet> triangles;

  triangles.push_back( glue::make_triplet(0u, 1u, 2u) );
  triangles.push_back( glue::make_triplet(0u, 2u, 3u) );
  triangles.push_back( glue::make_triplet(0u, 3u, 1u) );

  unsigned int const V = 4u;  // We got 4 vertices

  std::vector<util::Block3x3Tensor2> Ae;

  Ae.resize(3u); // We got 3 triangles

  util::Block3x3Tensor2 E;

  // Element matrix is the same for all elements
  //
  //         1 0 2 0 4 0
  //         0 1 0 2 0 4
  //         2 0 1 0 3 0
  //   E =   0 2 0 1 0 3
  //         4 0 3 0 1 0
  //         0 4 0 3 0 1
  //
  E.m_block[0][0] = util::identity();
  E.m_block[1][1] = util::identity();
  E.m_block[2][2] = util::identity();
  E.m_block[0][1] = util::mul( 2.0, util::identity() );
  E.m_block[0][2] = util::mul( 4.0, util::identity() );
  E.m_block[1][2] = util::mul( 3.0, util::identity() );
  E.m_block[1][0] = E.m_block[0][1];
  E.m_block[2][0] = E.m_block[0][2];
  E.m_block[2][1] = E.m_block[1][2];

  Ae[0] = E;
  Ae[1] = E;
  Ae[2] = E;

  // Do the assembley, the "interlaced" results should be
  //
  //  3     0     6     0     6     0     6     0
  //  0     3     0     6     0     6     0     6
  //  6     0     2     0     3     0     3     0
  //  0     6     0     2     0     3     0     3
  //  6     0     3     0     2     0     3     0
  //  0     6     0     3     0     2     0     3
  //  6     0     3     0     3     0     2     0
  //  0     6     0     3     0     3     0     2
  //
  // and non-interlaced should be
  //
  //  3     6     6     6     0     0     0     0
  //  6     2     3     3     0     0     0     0
  //  6     3     2     3     0     0     0     0
  //  6     3     3     2     0     0     0     0
  //  0     0     0     0     3     6     6     6
  //  0     0     0     0     6     2     3     3
  //  0     0     0     0     6     3     2     3
  //  0     0     0     0     6     3     3     2

  util::COOMatrix<double> A = glue::matrix_assembly<double>( triangles, V, Ae, interlaced );

  std::cout << util::to_string(A) << std::endl;

  return A;
}

/**
 * Specific for tensor1 data using cell-centered variables
 */
inline util::COOMatrix<double> do_assembly_of_tensor1_test_problem( bool const & interlaced )
{
  std::vector<glue::Triplet> triangles;

  triangles.push_back( glue::make_triplet(0u, 1u, 2u) );
  triangles.push_back( glue::make_triplet(0u, 2u, 3u) );
  triangles.push_back( glue::make_triplet(0u, 3u, 1u) );

  unsigned int const V = 4u;  // We got 4 vertices

  std::vector<util::Block3x1Tensor1> Ae;

  Ae.resize(3u); // We got 3 triangles

  util::Block3x1Tensor1 E;

  E.m_block[0] = util::make(1.0, 1.0);
  E.m_block[1] = util::make(1.0, 1.0);
  E.m_block[2] = util::make(1.0, 1.0);

  Ae[0] = E;
  Ae[1] = E;
  Ae[2] = E;

  // Do the assembley, the "interlaced" results should be
  //
  //  1     1     1
  //  1     1     1
  //  1     0     1
  //  1     0     1
  //  1     1     0
  //  1     1     0
  //  0     1     1
  //  0     1     1
  //
  // and non-interlaced should be
  //
  //  1     1     1
  //  1     0     1
  //  1     1     0
  //  0     1     1
  //  1     1     1
  //  1     0     1
  //  1     1     0
  //  0     1     1

  util::COOMatrix<double> A = glue::matrix_assembly<double>( triangles, V, Ae, interlaced );

  std::cout << util::to_string(A) << std::endl;
  
  return A;
}

/**
 * Specific for tensor0 data using vertex variables
 */
inline util::COOMatrix<double> do_assembly_of_tensor0_test_problem( )
{
  std::vector<glue::Triplet> triangles;

  triangles.push_back( glue::make_triplet(0u, 1u, 2u) );
  triangles.push_back( glue::make_triplet(0u, 2u, 3u) );
  triangles.push_back( glue::make_triplet(0u, 3u, 1u) );

  unsigned int const V = 4u;  // We got 4 vertices

  std::vector<util::Block3x3Tensor0> Ae;

  Ae.resize(3u); // We got 3 triangles

  util::Block3x3Tensor0 E;

  // Element matrix is the same for all elements
  //
  //         1 2 4
  //         2 1 3
  //   E =   4 3 1
  //
  E.m_block[0][0] = 1.0;
  E.m_block[1][1] = 1.0;
  E.m_block[2][2] = 1.0;
  E.m_block[0][1] = 2.0;
  E.m_block[0][2] = 4.0;
  E.m_block[1][2] = 3.0;
  E.m_block[1][0] = E.m_block[0][1];
  E.m_block[2][0] = E.m_block[0][2];
  E.m_block[2][1] = E.m_block[1][2];

  Ae[0] = E;
  Ae[1] = E;
  Ae[2] = E;

  // Do the assembley, the results should be
  //
  //
  //  3     6     6     6
  //  6     2     3     3
  //  6     3     2     3
  //  6     3     3     2

  util::COOMatrix<double> A = glue::matrix_assembly<double>( triangles, V, Ae );

  std::cout << util::to_string(A) << std::endl;
  
  return A;
}

BOOST_AUTO_TEST_SUITE(coo_matrix);

BOOST_AUTO_TEST_CASE(assemble_tensor2_matrix_interlaced)
{
  util::COOMatrix<double> const A = do_assembly_of_tensor2_test_problem( true );

  BOOST_CHECK_EQUAL(A.row_indices()[0], 0u);
  BOOST_CHECK_EQUAL(A.row_indices()[1], 0u);
  BOOST_CHECK_EQUAL(A.row_indices()[2], 0u);
  BOOST_CHECK_EQUAL(A.row_indices()[3], 0u);
  BOOST_CHECK_EQUAL(A.row_indices()[4], 1u);
  BOOST_CHECK_EQUAL(A.row_indices()[5], 1u);
  BOOST_CHECK_EQUAL(A.row_indices()[6], 1u);
  BOOST_CHECK_EQUAL(A.row_indices()[7], 1u);
  BOOST_CHECK_EQUAL(A.row_indices()[8], 2u);
  BOOST_CHECK_EQUAL(A.row_indices()[9], 2u);
  BOOST_CHECK_EQUAL(A.row_indices()[10], 2u);
  BOOST_CHECK_EQUAL(A.row_indices()[11], 2u);
  BOOST_CHECK_EQUAL(A.row_indices()[12], 3u);
  BOOST_CHECK_EQUAL(A.row_indices()[13], 3u);
  BOOST_CHECK_EQUAL(A.row_indices()[14], 3u);
  BOOST_CHECK_EQUAL(A.row_indices()[15], 3u);
  BOOST_CHECK_EQUAL(A.row_indices()[16], 4u);
  BOOST_CHECK_EQUAL(A.row_indices()[17], 4u);
  BOOST_CHECK_EQUAL(A.row_indices()[18], 4u);
  BOOST_CHECK_EQUAL(A.row_indices()[19], 4u);
  BOOST_CHECK_EQUAL(A.row_indices()[20], 5u);
  BOOST_CHECK_EQUAL(A.row_indices()[21], 5u);
  BOOST_CHECK_EQUAL(A.row_indices()[22], 5u);
  BOOST_CHECK_EQUAL(A.row_indices()[23], 5u);
  BOOST_CHECK_EQUAL(A.row_indices()[24], 6u);
  BOOST_CHECK_EQUAL(A.row_indices()[25], 6u);
  BOOST_CHECK_EQUAL(A.row_indices()[26], 6u);
  BOOST_CHECK_EQUAL(A.row_indices()[27], 6u);
  BOOST_CHECK_EQUAL(A.row_indices()[28], 7u);
  BOOST_CHECK_EQUAL(A.row_indices()[29], 7u);
  BOOST_CHECK_EQUAL(A.row_indices()[30], 7u);
  BOOST_CHECK_EQUAL(A.row_indices()[31], 7u);

  BOOST_CHECK_EQUAL(A.column_indices()[0], 0u);
  BOOST_CHECK_EQUAL(A.column_indices()[1], 2u);
  BOOST_CHECK_EQUAL(A.column_indices()[2], 4u);
  BOOST_CHECK_EQUAL(A.column_indices()[3], 6u);
  BOOST_CHECK_EQUAL(A.column_indices()[4], 1u);
  BOOST_CHECK_EQUAL(A.column_indices()[5], 3u);
  BOOST_CHECK_EQUAL(A.column_indices()[6], 5u);
  BOOST_CHECK_EQUAL(A.column_indices()[7], 7u);
  BOOST_CHECK_EQUAL(A.column_indices()[8], 0u);
  BOOST_CHECK_EQUAL(A.column_indices()[9], 2u);
  BOOST_CHECK_EQUAL(A.column_indices()[10], 4u);
  BOOST_CHECK_EQUAL(A.column_indices()[11], 6u);
  BOOST_CHECK_EQUAL(A.column_indices()[12], 1u);
  BOOST_CHECK_EQUAL(A.column_indices()[13], 3u);
  BOOST_CHECK_EQUAL(A.column_indices()[14], 5u);
  BOOST_CHECK_EQUAL(A.column_indices()[15], 7u);
  BOOST_CHECK_EQUAL(A.column_indices()[16], 0u);
  BOOST_CHECK_EQUAL(A.column_indices()[17], 2u);
  BOOST_CHECK_EQUAL(A.column_indices()[18], 4u);
  BOOST_CHECK_EQUAL(A.column_indices()[19], 6u);
  BOOST_CHECK_EQUAL(A.column_indices()[20], 1u);
  BOOST_CHECK_EQUAL(A.column_indices()[21], 3u);
  BOOST_CHECK_EQUAL(A.column_indices()[22], 5u);
  BOOST_CHECK_EQUAL(A.column_indices()[23], 7u);
  BOOST_CHECK_EQUAL(A.column_indices()[24], 0u);
  BOOST_CHECK_EQUAL(A.column_indices()[25], 2u);
  BOOST_CHECK_EQUAL(A.column_indices()[26], 4u);
  BOOST_CHECK_EQUAL(A.column_indices()[27], 6u);
  BOOST_CHECK_EQUAL(A.column_indices()[28], 1u);
  BOOST_CHECK_EQUAL(A.column_indices()[29], 3u);
  BOOST_CHECK_EQUAL(A.column_indices()[30], 5u);
  BOOST_CHECK_EQUAL(A.column_indices()[31], 7u);

  BOOST_CHECK_CLOSE(A.values()[0], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[1], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[2], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[3], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[4], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[5], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[6], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[7], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[8], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[9], 2.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[10], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[11], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[12], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[13], 2.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[14], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[15], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[16], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[17], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[18], 2.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[19], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[20], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[21], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[22], 2.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[23], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[24], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[25], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[26], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[27], 2.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[28], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[29], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[30], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[31], 2.0, 0.01);

}

BOOST_AUTO_TEST_CASE(assemble_tensor2_matrix_non_interlaced)
{
  util::COOMatrix<double> const A = do_assembly_of_tensor2_test_problem( false );

  BOOST_CHECK_EQUAL(A.row_indices()[0], 0u);
  BOOST_CHECK_EQUAL(A.row_indices()[1], 0u);
  BOOST_CHECK_EQUAL(A.row_indices()[2], 0u);
  BOOST_CHECK_EQUAL(A.row_indices()[3], 0u);
  BOOST_CHECK_EQUAL(A.row_indices()[4], 1u);
  BOOST_CHECK_EQUAL(A.row_indices()[5], 1u);
  BOOST_CHECK_EQUAL(A.row_indices()[6], 1u);
  BOOST_CHECK_EQUAL(A.row_indices()[7], 1u);
  BOOST_CHECK_EQUAL(A.row_indices()[8], 2u);
  BOOST_CHECK_EQUAL(A.row_indices()[9], 2u);
  BOOST_CHECK_EQUAL(A.row_indices()[10], 2u);
  BOOST_CHECK_EQUAL(A.row_indices()[11], 2u);
  BOOST_CHECK_EQUAL(A.row_indices()[12], 3u);
  BOOST_CHECK_EQUAL(A.row_indices()[13], 3u);
  BOOST_CHECK_EQUAL(A.row_indices()[14], 3u);
  BOOST_CHECK_EQUAL(A.row_indices()[15], 3u);
  BOOST_CHECK_EQUAL(A.row_indices()[16], 4u);
  BOOST_CHECK_EQUAL(A.row_indices()[17], 4u);
  BOOST_CHECK_EQUAL(A.row_indices()[18], 4u);
  BOOST_CHECK_EQUAL(A.row_indices()[19], 4u);
  BOOST_CHECK_EQUAL(A.row_indices()[20], 5u);
  BOOST_CHECK_EQUAL(A.row_indices()[21], 5u);
  BOOST_CHECK_EQUAL(A.row_indices()[22], 5u);
  BOOST_CHECK_EQUAL(A.row_indices()[23], 5u);
  BOOST_CHECK_EQUAL(A.row_indices()[24], 6u);
  BOOST_CHECK_EQUAL(A.row_indices()[25], 6u);
  BOOST_CHECK_EQUAL(A.row_indices()[26], 6u);
  BOOST_CHECK_EQUAL(A.row_indices()[27], 6u);
  BOOST_CHECK_EQUAL(A.row_indices()[28], 7u);
  BOOST_CHECK_EQUAL(A.row_indices()[29], 7u);
  BOOST_CHECK_EQUAL(A.row_indices()[30], 7u);
  BOOST_CHECK_EQUAL(A.row_indices()[31], 7u);

  BOOST_CHECK_EQUAL(A.column_indices()[0], 0u);
  BOOST_CHECK_EQUAL(A.column_indices()[1], 1u);
  BOOST_CHECK_EQUAL(A.column_indices()[2], 2u);
  BOOST_CHECK_EQUAL(A.column_indices()[3], 3u);
  BOOST_CHECK_EQUAL(A.column_indices()[4], 0u);
  BOOST_CHECK_EQUAL(A.column_indices()[5], 1u);
  BOOST_CHECK_EQUAL(A.column_indices()[6], 2u);
  BOOST_CHECK_EQUAL(A.column_indices()[7], 3u);
  BOOST_CHECK_EQUAL(A.column_indices()[8], 0u);
  BOOST_CHECK_EQUAL(A.column_indices()[9], 1u);
  BOOST_CHECK_EQUAL(A.column_indices()[10], 2u);
  BOOST_CHECK_EQUAL(A.column_indices()[11], 3u);
  BOOST_CHECK_EQUAL(A.column_indices()[12], 0u);
  BOOST_CHECK_EQUAL(A.column_indices()[13], 1u);
  BOOST_CHECK_EQUAL(A.column_indices()[14], 2u);
  BOOST_CHECK_EQUAL(A.column_indices()[15], 3u);
  BOOST_CHECK_EQUAL(A.column_indices()[16], 4u);
  BOOST_CHECK_EQUAL(A.column_indices()[17], 5u);
  BOOST_CHECK_EQUAL(A.column_indices()[18], 6u);
  BOOST_CHECK_EQUAL(A.column_indices()[19], 7u);
  BOOST_CHECK_EQUAL(A.column_indices()[20], 4u);
  BOOST_CHECK_EQUAL(A.column_indices()[21], 5u);
  BOOST_CHECK_EQUAL(A.column_indices()[22], 6u);
  BOOST_CHECK_EQUAL(A.column_indices()[23], 7u);
  BOOST_CHECK_EQUAL(A.column_indices()[24], 4u);
  BOOST_CHECK_EQUAL(A.column_indices()[25], 5u);
  BOOST_CHECK_EQUAL(A.column_indices()[26], 6u);
  BOOST_CHECK_EQUAL(A.column_indices()[27], 7u);
  BOOST_CHECK_EQUAL(A.column_indices()[28], 4u);
  BOOST_CHECK_EQUAL(A.column_indices()[29], 5u);
  BOOST_CHECK_EQUAL(A.column_indices()[30], 6u);
  BOOST_CHECK_EQUAL(A.column_indices()[31], 7u);

  BOOST_CHECK_CLOSE(A.values()[0], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[1], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[2], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[3], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[4], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[5], 2.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[6], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[7], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[8], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[9], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[10], 2.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[11], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[12], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[13], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[14], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[15], 2.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[16], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[17], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[18], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[19], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[20], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[21], 2.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[22], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[23], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[24], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[25], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[26], 2.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[27], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[28], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[29], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[30], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[31], 2.0, 0.01);

}

BOOST_AUTO_TEST_CASE(assemble_tensor0_matrix)
{
  util::COOMatrix<double> const A = do_assembly_of_tensor0_test_problem();

  BOOST_CHECK_EQUAL(A.row_indices()[0], 0u);
  BOOST_CHECK_EQUAL(A.row_indices()[1], 0u);
  BOOST_CHECK_EQUAL(A.row_indices()[2], 0u);
  BOOST_CHECK_EQUAL(A.row_indices()[3], 0u);
  BOOST_CHECK_EQUAL(A.row_indices()[4], 1u);
  BOOST_CHECK_EQUAL(A.row_indices()[5], 1u);
  BOOST_CHECK_EQUAL(A.row_indices()[6], 1u);
  BOOST_CHECK_EQUAL(A.row_indices()[7], 1u);
  BOOST_CHECK_EQUAL(A.row_indices()[8], 2u);
  BOOST_CHECK_EQUAL(A.row_indices()[9], 2u);
  BOOST_CHECK_EQUAL(A.row_indices()[10], 2u);
  BOOST_CHECK_EQUAL(A.row_indices()[11], 2u);
  BOOST_CHECK_EQUAL(A.row_indices()[12], 3u);
  BOOST_CHECK_EQUAL(A.row_indices()[13], 3u);
  BOOST_CHECK_EQUAL(A.row_indices()[14], 3u);
  BOOST_CHECK_EQUAL(A.row_indices()[15], 3u);

  BOOST_CHECK_EQUAL(A.column_indices()[0], 0u);
  BOOST_CHECK_EQUAL(A.column_indices()[1], 1u);
  BOOST_CHECK_EQUAL(A.column_indices()[2], 2u);
  BOOST_CHECK_EQUAL(A.column_indices()[3], 3u);
  BOOST_CHECK_EQUAL(A.column_indices()[4], 0u);
  BOOST_CHECK_EQUAL(A.column_indices()[5], 1u);
  BOOST_CHECK_EQUAL(A.column_indices()[6], 2u);
  BOOST_CHECK_EQUAL(A.column_indices()[7], 3u);
  BOOST_CHECK_EQUAL(A.column_indices()[8], 0u);
  BOOST_CHECK_EQUAL(A.column_indices()[9], 1u);
  BOOST_CHECK_EQUAL(A.column_indices()[10], 2u);
  BOOST_CHECK_EQUAL(A.column_indices()[11], 3u);
  BOOST_CHECK_EQUAL(A.column_indices()[12], 0u);
  BOOST_CHECK_EQUAL(A.column_indices()[13], 1u);
  BOOST_CHECK_EQUAL(A.column_indices()[14], 2u);
  BOOST_CHECK_EQUAL(A.column_indices()[15], 3u);

  BOOST_CHECK_CLOSE(A.values()[0], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[1], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[2], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[3], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[4], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[5], 2.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[6], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[7], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[8], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[9], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[10], 2.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[11], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[12], 6.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[13], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[14], 3.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[15], 2.0, 0.01);

}

BOOST_AUTO_TEST_CASE(assemble_tensor1_matrix_interlaced)
{
  util::COOMatrix<double> const A = do_assembly_of_tensor1_test_problem( true );

  BOOST_CHECK_EQUAL(A.row_indices()[0], 0u);
  BOOST_CHECK_EQUAL(A.row_indices()[1], 0u);
  BOOST_CHECK_EQUAL(A.row_indices()[2], 0u);
  BOOST_CHECK_EQUAL(A.row_indices()[3], 1u);
  BOOST_CHECK_EQUAL(A.row_indices()[4], 1u);
  BOOST_CHECK_EQUAL(A.row_indices()[5], 1u);
  BOOST_CHECK_EQUAL(A.row_indices()[6], 2u);
  BOOST_CHECK_EQUAL(A.row_indices()[7], 2u);
  BOOST_CHECK_EQUAL(A.row_indices()[8], 3u);
  BOOST_CHECK_EQUAL(A.row_indices()[9], 3u);
  BOOST_CHECK_EQUAL(A.row_indices()[10], 4u);
  BOOST_CHECK_EQUAL(A.row_indices()[11], 4u);
  BOOST_CHECK_EQUAL(A.row_indices()[12], 5u);
  BOOST_CHECK_EQUAL(A.row_indices()[13], 5u);
  BOOST_CHECK_EQUAL(A.row_indices()[14], 6u);
  BOOST_CHECK_EQUAL(A.row_indices()[15], 6u);
  BOOST_CHECK_EQUAL(A.row_indices()[16], 7u);
  BOOST_CHECK_EQUAL(A.row_indices()[17], 7u);


  BOOST_CHECK_EQUAL(A.column_indices()[0], 0u);
  BOOST_CHECK_EQUAL(A.column_indices()[1], 1u);
  BOOST_CHECK_EQUAL(A.column_indices()[2], 2u);
  BOOST_CHECK_EQUAL(A.column_indices()[3], 0u);
  BOOST_CHECK_EQUAL(A.column_indices()[4], 1u);
  BOOST_CHECK_EQUAL(A.column_indices()[5], 2u);
  BOOST_CHECK_EQUAL(A.column_indices()[6], 0u);
  BOOST_CHECK_EQUAL(A.column_indices()[7], 2u);
  BOOST_CHECK_EQUAL(A.column_indices()[8], 0u);
  BOOST_CHECK_EQUAL(A.column_indices()[9], 2u);
  BOOST_CHECK_EQUAL(A.column_indices()[10], 0u);
  BOOST_CHECK_EQUAL(A.column_indices()[11], 1u);
  BOOST_CHECK_EQUAL(A.column_indices()[12], 0u);
  BOOST_CHECK_EQUAL(A.column_indices()[13], 1u);
  BOOST_CHECK_EQUAL(A.column_indices()[14], 1u);
  BOOST_CHECK_EQUAL(A.column_indices()[15], 2u);
  BOOST_CHECK_EQUAL(A.column_indices()[16], 1u);
  BOOST_CHECK_EQUAL(A.column_indices()[17], 2u);

  BOOST_CHECK_CLOSE(A.values()[0], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[1], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[2], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[3], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[4], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[5], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[6], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[7], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[8], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[9], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[10], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[11], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[12], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[13], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[14], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[15], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[16], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[17], 1.0, 0.01);

}

BOOST_AUTO_TEST_CASE(assemble_tensor1_matrix_non_interlaced)
{
  util::COOMatrix<double> const A = do_assembly_of_tensor1_test_problem( false );

  BOOST_CHECK_EQUAL(A.row_indices()[0], 0u);
  BOOST_CHECK_EQUAL(A.row_indices()[1], 0u);
  BOOST_CHECK_EQUAL(A.row_indices()[2], 0u);
  BOOST_CHECK_EQUAL(A.row_indices()[3], 1u);
  BOOST_CHECK_EQUAL(A.row_indices()[4], 1u);
  BOOST_CHECK_EQUAL(A.row_indices()[5], 2u);
  BOOST_CHECK_EQUAL(A.row_indices()[6], 2u);
  BOOST_CHECK_EQUAL(A.row_indices()[7], 3u);
  BOOST_CHECK_EQUAL(A.row_indices()[8], 3u);
  BOOST_CHECK_EQUAL(A.row_indices()[9], 4u);
  BOOST_CHECK_EQUAL(A.row_indices()[10], 4u);
  BOOST_CHECK_EQUAL(A.row_indices()[11], 4u);
  BOOST_CHECK_EQUAL(A.row_indices()[12], 5u);
  BOOST_CHECK_EQUAL(A.row_indices()[13], 5u);
  BOOST_CHECK_EQUAL(A.row_indices()[14], 6u);
  BOOST_CHECK_EQUAL(A.row_indices()[15], 6u);
  BOOST_CHECK_EQUAL(A.row_indices()[16], 7u);
  BOOST_CHECK_EQUAL(A.row_indices()[17], 7u);

  BOOST_CHECK_EQUAL(A.column_indices()[0], 0u);
  BOOST_CHECK_EQUAL(A.column_indices()[1], 1u);
  BOOST_CHECK_EQUAL(A.column_indices()[2], 2u);
  BOOST_CHECK_EQUAL(A.column_indices()[3], 0u);
  BOOST_CHECK_EQUAL(A.column_indices()[4], 2u);
  BOOST_CHECK_EQUAL(A.column_indices()[5], 0u);
  BOOST_CHECK_EQUAL(A.column_indices()[6], 1u);
  BOOST_CHECK_EQUAL(A.column_indices()[7], 1u);
  BOOST_CHECK_EQUAL(A.column_indices()[8], 2u);
  BOOST_CHECK_EQUAL(A.column_indices()[9], 0u);
  BOOST_CHECK_EQUAL(A.column_indices()[10], 1u);
  BOOST_CHECK_EQUAL(A.column_indices()[11], 2u);
  BOOST_CHECK_EQUAL(A.column_indices()[12], 0u);
  BOOST_CHECK_EQUAL(A.column_indices()[13], 2u);
  BOOST_CHECK_EQUAL(A.column_indices()[14], 0u);
  BOOST_CHECK_EQUAL(A.column_indices()[15], 1u);
  BOOST_CHECK_EQUAL(A.column_indices()[16], 1u);
  BOOST_CHECK_EQUAL(A.column_indices()[17], 2u);

  BOOST_CHECK_CLOSE(A.values()[0], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[1], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[2], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[3], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[4], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[5], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[6], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[7], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[8], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[9], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[10], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[11], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[12], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[13], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[14], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[15], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[16], 1.0, 0.01);
  BOOST_CHECK_CLOSE(A.values()[17], 1.0, 0.01);
  
}


BOOST_AUTO_TEST_SUITE_END();
