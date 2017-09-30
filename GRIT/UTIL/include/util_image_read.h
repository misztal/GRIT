#ifndef UTIL_IMAGE_READ_H
#define UTIL_IMAGE_READ_H

#include <util_image.h>

#include <util_il_wrap.h>

#include <util_log.h>

#include <string>

namespace util
{

  inline bool read_image(
                         std::string const & filename
                         , Image<unsigned char> & image
                         , bool show_statistics = true
                         )
  {
    ilInit();
    iluInit();
    ilutInit();

    //ilEnable (IL_ORIGIN_SET);
    //ilSetInteger (IL_ORIGIN_MODE, IL_ORIGIN_LOWER_LEFT);

    ilImage devil;

    ILboolean result = devil.Load(const_cast< char * >(filename.c_str()));

    if (!result)
    {
      return false;
    }

    if (show_statistics)
    {
      util::Log logging;

      logging << "--- file : " << filename << "---------------------------" << util::Log::newline();
      logging << "\twidth         = " << (int)devil.Width() << util::Log::newline();
      logging << "\theight        = " << (int)devil.Height() << util::Log::newline();
      logging << "\tdepth         = " << (int)devil.Depth() << util::Log::newline();
      logging << "\tBpp           = " << (int)devil.Bpp() << util::Log::newline();
      logging << "\tBitpp         = " << (int)devil.Bitpp() << util::Log::newline();
      logging << "\tPallette Type = " << get_IL_string(devil.PaletteType()) << util::Log::newline();
      logging << "\tFormat        = " << get_IL_string(devil.Format()) << util::Log::newline();
      logging << "\tType          = " << get_IL_string(devil.Type()) << util::Log::newline();
      logging << "\tNumImages     = " << (int)devil.NumImages() << util::Log::newline();
      logging << "\tNumMipmaps    = " << (int)devil.NumMipmaps() << util::Log::newline();
      logging << "\tOrigin        = " << get_IL_string(devil.GetOrigin()) << util::Log::newline();
    }

    if (devil.GetOrigin() != IL_ORIGIN_LOWER_LEFT)
      devil.Flip();

    unsigned int const width  = devil.Width();
    unsigned int const height = devil.Height();

    image = make_image<unsigned char>(width, height, 4);

    devil.Bind();
    ilCopyPixels(0, 0, 0, width, height, 1, IL_RGBA, IL_UNSIGNED_BYTE, image.get_data() );

    ilShutDown();

    return true;
  }

} // namespace util

//UTIL_IMAGE_READ_H
#endif
