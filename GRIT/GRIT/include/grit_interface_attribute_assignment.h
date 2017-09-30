#ifndef GRIT_INTERFACE_ATTRIBUTE_ASSIGNMENT_H
#define GRIT_INTERFACE_ATTRIBUTE_ASSIGNMENT_H

#include <grit_simplex.h>
#include <grit_simplex_set.h>
#include <grit_interface_mesh.h>

#include <map>

// 2015-10-08 Marek: very important - remember to assing submesh_boundary values, if time allows, move it to attribute vector...

namespace grit
{
  namespace details
  {
    /**
     * This base interface class is used to define functionality
     * that can assign attribute values to newly created
     * simplices.
     *
     * For new vertices this could for instance be based on
     * linear interpolation, for new phases it could be a "constant"
     * value assignment. The parenting information is useful for
     * assigning new values based on inheritence (copy values
     * from old simiplicies to new simplicies).
     */
    template< typename types>
    class InterfaceAttributeAssignment
    {
    protected:

      typedef typename types::attributes_type       AT;
      typedef typename types::param_type            PT;

      typedef          std::map<Simplex1,Simplex1>  simplex1_lut_type;
      typedef          std::map<Simplex2,Simplex2>  simplex2_lut_type;

    public:

      virtual void operator()(
                              SimplexSet          const & new_simplices
                              , SimplexSet        const & old_simplices
                              , InterfaceMesh     const & mesh
                              , PT                const & parameters
                              , AT                      & attributes
                              , simplex1_lut_type const & simplex1_lut
                              , simplex2_lut_type const & simplex2_lut
                              ) const = 0;

    };

  }//end namespace details

}//end namespace grit

// GRIT_INTERFACE_ATTRIBUTE_ASSIGNMENT_H
#endif
