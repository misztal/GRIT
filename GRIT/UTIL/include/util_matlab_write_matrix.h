#ifndef UTIL_MATLAB_WRITE_MATRIX_H
#define UTIL_MATLAB_WRITE_MATRIX_H

#include <util_matlab_write_vector.h>

#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace util
{

  template<typename T, typename I>
  inline std::string matlab_write_matrix(
                                         std::vector<I> const & rows
                                         , std::vector<I> const & columns
                                         , std::vector<T> const & values
                                         , std::size_t const & nzeros
                                         , std::size_t const & m
                                         , std::size_t const & n
                                         )
  {
    std::stringstream output;

    output << "sparse(";
    output << matlab_write_vector( rows, nzeros, true );
    output << ",";
    output << matlab_write_vector( columns, nzeros, true );
    output << ",";
    output << matlab_write_vector( values, nzeros );
    output << ",";
    output << m;
    output << ",";
    output << n;
    output << ",";
    output << nzeros;
    output << ")";

    output.flush();

    return output.str();
  }

  template<typename T, typename I>
  inline std::string matlab_write_matrix(
                                           std::string const & name
                                         , std::vector<I> const & rows
                                         , std::vector<I> const & columns
                                         , std::vector<T> const & values
                                         , std::size_t const & nzeros
                                         , std::size_t const & m
                                         , std::size_t const & n
                                         )
  {
    std::stringstream output;

    output << name << " = " << matlab_write_matrix(rows,columns, values, nzeros, m ,n) << ";";

    output.flush();

    return output.str();
  }
  
}

// UTIL_MATLAB_WRITE_MATRIX_H
#endif