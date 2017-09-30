#ifndef UTIL_IMAGE_H
#define UTIL_IMAGE_H

#include <vector>
#include <algorithm>

namespace util
{

  /**
   * An Simple Generic Image Class.
   */
  template<typename T>
  class Image
  {
  public:

    typedef T  value_type;

  protected:

    unsigned int    m_width;     ///< Number of pixels along x-axis.
    unsigned int    m_height;    ///< Number of pixels along y-axis.
    unsigned int    m_channels;  ///< Number of channels in image.
    std::vector<T>  m_data;      ///< The actual image data.

  public:

    unsigned int const & width()    const { return m_width;    }
    unsigned int const & height()   const { return m_height;   }
    unsigned int const & channels() const { return m_channels; }

    T & operator()(
                   unsigned int const & i
                   , unsigned int const & j
                   , unsigned int const & channel
                   )
    {
      return m_data[(j*m_width + i)*m_channels + channel];
    }

    T const & operator()(
                         unsigned int const & i
                         , unsigned int const & j
                         , unsigned int const & channel
                         ) const
    {
      return m_data[(j*m_width + i)*m_channels + channel];
    }

    void       * get_data()       { return &(m_data[0]); }
    void const * get_data() const { return &(m_data[0]); }

  public:

    Image()
    :  m_width(0u)
    ,  m_height(0u)
    ,  m_channels(0u)
    ,  m_data()
    {}

    Image(
            unsigned int const & width
          , unsigned int const & height
          , unsigned int const & channels
          , T const * data
          )
    : m_width(width)
    , m_height(height)
    , m_channels(channels)
    , m_data( data, data+(width*height*channels) )
    {}

    Image(
            unsigned int const & width
          , unsigned int const & height
          , unsigned int const & channels
          , std::vector<T> const & data
          )
    : m_width(width)
    , m_height(height)
    , m_channels(channels)
    , m_data(data)
    {}

    Image(
            unsigned int const & width
          , unsigned int const & height
          , unsigned int const & channels
          )
    : m_width(width)
    , m_height(height)
    , m_channels(channels)
    , m_data(width*height*channels)
    {
      std::fill(m_data.begin(),m_data.end(),static_cast<T>(0));
    }

  };

  template<typename T>
  inline Image<T> make_image(
                    unsigned int const & width
                  , unsigned int const & height
                  , unsigned int const & channels
                  )
  {
    return  Image<T>(width, height, channels);
  }

  inline Image<float> make_grayscale_image(Image<unsigned char> const & src)
  {
    Image<float>  img = Image<float>(src.width(), src.height(), 1u);

    //--- Combine all channeles into one ---------------------------------------
    float max_value = 0.0f;
    for (unsigned int j=0u; j<src.height();++j)
    {
      for (unsigned int i=0u; i<src.width();++i)
      {
        float intensity_sum = 0.0f;

        for (unsigned int k=0u; k<src.channels(); ++k)
        {
          intensity_sum += src(i,j,k);
        }

        max_value = (intensity_sum > max_value) ? intensity_sum : max_value;

        img(i,j,1) = intensity_sum;
      }
    }

    //--- Normalize to make values inside 0..1 range ---------------------------
    if(max_value>0)
    {
      for (unsigned int j=0u; j<src.height();++j)
      {
        for (unsigned int i=0u; i<src.width();++i)
        {
          img(i,j,1) /= max_value;
        }
      }
    }

    return img;
  }


  template<typename T1, typename T2>
  inline T1 get_image_value_at(
                              T2 const & x    // The x-coordinate of the position at where we want to know the pixel value
                            , T2 const & y    // The y-coordinate of the position at where we want to know the pixel value
                            , Image<T1> const & img
                            , T2 const & ox   // x-coordinate of origin of image. ie. position of pixel (0,0)
                            , T2 const & oy   // y-coordinate of origin of image. ie. position of pixel (0,0)
                            , T2 const & dx   // Horizontal distance between two neighboring pixel values
                            , T2 const & dy   // Vertical distance between two neighboring pixel values
                            )
  {
    using std::max;
    using std::min;
    using std::floor;

    T2 const W = (img.width()-1)*dx;
    T2 const H = (img.height()-1)*dy;
    T2 const nx = min( max(x-ox, T2(0)) , W) / dx;
    T2 const ny = min( max(y-oy, T2(0)) , H) / dy;

    unsigned int const i0 = static_cast<unsigned int const >( floor( nx ) );
    unsigned int const j0 = static_cast<unsigned int const >( floor( ny ) );
    unsigned int const i1 = ( i0 + 1 ) % img.width();
    unsigned int const j1 = ( j0 + 1 ) % img.height();

    T1 const v00 = img( i0, j0, 0);
    T1 const v10 = img( i1, j0, 0);
    T1 const v01 = img( i0, j1, 0);
    T1 const v11 = img( i1, j1, 0);
    T2 const s   = ( x - ( i0*dx + ox ) ) / dx;
    T2 const t   = ( y - ( j0*dy + oy ) ) / dy;
    T1 const val = (1-t)*((1-s)*v00 + s*v10)   +  t*((1-s)*v01 + s*v11);
    
    return val;
  }

} // namespace util

// UTIL_IMAGE_H
#endif
