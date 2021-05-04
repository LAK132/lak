#ifndef LAK_BITSET_HPP
#define LAK_BITSET_HPP

#include "lak/compiler.hpp"
#include "lak/endian.hpp"
#include "lak/packed_array.hpp"
#include "lak/span.hpp"
#include "lak/stdint.hpp"

namespace lak
{
  constexpr size_t sum(lak::span<const size_t> sizes)
  {
    size_t result = 0;
    for (const auto &v : sizes) result += v;
    return result;
  }

  constexpr uintmax_t bit_count_mask(size_t bit_count)
  {
    uintmax_t result = 0;
    while (bit_count-- > 0) result |= 1U << bit_count;
    return result;
  }

  template<lak::endian ENDIAN, typename UINT, size_t... SIZE>
  packed_struct bitset
  {
    static_assert((((sizeof(UINT) * 8) >= SIZE) && ...));
    static constexpr size_t member_size[sizeof...(SIZE)] = {SIZE...};
    static constexpr size_t bit_count = lak::sum(lak::span(member_size));
    static constexpr size_t byte_count =
      ((bit_count % 8 > 0) ? 1 : 0) + (bit_count / 8);
    // uint8_t _value[byte_count] = {};
    lak::packed_array<uint8_t, byte_count> _value = {};

    bitset() {}

  private:
    template<size_t... I>
    constexpr bitset(lak::span<const UINT, sizeof...(SIZE)> init,
                     std::index_sequence<I...>)
    {
      ((set<I>(init[I])), ...);
    }

  public:
    constexpr bitset(lak::span<const UINT, sizeof...(SIZE)> init)
    : bitset(init, std::make_index_sequence<sizeof...(SIZE)>{})
    {
    }

// See span.hpp
#ifdef LAK_COMPILER_CPP20
    constexpr
#endif
      bitset(lak::span<const UINT> init)
    : bitset(init.template first<sizeof...(SIZE)>())
    {
    }

    template<size_t INDEX>
    constexpr UINT get() const
    {
      constexpr size_t bit_offset =
        lak::sum(lak::span(member_size).template first<INDEX>());
      constexpr size_t partial_offset  = bit_offset % 8;
      constexpr size_t index_bit_count = member_size[INDEX];

      if constexpr (index_bit_count + partial_offset <= 8)
      {
        constexpr uintmax_t max = lak::bit_count_mask(index_bit_count);
        return (_value[bit_offset / 8] >> partial_offset) & max;
      }
      else if constexpr (ENDIAN == lak::endian::little)
      {
        UINT result = 0;

        // LSB
        constexpr size_t first_bit_count = 8 - partial_offset;
        constexpr uintmax_t first_max = lak::bit_count_mask(first_bit_count);
        result |= (_value[bit_offset / 8] >> partial_offset) & first_max;

        constexpr size_t from = (bit_offset / 8) + 1;
        constexpr size_t to   = (bit_offset + index_bit_count) / 8;
        static_assert(from <= to);
        for (size_t i = from, off = 0; i < to; ++i, off += 8)
        {
          result |= _value[i] << (off + first_bit_count);
        }

        // MSB
        constexpr size_t last_bit_count =
          (index_bit_count + partial_offset) % 8;
        if constexpr (last_bit_count > 0)
        {
          constexpr uintmax_t last_max = lak::bit_count_mask(last_bit_count);
          result |= (_value[(bit_offset + index_bit_count) / 8] & last_max)
                    << (((to - from) * 8) + first_bit_count);
        }

        return result;
      }
      else if constexpr (ENDIAN == lak::endian::big)
      {
        UINT result = 0;

        // MSB
        constexpr size_t first_bit_count = 8 - partial_offset;
        constexpr uintmax_t first_max = lak::bit_count_mask(first_bit_count);
        result |= (_value[bit_offset / 8] & (first_max << partial_offset))
                  << (index_bit_count - 8);

        constexpr size_t last_bit_count =
          (index_bit_count + partial_offset) % 8;
        constexpr size_t from = (bit_offset / 8) + 1;
        constexpr size_t to   = (bit_offset + index_bit_count) / 8;
        static_assert(from <= to);
        for (size_t i = from, off = ((to - from) - 1) * 8; i < to;
             ++i, off -= 8)
        {
          result |= _value[i] << (off + last_bit_count);
        }

        // LSB
        if constexpr (last_bit_count > 0)
        {
          constexpr uintmax_t last_max = lak::bit_count_mask(last_bit_count);
          result |= _value[(bit_offset + index_bit_count) / 8] & last_max;
        }

        return result;
      }
    }

    template<size_t INDEX>
    constexpr void set(UINT value)
    {
      constexpr size_t bit_offset =
        lak::sum(lak::span(member_size).template first<INDEX>());
      constexpr size_t partial_offset  = bit_offset % 8;
      constexpr size_t index_bit_count = member_size[INDEX];
      ASSERT_GREATER_OR_EQUAL(lak::bit_count_mask(index_bit_count), value);

      if constexpr (index_bit_count + partial_offset <= 8)
      {
        constexpr uintmax_t max = lak::bit_count_mask(index_bit_count);
        _value[bit_offset / 8] =
          ((value & max) << partial_offset) |
          (_value[bit_offset / 8] & ~(max << partial_offset));
      }
      else if constexpr (ENDIAN == lak::endian::little)
      {
        // LSB
        constexpr size_t first_bit_count = 8 - partial_offset;
        constexpr uintmax_t first_max = lak::bit_count_mask(first_bit_count);
        _value[bit_offset / 8] =
          ((value & first_max) << partial_offset) |
          (_value[bit_offset / 8] & ~(first_max << partial_offset));

        constexpr size_t from = (bit_offset / 8) + 1;
        constexpr size_t to   = (bit_offset + index_bit_count) / 8;
        static_assert(from <= to);
        for (size_t i = from, off = 0; i < to; ++i, off += 8)
        {
          _value[i] = value >> (off + first_bit_count);
        }

        // MSB
        constexpr size_t last_bit_count =
          (index_bit_count + partial_offset) % 8;
        if constexpr (last_bit_count > 0)
        {
          constexpr uintmax_t last_max = lak::bit_count_mask(last_bit_count);
          _value[(bit_offset + index_bit_count) / 8] =
            (value >> (((to - from) * 8) + first_bit_count)) |
            (_value[(bit_offset + index_bit_count) / 8] & ~last_max);
        }
      }
      else if constexpr (ENDIAN == lak::endian::big)
      {
        // MSB
        constexpr size_t first_bit_count = 8 - partial_offset;
        constexpr uintmax_t first_max = lak::bit_count_mask(first_bit_count);
        _value[bit_offset / 8] =
          (((value >> (index_bit_count - first_bit_count)) & first_max)
           << partial_offset) |
          (_value[bit_offset / 8] & ~(first_max << partial_offset));

        constexpr size_t last_bit_count =
          (index_bit_count + partial_offset) % 8;
        constexpr size_t from = (bit_offset / 8) + 1;
        constexpr size_t to   = (bit_offset + index_bit_count) / 8;
        static_assert(from <= to);
        for (size_t i = from, off = ((to - from) - 1) * 8; i < to;
             ++i, off -= 8)
        {
          _value[i] = value >> (off + last_bit_count);
        }

        // LSB
        if constexpr (last_bit_count > 0)
        {
          constexpr uintmax_t last_max = lak::bit_count_mask(last_bit_count);
          _value[(bit_offset + index_bit_count) / 8] =
            (value & last_max) |
            (_value[(bit_offset + index_bit_count) / 8] & ~last_max);
        }
      }
    }

    force_inline bool operator==(const bitset &other) const
    {
      if constexpr (bit_count % 8 > 0)
      {
        for (size_t i = 0; i < byte_count - 1; ++i)
          if (_value[i] != other._value[i]) return false;
        constexpr uint8_t mask = 0xFF >> (8 - (bit_count % 8));
        return (_value[byte_count - 1] & mask) ==
               (other._value[byte_count - 1] & mask);
      }
      else
      {
        for (size_t i = 0; i < byte_count; ++i)
          if (_value[i] != other._value[i]) return false;
        return true;
      }
    }

    force_inline bool operator!=(const bitset &other) const
    {
      if constexpr (bit_count % 8 > 0)
      {
        for (size_t i = 0; i < byte_count - 1; ++i)
          if (_value[i] == other._value[i]) return false;
        constexpr uint8_t mask = 0xFF >> (8 - (bit_count % 8));
        return (_value[byte_count - 1] & mask) !=
               (other._value[byte_count - 1] & mask);
      }
      else
      {
        for (size_t i = 0; i < byte_count; ++i)
          if (_value[i] == other._value[i]) return false;
        return true;
      }
    }
  };

  template<typename UINT, size_t... SIZE>
  using native_bitset = lak::bitset<lak::endian::native, UINT, SIZE...>;
  template<typename UINT, size_t... SIZE>
  using big_bitset = lak::bitset<lak::endian::big, UINT, SIZE...>;
  template<typename UINT, size_t... SIZE>
  using little_bitset = lak::bitset<lak::endian::little, UINT, SIZE...>;

  static_assert(lak::native_bitset<uint8_t, 1, 2, 3, 2>::bit_count == 8);
  static_assert(lak::native_bitset<uint16_t, 1, 2, 10, 3>::bit_count == 16);
  static_assert(sizeof(lak::native_bitset<uint8_t, 1, 2, 3, 2>) == 1);
  static_assert(sizeof(lak::native_bitset<uint16_t, 1, 2, 10, 3>) == 2);
}

#endif