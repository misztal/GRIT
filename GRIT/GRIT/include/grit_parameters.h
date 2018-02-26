#ifndef GRIT_PARAMETERS_H
#define GRIT_PARAMETERS_H

#include <util_config_file.h>
#include <util_log.h>

#include <string>
#include <vector>
#include <map>

namespace grit
{

  namespace details
  {
    /**
     * Container class for the GRIT parameters.
     */
    class Parameters
    {
    protected:

      typedef  std::vector<unsigned int>   LV;  ///< labels vector type

    protected:

      std::map< std::string, std::vector<unsigned int> >  m_max_iterations;   ///< max_iteration values for each module or remeshing operation

      std::map< std::string, std::vector<double> >        m_strength;           ///< strengths of remeshing operations (interpretation of this parameter is operation-dependent)
      std::map< std::string, std::vector<double> >        m_angle_threshold;    ///< angle thresholds of remeshing operations (typically measuring interface flatness, when needed)
      std::map< std::string, std::vector<double> >        m_area_threshold;     ///< area thresholds of remeshing operations (used to prevent operations from creating degenerate triangles)
      std::map< std::string, std::vector<double> >        m_distance_threshold; ///< distance thresholds of remeshing operations.
      std::map< std::string, std::vector<double> >        m_lower_threshold;    ///< lower threshold values for threshold quality measures used by remeshing operations (typically lower threshold on edge lengths)
      std::map< std::string, std::vector<double> >        m_upper_threshold;    ///< upper threshold values for threshold quality measures used by remeshing operations (typically upper threshold on edge lengths)

      std::map< std::string, std::string >                m_lower_threshold_attribute; ///< operation_name to attribute_name map, used for custom sizing fields
      std::map< std::string, std::string >                m_upper_threshold_attribute; ///< operation_name to attribute_name map, used for custom sizing fields

      LV                m_input_labels;                ///< labels to consider in the input file
      LV                m_labels;                      ///< labels used by GRIT, in the same order as m_input_labels
      unsigned int      m_ambient_label;               ///< label of the ambient (vacuum) phase
      unsigned int      m_number_of_subdomains;        ///< number of subdomains used by GRIT (equal to the number of threads)
      unsigned int      m_debug_render_level;          ///< the "size" of the neighborhood to be output in the debug render
      bool              m_make_unit;                   ///< a flag notifying whether the domain should be scaled to [0,1]x[0,1] at initialization
      bool              m_profiling;                   ///< a flag to switch profiling on/off
      bool              m_verbose;                     ///< a flag specifying verbosity of the log output
      bool              m_debug_render;                ///< a flat to switch visual debugging on/off
      bool              m_silent_threads;              ///< a flag for switching on/off in-thread console output, false only for debugging purposes or in single-threaded case
      bool              m_use_ambient;                 ///< a flag for switching on/off use of the ambient phase
      bool              m_use_one_phase_style;         ///< a flag for switching on/off the drawing of multiple phases as one
      bool              m_use_only_interface;          ///< a flag for switching on/off moving the entire phase (instead of just interface)
      bool              m_use_manifold;                ///< a flag telling whether one only wants to use initial manifold meshes or not (typical always true).
      bool              m_no_ears;                     ///< a flag telling whether one allows an initial mesh to have an ear or not
      bool              m_use_simple_boundary;         ///< a flag telling whether one allows an initial mesh to have holes or not.
      bool              m_no_multiple_vertices;        ///< a flag telling whether one allows an initial mesh to have multiple vertices with same coordinates
      bool              m_use_sparse_edge_attributes;  ///< a flag telling whether one uses sparse edge attributes, and switches off attribute "smearing"
      double            m_multiple_vertices_threshold; ///< the accuracy/precision used to test if two vertices have the same coordinates.

      std::string       m_debug_render_filename;       ///< the filename of the debug render SVG.

      std::vector< std::string>  m_operation_names;

    public:

