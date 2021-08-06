#ifndef LAK_STREAMIFY_HPP
#define LAK_STREAMIFY_HPP

#include "lak/string.hpp"

namespace lak
{
  template<typename T>
  constexpr auto _is_streamable(int)
    -> decltype(std::declval<std::ostream &>() << std::declval<T>(), bool())
  {
    return true;
  }
  template<typename T>
  constexpr auto _is_streamable(...) -> decltype(bool())
  {
    return false;
  }
  template<typename T>
  constexpr bool is_streamable()
  {
    return _is_streamable<T>(0);
  }

  template<typename... ARGS>
  lak::u8string streamify(const ARGS &...args);
}

#include "lak/streamify.inl"

#endif