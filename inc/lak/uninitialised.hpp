#ifndef LAK_UNINITIALISED_HPP
#define LAK_UNINITIALISED_HPP

#include "lak/variant.hpp"

namespace lak
{
  /* --- uninitialised --- */

  template<typename T>
  struct uninitialised
  {
    static_assert(lak::is_default_constructible_v<uninitialised>);

    using value_type = T;
    union
    {
      lak::monostate _uninitialised_value;
      value_type _value;
    };

    uninitialised() : _uninitialised_value()
    {
      _uninitialised_value.~monostate();
    }

    template<typename... ARGS>
    uninitialised(ARGS &&... args) : _value(lak::forward<ARGS>(args)...)
    {
    }

    template<typename... ARGS>
    value_type &create(ARGS &&... args)
    {
      new (&_value) value_type(lak::forward<ARGS>(args)...);
    }

    void destroy() { _value.~value_type(); }

    value_type &value() { return _value; }

    const value_type &value() const { return _value; }
  };

}

#endif