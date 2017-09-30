#ifndef GRIT_ENGINE_2D_H
#define GRIT_ENGINE_2D_H

#include <grit_default_types.h>

#include <string>
#include <vector>

namespace grit
{

  namespace details
  {

    /**
     * The Engine Class.
     * An insance of this class represents the one and single entry point for
     * an end-user to interact with this meshing framework.
     *
     * @tparam types     The type-binder type to be used with the moving mesh framework.
     */
    template<typename types>
    class Engine2D
    {
    public:

      typedef typename types::mesh_impl                MI;
      typedef typename types::math_types               MT;

      typedef typename types::scheduler_type           ST;
      typedef typename types::attributes_type          AMT;
      typedef typename types::param_type               PT;

      typedef typename types::real_type                T;
      typedef typename types::vector3_type             V;

      typedef typename types::sim0_attributes_v3_type  AV0;

      typedef          std::vector<unsigned int>       LT; //!< labels vector type

    protected:

      ST  m_scheduler;

    public:

      ST  const & scheduler()  const { return m_scheduler; }
      AMT const & attributes() const { return m_scheduler.attributes(); }
      MI  const & mesh()       const { return m_scheduler.mesh();}

    public:

      ST  & scheduler()  { return m_scheduler; }
      AMT & attributes() { return m_scheduler.attributes(); }
      MI  & mesh()       { return m_scheduler.mesh(); }

    public:

      void update(PT const & parameters)
      {
        m_scheduler.run(parameters);
      }

      // 2015-09-28 Marek: Have to think how to configure default remeshing parameters (per phase)
      // 2015-03-13 Marek: I don't think that make_default_parameters() is something that we can
      //                   make sensibly in a short amount of time, I'd say we should ditch it for now.
      /*void update()
      {
        m_scheduler.run(make_default_parameters());
      }*/

    }; // class Engine2D

  } // namespace details

  /// The default moving mesh type
  typedef details::Engine2D<default_grit_types> engine2d_type;

} // namespace grit

// GRIT_ENGINE_2D_H
#endif
