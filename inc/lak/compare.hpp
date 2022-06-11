#ifndef LAK_COMPARE_HPP
#define LAK_COMPARE_HPP

#include "lak/stdint.hpp"

#include <climits>
#include <limits>

namespace lak
{
	enum struct _ordering : signed char
	{
		equal      = 0,
		equivalent = equal,
		less       = -1,
		greater    = 1,
		unordered  = SCHAR_MIN,
	};

	/* --- partial_ordering --- */

	struct partial_ordering
	{
		signed char _value;

		static const lak::partial_ordering less;
		static const lak::partial_ordering greater;
		static const lak::partial_ordering equivalent;
		static const lak::partial_ordering unordered;

		/* --- operator== --- */

		[[nodiscard]] friend constexpr bool operator==(
		  lak::partial_ordering, lak::partial_ordering) noexcept = default;

		[[nodiscard]] friend constexpr bool operator==(
		  const lak::partial_ordering val, nullptr_t) noexcept
		{
			return val._value == 0;
		}

		[[nodiscard]] friend constexpr bool operator==(
		  nullptr_t, const lak::partial_ordering val) noexcept
		{
			return val == 0;
		}

		/* --- operator< --- */

		[[nodiscard]] friend constexpr bool operator<(
		  const lak::partial_ordering val, nullptr_t) noexcept
		{
			return val._value == static_cast<signed char>(lak::_ordering::less);
		}

		[[nodiscard]] friend constexpr bool operator<(
		  nullptr_t, const lak::partial_ordering val) noexcept
		{
			return val > 0;
		}

		/* --- operator> --- */

		[[nodiscard]] friend constexpr bool operator>(
		  const lak::partial_ordering val, nullptr_t)
		{
			return val._value == static_cast<signed char>(lak::_ordering::greater);
		}

		[[nodiscard]] friend constexpr bool operator>(
		  nullptr_t, const lak::partial_ordering val)
		{
			return val < 0;
		}

		/* --- operator<= --- */

		[[nodiscard]] friend constexpr bool operator<=(
		  const lak::partial_ordering val, nullptr_t)
		{
			return static_cast<signed char>(
			         0 - static_cast<unsigned char>(val._value)) >= 0;
		}

		[[nodiscard]] friend constexpr bool operator<=(
		  nullptr_t, const lak::partial_ordering val)
		{
			return val >= 0;
		}

		/* --- operator>= --- */

		[[nodiscard]] friend constexpr bool operator>=(
		  const lak::partial_ordering val, nullptr_t)
		{
			return val._value >= 0;
		}

		[[nodiscard]] friend constexpr bool operator>=(
		  nullptr_t, const lak::partial_ordering val)
		{
			return val <= 0;
		}

		/* --- operator<=> --- */

		[[nodiscard]] friend constexpr lak::partial_ordering operator<=>(
		  const lak::partial_ordering val, nullptr_t)
		{
			return val;
		}

		[[nodiscard]] friend constexpr lak::partial_ordering operator<=>(
		  nullptr_t, const lak::partial_ordering val)
		{
			return {._value = static_cast<signed char>(
			          0 - static_cast<unsigned char>(val._value))};
		}
	};

	inline constexpr const lak::partial_ordering lak::partial_ordering::less{
	  static_cast<signed char>(lak::_ordering::less)};
	inline constexpr const lak::partial_ordering lak::partial_ordering::greater{
	  static_cast<signed char>(lak::_ordering::greater)};
	inline constexpr const lak::partial_ordering
	  lak::partial_ordering::equivalent{
	    static_cast<signed char>(lak::_ordering::equivalent)};
	inline constexpr const lak::partial_ordering
	  lak::partial_ordering::unordered{
	    static_cast<signed char>(lak::_ordering::unordered)};

	/* --- weak_ordering --- */

	struct weak_ordering
	{
		signed char _value;

		static const lak::weak_ordering less;
		static const lak::weak_ordering greater;
		static const lak::weak_ordering equivalent;

		constexpr operator partial_ordering() const noexcept
		{
			return {._value = _value};
		}

		/* --- operator== --- */

		[[nodiscard]] friend constexpr bool operator==(
		  lak::weak_ordering, lak::weak_ordering) noexcept = default;

		[[nodiscard]] friend constexpr bool operator==(
		  const lak::weak_ordering val, nullptr_t) noexcept
		{
			return val._value == 0;
		}

		[[nodiscard]] friend constexpr bool operator==(
		  nullptr_t, const lak::weak_ordering val) noexcept
		{
			return val == 0;
		}

		/* --- operator< --- */

		[[nodiscard]] friend constexpr bool operator<(const lak::weak_ordering val,
		                                              nullptr_t) noexcept
		{
			return val._value < 0;
		}

		[[nodiscard]] friend constexpr bool operator<(
		  nullptr_t, const lak::weak_ordering val) noexcept
		{
			return val > 0;
		}

		/* --- operator> --- */

		[[nodiscard]] friend constexpr bool operator>(const lak::weak_ordering val,
		                                              nullptr_t)
		{
			return val._value > 0;
		}

		[[nodiscard]] friend constexpr bool operator>(nullptr_t,
		                                              const lak::weak_ordering val)
		{
			return val < 0;
		}

		/* --- operator<= --- */

		[[nodiscard]] friend constexpr bool operator<=(
		  const lak::weak_ordering val, nullptr_t)
		{
			return val._value <= 0;
		}

