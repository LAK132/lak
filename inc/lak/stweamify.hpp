#ifndef LAK_STWEAMIFY_HPP
#define LAK_STWEAMIFY_HPP

#include "lak/streamify.hpp"
#include "lak/uwuify.hpp"

namespace lak
{
  template<typename CHAR, typename... ARGS>
  lak::string<CHAR> stweamify(const ARGS &... args)
  {
    return lak::uwuify(lak::streamify<CHAR>(args...));
  }
}

#endif
