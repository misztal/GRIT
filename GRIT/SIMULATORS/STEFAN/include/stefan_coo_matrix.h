#ifndef STEFAN_COO_MATRIX_H
#define STEFAN_COO_MATRIX_H

#include <util_coo_matrix.h>

#include <sstream>
#include <string>
#include <cassert>
#include <cmath>
#include <vector>

namespace stefan
{
  /**
   * This is a modified version of util::COOMatrix. In this version, duplicate
   * entries in insert() are allowed, and when encountered, addition is performed,
   * rather than insert. It is useful for assembling nodal Finite Element systems.
   */
  template<typename T>
  class COOMatrix
  : public util::COOMatrix<T>
  {
  protected:

    typedef util::COOMatrix<T>  base_class;

  public:

    COOMatrix()
    : base_class()
    {}

    virtual ~COOMatrix()
    {}

    COOMatrix(
                unsigned int const & rows
              , unsigned int const & cols
              )
    : base_class( rows, cols )
    {}

    COOMatrix(
              unsigned int const & nvars
              )
    : base_class( nvars )
    {}

  public:

    /**
     * Insert a new matrix entry
     *
     * @param  r                     The row index value
     * @param  c                     The column index value
     * @param  value                 The actual value to store in location (r,c).
     * @param  guard_against_zeros   If this argument is set to true (default value if omitted)
     *                               then the method guards against user trying to insert
     *                               zeros. In that case if a zero is inserted then the method
     *                               just bails out. If set to false then the method allows
     *                               the end-user to insert a zero.
     * @param verbose                If set to true then full details is written to the log. If
     *                               set to false (default value) then only error information is
     *                               written to log.
     */
    bool insert(
                  unsigned int const & r
                , unsigned int const & c
                , T const & value
                , bool const & guard_against_zeros = true
                , bool const & verbose = false
                )
    {
      std::string const newline = util::Log::newline();
      std::string const tab     = util::Log::tab();

      util::Log log;

      assert( r < this->m_rows || !"COOMatrix::insert(): Error invalid r-index");
      assert( c < this->m_cols || !"COOMatrix::insert(): Error invalid c-index");
      assert( is_finite(value) || !"COOMatrix::insert(): Error value was infinite");
      assert( is_number(value) || !"COOMatrix::insert(): Error value was not a number");

      bool const is_non_zero = (value<0.0) || (value>0.0);
      if(guard_against_zeros &&  ! is_non_zero  )
      {
        if(verbose)
          log <<  "COOMatrix::insert(): Warning value is zero" << newline;
        return false;
      }

      //--- Resize storage if there is no space --------------------------------
      if( this->m_values.size() <= this->m_nzeros )
      {
        if(verbose)
        {
          log <<  "COOMatrix::insert(): Warning Storage exhausted, we must allocate more space" << newline;
          log <<  "COOMatrix::insert(): Warning Try increasing row_fill_in when initializing COOMatrix!" << newline;
        }

        unsigned int const new_size = this->m_nzeros*2u;

        this->m_values.resize( new_size );
        this->m_row_indices.resize( new_size );
        this->m_column_indices.resize( new_size );
      }

      if(this->m_nzeros == 0u)
      {
        //--- Insert to empty COO matrix
        this->m_row_indices[this->m_nzeros]    = r;
        this->m_column_indices[this->m_nzeros] = c;
        this->m_values[this->m_nzeros]         = value;
        ++this->m_nzeros;

        return true;
      }
      else
      {
        //--- Insert to non-empty COO matrix
        unsigned int const last_r = this->m_row_indices[this->m_nzeros-1u];
        unsigned int const last_c = this->m_column_indices[this->m_nzeros-1u];

        if( last_r == r && last_c == c )
        {
          this->m_values[ this->m_nzeros-1u ] += value;
          return true;
        }

        bool const last_is_smaller =  (last_r < r) || (last_r==r && last_c < c);

        if( last_is_smaller )
        {
          //--- It is safe to just insert the new value to the "end"
          this->m_row_indices[this->m_nzeros]    = r;
          this->m_column_indices[this->m_nzeros] = c;
          this->m_values[this->m_nzeros]         = value;
          ++this->m_nzeros;

          return true;
        }
        else
        {
          //--- It is non-safe to just add the new value to the "end" ----------
          if(verbose)
          {
            log <<  "COOMatrix::insert(): Warning non lexigraphic order encountered "
                << "(" << r << "," << c << ")"
                <<  " should not be inserted after "
                << "(" << last_r << "," << last_c << ")"
                << newline;
          }

          unsigned int const first_r = this->m_row_indices[0];
          unsigned int const first_c = this->m_column_indices[0];

          if( first_r == r && first_c == c )
          {
            this->m_values[ 0u ] += value;
            return true;
          }

          bool const first_is_smaller =  (first_r < r) || (first_r==r && first_c < c);

          if (! first_is_smaller)
          {
            //--- We must "insert" before head of array ------------------------
            base_class::shift_right( 0u );

            this->m_row_indices[ 0u ]    = r;
            this->m_column_indices[ 0u ] = c;
            this->m_values[ 0u ]         = value;
            ++this->m_nzeros;

            return true;
          }
          else
          {
            //--- We must find the "proper" placement to insert at -----------
            unsigned int pos = 0u;

            bool pos_is_smaller = true;

            while (pos_is_smaller)
            {
              ++pos;

              unsigned int const pos_r = this->m_row_indices[pos];
              unsigned int const pos_c = this->m_column_indices[pos];

              if( pos_r == r && pos_c == c )
              {
                this->m_values[ pos ] += value;
                return true;
              }

              pos_is_smaller =  (pos_r < r) || (pos_r==r && pos_c < c);
            }

            //--- Now we know that we must shuffle all elements from pos to
            //--- end one place to the right and insert new element at pos-place.

            base_class::shift_right( pos );

            this->m_row_indices[ pos ]    = r;
            this->m_column_indices[ pos ] = c;
            this->m_values[ pos ]         = value;
            ++this->m_nzeros;

            return true;

          }

        }

      }

      return false;
    }

  };

} //namespace stefan

// STEFAN_COO_MATRIX_H
#endif