      LV                const & input_labels()          const { return m_input_labels;         }
      LV                const & labels()                const { return m_labels;               }
      unsigned int      const & ambient_label()         const { return m_ambient_label;        }
      unsigned int      const & number_of_subdomains()  const { return m_number_of_subdomains; }
      unsigned int      const & debug_render_level()    const { return m_debug_render_level;   }
      bool              const & make_unit()             const { return m_make_unit;            }
      bool              const & profiling()             const { return m_profiling;            }
      bool              const & verbose()               const { return m_verbose;              }
      bool              const & debug_render()          const { return m_debug_render;         }
      bool              const & silent_threads()        const { return m_silent_threads;       }
      bool              const & use_ambient()           const { return m_use_ambient;          }
      bool              const & use_one_phase_style()   const { return m_use_one_phase_style;  }
      bool              const & use_only_interface()    const { return m_use_only_interface;   }
      bool              const & use_manifold()          const { return m_use_manifold;         }
      bool              const & no_ears()               const { return m_no_ears;              }
      bool              const & use_simple_boundary()   const { return m_use_simple_boundary;  }
      bool              const & no_multiple_vertices()  const { return m_no_multiple_vertices; }
      bool              const & use_sparse_edge_attributes()  const { return m_use_sparse_edge_attributes;  }
      double            const & multiple_vertices_threshold() const { return m_multiple_vertices_threshold; }
      std::string       const & debug_render_filename()       const { return m_debug_render_filename;       }

      std::vector< std::string> const & operation_names() const { return m_operation_names; }

    public:

      unsigned int      & ambient_label()        { return m_ambient_label;        }
      unsigned int      & number_of_subdomains() { return m_number_of_subdomains; }
      unsigned int      & debug_render_level()   { return m_debug_render_level;   }
      bool              & make_unit()            { return m_make_unit;            }
      bool              & profiling()            { return m_profiling;            }
      bool              & verbose()              { return m_verbose;              }
      bool              & debug_render()         { return m_debug_render;         }
      bool              & silent_threads()       { return m_silent_threads;       }
      bool              & use_ambient()          { return m_use_ambient;          }
      bool              & use_one_phase_style()  { return m_use_one_phase_style;  }
      bool              & use_only_interface()   { return m_use_only_interface;   }
      bool              & use_manifold()         { return m_use_manifold;         }
      bool              & no_ears()              { return m_no_ears;              }
      bool              & use_simple_boundary()  { return m_use_simple_boundary;  }
      bool              & no_multiple_vertices() { return m_no_multiple_vertices; }
      bool              & use_sparse_edge_attributes()  { return m_use_sparse_edge_attributes;  }
      double            & multiple_vertices_threshold() { return m_multiple_vertices_threshold; }
      std::string       & debug_render_filename()       { return m_debug_render_filename;       }

    public:

      Parameters()
      : m_ambient_label(0u)
      , m_number_of_subdomains(1u)
      , m_debug_render_level(0u)
      , m_make_unit(true)
      , m_profiling(false)
      , m_verbose(true)
      , m_debug_render(false)
      , m_silent_threads(true)
      , m_use_ambient(true)
      , m_use_one_phase_style(false)
      , m_use_only_interface(true)
      , m_use_manifold(true)
      , m_no_ears(false)
      , m_use_simple_boundary(true)
      , m_no_multiple_vertices(true)
      , m_use_sparse_edge_attributes(false)
      , m_multiple_vertices_threshold(0.0001)
      , m_debug_render_filename("./output/debug.svg")
      {}

    public:

      unsigned int max_iterations( std::string const & module_name ) const
      {
        if( m_max_iterations.find(module_name) != m_max_iterations.end())
        {
          LV const & values = m_max_iterations.at(module_name);

          if( values.size() > 0u)
          {
            return values[0u];
          }
        }

        return 0u;
      }

    public:

      unsigned int get_max_iterations(
                                      std::string    const & operation_name
                                      , unsigned int const   label
                                      ) const
      {
        if( m_max_iterations.find(operation_name) != m_max_iterations.end())
        {
          LV const & values = m_max_iterations.at(operation_name);

          if( label < values.size())
          {
            return values[label];
          }
        }

        return 0u;
      }

    public:

      double get_strength(
                          std::string    const & operation_name
                          , unsigned int const   label
                          ) const
      {
        if( m_strength.find(operation_name) != m_strength.end())
        {
          std::vector< double> const & values = m_strength.at(operation_name);

          if( label < values.size())
          {
            return values[label];
          }
        }

        return 0.0;
      }

    public:

      double get_distance_threshold(
                           std::string    const & operation_name
                           , unsigned int const   label
                           ) const
      {
        if( m_distance_threshold.find(operation_name) != m_distance_threshold.end())
        {
          std::vector< double> const & values = m_distance_threshold.at(operation_name);

          if( label < values.size())
          {
            return values[label];
          }
        }

        return 0.0;
      }

    public:

