#ifndef LIQUID_PARAMETERS_H
#define LIQUID_PARAMETERS_H

#include <util_config_file.h>
#include <util_string_helper.h>
#include <util_is_finite.h>
#include <util_is_number.h>

#include <cassert>

namespace liquid
{

  class Parameters
  {
  protected:

    double m_dt;
    double m_g;
    double m_rho;
    double m_gamma;
    double m_mu;
    double m_epsilon;

    bool m_use_gravity;
    bool m_use_pressure;
    bool m_use_viscosity;
    bool m_use_surface_tension;
    bool m_use_stabilization;
    bool m_use_profiling;

    unsigned int m_solver_choice;
    unsigned int m_precond_choice;

    bool m_use_host;

    unsigned int m_max_iterations;

    double m_absolute_tolereance;
    double m_relative_tolerance;

    bool m_matlab_write;
    bool m_matlab_append;

    std::string m_matlab_file;
    std::string m_output_path;

  public:

    double const & dt()      const { return m_dt;     }
    double const & g()       const { return m_g;      }
    double const & rho()     const { return m_rho;    }
    double const & gamma()   const { return m_gamma;  }
    double const & mu()      const { return m_mu;     }
    double const & epsilon() const { return m_epsilon;}

    bool const & use_gravity()         const { return m_use_gravity;        }
    bool const & use_pressure()        const { return m_use_pressure;       }
    bool const & use_viscosity()       const { return m_use_viscosity;      }
    bool const & use_surface_tension() const { return m_use_surface_tension;}
    bool const & use_stabilization()   const { return m_use_stabilization;  }
    bool const & use_profiling()       const { return m_use_profiling;      }
    bool const & use_host()            const { return m_use_host;           }

    unsigned int const & solver_choice()  const { return m_solver_choice; }
    unsigned int const & precond_choice() const { return m_precond_choice;}

    unsigned int const & max_iterations() const { return m_max_iterations;}

    double const & absolute_tolerance() const { return m_absolute_tolereance;}
    double const & relative_tolerance() const { return m_relative_tolerance;}

    bool const & matlab_write()  const { return m_matlab_write; }
    bool const & matlab_append() const { return m_matlab_append;}

    std::string const & matlab_filename() const { return m_matlab_file; }
    std::string const & output_path()     const { return m_output_path; }

  public:

    double & dt()       { return m_dt;     }
    double & g()        { return m_g;      }
    double & rho()      { return m_rho;    }
    double & gamma()    { return m_gamma;  }
    double & mu()       { return m_mu;     }
    double & epsilon()  { return m_epsilon;}

    bool & use_gravity()          { return m_use_gravity;        }
    bool & use_pressure()         { return m_use_pressure;       }
    bool & use_viscosity()        { return m_use_viscosity;      }
    bool & use_surface_tension()  { return m_use_surface_tension;}
    bool & use_stabilization()    { return m_use_stabilization;  }
    bool & use_profiling()        { return m_use_profiling;      }
    bool & use_host()             { return m_use_host;           }

    unsigned int & solver_choice()   { return m_solver_choice; }
    unsigned int & precond_choice()  { return m_precond_choice;}

    unsigned int & max_iterations()  { return m_max_iterations;}

    double & absolute_tolerance()  { return m_absolute_tolereance;}
    double & relative_tolerance()  { return m_relative_tolerance;}

    bool & matlab_write()   { return m_matlab_write; }
    bool & matlab_append()  { return m_matlab_append;}

    std::string & matlab_filename()  { return m_matlab_file; }
    std::string & output_path()      { return m_output_path; }

  public:

    Parameters()
    : m_dt(0.001)
    , m_g(9.81)
    , m_rho(0.1)
    , m_gamma(0.01)
    , m_mu(0.01)
    , m_epsilon(0.001)
    , m_use_gravity(true)
    , m_use_pressure(true)
    , m_use_viscosity(true)
    , m_use_surface_tension(true)
    , m_use_stabilization(true)
    , m_use_profiling(true)
    , m_solver_choice(1u)
    , m_precond_choice(0u)
    , m_use_host("false")
    , m_max_iterations(100u)
    , m_absolute_tolereance(0.000001)
    , m_relative_tolerance(0.0001)
    , m_matlab_write("true")
    , m_matlab_append("false")
    , m_matlab_file("liquid.m")
    , m_output_path("")
    {}

  };


