#ifndef GRIT_EMPTY_ATTRIBUTE_ASSIGNMENT
#define GRIT_EMPTY_ATTRIBUTE_ASSIGNMENT

#include <grit_interface_attribute_assignment.h>
#include <grit_interface_mesh.h>

#include <grit_simplex_set.h>

#include <map>

namespace grit
{
  namespace details
  {
    template< typename types>
    class EmptyAttributeAssignment
    : public InterfaceAttributeAssignment<types>
    {
    protected:

      typedef          InterfaceAttributeAssignment<types> base_class;

      typedef typename types::attributes_type              AT;
      typedef typename types::param_type                   PT;

      typedef typename base_class::simplex1_lut_type       simplex1_lut_type;
      typedef typename base_class::simplex2_lut_type       simplex2_lut_type;

    public:

      void operator()(
                      SimplexSet          const & new_simplices
                      , SimplexSet        const & old_simplices
                      , InterfaceMesh     const & mesh
                      , PT                const & parameters
                      , AT                      & attributes
                      , simplex1_lut_type const & simplex1_lut
                      , simplex2_lut_type const & simplex2_lut
                      ) const
      { }

    };

  } // end namespace details
} // end namespace grit

// GRIT_EMPTY_ATTRIBUTE_ASSIGNMENT
#endif