      double get_area_threshold(
                           std::string    const & operation_name
                           , unsigned int const   label
                           ) const
      {
        if( m_area_threshold.find(operation_name) != m_area_threshold.end())
        {
          std::vector< double> const & values = m_area_threshold.at(operation_name);

          if( label < values.size())
          {
            return values[label];
          }
        }

        return 0.0;
      }

    public:

      double get_angle_threshold(
                                 std::string    const & operation_name
                                 , unsigned int const   label
                                 ) const
      {
        if( m_angle_threshold.find(operation_name) != m_angle_threshold.end())
        {
          std::vector< double> const & values = m_angle_threshold.at(operation_name);

          if( label < values.size())
          {
            return values[label];
          }
        }

        return 180.0;
      }

    public:

      double get_lower_threshold(
                                 std::string    const & operation_name
                                 , unsigned int const   label
                                 ) const
      {
        if( m_lower_threshold.find(operation_name) != m_lower_threshold.end())
        {
          std::vector< double> const & values = m_lower_threshold.at(operation_name);

          if( label < values.size())
          {
            return values[label];
          }
        }

        return 0.0;
      }

    public:

      double get_upper_threshold(
                                 std::string    const & operation_name
                                 , unsigned int const   label
                                 ) const
      {
        if( m_upper_threshold.find(operation_name) != m_upper_threshold.end())
        {
          std::vector< double> const & values = m_upper_threshold.at(operation_name);

          if( label < values.size())
          {
            return values[label];
          }
        }

        return 0.0;
      }

    public:

      void set_max_iterations(
                              std::string    const & operation_name
                              , unsigned int const   label
                              , unsigned int const   value
                              )
      {
        std::vector< unsigned int> & values = m_max_iterations[operation_name];

        if (label >= values.size())
        {
          values.resize(label+1u);
        }

        values[label] = value;
      }

    public:

      void set_strength(
                        std::string    const & operation_name
                        , unsigned int const   label
                        , double       const   value
                        )
      {
        std::vector< double> & values = m_strength[operation_name];

        if( label >= values.size() )
        {
          values.resize(label+1u);
        }

        values[label] = value;
      }

    public:

      void set_distance_threshold(
                         std::string    const & operation_name
                         , unsigned int const   label
                         , double       const   value
                         )
      {
        std::vector< double> & values = m_distance_threshold[operation_name];

        if( label >= values.size())
        {
          values.resize(label+1u);
        }

        values[label] = value;
      }

    public:

      void set_angle_threshold(
                               std::string    const & operation_name
                               , unsigned int const   label
                               , double       const   value
                               )
      {
        std::vector< double> & values = m_angle_threshold[operation_name];

        if( label >= values.size())
        {
          values.resize(label+1u);
        }

        values[label] = value;
      }

    public:

      void set_area_threshold(
                               std::string    const & operation_name
                               , unsigned int const   label
                               , double       const   value
                               )
      {
        std::vector< double> & values = m_area_threshold[operation_name];

        if( label >= values.size())
        {
          values.resize(label+1u);
        }

        values[label] = value;
      }

    public:

      void set_lower_threshold(
                               std::string    const & name
                               , unsigned int const   label
                               , double       const   value
                               )
      {
        if (label >= m_lower_threshold[name].size())
        {
          m_lower_threshold[name].resize(label+1);
        }
        m_lower_threshold[name][label] = value;
      }

      void set_upper_threshold(
                               std::string    const & name
                               , unsigned int const   label
                               , double       const   value
                               )
      {
        if (label >= m_upper_threshold[name].size())
        {
          m_upper_threshold[name].resize(label+1);
        }
        m_upper_threshold[name][label] = value;
      }

      /**
       * Adds an input label to the set of all known input labels. An
       * input label is a label in a mesh input file. The input labels
       * maps to "GRIT" labels who defines the phases in the mesh.
       *
       * It is assumed that input labels and (GRIT) labels (just called
       * labels) are stored in sequential order in such a way that their
       * index defines a bi-jective mapping. That is
       *
       * m_input_labels[i] <-> m_labels[i] for all i.
       *
       * Presently GRIT contains not logic to guard or test for whether
       * labels are specified correctly. This is left for the
       * programming to make sure.
       */
      void add_input_label( unsigned int const label )
      {
        m_input_labels.push_back(label);
      }

      /**
       * Add a "GRIT" label.
       *
       * @see method add_input_label for details.
       */
      void add_label( unsigned int const label )
      {
        m_labels.push_back(label);
      }

    public:

      void add_operation_name( std::string const & operation_name)
      {
        m_operation_names.push_back( operation_name);
      }

    public:

