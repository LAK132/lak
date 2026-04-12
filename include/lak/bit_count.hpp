#ifndef LAK_BIT_COUNT
#define LAK_BIT_COUNT

#include "lak/compare.hpp"
#include "lak/stdint.hpp"

namespace lak
{
	struct bit_count
	{
		size_t bytes = 0U;
		uint8_t bits = 0U;

		static inline bit_count from_bits(size_t bits);
		static inline bit_count from_bytes(size_t bytes);

		inline size_t to_bits() const;

		inline bit_count &normalise();
		inline bit_count normalised() const;

		inline bit_count &operator+=(const bit_count &other);
		inline bit_count operator+(const bit_count &other) const;

		inline bit_count &operator-=(const bit_count &other);
		inline bit_count operator-(const bit_count &other) const;

		inline lak::strong_ordering operator<=>(const bit_count &other) const;
		inline bool operator==(const bit_count &other) const;
	};
}

#include "lak/bit_count.inl"

#endif
