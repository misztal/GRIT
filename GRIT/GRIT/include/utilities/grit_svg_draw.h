#ifndef GRIT_SVG_DRAW_H
#define GRIT_SVG_DRAW_H

#include <grit_interface_mesh.h>
#include <grit_parameters.h>
#include <grit_logic_expressions.h>

#include <util_svg_file.h>

#include <string>
#include <vector>

namespace grit
{
  namespace details
  {

    class ColorFactory
    {
    public:

      static float red(unsigned int const & idx)
      {
        unsigned int const N = 12u;
        static float const values[]   = {255.0, 205.0, 255.0, 255.0, 211.0, 170.0, 135.0,   0.0, 128.0, 170.0, 170.0, 221.0  };
        return values[idx%N]/255.0;
      }

      static float green(unsigned int const & idx)
      {
        unsigned int const N = 12u;
        static float const values[] = {170.0, 222.0, 230.0, 153.0,  95.0, 255.0, 222.0, 212.0, 229.0, 170.0, 135.0, 175.0  };
        return values[idx%N]/255.0;
      }

      static float blue(unsigned int const & idx)
      {
        unsigned int const N = 12u;
        static float const values[]  = {170.0, 135.0, 128.0,  85.0,  95.0, 204.0, 205.0, 170.0, 255.0, 255.0, 222.0, 233.0  };
        return values[idx%N]/255.0;
      }

    };

    class DrawStyle
    {
    public:

      util::SVGStyle m_vertex;
      util::SVGStyle m_edge;
      util::SVGStyle m_triangle;
      util::SVGStyle m_vertex_interface;
      util::SVGStyle m_edge_interface;
      util::SVGStyle m_vertex_boundary;
      util::SVGStyle m_edge_boundary;
    };

    inline DrawStyle make_default_style()
    {
      DrawStyle style;

      style.m_vertex.set_stroke_width(1.0);

      style.m_edge.set_stroke_width(0.5);
      style.m_triangle.set_stroke_width(0.5);
      style.m_vertex_interface.set_stroke_width(2.0);
      style.m_edge_interface.set_stroke_width(1.5);

      style.m_vertex_boundary.set_stroke_width(2.0);
      style.m_edge_boundary.set_stroke_width(1.5);

      style.m_vertex.set_color( 0.0, 0.0 , 0.0 );
      style.m_edge.set_color( 0.0, 0.0 , 0.0 );
      style.m_triangle.set_color( 0.95, 0.45 ,0.45 );
      style.m_vertex_interface.set_color( 0.1, 0.1 , 0.75 );
      style.m_edge_interface.set_color( 0.1, 0.1 , 0.75 );

      style.m_vertex_boundary.set_color( 0.45, 0.1 , 0.1 );
      style.m_edge_boundary.set_color( 0.45, 0.1 , 0.1 );

      return style;
    }

    inline DrawStyle make_ambient_style()
    {
      DrawStyle style = make_default_style();

      style.m_triangle.set_stroke( 1.0, 1.0 , 1.0 );
      style.m_triangle.set_fill( 1.0, 1.0 , 1.0 );

      return style;
    }

    inline DrawStyle make_style(unsigned int const & seed)
    {
      DrawStyle style = make_default_style();

      style.m_triangle.set_color(
                                 ColorFactory::red(seed)
                                 , ColorFactory::green(seed)
                                 , ColorFactory::blue(seed)
                                 );

      return style;
    }

    inline DrawStyle make_one_phase_style()
    {
      DrawStyle style = make_default_style();

      unsigned int const seed = 1;

      style.m_triangle.set_color(
                                 ColorFactory::red(seed)
                                 , ColorFactory::green(seed)
                                 , ColorFactory::blue(seed)
                                 );

      style.m_vertex_interface.set_stroke_width(0.5);
      style.m_edge_interface.set_stroke_width(0.5);

      style.m_vertex_interface.set_color( 0.0, 0.0 , 0.0 );
      style.m_edge_interface.set_color( 0.0, 0.0 , 0.0 );

      return style;
    }

