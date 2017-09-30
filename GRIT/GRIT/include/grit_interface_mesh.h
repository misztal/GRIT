#ifndef GRIT_INTERFACE_MESH_H
#define GRIT_INTERFACE_MESH_H

#include <grit_simplex.h>
#include <grit_simplex_set.h>
#include <grit_parameters.h>

#include <map>

namespace grit
{

  class InterfaceMesh
  {
  public:

    InterfaceMesh() { }
    virtual ~InterfaceMesh() { }

    typedef enum { negative = -1, none = 0, positive = 1 } sign_type;

  public:

    virtual SimplexSet boundary(Simplex0 const & s) const = 0;
    virtual SimplexSet boundary(Simplex1 const & s) const = 0;
    virtual SimplexSet boundary(Simplex2 const & s) const = 0;

    virtual SimplexSet full_boundary(Simplex0 const & s) const = 0;
    virtual SimplexSet full_boundary(Simplex1 const & s) const = 0;
    virtual SimplexSet full_boundary(Simplex2 const & s) const = 0;

    virtual SimplexSet star(Simplex0 const & s) const = 0;
    virtual SimplexSet star(Simplex1 const & s) const = 0;
    virtual SimplexSet star(Simplex2 const & s) const = 0;
    virtual SimplexSet star(SimplexSet const & simplex_set) const = 0;

    virtual SimplexSet closure(Simplex0 const & s) const = 0;
    virtual SimplexSet closure(Simplex1 const & s) const = 0;
    virtual SimplexSet closure(Simplex2 const & s) const = 0;
    virtual SimplexSet closure(SimplexSet const & simplex_set) const = 0;

    virtual SimplexSet link(Simplex0 const & s) const = 0;

    virtual SimplexSet get_all_simplices() const = 0;
    virtual SimplexSet get_simplices(unsigned int const & label) const = 0;

    virtual void remove_all_simplices() = 0;
    virtual void remove(Simplex0 const & s) = 0;
    virtual void remove(Simplex1 const & s) = 0;
    virtual void remove(Simplex2 const & s) = 0;
    virtual void remove(SimplexSet const & simplex_set) = 0;

    virtual void replace(
                         SimplexSet const & old_simplices
                         , SimplexSet const & new_simplices
                         , std::map<Simplex2,Simplex2> const & parent_lut
                         , details::Parameters const & parameters
                         ) = 0;

    virtual Simplex2 insert(Simplex0 const & s0, Simplex0 const & s1, Simplex0 const & s2) = 0;
    virtual Simplex0 insert() = 0;

    virtual bool & submesh_boundary(Simplex0 const & s) = 0;

    virtual bool is_submesh_boundary(Simplex0 const & s) const = 0;
    virtual bool is_submesh_boundary(Simplex1 const & s) const = 0;

    bool is_submesh_boundary(Simplex2 const & ) const
    {
      // 2015-08-01 Kenny: The whole Simplex2 can not be a submesh boundary (only a face of it can be).

      return false;
    }

    virtual bool is_submesh_boundary(SimplexSet const & simplex_set) const = 0;

    // 2015-11-08 Kenny: One could also argue
    //                   that labels are a kind of Simplex2 attribute and not
    //                   something we should store in the mesh. The mesh should
    //                   basically only store "connectivity" info (coordinates
    //                   are not even needed to be stored in the actual
    //                   mesh -- these too are attributes).
    // 2015-11-08 Marek: I agree, the only problem is that IsInterface() logical
    //                   expression would have to start depending on attributes object.
    //                   Some heavy refactoring might be needed.
    virtual unsigned int       & label(Simplex2 const & s)       = 0;
    virtual unsigned int const & label(Simplex2 const & s) const = 0;

    unsigned int label(Simplex0 const & ) const
    {
      assert(false || !"label(s0) is not defined");
      return 0;
    }

    unsigned int label(Simplex1 const & ) const
    {
      assert(false || !"label(s1) is not defined");
      return 0;
    }

    virtual sign_type orientation(Simplex2 const & s) const = 0;

    virtual bool is_interface(Simplex1 const & s) const = 0;
    virtual bool is_interface(Simplex0 const & s) const = 0;

    bool is_interface(Simplex2 const & ) const
    {
      // 2015-08-01 Kenny: The whole Simplex2 can not be inteface (only a face of it can be).

      return false;
    }

    virtual bool is_interface(SimplexSet const & simplex_set) const = 0;

    virtual bool is_boundary(Simplex0 const & s) const = 0;
    virtual bool is_boundary(Simplex1 const & s) const = 0;

    bool is_boundary(Simplex2 const & ) const
    {
      // 2015-08-01 Kenny: The whole Simplex2 can not be a boundary (only a face of it can be).

      return false;
    }

    virtual bool is_valid_simplex(Simplex0 const & s) const = 0;
    virtual bool is_valid_simplex(Simplex1 const & s) const = 0;
    virtual bool is_valid_simplex(Simplex2 const & s) const = 0;

    virtual bool is_manifold(Simplex0 const & s) const = 0;
    virtual bool is_manifold(Simplex1 const & s) const = 0;

    bool is_manifold(Simplex2 const & ) const
    {
      // 2015-08-01 Kenny: A Simplex2 is a discrete manifold by itself, hence we can trivailly return true.

      return true;
    }

    virtual bool is_ear(Simplex2 const & s) const = 0;

  };

}

// GRIT_INTERFACE_MESH_H
#endif
