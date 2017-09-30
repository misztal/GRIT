#ifndef GRIT_CONSISTENT_QUALITY_MESAURE_H
#define GRIT_CONSISTENT_QUALITY_MESAURE_H

#include <grit_interface_quality_measure.h>
#include <grit_interface_mesh.h>

#include <util_log.h>

namespace grit
{
  namespace details
  {
    template< typename types, typename S>
    class AlwaysBadQualityMeasure
    : public InterfaceQualityMeasure<S>
    {
    protected:

      typedef typename types::attributes_type  AT;
      typedef typename types::param_type       PT;

    public:

      AlwaysBadQualityMeasure(
                              std::string     const & name
                              , unsigned int  const & label
                              , PT            const & parameters
                              , InterfaceMesh const & mesh
                              , AT            const & attributes
                              )
      {}

      bool is_bad(S const & /*s*/) const
      {
        return true;
      }

   };

  } //end namespace details
}//end namespace grit

// GRIT_CONSISTENT_QUALITY_MESAURE_H
#endif
