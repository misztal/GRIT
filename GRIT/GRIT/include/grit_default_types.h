#ifndef GRIT_DEFAULT_TYPES_H
#define GRIT_DEFAULT_TYPES_H

#include <grit_parameters.h>

#include <grit_default_algorithm.h>
#include <grit_attribute_vector.h>
#include <grit_pthread_scheduler.h>
#include <grit_domain_manager.h>
#include <grit_subdomain.h>
#include <grit_slab_decomposition.h>
#include <grit_interface_monitor.h>
#include <grit_default_algorithm.h>
#include <grit_attribute_manager.h>

#include <grit_opentissue_mesh.h>
#include <grit_opentissue_math_types.h>

#include <monitors/grit_count_monitor.h>

#include <OpenTissue/core/containers/mesh/polymesh/polymesh.h>

#include <vector>

namespace grit
{
  template<typename T>
  class Simplex0MultiAttributeVector
    : public Simplex0AttributeVector<std::vector<T> >
  {};

  template<typename T>
  class Simplex1MultiAttributeVector
    : public Simplex1AttributeVector<std::vector<T> >
  {};


  template<
    template< typename > class scheduler_class
  , template< typename > class attributes_class
  , template< typename > class attribute_manager_class
  , template< typename > class domain_manager_class
  , template< typename > class subdomain_class
  , template< typename > class domain_decomposition_class
  , template< typename > class monitor_class
  , template< typename > class algorithm_class
  >
  class GRITTypes
  {
  public:

    typedef GRITTypes<
            scheduler_class
            , attributes_class
            , attribute_manager_class
            , domain_manager_class
            , subdomain_class
            , domain_decomposition_class
            , monitor_class
            , algorithm_class
            >  types;

    typedef OpenTissue::polymesh::PolyMesh<>                 mesh_type;

    typedef OpenTissueMesh<mesh_type>                        mesh_impl;
    typedef OpenTissueMathTypes<mesh_type>                   math_types;

    typedef math_types::vector3_type                         vector3_type;
    typedef math_types::real_type                            real_type;

  public:

    /// Attribute vector types

    typedef Simplex0MultiAttributeVector<vector3_type>       sim0_attributes_v3_type;
    //typedef Simplex1MultiAttributeVector<vector3_type>        sim1_attributes_v3_type;
    typedef Simplex2AttributeVector<vector3_type>            sim2_attributes_v3_type;

    typedef Simplex0MultiAttributeVector<real_type>          sim0_attributes_real_type;
    // 2016-01-11 Marek: should we use a multiphase attribute with edges, or single phase?
    typedef Simplex1AttributeVector<real_type>               sim1_attributes_real_type;
    typedef Simplex2AttributeVector<real_type>               sim2_attributes_real_type;

    typedef Simplex0MultiAttributeVector<unsigned int>       sim0_attributes_uint_type;
    typedef Simplex2AttributeVector<unsigned int>            sim2_attributes_uint_type;

  public:

    typedef scheduler_class<types>                           scheduler_type;
    typedef attributes_class<types>                          attributes_type;
    typedef attribute_manager_class<types>                   attribute_manager_type;
    typedef domain_manager_class<types>                      domain_manager_type;
    typedef subdomain_class<types>                           subdomain_type;
    typedef domain_decomposition_class<types>                domain_decomposition_type;
    typedef monitor_class<types>                             monitor_type;
    typedef algorithm_class<types>                           algorithm_type;

    typedef details::Parameters                              param_type;
  };


  typedef GRITTypes<
    details::PThreadScheduler
  , details::Attributes
  , details::AttributeManager
  , details::DomainManager
  , details::SubDomain
  , details::SlabDecomposition
  , details::CountMonitor
  , details::DefaultAlgorithm
  >  default_grit_types;

  typedef default_grit_types::math_types  math_types;

} // namespace grit

// GRIT_DEFAULT_TYPES_H
#endif
