#ifndef ELASTICITY_PARAMETERS_H
#define ELASTICITY_PARAMETERS_H

#include <util_config_file.h>
#include <util_log.h>
#include <util_string_helper.h>

#include <cassert>

namespace elasticity
{

  class Parameters
  {
  protected:

    double       m_E;                 ///< Young modulus
    double       m_nu;                ///< Poisson ratio
    double       m_rho0;              ///< Material space mass density
    double       m_dt;                ///< Time step size
    double       m_g;                 ///< Acceleration of gravity
    double       m_c;                 ///< Viscous mass damping
    bool         m_use_dirichlet;     ///< Toggles Dirichlet conditions on and off
    bool         m_use_traction;      ///< Toggles Traction forces on and off
    bool         m_use_gravity;       ///< Toggles gravity force on and off
    bool         m_use_damping;       ///< Toggles viscous mass-scale damping forces on and off
    bool         m_use_elasticity;    ///< Toggles hyper elastic forces on and off
    bool         m_write_data;        ///< Toggles whether simulation data is written to a matlab script file.
    bool         m_data_append;       ///< Toggles whether simulation data is appended to existing matlab script file.
    std::string  m_data_filename;     ///< The filename of associated matlab script file.
    std::string  m_data_output_path;  ///< The output path of the matlab script file(s).
    std::string  m_integrator;        ///< The time integrator to use
    double       m_min_dt;            ///< For controlling minimum available time step size in adaptive time stepping
    double       m_max_dt;            ///< For controlling maximum available time step size in adaptive time stepping
    double       m_halving_tolerance; ///< Tolerance for halving time step size
    unsigned int m_doubling_count;    ///< Number of time steps to run before trying to double time-step size

  public:

    double const & E()    const { return m_E;    }
    double       & E()          { return m_E;    }
    double const & nu()   const { return m_nu;   }
    double       & nu()         { return m_nu;   }
    double const & rho0() const { return m_rho0; }
    double       & rho0()       { return m_rho0; }
    double const & dt()   const { return m_dt;   }
    double       & dt()         { return m_dt;   }
    double const & g()    const { return m_g;    }
    double       & g()          { return m_g;    }
    double const & c()    const { return m_c;    }
    double       & c()          { return m_c;    }

    double const & min_dt()             const { return m_min_dt;            }
    double       & min_dt()                   { return m_min_dt;            }
    double const & max_dt()             const { return m_max_dt;            }
    double       & max_dt()                   { return m_max_dt;            }
    double const & halving_tolerance()  const { return m_halving_tolerance; }
    double       & halving_tolerance()        { return m_halving_tolerance; }

    unsigned int const & doubling_count()  const { return m_doubling_count; }
    unsigned int       & doubling_count()        { return m_doubling_count; }

    bool const & use_dirichlet()  const { return m_use_dirichlet;  }
    bool       & use_dirichlet()        { return m_use_dirichlet;  }
    bool const & use_traction()   const { return m_use_traction;   }
    bool       & use_traction()         { return m_use_traction;   }
    bool const & use_gravity()    const { return m_use_gravity;    }
    bool       & use_gravity()          { return m_use_gravity;    }
    bool const & use_damping()    const { return m_use_damping;    }
    bool       & use_damping()          { return m_use_damping;    }
    bool const & use_elasticity() const { return m_use_elasticity; }
    bool       & use_elasticity()       { return m_use_elasticity; }
    bool const & write_data()   const { return m_write_data;   }
    bool       & write_data()         { return m_write_data;   }
    bool const & data_append()  const { return m_data_append;  }
    bool       & data_append()        { return m_data_append;  }

    std::string const & data_filename() const { return m_data_filename; }
    std::string       & data_filename()       { return m_data_filename; }

    std::string const & data_output_path() const { return m_data_output_path; }
    std::string       & data_output_path()       { return m_data_output_path; }

    std::string const & integrator() const { return m_integrator; }
    std::string       & integrator()       { return m_integrator; }

  public:

    Parameters()
    : m_E(10000)
    , m_nu(0.4)
    , m_rho0(100)
    , m_dt(0.001)
    , m_g(9.81)
    , m_c(0.1)
    , m_use_dirichlet(true)
    , m_use_traction(true)
    , m_use_gravity(true)
    , m_use_damping(true)
    , m_use_elasticity(true)
    , m_write_data(false)
    , m_data_append(false)
    , m_data_filename("run.m")
    , m_data_output_path("")
    , m_integrator("semi_implicit")
    , m_min_dt(0.00001)
    , m_max_dt(0.001)
    , m_halving_tolerance(0.0001)
    , m_doubling_count(5u)
    {}

  public:

    friend std::ostream& operator<< (std::ostream& stream, Parameters const & parameters)
    {
      stream << "# Hyperelasticity parameters are " << std::endl;

      stream << "E    = " << parameters.E()    << std::endl;
      stream << "nu   = " << parameters.nu()   << std::endl;
      stream << "rho0 = " << parameters.rho0() << std::endl;
      stream << "dt   = " << parameters.dt()   << std::endl;
      stream << "g    = " << parameters.g()    << std::endl;
      stream << "c    = " << parameters.c()    << std::endl;

      stream << "use_dirichlet     = " << parameters.use_dirichlet()     << std::endl;
      stream << "use_traction      = " << parameters.use_traction()      << std::endl;
      stream << "use_gravity       = " << parameters.use_gravity()       << std::endl;
      stream << "use_damping       = " << parameters.use_damping()       << std::endl;
      stream << "use_elasticity    = " << parameters.use_elasticity()    << std::endl;
      stream << "write_data        = " << parameters.write_data()        << std::endl;
      stream << "data_append       = " << parameters.data_append()       << std::endl;
      stream << "data_filename     = " << parameters.data_filename()     << std::endl;
      stream << "data_output_path  = " << parameters.data_output_path()  << std::endl;
      stream << "integrator        = " << parameters.integrator()        << std::endl;
      stream << "min_dt            = " << parameters.min_dt()            << std::endl;
      stream << "max_dt            = " << parameters.max_dt()            << std::endl;
      stream << "halving_tolerance = " << parameters.halving_tolerance() << std::endl;
      stream << "doubling_count    = " << parameters.doubling_count()    << std::endl;

      return stream;
    }
  };

