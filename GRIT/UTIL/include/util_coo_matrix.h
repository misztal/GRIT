#ifndef UTIL_COO_MATRIX_H
#define UTIL_COO_MATRIX_H

#include <util_log.h>
#include <util_is_finite.h>
#include <util_is_number.h>
#include <util_matlab_write_vector.h>
#include <util_tensors.h>

#include <sstream>
#include <string>
#include <cassert>
#include <cmath>
#include <vector>

namespace util
{

  template<typename T>
  class COOMatrix
  {
  protected:

    unsigned int m_rows;     ///< The actual number of rows.
    unsigned int m_cols;     ///< The actual number of columns.
    unsigned int m_nzeros;   ///< The actual number of non-zeros stored in the matirx.

    std::vector<unsigned int> m_row_indices;
    std::vector<unsigned int> m_column_indices;
    std::vector<T>            m_values;

  protected:

    void shift_right( unsigned int const & pos)
    {
      unsigned int i = this->m_nzeros;

      for(unsigned int items_to_move = this->m_nzeros-pos +1u; items_to_move > 0u; --items_to_move, --i)
      {
        this->m_row_indices[i+1]    = this->m_row_indices[i];
        this->m_column_indices[i+1] = this->m_column_indices[i];
        this->m_values[i+1]         = this->m_values[i];
      }
    }

  public:

    unsigned int const & rows()  const  { return this->m_rows;   }
    unsigned int const & cols()  const  { return this->m_cols;   }
    unsigned int const & nzeros() const { return this->m_nzeros; }

    std::vector<unsigned int> const & row_indices()    const { return this->m_row_indices;    }
    std::vector<unsigned int> const & column_indices() const { return this->m_column_indices; }
    std::vector<T>            const & values()         const { return this->m_values;         }

  public:

    COOMatrix()
    : m_rows(0)
    , m_cols(0)
    , m_nzeros(0)
    , m_row_indices()
    , m_column_indices()
    , m_values()
    {}

    virtual ~COOMatrix()
    {
      this->clear();
    }

    COOMatrix(
              unsigned int const & rows
              , unsigned int const & cols
              )
    : m_rows(rows)
    , m_cols(cols)
    , m_nzeros(0u)
    , m_row_indices()
    , m_column_indices()
    , m_values()
    {
      assert(rows > 0u || !"COOMatrix(...): internal error? Zero-sized matrix?"     );
      assert(cols > 0u || !"COOMatrix(...): internal error? Zero-sized matrix?"     );
    }

    COOMatrix(
              unsigned int const & nvars
              )
    : m_rows(nvars)
    , m_cols(nvars)
    , m_nzeros(0u)
    , m_row_indices()
    , m_column_indices()
    , m_values()
    {
      assert(nvars > 0u || !"COOMatrix(...): internal error? Zero-sized matrix?"     );
    }

  public:

    bool is_zero() const
    {
      return this->m_nzeros == 0u;
    }

    void clear()
    {
      this->m_rows = 0;
      this->m_cols = 0;
      this->m_nzeros = 0;
      this->m_row_indices.clear();
      this->m_column_indices.clear();
      this->m_values.clear();
    }

    /**
     * Initialize Matrix
     *
     * @param rows         The total number of rows in the matrix.
     * @param cols         The total number of cols in the matrix.
     * @param row_fill_in  An upper conservative bound on the maximum number of non-zero entries in any given row.
     */
    void init(unsigned int const & rows, unsigned int const & cols, unsigned int const & row_fill_in)
    {
      assert(rows > 0u        || !"COOMatrix::init(): internal error? Zero-sized matrix?"     );
      assert(cols > 0u        || !"COOMatrix::init(): internal error? Zero-sized matrix?"     );
      assert(row_fill_in > 0u || !"COOMatrix::init(): Internal error? No row fill in at all?" );

      this->m_rows   = rows;
      this->m_cols   = cols;
      this->m_nzeros = 0;

      this->m_row_indices.resize(rows*row_fill_in);
      this->m_column_indices.resize(rows*row_fill_in);
      this->m_values.resize(rows*row_fill_in);

      std::fill( this->m_row_indices.begin(),    this->m_row_indices.end(),    0   );
      std::fill( this->m_column_indices.begin(), this->m_column_indices.end(), 0   );
      std::fill( this->m_values.begin(),         this->m_values.end(),         0.0 );
    }

