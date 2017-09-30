#ifndef FPS_LOOP_H
#define FPS_LOOP_H

#include <simulation.h>
#include <write_profiling.h>
#include <write_svg_files.h>

#include <grit.h>


inline void fps_loop(
                     grit::engine2d_type       &  engine
                     , grit::param_type    const & parameters
                     , util::ConfigFile     const & settings
)
{
  using std::min;
  using std::max;

  double             T            = util::to_value<double>(       settings.get_value("total_time",   "1.0"   )  );
  double       const fps          = util::to_value<double>(       settings.get_value("fps" ,         "25"    )  );
  bool         const profiling    = util::to_value<bool>(         settings.get_value("profiling",    "false" )  );
  double       const dt           = util::to_value<double>(       settings.get_value("dt",           "0.01"  )  );
  double       const dF           = 1.0/fps;
  int                frame_number = 0u;

  elasticity::Parameters controls = elasticity::make_parameters_from_config_file(settings);

  setup_simulation_fields( engine );

  engine.update(parameters);

  write_svg_files(frame_number++, engine, parameters, settings);

  while( T>0.0)
  {
    double const dT      = min(dF,T);
    double       dt_left = dT;

    while( dt_left >0.0)
    {
      controls.dt() = dt;
      
      do_simulation_step(engine, controls);

      engine.update(parameters);

      dt_left -= dt;
    }

    write_svg_files(frame_number++, engine, parameters, settings);

    T -= dT;
  }

  if(profiling)
    write_profiling(settings);
}





// FPS_LOOP_H
#endif