    template< typename AT>
    inline typename AT::V get_coordinates(
                                          Simplex0       const & simplex
                                          , AT           const & attributes
                                          , std::string  const & attribute_name_x
                                          , std::string  const & attribute_name_y
                                          , unsigned int const   label
                                          )
    {
      typedef typename AT::V V;

      bool const x_not_current = attribute_name_x.compare("current") != 0;
      bool const y_not_current = attribute_name_y.compare("current") != 0;

      if( x_not_current && y_not_current)
      {
        V coords;
        try
        {
          coords[0u] = attributes.get_attribute_value( attribute_name_x, simplex, label);

          coords[1u] = attributes.get_attribute_value( attribute_name_y, simplex, label);
        }
        catch (std::logic_error) {};
        return coords;
      }
      else
      {
        return attributes.get_current_value(simplex);
      }
    }

    template<typename AT>
    inline void draw_triangles(
                               util::SVGFile          & file
                               , SimplexSet     const & S
                               , AT             const & attributes
                               , std::string    const & attribute_name_x
                               , std::string    const & attribute_name_y
                               , unsigned int   const   label
                               , util::SVGStyle const & style
                               )
    {
      typedef typename AT::V V;

      file.set_style(style);

      SimplexSet::simplex2_const_iterator it = S.begin2();

      for( ; it != S.end2(); ++it)
      {
        Simplex2 const & t = *it;

        Simplex0 const   i(t.get_idx0());
        Simplex0 const   j(t.get_idx1());
        Simplex0 const   k(t.get_idx2());

        V const pos0 = get_coordinates( i, attributes, attribute_name_x, attribute_name_y, label);
        V const pos1 = get_coordinates( j, attributes, attribute_name_x, attribute_name_y, label);
        V const pos2 = get_coordinates( k, attributes, attribute_name_x, attribute_name_y, label);

        double const & x0 = pos0[0u];
        double const & y0 = pos0[1u];

        double const & x1 = pos1[0u];
        double const & y1 = pos1[1u];

        double const & x2 = pos2[0u];
        double const & y2 = pos2[1u];

        file.draw_triangle( x0, y0, x1, y1, x2, y2);
      }
    }

    template<typename AT>
    inline void draw_edges(
                           util::SVGFile          & file
                           , SimplexSet     const & S
                           , AT             const & attributes
                           , std::string    const & attribute_name_x
                           , std::string    const & attribute_name_y
                           , unsigned int   const   label
                           , util::SVGStyle const & style
                           )
    {
      typedef typename AT::V V;

      file.set_style(style);

      SimplexSet::simplex1_const_iterator it = S.begin1();

      for( ; it != S.end1(); ++it)
      {
        Simplex1 const & e = *it;

        Simplex0 const   i(e.get_idx0());
        Simplex0 const   j(e.get_idx1());

        V const pos0 = get_coordinates( i, attributes, attribute_name_x, attribute_name_y, label);
        V const pos1 = get_coordinates( j, attributes, attribute_name_x, attribute_name_y, label);

        double const & x0 = pos0[0u];
        double const & y0 = pos0[1u];

        double const & x1 = pos1[0u];
        double const & y1 = pos1[1u];

        file.draw_line( x0, y0, x1, y1);
      }
    }

    template<typename AT>
    inline void draw_vertices(
                              util::SVGFile          & file
                              , SimplexSet     const & S
                              , AT             const & attributes
                              , std::string    const & attribute_name_x
                              , std::string    const & attribute_name_y
                              , unsigned int   const   label
                              , util::SVGStyle const & style
                              )
    {
      typedef typename AT::V V;

      file.set_style(style);

      double const r = file.get_style().m_stroke_width /file.scale();

      SimplexSet::simplex0_const_iterator it = S.begin0();

      for( ; it != S.end0(); ++it)
      {
        V const pos = get_coordinates( *it, attributes, attribute_name_x, attribute_name_y, label);

        double const & x0 = pos[0u];
        double const & y0 = pos[1u];

        file.draw_circle(x0, y0, r);
      }
    }

