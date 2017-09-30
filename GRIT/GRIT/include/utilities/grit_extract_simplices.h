#ifndef GRIT_EXTRACT_SIMPLICES_H
#define GRIT_EXTRACT_SIMPLICES_H

#include <grit_simplex.h>
#include <grit_simplex_set.h>

namespace grit
{

  inline void extract_simplices(
                                  Simplex0 & v1
                                , Simplex0 & v2
                                , SimplexSet const & A
                                )
  {
    SimplexSet::simplex0_const_iterator it = A.begin0();

    v1 = *it; ++it;
    v2 = *it;
  }

  inline void extract_simplices(
                                  Simplex0 & v1
                                , Simplex0 & v2
                                , Simplex0 & v3
                                , SimplexSet const & A
                                )
  {
    SimplexSet::simplex0_const_iterator it = A.begin0();

    v1 = *it; ++it;
    v2 = *it; ++it;
    v3 = *it;
  }

  inline void extract_simplices(
                                  Simplex0 & v1
                                , Simplex0 & v2
                                , Simplex0 & v3
                                , Simplex0 & v4
                                , SimplexSet const & A
                                )
  {
    SimplexSet::simplex0_const_iterator it = A.begin0();

    v1 = *it; ++it;
    v2 = *it; ++it;
    v3 = *it; ++it;
    v4 = *it;
  }
  
  inline void extract_simplices(
                                  Simplex2 & v1
                                , Simplex2 & v2
                                , SimplexSet const & A
                                )
  {
    SimplexSet::simplex2_const_iterator it = A.begin2();
    
    v1 = *it; ++it;
    v2 = *it;
  }
  
}//end namespace grit

// GRIT_EXTRACT_SIMPLICES_H
#endif