		[[nodiscard]] friend constexpr bool operator<=(
		  nullptr_t, const lak::weak_ordering val)
		{
			return val >= 0;
		}

		/* --- operator>= --- */

		[[nodiscard]] friend constexpr bool operator>=(
		  const lak::weak_ordering val, nullptr_t)
		{
			return val._value >= 0;
		}

		[[nodiscard]] friend constexpr bool operator>=(
		  nullptr_t, const lak::weak_ordering val)
		{
			return val <= 0;
		}

		/* --- operator<=> --- */

		[[nodiscard]] friend constexpr lak::weak_ordering operator<=>(
		  const lak::weak_ordering val, nullptr_t)
		{
			return val;
		}

		[[nodiscard]] friend constexpr lak::weak_ordering operator<=>(
		  nullptr_t, const lak::weak_ordering val)
		{
			return {._value = static_cast<signed char>(-val._value)};
		}
	};

	inline constexpr lak::weak_ordering lak::weak_ordering::less{
	  static_cast<signed char>(lak::_ordering::less)};
	inline constexpr lak::weak_ordering lak::weak_ordering::greater{
	  static_cast<signed char>(lak::_ordering::greater)};
	inline constexpr lak::weak_ordering lak::weak_ordering::equivalent{
	  static_cast<signed char>(lak::_ordering::equivalent)};

	/* --- strong_ordering --- */

	struct strong_ordering
	{
		signed char _value;

		static const lak::strong_ordering less;
		static const lak::strong_ordering equal;
		static const lak::strong_ordering greater;
		static const lak::strong_ordering equivalent;

		constexpr operator partial_ordering() const noexcept
		{
			return {._value = _value};
		}

		constexpr operator weak_ordering() const noexcept
		{
			return {._value = _value};
		}

		/* --- operator== --- */

		[[nodiscard]] friend constexpr bool operator==(
		  lak::strong_ordering, lak::strong_ordering) noexcept = default;

		[[nodiscard]] friend constexpr bool operator==(
		  const lak::strong_ordering val, nullptr_t) noexcept
		{
			return val._value == 0;
		}

		[[nodiscard]] friend constexpr bool operator==(
		  nullptr_t, const lak::strong_ordering val) noexcept
		{
			return val == 0;
		}

		/* --- operator< --- */

		[[nodiscard]] friend constexpr bool operator<(
		  const lak::strong_ordering val, nullptr_t) noexcept
		{
			return val._value < 0;
		}

		[[nodiscard]] friend constexpr bool operator<(
		  nullptr_t, const lak::strong_ordering val) noexcept
		{
			return val > 0;
		}

		/* --- operator> --- */

		[[nodiscard]] friend constexpr bool operator>(
		  const lak::strong_ordering val, nullptr_t)
		{
			return val._value > 0;
		}

		[[nodiscard]] friend constexpr bool operator>(
		  nullptr_t, const lak::strong_ordering val)
		{
			return val < 0;
		}

		/* --- operator<= --- */

		[[nodiscard]] friend constexpr bool operator<=(
		  const lak::strong_ordering val, nullptr_t)
		{
			return val._value <= 0;
		}

		[[nodiscard]] friend constexpr bool operator<=(
		  nullptr_t, const lak::strong_ordering val)
		{
			return val >= 0;
		}

		/* --- operator>= --- */

		[[nodiscard]] friend constexpr bool operator>=(
		  const lak::strong_ordering val, nullptr_t)
		{
			return val._value >= 0;
		}

		[[nodiscard]] friend constexpr bool operator>=(
		  nullptr_t, const lak::strong_ordering val)
		{
			return val <= 0;
		}

		/* --- operator<=> --- */

		[[nodiscard]] friend constexpr lak::strong_ordering operator<=>(
		  const lak::strong_ordering val, nullptr_t)
		{
			return val;
		}

		[[nodiscard]] friend constexpr lak::strong_ordering operator<=>(
		  nullptr_t, const lak::strong_ordering val)
		{
			return {._value = static_cast<signed char>(-val._value)};
		}
	};

	inline constexpr lak::strong_ordering lak::strong_ordering::less{
	  static_cast<signed char>(lak::_ordering::less)};
	inline constexpr lak::strong_ordering lak::strong_ordering::equal{
	  static_cast<signed char>(lak::_ordering::equal)};
	inline constexpr lak::strong_ordering lak::strong_ordering::greater{
	  static_cast<signed char>(lak::_ordering::greater)};
	inline constexpr lak::strong_ordering lak::strong_ordering::equivalent{
	  static_cast<signed char>(lak::_ordering::equivalent)};

	/* --- comparison functions --- */

	[[nodiscard]] inline constexpr bool is_eq(
	  const lak::partial_ordering val) noexcept
	{
		return val == 0;
	}

	[[nodiscard]] inline constexpr bool is_neq(
	  const lak::partial_ordering val) noexcept
	{
		return val != 0;
	}

	[[nodiscard]] inline constexpr bool is_lt(
	  const lak::partial_ordering val) noexcept
	{
		return val < 0;
	}

	[[nodiscard]] inline constexpr bool is_lteq(
	  const lak::partial_ordering val) noexcept
	{
		return val <= 0;
	}

	[[nodiscard]] inline constexpr bool is_gt(
	  const lak::partial_ordering val) noexcept
	{
		return val > 0;
	}

	[[nodiscard]] inline constexpr bool is_gteq(
	  const lak::partial_ordering val) noexcept
	{
		return val >= 0;
	}
}

#endif
