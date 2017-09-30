#ifndef GRIT_INTERFACE_MESH_OPERATION_H
#define GRIT_INTERFACE_MESH_OPERATION_H

#include <grit_simplex.h>
#include <grit_simplex_set.h>
#include <grit_interface_mesh.h>

#include <map>

namespace grit
{

  template<
           typename   S
           , typename types
           >
  class InterfaceMeshOperation
  {
  protected:

    typedef typename types::attributes_type      AT;

    typedef          std::map<Simplex1,Simplex1> simplex1_lut_type;
    typedef          std::map<Simplex2,Simplex2> simplex2_lut_type;

  public:

    /**
     * This interface function will implement any global initialization
     * necessary for performing individual mesh operations subsequently.
     * Typically relevant for move operations to compute globally how
     * far vertices could be moved, or for smoothing operations to
     * compute global targets at once for all vertices, which can be
     * moved sequentially afterwards.
     */
    virtual void init(
                      InterfaceMesh  const & mesh
                      , AT                 & attributes
                      ) = 0;



    /**
     * This interface function will implement any relevant attribute
     * updates. This could for instance be a type of smoothing
     * operation.
     */
    virtual bool update_local_attributes(
                                          S               const & simplex
                                          , InterfaceMesh const & mesh
                                          , AT                  & attributes
                                          ) = 0;



    /**
     * This interface function will implement a method that compute what
     * connectivity changes should be made to the mesh.
     *
     * @param new_simplices    Upon return contains all simplicies that should be inserted into the mesh.
     * @param old_simplices    Upon return contains all simplicies that should be removed from the mesh.
     * @param simplex1_lut     Upon return contains parent lookup table for Simplex1 types.
     * @param simplex2_lut     Upon return contains parent lookup table for Simplex2 types.
     */
    virtual bool plan_local_connectivity_changes(
                                                 S                   const & simplex
                                                 , InterfaceMesh           & mesh
                                                 , AT                const & attributes
                                                 , SimplexSet              & new_simplices
                                                 , SimplexSet              & old_simplices
                                                 , simplex1_lut_type       & simplex1_lut
                                                 , simplex2_lut_type       & simplex2_lut
                                                 ) = 0;

  };

}//end namespace grit

// GRIT_INTERFACE_MESH_OPERATION_H
#endif
