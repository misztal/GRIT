#ifndef UTIL_IMAGE_WRITE_H
#define UTIL_IMAGE_WRITE_H

#include <util_image.h>

#include <util_il_wrap.h>

#include <iostream>
#include <string>

namespace util
{

  inline bool write_image(
                          std::string const & filename
                          , Image<unsigned char> const & image
                          )
  {
    ilInit();
    iluInit();
    ilutInit();

    //ilEnable (IL_ORIGIN_SET);
    //ilSetInteger (IL_ORIGIN_MODE, IL_ORIGIN_LOWER_LEFT);

    ilImage devil;

    unsigned int const width  = image.width();
    unsigned int const height = image.height();

    unsigned char * pixel = static_cast<unsigned char *>( const_cast<void *>(image.get_data()) );

    devil.TexImage(width, height, 1, 4, IL_RGBA, IL_UNSIGNED_BYTE, pixel);

    ilEnable(IL_FILE_OVERWRITE);

    bool result = !(0 == devil.Save(const_cast< char * >(filename.c_str())) );

    ilShutDown();

    return result;
  }

} // namespace util

//UTIL_IMAGE_WRITE_H
#endif
