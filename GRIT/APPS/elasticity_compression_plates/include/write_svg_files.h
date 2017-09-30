#ifndef WRITE_SVG_FILES_H
#define WRITE_SVG_FILES_H

#include <grit.h>
#include <util.h>
#include <glue.h>

inline void write_svg_files(
                     int const & frame_number
                     , grit::engine2d_type       & engine
                     , grit::param_type    const & parameters
                     , util::ConfigFile     const & settings
                     )
{
  util::Log log;

  std::string  const output_path  = settings.get_value("output_path","");

  std::string const filename_spatial = output_path + util::generate_filename("/elasticity_compression_plates_spatial", frame_number, "svg");

  glue::svg_draw(filename_spatial, engine, parameters);

  log << "write_svg_files(): wrote " << filename_spatial << util::Log::newline();

  std::string const filename_material = output_path + util::generate_filename("/elasticity_compression_plates_material", frame_number, "svg");

  glue::svg_draw(filename_material, engine, parameters, "px0", "py0");
  log << "write_svg_files(): wrote " << filename_material << util::Log::newline();

}

// WRITE_SVG_FILES_H
#endif