      bool has_lower_threshold_attribute( std::string const & operation_name) const
      {
        if( m_lower_threshold_attribute.find(operation_name) != m_lower_threshold_attribute.end())
          return true;

        return false;
      }

      std::string const & get_lower_threshold_attribute( std::string const & operation_name) const
      {
        return m_lower_threshold_attribute.at(operation_name);
      }

      void set_lower_threshold_attribute(
                                         std::string  const  & operation_name
                                         , std::string const & attribute_name
                                         )
      {
        m_lower_threshold_attribute[operation_name] = attribute_name;
      }

    public:

      bool has_upper_threshold_attribute( std::string const & operation_name) const
      {
        if( m_upper_threshold_attribute.find(operation_name) != m_upper_threshold_attribute.end())
          return true;

        return false;
      }

      std::string const & get_upper_threshold_attribute( std::string const & operation_name) const
      {
        return m_upper_threshold_attribute.at(operation_name);
      }

      void set_upper_threshold_attribute(
                                         std::string  const  & operation_name
                                         , std::string const & attribute_name
                                         )
      {
        m_upper_threshold_attribute[operation_name] = attribute_name;
      }

    }; // class Parameters

  } // namespace details

  /**
   * Performs validity checks on parameters in param and returns true if valid.
   */
  inline bool is_valid( details::Parameters const & param )
  {
    util::Log logging;

    if (param.input_labels().size() != param.labels().size())
    {
      logging << "Parameters::is_valid() Warning! input_labels and labels lists of different lengths." << util::Log::newline();
      return false;
    }

    if (param.use_ambient() && param.get_max_iterations( "move", param.ambient_label()) > 0u)
    {
      logging << "Parameters::is_valid() Warning! ambient phase cannot be moved, set move max_iterations to 0u in cfg-file." << util::Log::newline();
      return false;
    }

    if (param.multiple_vertices_threshold() < 0.0) // TODO: change to VT::zero() after re-plugging the type binder
    {
      logging << "Parameters::is_valid() Warning! multiple vertices threshold must be non-negative." << util::Log::newline();
      return false;
    }

    std::vector< unsigned int> const & labels          = param.labels();

    std::vector< std::string>  const & operation_names = param.operation_names();
    std::vector< std::string>::const_iterator it       = operation_names.begin();

    // 2016-04-14 Marek: this is wrong, this should compare thresholds for two different operations.
    for( ; it != operation_names.end(); ++it)
    {
      std::string const & operation_name = *it;

      std::vector< unsigned int>::const_iterator l = labels.begin();
      for( ; l != labels.end(); ++l)
      {
        unsigned int const label           = *l;
        double       const lower_threshold = param.get_lower_threshold( operation_name, label);
        double       const upper_threshold = param.get_upper_threshold( operation_name, label);

        if( lower_threshold > 0.0 && upper_threshold > 0.0 && 2.0*lower_threshold >= upper_threshold)
        {
          //logging << "Parameters::is_valid() Warning! upper_threshold for operation " << operation_name <<
          //           ", label " << label << " should be at least two times higher than the lower_threshold" << util::Log::newline();
        }
      }
    }

    // 2016-04-11 Marek: Add checks for refinement/coarsening, interface vs. phase
    //                   (interface has to at least as fine as surrounding mesh)

    // 2016-01-27 Kenny code review: We should verify if upper/lower threshold
    // attribute names are assigned meaningful to operations... if a attribute
    // field is shared by more than one operation then we should write out
    // a warning to the end-user.


    return true;
  }