  inline Parameters make_parameters_from_config_file(util::ConfigFile const & cfg)
  {
    Parameters parameters;

    parameters.dt()                  = util::to_value<double>(cfg.get_value("dt", "0.001") );            // timestep size
    parameters.g()                   = util::to_value<double>(cfg.get_value("g",  "9.81") );             // acceleration of gravity
    parameters.rho()                 = util::to_value<double>(cfg.get_value("rho",      "0.1")  );       // mass density
    parameters.gamma()               = util::to_value<double>(cfg.get_value("gamma",    "0.01")  );      // surface tension coefficient
    parameters.mu()                  = util::to_value<double>(cfg.get_value("mu",       "0.01")  );      // viscosity coefficient
    parameters.epsilon()             = util::to_value<double>(cfg.get_value("epsilon",  "0.001")  );     // stabilization coefficient

    parameters.use_gravity()         = util::to_value<bool>(cfg.get_value("gravity",         "true")  );
    parameters.use_pressure()        = util::to_value<bool>(cfg.get_value("pressure",        "true")  );
    parameters.use_viscosity()       = util::to_value<bool>(cfg.get_value("viscosity",       "true")  );
    parameters.use_surface_tension() = util::to_value<bool>(cfg.get_value("surface_tension", "true")  );
    parameters.use_stabilization()   = util::to_value<bool>(cfg.get_value("stabilization",   "true")  );
    parameters.use_profiling()       = util::to_value<bool>(cfg.get_value("profiling",       "false"  ));

    parameters.solver_choice()       = util::to_value<unsigned int>(cfg.get_value("solver_choice", "1"));
    parameters.precond_choice()      = util::to_value<unsigned int>(cfg.get_value("precond_choice", "0"));

    parameters.use_host()            = util::to_value<bool>(cfg.get_value("solver_use_host", "false"));
    parameters.max_iterations()      = util::to_value<unsigned int>(cfg.get_value("solver_max_iterations", "100"));
    parameters.relative_tolerance()  = util::to_value<double>(cfg.get_value("solver_relative_tolerance", "0.0001"));
    parameters.absolute_tolerance()  = util::to_value<double>(cfg.get_value("solver_absolute_tolerance", "0.0000001"));

    parameters.matlab_write()        = util::to_value<bool>(cfg.get_value("matlab_write", "false"));
    parameters.matlab_append()       = util::to_value<bool>(cfg.get_value("matlab_append", "false"));
    parameters.matlab_filename()     = cfg.get_value("matlab_filename","liquid.m");
    
    parameters.output_path()         = cfg.get_value("output_path","");
    
    return parameters;
  }

  inline bool is_valid( Parameters const & params)
  {
    assert(is_finite(params.dt())      || !"is_valid() not finite number");
    assert(is_number(params.dt())      || !"is_valid() not a number"     );
    assert(params.dt() > 0.0           || !"is_valid() non positive"     );

    assert(is_finite(params.g())       || !"is_valid() not finite number");
    assert(is_number(params.g())       || !"is_valid() not a number"     );
    assert(params.g() >= 0.0           || !"is_valid() negative"         );

    assert(is_finite(params.rho())     || !"is_valid() not finite number");
    assert(is_number(params.rho())     || !"is_valid() not a number"     );
    assert(params.rho() > 0.0          || !"is_valid() non positive"     );

    assert(is_finite(params.gamma())   || !"is_valid() not finite number");
    assert(is_number(params.gamma())   || !"is_valid() not a number"     );
    assert(params.gamma() >= 0.0       || !"is_valid() negative"         );

    assert(is_finite(params.mu())      || !"is_valid() not finite number");
    assert(is_number(params.mu())      || !"is_valid() not a number"     );
    assert(params.mu() >= 0.0          || !"is_valid() negative"         );

    assert(is_finite(params.epsilon()) || !"is_valid() not finite number");
    assert(is_number(params.epsilon()) || !"is_valid() not a number"     );
    assert(params.epsilon() > 0.0      || !"is_valid() non positive"     );

    assert(is_finite(params.relative_tolerance()) || !"is_valid() not finite number");
    assert(is_number(params.relative_tolerance()) || !"is_valid() not a number"     );
    assert(params.relative_tolerance() >= 0.0     || !"is_valid() negative"         );

    assert(is_finite(params.absolute_tolerance()) || !"is_valid() not finite number");
    assert(is_number(params.absolute_tolerance()) || !"is_valid() not a number"     );
    assert(params.absolute_tolerance() >= 0.0     || !"is_valid() negative"         );

    return true;
  }

}// end of namespace liquid

// LIQUID_PARAMETERS_H
#endif
