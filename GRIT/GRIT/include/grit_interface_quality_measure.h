#ifndef GRIT_INTERFACE_QUALITY_MESAURE_H
#define GRIT_INTERFACE_QUALITY_MESAURE_H

namespace grit
{

  template<typename S>
  class InterfaceQualityMeasure
  {
  public:

    virtual bool is_bad(S const & s) const = 0;
  };

}//end namespace grit

// GRIT_INTERFACE_QUALITY_MESAURE_H
#endif