  inline Parameters make_default_parameters()
  {
    return Parameters();
  }

  inline Parameters make_parameters_from_config_file(util::ConfigFile const & settings)
  {
    util::Log log;

    std::string const newline = util::Log::newline();

    Parameters parameters;

    parameters.E()    =  util::to_value<double>( settings.get_value("E",    "10000" )  );
    parameters.nu()   =  util::to_value<double>( settings.get_value("nu",   "0.4"   )  );
    parameters.rho0() =  util::to_value<double>( settings.get_value("rho0", "100"   )  );
    parameters.g()    =  util::to_value<double>( settings.get_value("g",    "9.81"  )  );
    parameters.c()    =  util::to_value<double>( settings.get_value("c",    "0.1"   )  );
    parameters.dt()   =  util::to_value<double>( settings.get_value("dt",   "0.001" )  );

    if( parameters.E() <= 0.0 )
    {
      log << "$$$$$$$ BAD VALUE of E = "  << parameters.E() << newline;
    }
    if( parameters.nu() <= 0.0 )
    {
      log << "$$$$$$$ BAD VALUE of nu = "  << parameters.nu() << newline;
    }
    if( parameters.nu() > 0.5 )
    {
      log << "$$$$$$$ BAD VALUE of nu = "  << parameters.nu() << newline;
    }
    if( parameters.rho0() <= 0.0 )
    {
      log << "$$$$$$$ BAD VALUE of rho0 = "  << parameters.rho0() << newline;
    }
    if( parameters.g() < 0.0 )
    {
      log << "$$$$$$$ BAD VALUE of g = "  << parameters.g() << newline;
    }
    if( parameters.c() < 0.0 )
    {
      log << "$$$$$$$ BAD VALUE of c = "  << parameters.c() << newline;
    }
    if( parameters.dt() < 0.0 )
    {
      log << "$$$$$$$ BAD VALUE of dt = "  << parameters.dt() << newline;
    }
    if( parameters.E()*parameters.dt() > 1.0 )
    {
      log << "$$$$$$$ POSSIBLE INSTABILITY DETECTED, maybe lower dt = " << parameters.dt() << newline;
    }

    parameters.use_dirichlet()   = util::to_value<bool>( settings.get_value("use_dirichlet",  "true")  );
    parameters.use_traction()    = util::to_value<bool>( settings.get_value("use_traction",   "true")  );
    parameters.use_gravity()     = util::to_value<bool>( settings.get_value("use_gravity",    "true")  );
    parameters.use_damping()     = util::to_value<bool>( settings.get_value("use_damping",    "true")  );
    parameters.use_elasticity()  = util::to_value<bool>( settings.get_value("use_elasticity", "true")  );
    parameters.write_data()      = util::to_value<bool>( settings.get_value("write_data",     "false") );
    parameters.data_append()     = util::to_value<bool>( settings.get_value("data_append",    "false") );

    parameters.data_filename()     = settings.get_value("data_filename",    "data.m"         );
    parameters.data_output_path()  = settings.get_value("data_output_path", ""               );
    parameters.integrator()        = settings.get_value("integrator",       "semi_implicit"  );

    parameters.min_dt()            =  util::to_value<double>(       settings.get_value("min_time_step",               "0.00001" )   );
    parameters.max_dt()            =  util::to_value<double>(       settings.get_value("max_time_step",               "0.001"   )   );
    parameters.halving_tolerance() =  util::to_value<double>(       settings.get_value("time_step_halving_tolerance", "0.0001"  )   );
    parameters.doubling_count()    =  util::to_value<unsigned int>( settings.get_value("time_step_doubling_count",    "5"       )   );

    if( parameters.min_dt() < 0.0 )
    {
      log << "$$$$$$$ BAD VALUE of min_dt = "  << parameters.min_dt() << newline;
    }
    if( parameters.max_dt() < 0.0 )
    {
      log << "$$$$$$$ BAD VALUE of max_dt = "  << parameters.max_dt() << newline;
    }
    if( parameters.halving_tolerance() < 0.0 )
    {
      log << "$$$$$$$ BAD VALUE of halving_tolerance = "  << parameters.halving_tolerance() << newline;
    }
    if( parameters.doubling_count() == 0 )
    {
      log << "$$$$$$$ BAD VALUE of doubling_count= "  << parameters.doubling_count() << newline;
    }
    if( parameters.min_dt() > parameters.max_dt() )
    {
      log << "$$$$$$$ BAD VALUE of min_dt = "  << parameters.min_dt()
          << " was greather than max_dt = "    << parameters.max_dt()
          << newline;
    }

    log << parameters << newline;
    
    return parameters;
  }

  inline Parameters make_parameters(std::string const & filename)
  {
    Parameters parameters;

    util::ConfigFile settings;

    settings.load(filename);

    return make_parameters_from_config_file( settings );
  }

}// end of namespace elasticity

// ELASTICITY_PARAMETERS_H
#endif
