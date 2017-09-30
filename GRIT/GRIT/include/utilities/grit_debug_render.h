#ifndef GRIT_DEBUG_RENDER_H
#define GRIT_DEBUG_RENDER_H

#include <grit_interface_mesh.h>
#include <grit_parameters.h>

#include <utilities/grit_svg_draw.h>

namespace grit
{
  namespace details
  {
    /**
     * Saves an SVG file (to a file location specified by
     * parameters.debug_render_filename()) with simplex and
     * its neighborhood in the mesh (depth of neighborhood
     * specified by parameters.debug_render_level()).
     */
    template<
             typename S
             , typename AT
             >
    void debug_render(
                      S               const & simplex
                      , InterfaceMesh const & mesh
                      , Parameters    const & parameters
                      , AT            const & attributes
                      )
    {
      SimplexSet T;
      T.insert(simplex);

      for( unsigned int i=0u; i<parameters.debug_render_level(); ++i)
      {
        T = mesh.star(T);
        T = mesh.closure(T);
      }

      svg_draw( parameters.debug_render_filename(), T, mesh, parameters, attributes);
    }
  }
} // namespace grit

// GRIT_DEBUG_RENDER_H
#endif