  /**
   * Returns a Parameters object initialized with values for a ConfigFile cfg.
   */
  inline details::Parameters make_parameters_from_config_file(util::ConfigFile const & cfg)
  {
    details::Parameters parameters;

    parameters.number_of_subdomains()  = util::to_value< unsigned int>( cfg.get_value( "number_of_subdomains",  "1"      ));
    parameters.debug_render_level()    = util::to_value< unsigned int>( cfg.get_value( "debug_render_level",    "0"      ));
    parameters.make_unit()             = util::to_value< bool>(         cfg.get_value( "make_unit",             "true"   ));
    parameters.profiling()             = util::to_value< bool>(         cfg.get_value( "profiling",             "false"  ));
    parameters.verbose()               = util::to_value< bool>(         cfg.get_value( "verbose",               "true"   ));
    parameters.debug_render()          = util::to_value< bool>(         cfg.get_value( "debug_render",          "false"  ));
    parameters.silent_threads()        = util::to_value< bool>(         cfg.get_value( "silent_threads",        "true"   ));

    parameters.debug_render_filename() = cfg.get_value( "debug_render_filename", "");

    parameters.set_max_iterations( "scheduler",  0, util::to_value< unsigned int>( cfg.get_value( "scheduler.max_iterations",   "4" ) ));
    parameters.set_max_iterations( "algorithm",  0, util::to_value< unsigned int>( cfg.get_value( "algorithm.max_iterations",   "10") ));

    std::vector<std::string> const & input_labels = cfg.get_values( "input_labels");

    for (unsigned int i = 0; i < input_labels.size(); ++i)
      parameters.add_input_label(util::to_value< unsigned int>(input_labels[i]));

    std::vector< std::string> const & labels = cfg.get_values( "labels" );

    for( unsigned int i = 0; i < labels.size(); ++i)
      parameters.add_label(  util::to_value< unsigned int>( labels[i])  );

    parameters.use_ambient()                 = util::to_value< bool>(          cfg.get_value( "use_ambient",                 "true")  );
    parameters.ambient_label()               = util::to_value< unsigned int>(  cfg.get_value( "ambient_label",               "0")     );
    parameters.use_one_phase_style()         = util::to_value< bool>(          cfg.get_value( "use_one_phase_style",         "false") );
    parameters.use_only_interface()          = util::to_value< bool>(          cfg.get_value( "use_only_interface",          "true")  );
    parameters.use_manifold()                = util::to_value< bool>(          cfg.get_value( "use_manifold",                "true")  );
    parameters.no_ears()                     = util::to_value< bool>(          cfg.get_value( "no_ears",                     "false") );
    parameters.no_multiple_vertices()        = util::to_value< bool>(          cfg.get_value( "no_multiple_vertices",        "true")  );
    parameters.multiple_vertices_threshold() = util::to_value< double>(        cfg.get_value( "multiple_vertices_threshold", "0.001") );
    parameters.use_simple_boundary()         = util::to_value< bool>(          cfg.get_value( "use_simple_boundary",         "true")  );
    parameters.use_sparse_edge_attributes()  = util::to_value< bool>(          cfg.get_value( "use_sparse_edge_attributes",  "false") );

    std::vector< std::string> const & operations = cfg.get_values( "operations");

    //--- Setting the remeshing parameters
    for( unsigned int i = 0; i < parameters.labels().size(); ++i)
    {
      for ( std::vector< std::string>::const_iterator it = operations.begin(); it != operations.end(); ++it)
      {
        std::string const & operation_name = *it;

        parameters.add_operation_name( operation_name);

        if (parameters.verbose())
        {
          util::Log logging;

          logging << "make_parameters_from_config_file() : settings for operation "
                  << operation_name
                  << "  for phase "
                  << labels[i]
                  << util::Log::newline();
        }

        parameters.set_max_iterations(
                              operation_name
                              , parameters.labels()[i]
                              , util::to_value< unsigned int>( cfg.get_value(operation_name, labels[i], "max_iterations", "0"))
                              );

        parameters.set_strength(
                                operation_name
                                , parameters.labels()[i]
                                , util::to_value< double>( cfg.get_value( operation_name, labels[i], "strength", "0.0"))
                                );

        parameters.set_distance_threshold(
                              operation_name
                              , parameters.labels()[i]
                              , util::to_value< double>( cfg.get_value( operation_name, labels[i], "distance_threshold", "0.0"))
                              );

        parameters.set_angle_threshold(
                              operation_name
                              , parameters.labels()[i]
                              , util::to_value< double>( cfg.get_value( operation_name, labels[i], "angle_threshold", "180.0"))
                              );

        parameters.set_area_threshold(
                              operation_name
                              , parameters.labels()[i]
                              , util::to_value< double>( cfg.get_value( operation_name, labels[i], "area_threshold", "0.0"))
                              );

        parameters.set_lower_threshold(
                              operation_name
                              , parameters.labels()[i]
                              , util::to_value< double>( cfg.get_value( operation_name, labels[i], "lower_threshold", "0.0"))
                              );

        parameters.set_upper_threshold(
                              operation_name
                              , parameters.labels()[i]
                              , util::to_value< double>( cfg.get_value( operation_name, labels[i], "upper_threshold", "0.0"))
                              );
      }

    }

    return parameters;
  }

  /// Type added for consistency with naming used in namespace grit.
  typedef details::Parameters         param_type;

} // namespace grit

// GRIT_PARAMETERS_H
#endif
