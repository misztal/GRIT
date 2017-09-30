#ifndef GRIT_INTERFACE_SCHEDULER_H
#define GRIT_INTERFACE_SCHEDULER_H

namespace grit
{

  namespace details
  {

    template<typename types>
    class InterfaceScheduler
    {
    public:

      typedef typename types::mesh_impl               MI;
      typedef typename types::math_types              MT;

      typedef typename types::attributes_type         AMT;
      typedef typename types::param_type              PT;

      typedef typename types::real_type               T;
      typedef typename types::vector3_type            V;

      typedef typename types::sim0_attributes_v3_type AV0;

      typedef          std::vector<unsigned int>      LT;

    protected:

      MI            m_mesh;
      AMT           m_attributes;

    public:

      MI           const & mesh()       const { return m_mesh; }
      AMT          const & attributes() const { return m_attributes; }

    public:

      MI           & mesh()       { return m_mesh; }
      AMT          & attributes() { return m_attributes; }

    public:

      virtual void run( PT const & parameters) = 0;

    };

  } // namespace details

} // namespace grit

// GRIT_INTERFACE_SCHEDULER
#endif

