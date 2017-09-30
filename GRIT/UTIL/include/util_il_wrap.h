#ifndef UTIL_IL_WRAP_H
#define UTIL_IL_WRAP_H

#include <IL/ilut.h>  // Probably only have to #include this one

#include <devil_cpp_wrapper.hpp>

namespace util
{

  void check_IL_errors()
  {
    ILenum Error;
    while ((Error = ilGetError()) != IL_NO_ERROR)
    {
      std::cerr << "check_IL_errors(): "
      << Error
      << " : "
      << iluErrorString(Error)
      << std::endl;
    }
  }


  std::string get_IL_string(ILenum code)
  {
    switch(code)
    {
        //case IL_COLOUR_INDEX  : return "IL_COLOUR_INDEX";
      case IL_COLOR_INDEX : return "IL_COLOR_INDEX";
      case IL_RGB : return "IL_RGB";
      case IL_RGBA: return "IL_RGBA";
      case IL_BGR : return "IL_BGR";
      case IL_BGRA: return "IL_BGRA";
      case IL_LUMINANCE : return "IL_LUMINANCE";
      case IL_LUMINANCE_ALPHA : return "IL_LUMINANCE_ALPHA";
      case IL_BYTE: return "IL_BYTE";
      case IL_UNSIGNED_BYTE: return "IL_UNSIGNED_BYTE";
      case IL_SHORT: return "IL_SHORT";
      case IL_UNSIGNED_SHORT: return "IL_UNSIGNED_SHORT";
      case IL_INT: return "IL_INT";
      case IL_UNSIGNED_INT: return "IL_UNSIGNED_INT";
      case IL_FLOAT: return "IL_FLOAT";
      case IL_DOUBLE: return "IL_DOUBLE";

      case IL_PAL_NONE: return "IL_PAL_NONE";
      case IL_PAL_RGB24: return "IL_PAL_RGB24";
      case IL_PAL_RGB32: return "IL_PAL_RGB32";
      case IL_PAL_RGBA32: return "IL_PAL_RGBA32";
      case IL_PAL_BGR24: return "IL_PAL_BGR24";
      case IL_PAL_BGR32: return "IL_PAL_BGR32";
      case IL_PAL_BGRA32: return "IL_PAL_BGRA32";
      case IL_TYPE_UNKNOWN: return "IL_TYPE_UNKNOWN";
      case IL_BMP: return "IL_BMP";
      case IL_CUT: return "IL_CUT";
      case IL_DOOM: return "IL_DOOM";
      case IL_DOOM_FLAT: return "IL_DOOM_FLAT";
      case IL_ICO: return "IL_ICO";
      case IL_JPG: return "IL_JPG";
        //    case IL_JFIF: return "IL_JFIF";
        //case IL_LBM: return "IL_LBM"; 2010-07-15 perb: Uncommented as this is renamed to IL_ILBM since version 1.7.5
      case IL_PCD: return "IL_PCD";
      case IL_PCX: return "IL_PCX";
      case IL_PIC: return "IL_PIC";
      case IL_PNG: return "IL_PNG";
      case IL_PNM: return "IL_PNM";
      case IL_SGI: return "IL_SGI";
      case IL_TGA: return "IL_TGA";
      case IL_TIF: return "IL_TIF";
      case IL_CHEAD: return "IL_CHEAD";
      case IL_RAW: return "IL_RAW";
      case IL_MDL: return "IL_MDL";
      case IL_WAL: return "IL_WAL";
      case IL_LIF: return "IL_LIF";
      case IL_MNG: return "IL_MNG";
        //    case IL_JNG: return "IL_JNG";
      case IL_GIF: return "IL_GIF";
      case IL_DDS: return "IL_DDS";
      case IL_DCX: return "IL_DCX";
      case IL_PSD: return "IL_PSD";
      case IL_EXIF: return "IL_EXIF";
      case IL_PSP: return "IL_PSP";
      case IL_PIX: return "IL_PIX";
      case IL_PXR: return "IL_PXR";
      case IL_XPM: return "IL_XPM";
      case IL_JASC_PAL: return "IL_JASC_PAL";
      case IL_ORIGIN_SET: return "IL_ORIGIN_SET";
      case IL_ORIGIN_LOWER_LEFT: return "IL_ORIGIN_LOWER_LEFT";
      case IL_ORIGIN_UPPER_LEFT: return "IL_ORIGIN_UPPER_LEFT";
      case IL_ORIGIN_MODE: return "IL_ORIGIN_MODE";
      case IL_FORMAT_SET: return "IL_FORMAT_SET";
      case IL_FORMAT_MODE: return "IL_FORMAT_MODE";
      case IL_TYPE_SET: return "IL_TYPE_SET";
      case IL_TYPE_MODE: return "IL_TYPE_MODE";
    };
    return "unregonized";
  }

} // namespace util

// UTIL_IL_WRAP_H
#endif
