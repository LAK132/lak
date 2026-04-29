#ifndef LAK_BIT_FIELD_HPP
#define LAK_BIT_FIELD_HPP

#include "lak/math.hpp"
#include "lak/packed_array.hpp"
#include "lak/span.hpp"
#include "lak/stdint.hpp"
#include "lak/system/compiler.hpp"
#include "lak/system/endian.hpp"

#include <initializer_list>

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
		while (bit_count-- > 0) result |= 1ULL << bit_count;
		return result;
	}

	template<lak::endian ENDIAN, typename UINT, size_t... SIZE>
	packed_struct bit_field
	{
		static_assert((((sizeof(UINT) * 8) >= SIZE) && ...));
		static constexpr size_t member_size[sizeof...(SIZE)] = {SIZE...};
		static constexpr size_t bit_count  = lak::sum(lak::span(member_size));
		static constexpr size_t byte_count = lak::ceil_div<size_t>(bit_count, 8U);
		// uint8_t _value[byte_count] = {};
		lak::packed_array<uint8_t, byte_count> _value = {};

		bit_field() {}

	private:
		template<size_t... I>
		constexpr bit_field(lak::span<const UINT, sizeof...(SIZE)> init,
		                    std::index_sequence<I...>)
		{
			((set<I>(init[I])), ...);
		}

	public:
		constexpr bit_field(lak::span<const UINT, sizeof...(SIZE)> init)
		: bit_field(init, std::make_index_sequence<sizeof...(SIZE)>{})
		{
		}

// See span.hpp
#ifdef LAK_COMPILER_CPP20
		constexpr
#endif
		  bit_field(lak::span<const UINT> init)
		: bit_field(init.template first<sizeof...(SIZE)>())
		{
		}

		constexpr bit_field(lak::array<const UINT, sizeof...(SIZE)> init)
		: bit_field(lak::span<const UINT, sizeof...(SIZE)>(init))
		{
		}

#ifdef LAK_COMPILER_CPP20
		constexpr
#endif
		  bit_field(std::initializer_list<UINT> init)
		: bit_field(lak::span<const UINT>(init.begin(), init.end()))
		{
		}

		template<size_t INDEX>
		constexpr UINT get() const
		{
			constexpr size_t index_bit_count = member_size[INDEX];
			if constexpr (index_bit_count == 0U) return UINT{};

			constexpr size_t bit_offset =
			  lak::sum(lak::span(member_size).template first<INDEX>());
			constexpr size_t partial_offset = bit_offset % 8;

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
			constexpr size_t index_bit_count = member_size[INDEX];
			if constexpr (index_bit_count == 0U) return;

			constexpr size_t bit_offset =
			  lak::sum(lak::span(member_size).template first<INDEX>());
			constexpr size_t partial_offset = bit_offset % 8;
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
					_value[i] = uint8_t(value >> (off + first_bit_count));
				}

				// MSB
				constexpr size_t last_bit_count =
				  (index_bit_count + partial_offset) % 8;
				if constexpr (last_bit_count > 0)
				{
					constexpr uintmax_t last_max = lak::bit_count_mask(last_bit_count);
					_value[(bit_offset + index_bit_count) / 8] =
					  uint8_t((value >> (((to - from) * 8) + first_bit_count)) |
					          (_value[(bit_offset + index_bit_count) / 8] & ~last_max));
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

		force_inline bool operator==(const bit_field &other) const
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

		force_inline bool operator!=(const bit_field &other) const
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
	using native_bit_field = lak::bit_field<lak::endian::native, UINT, SIZE...>;
	template<typename UINT, size_t... SIZE>
	using big_bit_field = lak::bit_field<lak::endian::big, UINT, SIZE...>;
	template<typename UINT, size_t... SIZE>
	using little_bit_field = lak::bit_field<lak::endian::little, UINT, SIZE...>;

	static_assert(lak::native_bit_field<uint8_t, 1, 2, 3, 2>::bit_count == 8);
	static_assert(lak::native_bit_field<uint16_t, 1, 2, 10, 3>::bit_count == 16);
	static_assert(sizeof(lak::native_bit_field<uint8_t, 1, 2, 3, 2>) == 1);
	static_assert(sizeof(lak::native_bit_field<uint16_t, 1, 2, 10, 3>) == 2);
}

#endif
