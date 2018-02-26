#ifndef GRIT_SIMPLEX_SET_FUNCTIONS_H
#define GRIT_SIMPLEX_SET_FUNCTIONS_H

#include <grit_simplex.h>
#include <grit_simplex_set.h>
#include <grit_logic_expressions.h>

#include <util_log.h>

#include <cassert>
#include <algorithm>
#include <limits>
#include <iosfwd>
#include <iterator>

namespace grit
{

  template<typename Iter>
  inline Iter begin(SimplexSet const & A);

  template<>
  inline typename SimplexSet::simplex0_const_iterator begin<typename SimplexSet::simplex0_const_iterator>(SimplexSet const & A)
  {
    return A.begin0();
  }

  template<>
  inline typename SimplexSet::simplex1_const_iterator begin<typename SimplexSet::simplex1_const_iterator>(SimplexSet const & A)
  {
    return A.begin1();
  }

  template<>
  inline typename SimplexSet::simplex2_const_iterator begin<typename SimplexSet::simplex2_const_iterator>(SimplexSet const & A)
  {
    return A.begin2();
  }

  template<typename Iter>
  inline Iter end(SimplexSet const & A);

  template<>
  inline typename SimplexSet::simplex0_const_iterator end<typename SimplexSet::simplex0_const_iterator>(SimplexSet const & A)
  {
    return A.end0();
  }

  template<>
  inline typename SimplexSet::simplex1_const_iterator end<typename SimplexSet::simplex1_const_iterator>(SimplexSet const & A)
  {
    return A.end1();
  }

  template<>
  inline typename SimplexSet::simplex2_const_iterator end<typename SimplexSet::simplex2_const_iterator>(SimplexSet const & A)
  {
    return A.end2();
  }

  inline std::ostream & operator<<(std::ostream & stream, SimplexSet const & A)
  {
    typedef SimplexSet::simplex0_const_iterator iterator0;
    typedef SimplexSet::simplex1_const_iterator iterator1;
    typedef SimplexSet::simplex2_const_iterator iterator2;

    iterator0 const begin0 = A.begin0();
    iterator0 const end0   = A.end0();
    iterator1 const begin1 = A.begin1();
    iterator1 const end1   = A.end1();
    iterator2 const begin2 = A.begin2();
    iterator2 const end2   = A.end2();

    stream << "0-simplices:" << std::endl;
    for (iterator0 s = begin0; s != end0; ++s)
    {
      stream << *s << std::endl;
    }

    stream << std::endl << "1-simplices:" << std::endl;
    for (iterator1 s = begin1; s != end1; ++s)
    {
      stream << *s << std::endl;
    }

    stream << std::endl << "2-simplices:" << std::endl;
    for (iterator2 s = begin2; s != end2; ++s)
    {
      stream << *s << std::endl;
    }

    return stream;
  }

  inline SimplexSet difference(SimplexSet const & A, SimplexSet const & B)
  {
    SimplexSet C;

    std::set_difference(
                        A.begin0(), A.end0(),
                        B.begin0(), B.end0(),
                        std::inserter(SimplexSetAccess::simplices0(C), SimplexSetAccess::begin0(C))
                        );

    std::set_difference(
                        A.begin1(), A.end1(),
                        B.begin1(), B.end1(),
                        std::inserter(SimplexSetAccess::simplices1(C), SimplexSetAccess::begin1(C))
                        );

    std::set_difference(
                        A.begin2(), A.end2(),
                        B.begin2(), B.end2(),
                        std::inserter(SimplexSetAccess::simplices2(C), SimplexSetAccess::begin2(C))
                        );

    return C;
  }

  inline SimplexSet intersection(SimplexSet const & A, SimplexSet const & B)
  {
    SimplexSet C;

    std::set_intersection(
                          A.begin0(), A.end0(),
                          B.begin0(), B.end0(),
                          std::inserter(SimplexSetAccess::simplices0(C), SimplexSetAccess::begin0(C))
                          );

    std::set_intersection(
                          A.begin1(), A.end1(),
                          B.begin1(), B.end1(),
                          std::inserter(SimplexSetAccess::simplices1(C), SimplexSetAccess::begin1(C))
                          );

    std::set_intersection(
                          A.begin2(), A.end2(),
                          B.begin2(), B.end2(),
                          std::inserter(SimplexSetAccess::simplices2(C), SimplexSetAccess::begin2(C))
                          );

    return C;
  }

