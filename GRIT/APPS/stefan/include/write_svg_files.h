#ifndef WRITE_SVG_FILES_H
#define WRITE_SVG_FILES_H

#include <grit.h>
#include <glue.h>
#include <util.h>


void convert_temperature_to_rgb(
                                    double const   temperature
                                  , double const   t_min
                                  , double const   t_max
                                  , double       & r
                                  , double       & g
                                  , double       & b
                                  )
{
  //assert( ((temperature>=t_min) && (temperature<=t_max)) || !"convert_temperature_to_rgb() Error! Argument out of bounds." );

  double const t_avg = 0.5*(t_min+t_max);

  if( temperature<t_avg )
  {
    double const t = (temperature-t_min)/(t_avg-t_min);

    r = std::min( 1.0, std::max( 0., .192 + t*.808));
    g = std::min( 1.0, std::max( 0., .549 + t*.451));
    b = std::min( 1.0, std::max( 0., .906 + t*.094));
  }
  else
  {
    double const t = (t_max-temperature)/(t_max-t_avg);

    r = std::min( 1.0, std::max( 0., .768 + t*.232));
    g = std::min( 1.0, std::max( 0., .118 + t*.882));
    b = std::min( 1.0, std::max( 0., .227 + t*.773));
  }
}


inline void draw_triangles(
                           util::SVGFile                & file
                           , grit::engine2d_type        & engine
                           , grit::SimplexSet     const & S
                           , util::SVGStyle       const & style
                           , double               const   t_min
                           , double               const   t_max
                           , std::string          const & attribute_name = "temperature"
                           )
{
  typedef grit::engine2d_type::V    V;

  grit::SimplexSet::simplex2_const_iterator it = S.begin2();

  for( ; it != S.end2(); ++it)
  {
    grit::Simplex2 const & t = *it;

    grit::Simplex0 const   i(t.get_idx0());
    grit::Simplex0 const   j(t.get_idx1());
    grit::Simplex0 const   k(t.get_idx2());

    V const & pos0 = engine.attributes().get_current_value( i );
    V const & pos1 = engine.attributes().get_current_value( j );
    V const & pos2 = engine.attributes().get_current_value( k );

    unsigned int const label = engine.mesh().label( t );

    double const t0 = engine.attributes().get_attribute_value( attribute_name, i, label );
    double const t1 = engine.attributes().get_attribute_value( attribute_name, j, label );
    double const t2 = engine.attributes().get_attribute_value( attribute_name, k, label );

    double const t_avg = (t0+t1+t2)/3.;

    double r, g, b;

    convert_temperature_to_rgb( t_avg, t_min, t_max, r, g, b );

    double const & x0 = pos0[0u];
    double const & y0 = pos0[1u];

    double const & x1 = pos1[0u];
    double const & y1 = pos1[1u];

    double const & x2 = pos2[0u];
    double const & y2 = pos2[1u];

    util::SVGStyle custom_style;

    custom_style.set_color( r, g ,b );

    file.set_style(custom_style);

    file.draw_triangle( x0, y0, x1, y1, x2, y2);
  }
}


void write_temperature_plot(
                              grit::engine2d_type       & engine
                            , grit::param_type    const & parameters
                            , std::string         const & output_path
                            , unsigned int        const & frame_number
                            , double              const & t_min
                            , double              const & t_max
                            )
{
  util::Log log;

  std::string const & newline  = util::Log::newline();
  std::string const & tab      = util::Log::tab();
  std::string const   filename = output_path + util::generate_filename("/temperature", frame_number, "svg");
  std::string const   coords("current");

  util::SVGFile file;

  grit::SimplexSet const all = engine.mesh().get_all_simplices();

  grit::details::setup_svg_file(filename, all, engine.attributes(), file);

  grit::details::DrawStyle const style = grit::details::make_one_phase_style();

  //--- Custom function
  draw_triangles(  file, engine, all, style.m_triangle, t_min, t_max );

  grit::details::draw_edges(      file, all, engine.attributes(), coords, coords, 0u, style.m_edge     );
  grit::details::draw_vertices(   file, all, engine.attributes(), coords, coords, 0u, style.m_vertex   );

  {
    grit::SimplexSet interface_ = filter( all, grit::IsInterface(engine.mesh()) && !grit::IsDimension(engine.mesh(), 2u));

    grit::details::draw_edges(    file, interface_, engine.attributes(), coords, coords, 0u, style.m_edge_interface   );
    grit::details::draw_vertices( file, interface_, engine.attributes(), coords, coords, 0u, style.m_vertex_interface );
  }

  {
    grit::SimplexSet boundary  = filter( all, grit::IsBoundary(engine.mesh())  && !grit::IsDimension(engine.mesh(), 2u));

    grit::details::draw_edges(    file, boundary,  engine.attributes(), coords, coords, 0u, style.m_edge_boundary   );
    grit::details::draw_vertices( file, boundary,  engine.attributes(), coords, coords, 0u, style.m_vertex_boundary );
  }


  log << tab << tab << "write_temperature_plot() Done writing  " << filename << newline;
}


void write_svg_files(
                       grit::engine2d_type       & engine
                     , grit::param_type    const & parameters
                     , util::ConfigFile    const & settings
                     , std::string         const & output_path
                     , unsigned int        const & frame_number
                     )
{
  util::Log log;

  static double const t_outer   = util::to_value<double>(settings.get_value("temperature_outer"      , "100.0"));
  static double const t_melting = util::to_value<double>(settings.get_value("temperature_melting"    ,"1000.0"));

  std::string const & newline  = util::Log::newline();
  std::string const & tab      = util::Log::tab();
  std::string const   filename = output_path + util::generate_filename("/stefan", frame_number, "svg");

  glue::svg_draw(filename, engine, parameters);

  write_temperature_plot( engine, parameters, output_path, frame_number, t_outer, t_melting );

  log << tab << tab << "write_svg_files() Done writting  " << filename << newline;
}


// WRITE_SVG_FILES_H
#endif
