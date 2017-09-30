#ifndef UTIL_SVG_FILE_H
#define UTIL_SVG_FILE_H

#include <sstream>
#include <fstream>
#include <iostream>
#include <string>
#include <iomanip>

#include <cassert>

namespace util
{

  /**
   * Uitlity function that converts 0..1 range colors into hex colors and returns the corresponding hex string.
   */
  inline std::string make_hex_color(double const & red, double const & green, double const & blue)
  {
    assert( red   <= 1.0 || !"illegal red color value"  );
    assert( red   >= 0.0 || !"illegal red color value"  );
    assert( green <= 1.0 || !"illegal green color value");
    assert( green >= 0.0 || !"illegal green color value");
    assert( blue  <= 1.0 || !"illegal blue color value" );
    assert( blue  >= 0.0 || !"illegal blue color value" );

    unsigned int const int_red   =   0xFF*red;
    unsigned int const int_green =   0xFF*green;
    unsigned int const int_blue  =   0xFF*blue;

    std::ostringstream ss;
    ss << "#";
    ss.width(2);
    ss.fill('0');
    ss << std::hex << int_red;
    ss.width(2);
    ss.fill('0');
    ss << std::hex << int_green;
    ss.width(2);
    ss.fill('0');
    ss << std::hex << int_blue;
    return ss.str();
  }

  struct SVGStyle
  {
  public:

    std::string m_stroke;
    std::string m_fill;
    double      m_stroke_width;
    double      m_opacity;


  public:

    SVGStyle()
    : m_stroke("black")
    , m_fill("black")
    , m_stroke_width(0.5)
    , m_opacity(1.0)
    {
    }

    SVGStyle(std::string const & stroke, std::string const & fill)
    : m_stroke(stroke)
    , m_fill(fill)
    , m_stroke_width(0.5)
    , m_opacity(1.0)
    {
    }

  public:

    void set_color(
                   double const & red
                   , double const & green
                   , double const & blue
                   )
    {
      set_color( make_hex_color(red,green,blue) );
    }

    void set_color(std::string const & color)
    {
      m_fill = color;
      m_stroke = color;
    }

    void set_fill(
                  double const & red
                  , double const & green
                  , double const & blue
                  )
    {
      //std::ostringstream color;
      //color << "rgb(" << red << "," << green << "," << blue << ")";
      //set_fill(color.str());

      set_fill( make_hex_color(red,green,blue) );
    }

    void set_fill(std::string const & color)
    {
      m_fill = color;
    }

    void set_stroke(
                    double const & red
                    , double const & green
                    , double const & blue
                    )
    {
      //std::ostringstream color;
      //color << "rgb(" << red << "," << green << "," << blue << ")";
      //set_stroke(color.str());

      set_stroke( make_hex_color(red,green,blue) );
    }

    void set_stroke(std::string const & color)
    {
      m_stroke = color;
    }

    void set_stroke_width(double const & width)
    {
      m_stroke_width  = width;
    }

    void set_opacity(double const & alpha)
    {
      m_opacity  = alpha;
    }

  };

  /**
   * Have a look at http://www.svgbasics.com
   */
  class SVGFile
  {
  protected:

    std::ofstream         m_stream;

    double                m_min_x;
    double                m_max_x;
    double                m_min_y;
    double                m_max_y;

    double                m_scale;

    SVGStyle              m_style;
    unsigned int          m_arrow_count; ///< Used internally to generate unique IDs for arrow shapes

  public:

    SVGFile()
    : m_stream()
    , m_min_x(0.0)
    , m_max_x(0.0)
    , m_min_y(100.0)
    , m_max_y(100.0)
    , m_scale(1.0)
    , m_style()
    {
    }

    ~SVGFile()
    {
      if(m_stream.is_open() )
        this->close();
    }
  public:

    double width()  const { return m_max_x - m_min_x; }
    double height() const { return m_max_y - m_min_y; }
    double scale()  const { return m_scale;           }

  protected:

    double dx(double x)
    {
      return (x - m_min_x) * m_scale;
    }

    double dy(double y)
    {
      return (m_max_y - y) * m_scale;  // svg has inverted y axis
    }

    std::string make_style_attr(bool stroke = true)
    {
      std::ostringstream ss;
      ss << "style=\"";
      if (stroke)
      {
        ss
        //<< "stroke-linecap:round;" // Does not work for lines
        << "stroke-width:" << m_style.m_stroke_width << ";"
        << "stroke:" << m_style.m_stroke << ";";
      }
      ss << "fill:" << m_style.m_fill << ";";
      ss << "fill-opacity:" << m_style.m_opacity << ";\"";
      return ss.str();
    }

    void close()
    {
      assert(m_stream.is_open() || !"internal error");

      m_stream << "</svg>" << std::flush;
      m_stream.flush();
      m_stream.close();
    }

  public:

