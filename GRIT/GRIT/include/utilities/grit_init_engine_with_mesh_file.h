#ifndef GRIT_INIT_ENGINE_WITH_MESH_FILE_H
#define GRIT_INIT_ENGINE_WITH_MESH_FILE_H

#include <grit_engine_2d.h>

#include <utilities/grit_make_unit.h>
#include <io/grit_load_from_txt.h>
#include <io/grit_load_from_msh.h>
#include <utilities/grit_relabel_phase.h>
#include <utilities/grit_is_manifold.h>
#include <utilities/grit_has_ear.h>
#include <utilities/grit_is_any_inverted.h>
#include <utilities/grit_has_multiple_vertices.h>
#include <utilities/grit_has_simple_boundary.h>
#include <utilities/grit_init_attribute_manager_with_mesh.h>

#include <util_log.h>

#include <stdexcept>  // needed for std::logic_error
#include <string>

namespace grit
{
  /**
   *
   * @return        If succesful initialized the return value is true otherwise it is false.
   */
  template<typename types>
  inline bool init_engine_with_mesh_file(
                                             std::string const & filename
                                             , typename types::param_type const & parameters
                                             , details::Engine2D<types> & engine
                                             )
  {
    std::string const & newline = util::Log::newline();

    util::Log logging;

    typedef typename types::math_types             MT;

    // 2015-12-04 Kenny code review: Hmmm, this seems outdated... m_coords
    //                               should be nuked... coordinates are stored
    //                               in attribute manager...

    // 2015-12-20 Marek: this will require re-thinking OpenTissueMesh, particularly
    //                   when it comes to the orientation() function.

    engine.mesh().m_coords = &(engine.attributes().current());

    if( filename.find(".txt") != std::string::npos)
    {
      grit::load_from_txt<MT>(filename, engine.mesh(), engine.attributes().current());
    }
    else if( filename.find(".msh") != std::string::npos)
    {
      grit::load_from_msh<MT>(filename, engine.mesh(), engine.attributes().current());
    }
    else
    {
      logging << "**********************************************" << newline;
      logging << "*******        FATAL ERROR             *******" << newline;
      logging << "**********************************************" << newline;
      logging << "init_engine_with_mesh_file(): ERROR: "
              << " unrecognized file extension."
              << newline;
      logging << "**********************************************" << newline;
      logging << "**********************************************" << newline;
      logging << "**********************************************" << newline;

      throw std::logic_error("Unsupported file type");

      return false;
    }

    relabel_phase(
                  engine.mesh()
                  , parameters.input_labels()
                  , parameters.labels()
                  );

    // 2015-12-04 Kenny code review: This initialization seems to complicate
    //            matters a little... what if end-user wants to define custom
    //            attributes? Will this initialization still work?

    // 2015-12-20 Marek: this is just to initialize the simplex0 labels, so that
    //            the end user doesn't have to do it by hand, since the information
    //            can be automatically obtained from the input mesh. This enables
    //            defining custom attributes.

    init_attribute_manager_with_mesh<types>( engine.mesh(), parameters, engine.attributes());

    if (parameters.make_unit())
      grit::make_unit(engine);

    if(parameters.use_manifold() &&  ! grit::is_manifold(engine.mesh()) )
    {
      logging << "**********************************************" << newline;
      logging << "*******        FATAL ERROR             *******" << newline;
      logging << "**********************************************" << newline;
      logging << "init_engine_with_mesh_file(): ERROR: Mesh "
              << filename
              << " was not a manifold"
              << newline;
      logging << "**********************************************" << newline;
      logging << "**********************************************" << newline;
      logging << "**********************************************" << newline;

      throw std::logic_error("Input mesh was non-manifold");

      return false;
    }

    if(parameters.no_ears() &&  grit::has_ear(engine.mesh()) )
    {
      logging << "**********************************************" << newline;
      logging << "*******        FATAL ERROR             *******" << newline;
      logging << "**********************************************" << newline;
      logging << "init_engine_with_mesh_file(): ERROR: Mesh "
              << filename
              << " has an ear"
              << newline;
      logging << "**********************************************" << newline;
      logging << "**********************************************" << newline;
      logging << "**********************************************" << newline;

      throw std::logic_error("Input mesh has ears");

      return false;
    }

    if( grit::details::is_any_inverted<typename MT::vector3_type>(
                                       engine.mesh().get_all_simplices()
                                       , engine.attributes()
                                       )
       )
    {
      logging << "**********************************************" << newline;
      logging << "*******        FATAL ERROR             *******" << newline;
      logging << "**********************************************" << newline;
      logging << "init_engine_with_mesh_file(): ERROR: Mesh "
              << filename
              << " contained collapsed or inverted triangles"
              << newline;
      logging << "**********************************************" << newline;
      logging << "**********************************************" << newline;
      logging << "**********************************************" << newline;

      throw std::logic_error("Input mesh contains collapsed or inverted triangles");

      return false;
    }

    if(parameters.use_simple_boundary() && !grit::details::has_simple_boundary( engine ) )
    {
      logging << "**********************************************" << newline;
      logging << "*******        FATAL ERROR             *******" << newline;
      logging << "**********************************************" << newline;
      logging << "init_engine_with_mesh_file(): ERROR: Mesh "
      << filename
      << " contained holes"
      << newline;
      logging << "**********************************************" << newline;
      logging << "**********************************************" << newline;
      logging << "**********************************************" << newline;

      throw std::logic_error("Input mesh contains holes");

      return false;
    }

    if( parameters.no_multiple_vertices() && grit::details::has_multiple_vertices<typename MT::vector3_type>(
                                                                         engine.mesh().get_all_simplices()
                                                                         , engine.attributes()
                                                                         , parameters.multiple_vertices_threshold()
                                                                  )
       )
    {
      logging << "**********************************************" << newline;
      logging << "*******        FATAL ERROR             *******" << newline;
      logging << "**********************************************" << newline;
      logging << "init_engine_with_mesh_file(): ERROR: Mesh "
      << filename
      << " contained multiple vertices with same coordinates"
      << newline;
      logging << "**********************************************" << newline;
      logging << "**********************************************" << newline;
      logging << "**********************************************" << newline;

      throw std::logic_error("Input mesh contains multiple vertices with same coordinates");

      return false;
    }

    return true;
  }

} // namespace grit

// GRIT_INIT_ENGINE_WITH_MESH_FILE_H
#endif

