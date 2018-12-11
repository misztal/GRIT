#ifndef FIXED_STEP_LOOP_H
#define FIXED_STEP_LOOP_H

#include <simulation.h>
#include <write_svg_files.h>

#include <grit.h>
#include <util.h>

inline void fixed_step_loop(
                              grit::engine2d_type        & engine
                            , grit::param_type           & parameters
                            , util::ConfigFile     const & settings
                            )
{
  util::Log log;

  std::string  const & newline     = util::Log::newline();
  std::string  const & tab         = util::Log::tab();
  std::string  const   output_path = settings.get_value( "output_path", "" );
  unsigned int const   max_steps   = util::to_value<unsigned int>(settings.get_value("steps","100"));

  init_simulation( engine, parameters, settings );

  log << "Simulation initialized" << newline;

  write_svg_files( engine, parameters, settings, output_path, 0 );

  log << tab << "Wrote svg file for frame " << 0 << newline;

  for (unsigned int i = 1; i <= max_steps; ++i)
  {
    log << tab << "Simulating..." << newline;

    do_simulation_step( engine, parameters, settings );
    log << tab << "Simulation step " << i << " done..." << newline;

    engine.update( parameters );
    log << tab << "Updated the mesh" << newline;

    write_svg_files( engine, parameters, settings, output_path, i );
    log << tab << "Wrote svg file for frame " << i << newline;
  }

  log << "Done" << newline;

}

// FIXED_STEP_LOOP_H
#endif
