#ifndef SIZING_FIELD_H
#define SIZING_FIELD_H

#include <glue.h>
#include <grit.h>

void update_sizing_fields(
                          grit::engine2d_type       & engine
                          , double            const & refinement_value
                          , double            const & coarsening_value
                          , std::string       const & refinement_attribute_name
                          , std::string       const & coarsening_attribute_name
                          )
{
  grit::InterfaceMesh const & mesh = engine.mesh();

  glue::clear_attribute( engine
                        , refinement_attribute_name
                        , refinement_value
                        , glue::EDGE_ATTRIBUTE()
                        );

  glue::clear_attribute( engine
                        , coarsening_attribute_name
                        , coarsening_value
                        , glue::EDGE_ATTRIBUTE()
                        );

  grit::SimplexSet const all = engine.mesh().get_all_simplices();

  {
    glue::Phase const phase = glue::make_phase(engine, all);

    std::vector<double> const refinement_values( phase.m_edges.size(), refinement_value);
    std::vector<double> const coarsening_values( phase.m_edges.size(), coarsening_value);

    glue::set_sub_range( engine, phase, refinement_attribute_name, refinement_values, glue::EDGE_ATTRIBUTE());
    glue::set_sub_range( engine, phase, coarsening_attribute_name, coarsening_values, glue::EDGE_ATTRIBUTE());
  }

{
    grit::SimplexSet const interface = grit::filter(all, grit::IsInterface(mesh));
    grit::SimplexSet const S         = mesh.star(interface);
    grit::SimplexSet const C         = mesh.closure(S);
    grit::SimplexSet const SC        = mesh.star(C);
    grit::SimplexSet const CSC       = mesh.closure(SC);
    grit::SimplexSet const SCSC      = mesh.star(CSC);
    grit::SimplexSet const CSCSC     = mesh.closure(SCSC);

    glue::Phase      const zone3  = glue::make_phase(engine, CSCSC);
    glue::Phase      const zone2  = glue::make_phase(engine, CSC  );
    glue::Phase      const zone1  = glue::make_phase(engine, C    );

    {
      std::vector<double> const refinement_values( zone3.m_edges.size(), refinement_value/2.);
      std::vector<double> const coarsening_values( zone3.m_edges.size(), coarsening_value/2.);

      glue::set_sub_range( engine, zone3, refinement_attribute_name, refinement_values, glue::EDGE_ATTRIBUTE());
      glue::set_sub_range( engine, zone3, coarsening_attribute_name, coarsening_values, glue::EDGE_ATTRIBUTE());
    }

    {
      std::vector<double> const refinement_values( zone2.m_edges.size(), refinement_value/4.);
      std::vector<double> const coarsening_values( zone2.m_edges.size(), coarsening_value/4.);

      glue::set_sub_range( engine, zone2, refinement_attribute_name, refinement_values, glue::EDGE_ATTRIBUTE());
      glue::set_sub_range( engine, zone2, coarsening_attribute_name, coarsening_values, glue::EDGE_ATTRIBUTE());
    }

    {
      std::vector<double> const refinement_values( zone1.m_edges.size(), refinement_value/8.);
      std::vector<double> const coarsening_values( zone1.m_edges.size(), coarsening_value/8.);

      glue::set_sub_range( engine, zone1, refinement_attribute_name, refinement_values, glue::EDGE_ATTRIBUTE());
      glue::set_sub_range( engine, zone1, coarsening_attribute_name, coarsening_values, glue::EDGE_ATTRIBUTE());
    }
  }


  {
    //--- Extract the contact surface
    grit::SimplexSet const interface = grit::filter(all, grit::IsDimension(mesh, 1u)
                                                    && grit::IsInterface(mesh));

    glue::Phase const interface_line = glue::make_phase(engine, mesh.closure(interface));

    std::vector<double> const refinement_values( interface_line.m_edges.size(), refinement_value/12.);
    std::vector<double> const coarsening_values( interface_line.m_edges.size(), coarsening_value/12.);

    glue::set_sub_range( engine, interface_line, refinement_attribute_name, refinement_values, glue::EDGE_ATTRIBUTE());
    glue::set_sub_range( engine, interface_line, coarsening_attribute_name, coarsening_values, glue::EDGE_ATTRIBUTE());
  }
}



void setup_sizing_fields(
                          grit::engine2d_type       & engine
                        , grit::param_type          & parameters
                        , double              const & refinement_value
                        , double              const & coarsening_value
                        , std::string         const & refinement_attribute_name
                        , std::string         const & coarsening_attribute_name
                        )
{
  //--- Now create the actual edge attribute fields that will control maximum
  //--- edge length before doing a refinement and the lowest edge length
  //--- before doing coarsening.
  engine.attributes().create_attribute( refinement_attribute_name, 1u);
  engine.attributes().create_attribute( coarsening_attribute_name, 1u);

  //--- Fill in some sensible initial values for the refinement and coarsening
  //--- edge length values. Make sure coarsening is lower than refinement.
  glue::clear_attribute( engine
                        , refinement_attribute_name
                        , refinement_value
                        , glue::EDGE_ATTRIBUTE()
                        );

  glue::clear_attribute( engine
                        , coarsening_attribute_name
                        , coarsening_value
                        , glue::EDGE_ATTRIBUTE()
                        );

  //--- Next connect the user defined edge attribute fields to the actual
  //--- mesh algorithm operations that do refinement and coarsening.
  parameters.set_lower_threshold_attribute( "refinement",           refinement_attribute_name);
  parameters.set_lower_threshold_attribute( "interface_refinement", refinement_attribute_name);
  parameters.set_upper_threshold_attribute( "coarsening",           coarsening_attribute_name);
  parameters.set_upper_threshold_attribute( "interface_coarsening", coarsening_attribute_name);
}


// SIZING_FIELD_H
#endif
