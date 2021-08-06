#ifndef LAK_STWEAMIFY_HPP
#define LAK_STWEAMIFY_HPP

#include "lak/streamify.hpp"
#include "lak/uwuify.hpp"

namespace lak
{
  template<typename... ARGS>
  lak::u8string stweamify(const ARGS &...args)
  {
    return lak::uwuify(lak::streamify(args...));
  }
}

#endif
