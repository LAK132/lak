#ifndef LAK_BITFLAG_HPP
#define LAK_BITFLAG_HPP

#include "lak/stdint.hpp"

namespace lak
{
  template<typename ENUM, ENUM TO_FIND, ENUM FLAG, ENUM... FLAGS>
  constexpr size_t enum_index(size_t index = 0)
  {
    if constexpr (TO_FIND == FLAG)
      return index;
    else
      return enum_index<ENUM, TO_FIND, FLAGS...>(index + 1);
  }

  template<typename ENUM, ENUM... FLAGS>
  struct bitflag
  {
    static constexpr size_t bit_count = sizeof...(FLAGS);
    static constexpr size_t byte_count =
      ((bit_count % 8 > 0) ? 1 : 0) + (bit_count / 8);
    uint8_t _value[byte_count] = {};

    template<ENUM INDEX>
    constexpr bool get() const
    {
      constexpr size_t bit_index   = enum_index<ENUM, INDEX, FLAGS...>();
      constexpr size_t byte_index  = bit_index / 8;
      constexpr size_t byte_offset = bit_index % 8;

      return (_value[byte_index] >> byte_offset) & 0b1U;
    }

    template<ENUM INDEX>
    constexpr void set(bool value)
    {
      constexpr size_t bit_index   = enum_index<ENUM, INDEX, FLAGS...>();
      constexpr size_t byte_index  = bit_index / 8;
      constexpr size_t byte_offset = bit_index % 8;

      _value[byte_index] = ((value ? 0b1U : 0b0U) << byte_offset) |
                           (_value[byte_index] & ~(0b1U << byte_offset));
    }
  };
}

#endif