    void open(
              std::string const & filename
              , double const & min_x
              , double const & max_x
              , double const & min_y
              , double const & max_y
              , double const & scale = 1.0
              )
    {
      assert( max_x > min_x || !"max x must be larger than min x");
      assert( max_y > min_y || !"max y must be larger than min y");

      m_stream.open( filename.c_str() );

      assert(m_stream.is_open() || !"internal error");

      //--- Set bounding box information
      // y axis is inverted in svg, so translate to negative y (and then it will be reversed when drawing)
      this->m_min_x    = min_x;
      this->m_max_x    = max_x;
      this->m_min_y    = min_y;
      this->m_max_y    = max_y;

      this->m_scale = scale;
      this->m_arrow_count = 0;

      m_stream << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>" << std::endl;
      m_stream << "<svg xmlns=\"http://www.w3.org/2000/svg\" "
               << "width=\"" << m_scale*(max_x - min_x) << "\" "
               << "height=\"" << m_scale*(max_y - min_y) << "\" "
               << "version=\"1.1\"> "
               << std::endl;
    }

    void set_style( SVGStyle const & style )
    {
      m_style = style;
    }

    SVGStyle const & get_style() const
    {
      return m_style;
    }

    void draw_arrow2d(
                      double const & x1
                      , double const & y1
                      , double const & x2
                      , double const & y2
                      , double const xscale = 1.5
                      , double const yscale = 1.0
                      )
    {
      assert(m_stream.is_open() || !"internal error");

      double xmax = xscale*10.0;
      double ymax = yscale*10.0;
      double refX = xmax/10;
      double refY = ymax/2;

      m_stream << "<defs>"
               << "<marker id=\"my_arrow_" << this->m_arrow_count << "\" "
               << "viewBox=\"0 0 " << xmax << " " << ymax << "\" "
               << "refX=\"" << refX << "\" refY=\"" << refY << "\" "
               << "markerUnits=\"strokeWidth\" orient=\"auto\" "
               << "markerWidth=\"" << refX*3 << "\" "
               << "markerHeight=\"" << refX*3 << "\">"
               << "<polyline points=\"0,0 " << xmax << "," << refY << " 0," << ymax << " " << refX << ","<< refY << "\" "
               << "fill=\"" << this->m_style.m_fill << "\" />"
               << "</marker>"
               << "</defs>"
               << "<line "
               << "x1=\"" << dx(x1) << "\" "
               << "y1=\"" << dy(y1) << "\" "
               << "x2=\"" << dx(x2) << "\" "
               << "y2=\"" << dy(y2) << "\" "
               << "stroke=\"" << this->m_style.m_stroke << "\" "
               << "stroke-width=\"" << this->m_style.m_stroke_width << "\" "
               << "marker-end=\"url(#my_arrow_" << this->m_arrow_count << ")\" "
               << " />" << std::endl;

      this->m_arrow_count++;

    }

    void draw_line(
                   double const & x1
                   , double const & y1
                   , double const & x2
                   , double const & y2
                   )
    {
      assert(m_stream.is_open() || !"internal error");

      m_stream << "<line "
               << "x1=\"" << dx(x1) << "\" "
               << "y1=\"" << dy(y1) << "\" "
               << "x2=\"" << dx(x2) << "\" "
               << "y2=\"" << dy(y2) << "\" "
               << make_style_attr() << " />" << std::endl;
    }

    void draw_circle(
                     double const & x
                     , double const & y
                     , double const & r
                     )
    {
      assert(m_stream.is_open() || !"internal error");

      m_stream << "<circle "
               << "cx=\"" << dx(x) << "\" "
               << "cy=\"" << dy(y) << "\" "
               << "r=\"" << this->m_scale*r << "\" "
               << make_style_attr() << " />" << std::endl;
    }

    void draw_triangle(
                       double const & x1
                       , double const & y1
                       , double const & x2
                       , double const & y2
                       , double const & x3
                       , double const & y3
                       )
    {
      assert(m_stream.is_open() || !"internal error");

      m_stream << "<path "
               << "d=\"M"
               << dx(x1) << "," << dy(y1) << " "
               << dx(x2) << "," << dy(y2) << " "
               << dx(x3) << "," << dy(y3) << "z\" "
               << make_style_attr() << " />" << std::endl;
    }

    void draw_rect(
                   double const & x
                   , double const & y
                   , double const & w
                   , double const & h
                   )
    {
      assert(m_stream.is_open() || !"internal error");

      m_stream << "<rect "
               << "x=\"" << dx(x - (w/2.0)) << "\" "
               << "y=\"" << dy(y + (h/2.0)) << "\" "
               << "width=\"" << this->m_scale*w << "\" "
               << "height=\"" << this->m_scale*h << "\" "
               << make_style_attr() << " />" << std::endl;
    }

    /**
     * Embed Image into SVG file.
     *
     * @param img_64     This argument is an image stored as a base64 string
     * @param w          Width of embedded image
     * @param h          Height of embedded image
     * @param x          x-coordinate of image (optional)
     * @param y          y-coordinate of image (optional)
     */
    void embed_image(
                     std::string const & img_b64
                     , double const & w
                     , double const & h
                     , double const & x = 0.0
                     , double const & y = 0.0
                     )
    {
      assert(m_stream.is_open() || !"embed_image(): internal error");

      m_stream << "<image "
	             << "width=\"" << this->m_scale*w << "\" "
               << "height=\"" << this->m_scale*h << "\" "
               << "x=\"" << this->m_scale*x << "\" "
               << "y=\"" << this->m_scale*y << "\" "
      	       << "xlink:href=\"data:image/png;base64," << img_b64 << "\" "
      	       << " />" << std::endl;
    }
  };
  
}// namespace util

// UTIL_SVG_FILE_H
#endif