    template<typename AT>
    inline void setup_svg_file(
                               std::string    const & filename
                               , SimplexSet   const & S
                               , AT           const & attributes
                               , util::SVGFile      & file
                               )
    {
      typedef typename AT::V V;

      SimplexSet::simplex0_const_iterator it = S.begin0();

      //--- Size dictated by current, as in glue_svg_draw.h
      V const & pos = attributes.get_current_value( *it);

      double min_x  = pos[0u];
      double max_x  = pos[0u];
      double min_y  = pos[1u];
      double max_y  = pos[1u];

      for( ; it != S.end0(); ++it)
      {
        V const & pos = attributes.get_current_value( *it);

        min_x  = min_x < pos[0u] ? min_x : pos[0u];
        max_x  = max_x > pos[0u] ? max_x : pos[0u];
        min_y  = min_y < pos[1u] ? min_y : pos[1u];
        max_y  = max_y > pos[1u] ? max_y : pos[1u];
      }

      double const desired_width = 800;
      double const scale         = desired_width / (max_x - min_x);

      file.open( filename, min_x, max_x, min_y, max_y, scale );
    }

  } // end of namespace details


  template<typename AT>
  inline void svg_draw(
                       std::string     const & filename
                       , SimplexSet    const & S
                       , InterfaceMesh const & mesh
                       , details::Parameters const & parameters
                       , AT            const & attributes
                       , std::string   const & attribute_name_x = "current"
                       , std::string   const & attribute_name_y = "current"
                       )
  {
    util::SVGFile file;

    bool const x_not_current = attribute_name_x.compare("current") != 0;
    bool const y_not_current = attribute_name_y.compare("current") != 0;

    std::vector<unsigned int> const labels = parameters.labels();

    setup_svg_file(filename, S, attributes, file);

    for(unsigned int i=0u; i < labels.size(); ++i)
    {
      unsigned int          const & label               = labels[i];
      bool                  const   use_ambient_style   = (parameters.use_ambient() && label == parameters.ambient_label() );
      SimplexSet            const   part                = filter(S, InPhase( mesh, label));
      details::DrawStyle    const   style               = use_ambient_style ? details::make_ambient_style() :
                                                          (parameters.use_one_phase_style() ? details::make_one_phase_style() : details::make_style(i));

      if( !(use_ambient_style && x_not_current && y_not_current))
      {
        details::draw_triangles(  file, part, attributes, attribute_name_x, attribute_name_y, label, style.m_triangle );
        details::draw_edges(      file, part, attributes, attribute_name_x, attribute_name_y, label, style.m_edge     );
        details::draw_vertices(   file, part, attributes, attribute_name_x, attribute_name_y, label, style.m_vertex   );

        {
          SimplexSet interface = filter( part, IsInterface(mesh) && !IsDimension(mesh, 2u));

          details::draw_edges(      file, interface, attributes, attribute_name_x, attribute_name_y, label, style.m_edge_interface     );
          details::draw_vertices(   file, interface, attributes, attribute_name_x, attribute_name_y, label, style.m_vertex_interface   );
        }

        {
          SimplexSet boundary = filter( part, IsBoundary(mesh) && !IsDimension(mesh, 2u));

          details::draw_edges(      file, boundary, attributes, attribute_name_x, attribute_name_y, label, style.m_edge_boundary     );
          details::draw_vertices(   file, boundary, attributes, attribute_name_x, attribute_name_y, label, style.m_vertex_boundary   );
        }
      }

    }
  }

}//end namespace glue

// GRIT_SVG_DRAW_H
#endif