    void init(unsigned int const & nvars)
    {
      this->init(nvars, nvars, 10u );
    }

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
          log << "COOMatrix::insert(): Error, duplicate entry" << newline;
          return false;
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
            log << "COOMatrix::insert(): Error, duplicate entry" << newline;
            return false;
          }

          bool const first_is_smaller =  (first_r < r) || (first_r==r && first_c < c);

          if (! first_is_smaller)
          {
            //--- We must "insert" before head of array ------------------------
            shift_right( 0u );

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
                log << "COOMatrix::insert(): Error, duplicate entry" << newline;
                return false;
              }

              pos_is_smaller =  (pos_r < r) || (pos_r==r && pos_c < c);
            }

            //--- Now we know that we must shuffle all elements from pos to
            //--- end one place to the right and insert new element at pos-place.

            shift_right( pos );

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

  template<typename T>
  inline std::string to_string( COOMatrix<T> const & A )
  {
    std::stringstream output;

    output << "sparse(";
    output << matlab_write_vector( A.row_indices(), A.nzeros(), true );
    output << ",";
    output << matlab_write_vector( A.column_indices(), A.nzeros(), true );
    output << ",";
    output << matlab_write_vector( A.values(), A.nzeros() );
    output << ",";
    output << A.rows();
    output << ",";
    output << A.cols();
    output << ",";
    output << A.nzeros();
    output << ")";

    output.flush();

    return output.str();
  }

  template<typename T>
  inline std::ostream & operator<<(std::ostream & stream, COOMatrix<T> const & A )
  {
    stream << to_string(A);

    return stream;
  }

  template<typename T>
  inline COOMatrix<T> transpose( COOMatrix<T> const & A
                                , bool const & guard_against_zeros = true
                                , bool const & verbose = false
)
  {
    COOMatrix<T> At;

    unsigned int const fill_in = A.cols() > 0u ? A.nzeros() / A.cols() : 0u;

    At.init(A.cols(), A.rows(), fill_in );

    for (unsigned int i =0u; i< A.nzeros(); ++i)
    {
      unsigned int const & r = A.row_indices()[i];
      unsigned int const & c = A.column_indices()[i];
      T            const & v = A.values()[i];

      At.insert(c,r,v, guard_against_zeros, verbose);
    }

    return At;
  }

  template<typename T>
  inline COOMatrix<T> make_identity( unsigned int const & nvars)
  {
    T const one = T(1.0);

    COOMatrix<T>  I;

    I.init(nvars,nvars,1u);

    for (unsigned int i =0u; i<nvars; ++i)
    {
      I.insert(i,i,one);
    }

    return I;
  }

  template<typename T>
  inline COOMatrix<T> make_zero( unsigned int const & nvars)
  {
    return COOMatrix<T>(nvars);
  }

  /**
   * Assemble a matrix from blocks using the patterne below.
   *
   *  K = [  A    B;
   *         B^T  C    ]
   *
   */
  template<typename T>
  inline COOMatrix<T>  make_from_blocks( COOMatrix<T> const & A, COOMatrix<T> const & B, COOMatrix<T> const & C)
  {
    COOMatrix<T>  K;

    assert(A.rows() == B.rows() || !"make_from_blocks(): invalid dimensions");
    assert(A.cols() == B.rows() || !"make_from_blocks(): invalid dimensions");
    assert(C.rows() == B.cols() || !"make_from_blocks(): invalid dimensions");
    assert(C.cols() == B.cols() || !"make_from_blocks(): invalid dimensions");

    unsigned int const rows = A.rows() + C.rows();
    unsigned int const cols = A.cols() + B.cols();

    unsigned int const nzeros  = A.nzeros() + C.nzeros() + 2*B.nzeros();

    // 2016-07-23 Marek: adding +1 to make sure we don't underestimate the average fill_in
    //                   was causing allocation errors
    // 2016-08-11 Kenny: I removed the +1u and tested, but could not re-produce any error?
    //                   Subsequently I added a few more asserts to make sure all is
    //                   checked, and I changed the code to use row fill in and NOT
    //                   column fill in. Still no craching?
    unsigned int const row_fill_in = rows > 0u ? (nzeros/rows): 0u;

    K.init(cols, rows, row_fill_in );

    for (unsigned int i =0u; i< A.nzeros(); ++i)
    {
      unsigned int const & r = A.row_indices()[i];
      unsigned int const & c = A.column_indices()[i];
      T            const & v = A.values()[i];

      K.insert(r,c,v);
    }

    for (unsigned int i =0u; i< B.nzeros(); ++i)
    {
      unsigned int const & r = B.row_indices()[i];
      unsigned int const & c = B.column_indices()[i];
      T            const & v = B.values()[i];

      K.insert(r         , c+A.cols(), v);  // B part in top-right corner
      K.insert(c+A.rows(), r         , v);  // B^T part in lower-left corner
    }

    for (unsigned int i =0u; i< C.nzeros(); ++i)
    {
      unsigned int const & r = C.row_indices()[i];
      unsigned int const & c = C.column_indices()[i];
      T            const & v = C.values()[i];

      K.insert( r + A.rows(), c + A.cols(), v);
    }

    return K;
  }

  /**
   * Creates a vector from sub vectors c = [a b];
   */
  template<typename T>
  inline std::vector<T>  make_from_blocks( std::vector<T> const & a, std::vector<T> const & b, bool const & interlaced = false)
  {
    std::vector<T> c;

    if (interlaced)
    {
      assert(a.size() == b.size() || !"make_from_blocks(): a and b must have same size when making interlaced c");

      c.resize( a.size() + b.size() );

      for(unsigned int i = 0u; i < a.size(); ++i)
      {
        c[2u*i+0u] = a[i];
        c[2u*i+1u] = b[i];
      }
    }
    else
    {
      c = a;
      c.insert(c.end(), b.begin(), b.end());
    }

    return c;
  }

  template<typename T>
  inline void split_into_blocks(std::vector<T> const & c, std::vector<T> & a, std::vector<T> & b, bool const & interlaced = false)
  {
    assert(c.size()%2u == 0 || !"split_into_blocks(): c must have and even size");

    unsigned int const N = c.size() / 2u;

    a.resize( N );
    b.resize( N );

    for(unsigned int i = 0u; i < N; ++i)
    {
      a[i] = interlaced ? c[2u*i+0u] : c[i];
      b[i] = interlaced ? c[2u*i+1u] : c[i+N];
    }
  }

  /**
   * @param N   number of first elements from "c" that is split into "a", remainder of elements is split into "b".
   */
  template<typename T>
  inline void split_into_blocks(std::vector<T> const & c, unsigned int const & N, std::vector<T> & a, std::vector<T> & b)
  {
    assert(N <= c.size() || !"split_into_blocks(): N must be smaller than equal c.size()");

    unsigned int const M = c.size() - N;

    a.resize( N );
    b.resize( M );

    for(unsigned int i = 0u; i < N; ++i)
    {
      a[i] = c[i];
    }
    for(unsigned int i = 0u; i < M; ++i)
    {
      b[i] = c[i+N];
    }
  }

  template<typename T>
  inline std::vector<T> add(std::vector<T> const & a, std::vector<T> const & b)
  {
    assert(a.size() == b.size() || !"add(): a and b must have same size");

    std::vector<T> c;

    unsigned int const N = a.size();

    c.resize( N );

    for(unsigned int i = 0u; i < N; ++i)
    {
      c[i] = a[i] + b[i];
    }

    return c;
  }

  template<typename T>
  inline T dot(std::vector<T> const & a, std::vector<T> const & b)
  {
    assert(a.size() == b.size() || !"dot(): a and b must have same size");

    unsigned int const N = a.size();

    T value = 0.0;

    for(unsigned int i = 0u; i < N; ++i)
    {
      value += (a[i] * b[i]);
    }

    return value;
  }

  template<typename T>
  inline std::vector<T> sub(std::vector<T> const & a, std::vector<T> const & b)
  {
    assert(a.size() == b.size() || !"sub(): a and b must have same size");

    std::vector<T> c;

    unsigned int const N = a.size();

    c.resize( N );

    for(unsigned int i = 0u; i < N; ++i)
    {
      c[i] = a[i] - b[i];
    }

    return c;
  }

  template<typename T>
  inline void add_to(std::vector<T>  & a, std::vector<T> const & b)
  {
    assert(a.size() == b.size() || !"add_to(): a and b must have same size");

    for(unsigned int i = 0u; i < a.size(); ++i)
    {
      a[i] += b[i];
    }
  }

  template<typename T>
  inline std::vector<T> add_mul(std::vector<T>  const & a, T const & s, std::vector<T> const & b)
  {
    assert(a.size() == b.size() || !"add_mul(): a and b must have same size");

    std::vector<T> c;

    unsigned int const N = a.size();

    c.resize( N );

    for(unsigned int i = 0u; i < a.size(); ++i)
    {
      c[i] = a[i] + s*b[i];
    }

    return c;
  }

  template<typename T>
  inline std::vector<T> neg(std::vector<T> const & a)
  {
    std::vector<T> c;

    unsigned int const N = a.size();

    c.resize( N );

    for(unsigned int i = 0u; i < N; ++i)
    {
      c[i] = -a[i];
    }

    return c;
  }

  template<typename T>
  inline COOMatrix<T> mul(
                          COOMatrix<T> const & A
                          , T const & s
                          , bool const & guard_against_zeros = true
                          , bool const & verbose = false
                          )
  {
    COOMatrix<T> B;

    unsigned int const fill_in = A.cols() > 0u ? A.nzeros() / A.cols() : 0u;

    B.init(A.cols(), A.rows(), fill_in );

    for (unsigned int i =0u; i< A.nzeros(); ++i)
    {
      unsigned int const & r = A.row_indices()[i];
      unsigned int const & c = A.column_indices()[i];
      T            const & v = A.values()[i];

      B.insert(r,c,v*s, guard_against_zeros, verbose);
    }

    return B;
  }

  template<typename T>
  inline std::vector<T> mul( COOMatrix<T> const & A, std::vector<T> const & x)
  {
    assert( A.cols() == x.size() || !"mul(): size not compatible");

    std::vector<T> y;

    y.resize(A.rows(), 0);

    for (unsigned int i =0u; i< A.nzeros(); ++i)
    {
      unsigned int const & r = A.row_indices()[i];
      unsigned int const & c = A.column_indices()[i];
      T            const & v = A.values()[i];

      y[r] += v*x[c];
    }

    return y;
  }

  template<typename T>
  inline std::vector<T> mul(  T const & s, std::vector<T> const & x)
  {
    std::vector<T> y;

    y.resize(x.size(), 0);

    for (unsigned int i =0u; i< x.size(); ++i)
    {
      y[i] = s*x[i];
    }

    return y;
  }

  /**
   *
   * @note  This is not a general purpose addition of matrices. It
   *        assumes that A and B have the exact same fill in pattern.
   *        The implementation is not very efficient as it uses the
   *        general purpose "insert" method rather than exploiting the
   *        fill in patterns are the exact same.
   */
  template<typename T>
  inline COOMatrix<T> add( COOMatrix<T> const & A, COOMatrix<T> const & B)
  {
    assert( A.cols()   == B.cols() || !"add(): size not compatible");
    assert( A.rows()   == B.rows() || !"add(): size not compatible");
    assert( A.nzeros() == B.nzeros() || !"add(): fill in not compatible");

    unsigned int const fill_in = A.cols() > 0u ? A.nzeros() / A.cols() : 0u;

    COOMatrix<T> C;

    C.init(A.cols(), A.rows(), fill_in );

    for (unsigned int i =0u; i< A.nzeros(); ++i)
    {
      unsigned int const & Ar = A.row_indices()[i];
      unsigned int const & Ac = A.column_indices()[i];
      T            const & Av = A.values()[i];

      unsigned int const & Br = B.row_indices()[i];
      unsigned int const & Bc = B.column_indices()[i];
      T            const & Bv = B.values()[i];

      assert(Ar == Br || !"add(): incompatible fill in pattern");
      assert(Ac == Bc || !"add(): incompatible fill in pattern");

      C.insert( Ar, Ac, Av+Bv);

    }

    return C;
  }

}//namespace util

// UTIL_COO_MATRIX_H
#endif
