#ifndef UTIL_SAME_SIGN_H
#define UTIL_SAME_SIGN_H

namespace util
{

  template <typename T>
  inline bool same_sign(
                          T const & t1
                        , T const & t2
                        )
  {
    //2014-07-30 Kenny code review: Not sure why strict "<" and ">" are
    //                              not used below? Seems strange to me
    //                              that zero has a sign?

    return (t1 <= 0.0 && t2 <= 0.0) || (t1 >= 0.0 && t2 >= 0.0);
  }

}//end of namespace util

// UTIL_SAME_SIGN_H
#endif
