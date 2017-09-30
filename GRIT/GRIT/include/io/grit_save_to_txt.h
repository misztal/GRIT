#ifndef GRIT_SAVE_TO_TXT_H
#define GRIT_SAVE_TO_TXT_H

#include <grit_interface_mesh.h>
#include <grit_attribute_vector.h>

#include <util_index_helper.h>

#include <fstream>
#include <iostream>
#include <vector>

namespace grit
{

  namespace details
  {

    template <typename MT>
    inline void save_to_txt(
                            InterfaceMesh & mesh
                            , Simplex0AttributeVector<typename MT::vector3_type> const & coords
                            , std::ofstream & file
                            )
    {
      typedef typename MT::vector3_type V;

      SimplexSet const all = mesh.get_all_simplices();

      SimplexSet::simplex0_const_iterator begin0 = all.begin0();
      SimplexSet::simplex0_const_iterator end0   = all.end0();
      SimplexSet::simplex2_const_iterator begin2 = all.begin2();
      SimplexSet::simplex2_const_iterator end2   = all.end2();

      unsigned int max_id = 0;

      for (SimplexSet::simplex0_const_iterator it = begin0; it != end0; ++it)
      {
        Simplex0 const s = *it;

        if ( s.get_idx0() > max_id )
          max_id = s.get_idx0();
      }

      const unsigned int unassigned = 0xFFFFFFFF;

      std::vector< unsigned int > id_2_counter(max_id+1, unassigned);

      unsigned int counter = 0;
      for (SimplexSet::simplex0_const_iterator it = begin0; it != end0; ++it)
      {
        Simplex0 const s = *it;
        id_2_counter[ s.get_idx0() ] = counter++;

        V const & c = coords[s];

        file << "v "
        << c[0]
        << " "
        << c[1]
        << std::endl;
      }

      for (SimplexSet::simplex2_const_iterator it = begin2; it != end2; ++it)
      {
        Simplex2 const s = *it;

        file << "t "
        << util::to_one_based( id_2_counter[s.get_idx0()] ) << " "
        << util::to_one_based( id_2_counter[s.get_idx1()] ) << " "
        << util::to_one_based( id_2_counter[s.get_idx2()] )
        << std::endl;
      }

      for (SimplexSet::simplex2_const_iterator it = begin2; it != end2; ++it)
      {
        Simplex2 const s = *it;

        file << "l "
        << util::to_one_based( id_2_counter[s.get_idx0()] ) << " "
        << util::to_one_based( id_2_counter[s.get_idx1()] ) << " "
        << util::to_one_based( id_2_counter[s.get_idx2()] ) << " "
        << mesh.label(s)
        << std::endl;
      }
    }

  }//namespace details

  template <typename MT>
  inline void save_to_txt(
                          std::string const & filename
                          , InterfaceMesh & mesh
                          , Simplex0AttributeVector<typename MT::vector3_type> const & coords
                          )
  {
    std::ofstream file(filename.c_str());

    details::save_to_txt<MT>(mesh, coords, file);
  }

  template <typename MT>
  inline void save_to_txt(
                          std::string const & filename
                          , InterfaceMesh & mesh
                          , Simplex0AttributeVector<typename MT::vector3_type> const & coords
                          , Simplex0AttributeVector<typename MT::real_type> const & data
                          )
  {
    typedef typename MT::real_type    T;

    std::ofstream file(filename.c_str());

    details::save_to_txt<MT>(mesh, coords, file);

    //--- Export vertex data ---------------------------------------------------
    SimplexSet all = mesh.get_all_simplices();

    SimplexSet::simplex0_const_iterator begin = all.begin0();
    SimplexSet::simplex0_const_iterator end   = all.end0();

    for (SimplexSet::simplex0_const_iterator it = begin; it != end; ++it)
    {
      Simplex0 const s = *it;

      T const & d = data[s];

      file << "d " << s.get_idx0() << " " << d << std::endl;
    }
  }

}//end namespace grit

// GRIT_SAVE_TO_TXT_H
#endif
