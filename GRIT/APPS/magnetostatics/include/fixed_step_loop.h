#ifndef FIXED_STEP_LOOP_H
#define FIXED_STEP_LOOP_H

#include <simulation.h>
#include <write_profiling.h>
#include <write_svg_files.h>

#include <grit.h>

inline void fixed_step_loop(
                              grit::engine2d_type       & engine
                            , grit::param_type    const & parameters
                            , util::ConfigFile     const & settings
                            )
{
  unsigned int const frames        = util::to_value<unsigned int>( settings.get_value("frames",      "10"   )  );
  unsigned int const skip_frames   = util::to_value<unsigned int>( settings.get_value("skip_frames", "1!"   )  );
  bool        const  profiling     = util::to_value<bool>(         settings.get_value("profiling",   "false")  );

  setup_simulation_fields( engine );

  for (size_t i = 0; i <= frames; ++i)
  {
    if( (i%skip_frames)==0)
      write_svg_files(i, engine, parameters, settings);

    do_simulation_step(engine, settings);

    engine.update(parameters);

    //if( (i%skip_frames)==0)
    //  write_svg_files(i, engine, parameters, settings);
  }

  if(profiling)
    write_profiling(settings);
}

// FIXED_STEP_LOOP_H
#endif