  inline SimplexSet set_union(SimplexSet const & A, SimplexSet const & B)
  {
    SimplexSet C;

    std::set_union(
                   A.begin0(), A.end0(),
                   B.begin0(), B.end0(),
                   std::inserter(SimplexSetAccess::simplices0(C), SimplexSetAccess::begin0(C))
                   );

    std::set_union(
                   A.begin1(), A.end1(),
                   B.begin1(), B.end1(),
                   std::inserter(SimplexSetAccess::simplices1(C), SimplexSetAccess::begin1(C))
                   );

    std::set_union(
                   A.begin2(), A.end2(),
                   B.begin2(), B.end2(),
                   std::inserter(SimplexSetAccess::simplices2(C), SimplexSetAccess::begin2(C))
                   );

    return C;
  }

  inline SimplexSet filter(SimplexSet const & A, unsigned int const & dimension)
  {
    SimplexSet C;

    switch (dimension)
    {
      case 0:
        SimplexSetAccess::simplices0(C) = A.simplices0();
        break;
      case 1:
        SimplexSetAccess::simplices1(C) = A.simplices1();
        break;
      case 2:
        SimplexSetAccess::simplices2(C) = A.simplices2();
        break;
    }

    return C;
  }

  template<typename E>
  inline SimplexSet filter(SimplexSet const & A, LogicExpression<E> const & condition)
  {
    SimplexSet result;

    // 2015-08-01 Kenny code review : Possible rewrite to use std::copy_if to take
    //                                advantage of any acceleration/optimization that
    //                                STL can offer over our own "naive" iteration
    //                                below.

    SimplexSet::simplex0_const_iterator begin0 = A.begin0();
    SimplexSet::simplex0_const_iterator end0   = A.end0();

    for (SimplexSet::simplex0_const_iterator s0 = begin0; s0 != end0; ++s0)
    {
      if (condition(*s0))
        result.insert(*s0);
    }

    SimplexSet::simplex1_const_iterator begin1 = A.begin1();
    SimplexSet::simplex1_const_iterator end1   = A.end1();

    for (SimplexSet::simplex1_const_iterator s1 = begin1; s1 != end1; ++s1)
    {
      if (condition(*s1))
        result.insert(*s1);
    }

    SimplexSet::simplex2_const_iterator begin2 = A.begin2();
    SimplexSet::simplex2_const_iterator end2   = A.end2();

    for (SimplexSet::simplex2_const_iterator s2 = begin2; s2 != end2; ++s2)
    {
      if (condition(*s2))
        result.insert(*s2);
    }

    return result;
  }

  inline SimplexSet compute_boundary(InterfaceMesh const & mesh, SimplexSet const & A)
  {
    return filter(A, (IsDimension(mesh,0) || IsDimension(mesh,1)) && IsBoundary(mesh) );
  }

  inline SimplexSet compute_boundary(InterfaceMesh const & mesh)
  {
    return compute_boundary(mesh, mesh.get_all_simplices() );
  }

  inline SimplexSet compute_interface(InterfaceMesh const & mesh, SimplexSet const & A)
  {
    return filter(A, (IsDimension(mesh,0) || IsDimension(mesh,1)) && IsInterface(mesh) );
  }

  inline SimplexSet compute_interface(InterfaceMesh const & mesh)
  {
    return compute_interface(mesh, mesh.get_all_simplices() );
  }

  /**
   * Get simplex set of specified label (the phase).
   *
   * @note  This function gives one the closure of all simplex 2 with the given label.
   */
  inline SimplexSet compute_phase(InterfaceMesh const & mesh, unsigned int const & label)
  {
    SimplexSet result = filter(mesh.get_all_simplices(), IsDimension(mesh,2) && InPhase(mesh,label) );
    return mesh.closure(result);
  }

}//end namespace grit

// GRIT_SIMPLEX_SET_FUNCTIONS_H
#endif
