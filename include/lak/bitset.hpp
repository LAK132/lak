#ifndef LAK_BITSET_HPP
#define LAK_BITSET_HPP

#include "lak/array.hpp"
#include "lak/math.hpp"
#include "lak/span_manip.hpp"

namespace lak
{
	template<size_t SIZE>
	struct bitset
	{
	private:
		static constexpr size_t _bits_per = sizeof(uint_least8_t) * CHAR_BIT;
		static constexpr size_t _byte_count =
		  lak::ceil_div<size_t>(SIZE, _bits_per);
		lak::array<uint_least8_t, _byte_count> _value;

	public:
		bitset()                          = default;
		bitset(const bitset &)            = default;
		bitset &operator=(const bitset &) = default;

		bool get(size_t index) const
		{
			return (_value[index / _bits_per] & (1U << (index % _bits_per))) != 0U;
		}

		void set(size_t index, bool value)
		{
			uint_least8_t &v            = _value[index / _bits_per];
			const uint_least8_t bitmask = 1U << (index % _bits_per);
			v                           = (v & ~bitmask) | (value ? bitmask : 0U);
		}

		bitset operator&(const bitset &other) const
		{
			bitset result;
			for (size_t i = 0U; i < _byte_count; ++i)
				result._value[i] = _value[i] & other._value[i];
			return result;
		}

		bitset &operator&=(const bitset &other)
		{
			for (size_t i = 0U; i < _byte_count; ++i) _value[i] &= other._value[i];
			return *this;
		}

		bitset operator|(const bitset &other) const
		{
			bitset result;
			for (size_t i = 0U; i < _byte_count; ++i)
				result._value[i] = _value[i] | other._value[i];
			return result;
		}

		bitset &operator|=(const bitset &other)
		{
			for (size_t i = 0U; i < _byte_count; ++i) _value[i] |= other._value[i];
			return *this;
		}

		bool operator==(const bitset &other) const
		{
			return lak::compare(lak::span<const uint_least8_t>(_value),
			                    lak::span<const uint_least8_t>(other._value)) ==
			       _value.size();
		}

		bool operator!=(const bitset &other) const { return !operator==(other); }
	};
}

#endif
