#ifndef LAK_BIGINT_HPP
#define LAK_BIGINT_HPP

#include "lak/array.hpp"
#include "lak/compare.hpp"
#include "lak/defer.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"
#include "lak/stdint.hpp"

#include <iostream>

namespace lak
{
	struct bigint
	{
#ifndef LAK_BIGINT_STANDALONE_HPP
		template<typename T>
		using _array = lak::array<T>;
		template<typename T>
		using _span = lak::span<T>;
		template<typename T, typename U>
		using _pair = lak::pair<T, U>;
#else
		template<typename T>
		using _array = std::vector<T>;
		template<typename T>
		using _span = std::span<T>;
		template<typename T, typename U>
		using _pair = std::pair<T, U>;
#endif

		using div_rem_result = _pair<lak::bigint, lak::bigint>;

	private:
		bool _negative = false;
		_array<uintmax_t> _data;

		lak::bigint &negate();
		void reserve(size_t count);
		void normalise(size_t min_count = 0U);
		size_t min_size() const;

		_span<const uintmax_t> min_span() const;

		// ignores _negative
		void add(uintmax_t value);
		void sub(uintmax_t value); // must not overflow
		[[nodicard]] div_rem_result div_rem_impl(uintmax_t value,
		                                         bool negate_result) const;
		void add(const lak::bigint &value);
		void sub(const lak::bigint &value); // must not overflow
		[[nodicard]] div_rem_result div_rem_impl(const lak::bigint &value,
		                                         bool negate_result) const;

	public:
		lak::bigint()                               = default;
		lak::bigint(const lak::bigint &)            = default;
		lak::bigint(lak::bigint &&)                 = default;
		lak::bigint &operator=(const lak::bigint &) = default;
		lak::bigint &operator=(lak::bigint &&)      = default;

		lak::bigint(uintmax_t value);
		lak::bigint(intmax_t value);
		lak::bigint(unsigned value) : lak::bigint(uintmax_t(value)) {}
		lak::bigint(signed value) : lak::bigint(intmax_t(value)) {}

		lak::bigint &operator=(uintmax_t value);
		lak::bigint &operator=(intmax_t value);
		lak::bigint &operator=(unsigned value) { return *this = uintmax_t(value); }
		lak::bigint &operator=(signed value) { return *this = intmax_t(value); }

		lak::result<uintmax_t> to_uintmax() const;
		lak::result<intmax_t> to_intmax() const;
		double to_double() const;

		bool is_negative() const;
		bool is_positive() const { return !is_negative(); }
		bool is_zero() const;
		bool is_big() const;

		uintmax_t bit_count() const;
		uintmax_t min_bit_count() const;
		unsigned bit(uintmax_t index) const;
		void set_bit(uintmax_t index, unsigned value);

		/* --- uintmax_t --- */

		[[nodiscard]] div_rem_result div_rem(uintmax_t rhs) const;

		lak::bigint &operator+=(uintmax_t rhs);
		lak::bigint &operator-=(uintmax_t rhs);
		lak::bigint &operator*=(uintmax_t rhs);
		lak::bigint &operator/=(uintmax_t rhs);
		lak::bigint &operator%=(uintmax_t rhs);
		lak::bigint &operator<<=(uintmax_t rhs);
		lak::bigint &operator>>=(uintmax_t rhs);

		lak::bigint operator+(uintmax_t rhs) &&;
		lak::bigint operator-(uintmax_t rhs) &&;
		lak::bigint operator*(uintmax_t rhs) &&;
		lak::bigint operator/(uintmax_t rhs) &&;
		lak::bigint operator%(uintmax_t rhs) &&;
		lak::bigint operator<<(uintmax_t rhs) &&;
		lak::bigint operator>>(uintmax_t rhs) &&;

		lak::bigint operator+(uintmax_t rhs) const &;
		lak::bigint operator-(uintmax_t rhs) const &;
		lak::bigint operator*(uintmax_t rhs) const &;
		lak::bigint operator/(uintmax_t rhs) const &;
		lak::bigint operator%(uintmax_t rhs) const &;
		lak::bigint operator<<(uintmax_t rhs) const &;
		lak::bigint operator>>(uintmax_t rhs) const &;

		lak::strong_ordering operator<=>(uintmax_t rhs) const;
		bool operator<(uintmax_t rhs) const { return operator<=>(rhs) < 0; }
		bool operator>(uintmax_t rhs) const { return operator<=>(rhs) > 0; }
		bool operator==(uintmax_t rhs) const { return operator<=>(rhs) == 0; }
		bool operator!=(uintmax_t rhs) const { return operator<=>(rhs) != 0; }

		/* --- intmax_t --- */

		[[nodiscard]] div_rem_result div_rem(intmax_t rhs) const;

		lak::bigint &operator+=(intmax_t rhs);
		lak::bigint &operator-=(intmax_t rhs);
		lak::bigint &operator*=(intmax_t rhs);
		lak::bigint &operator/=(intmax_t rhs);
		lak::bigint &operator%=(intmax_t rhs);

		lak::bigint operator+(intmax_t rhs) &&;
		lak::bigint operator-(intmax_t rhs) &&;
		lak::bigint operator*(intmax_t rhs) &&;
		lak::bigint operator/(intmax_t rhs) &&;
		lak::bigint operator%(intmax_t rhs) &&;

		lak::bigint operator+(intmax_t rhs) const &;
		lak::bigint operator-(intmax_t rhs) const &;
		lak::bigint operator*(intmax_t rhs) const &;
		lak::bigint operator/(intmax_t rhs) const &;
		lak::bigint operator%(intmax_t rhs) const &;

		lak::strong_ordering operator<=>(intmax_t rhs) const;
		bool operator<(intmax_t rhs) const { return operator<=>(rhs) < 0; }
		bool operator>(intmax_t rhs) const { return operator<=>(rhs) > 0; }
		bool operator==(intmax_t rhs) const { return operator<=>(rhs) == 0; }
		bool operator!=(intmax_t rhs) const { return operator<=>(rhs) != 0; }

		/* --- unsigned --- */

		lak::bigint &operator+=(unsigned rhs)
		{
			return operator+=(uintmax_t(rhs));
		}
		lak::bigint &operator-=(unsigned rhs)
		{
			return operator-=(uintmax_t(rhs));
		}
		lak::bigint &operator*=(unsigned rhs)
		{
			return operator*=(uintmax_t(rhs));
		}
		lak::bigint &operator/=(unsigned rhs)
		{
			return operator/=(uintmax_t(rhs));
		}
		lak::bigint &operator%=(unsigned rhs)
		{
			return operator%=(uintmax_t(rhs));
		}
		lak::bigint &operator<<=(unsigned rhs)
		{
			return operator<<=(uintmax_t(rhs));
		}
		lak::bigint &operator>>=(unsigned rhs)
		{
			return operator>>=(uintmax_t(rhs));
		}

		lak::bigint operator+(unsigned rhs) &&
		{
			return operator+(uintmax_t(rhs));
		}
		lak::bigint operator-(unsigned rhs) &&
		{
			return operator-(uintmax_t(rhs));
		}
		lak::bigint operator*(unsigned rhs) &&
		{
			return operator*(uintmax_t(rhs));
		}
		lak::bigint operator/(unsigned rhs) &&
		{
			return operator/(uintmax_t(rhs));
		}
		lak::bigint operator%(unsigned rhs) &&
		{
			return operator%(uintmax_t(rhs));
		}
		lak::bigint operator<<(unsigned rhs) &&
		{
			return operator<<(uintmax_t(rhs));
		}
		lak::bigint operator>>(unsigned rhs) &&
		{
			return operator>>(uintmax_t(rhs));
		}

		lak::bigint operator+(unsigned rhs) const &
		{
			return operator+(uintmax_t(rhs));
		}
		lak::bigint operator-(unsigned rhs) const &
		{
			return operator-(uintmax_t(rhs));
		}
		lak::bigint operator*(unsigned rhs) const &
		{
			return operator*(uintmax_t(rhs));
		}
		lak::bigint operator/(unsigned rhs) const &
		{
			return operator/(uintmax_t(rhs));
		}
		lak::bigint operator%(unsigned rhs) const &
		{
			return operator%(uintmax_t(rhs));
		}
		lak::bigint operator<<(unsigned rhs) const &
		{
			return operator<<(uintmax_t(rhs));
		}
		lak::bigint operator>>(unsigned rhs) const &
		{
			return operator>>(uintmax_t(rhs));
		}

		lak::strong_ordering operator<=>(unsigned rhs) const
		{
			return operator<=>(uintmax_t(rhs));
		}
		bool operator<(unsigned rhs) const { return operator<=>(rhs) < 0; }
		bool operator>(unsigned rhs) const { return operator<=>(rhs) > 0; }
		bool operator==(unsigned rhs) const { return operator<=>(rhs) == 0; }
		bool operator!=(unsigned rhs) const { return operator<=>(rhs) != 0; }

		/* --- signed --- */

		lak::bigint &operator+=(signed rhs) { return operator+=(intmax_t(rhs)); }
		lak::bigint &operator-=(signed rhs) { return operator-=(intmax_t(rhs)); }
		lak::bigint &operator*=(signed rhs) { return operator*=(intmax_t(rhs)); }
		lak::bigint &operator/=(signed rhs) { return operator/=(intmax_t(rhs)); }
		lak::bigint &operator%=(signed rhs) { return operator%=(intmax_t(rhs)); }

		lak::bigint operator+(signed rhs) && { return operator+(intmax_t(rhs)); }
		lak::bigint operator-(signed rhs) && { return operator-(intmax_t(rhs)); }
		lak::bigint operator*(signed rhs) && { return operator*(intmax_t(rhs)); }
		lak::bigint operator/(signed rhs) && { return operator/(intmax_t(rhs)); }
		lak::bigint operator%(signed rhs) && { return operator%(intmax_t(rhs)); }

		lak::bigint operator+(signed rhs) const &
		{
			return operator+(intmax_t(rhs));
		}
		lak::bigint operator-(signed rhs) const &
		{
			return operator-(intmax_t(rhs));
		}
		lak::bigint operator*(signed rhs) const &
		{
			return operator*(intmax_t(rhs));
		}
		lak::bigint operator/(signed rhs) const &
		{
			return operator/(intmax_t(rhs));
		}
		lak::bigint operator%(signed rhs) const &
		{
			return operator%(intmax_t(rhs));
		}

		lak::strong_ordering operator<=>(signed rhs) const
		{
			return operator<=>(intmax_t(rhs));
		}
		bool operator<(signed rhs) const { return operator<=>(rhs) < 0; }
		bool operator>(signed rhs) const { return operator<=>(rhs) > 0; }
		bool operator==(signed rhs) const { return operator<=>(rhs) == 0; }
		bool operator!=(signed rhs) const { return operator<=>(rhs) != 0; }

		/* --- bigint --- */

		[[nodiscard]] div_rem_result div_rem(const lak::bigint &rhs) const;

		lak::bigint &operator+=(const lak::bigint &rhs);
		lak::bigint &operator-=(const lak::bigint &rhs);
		lak::bigint &operator*=(const lak::bigint &rhs);
		lak::bigint &operator/=(const lak::bigint &rhs);
		lak::bigint &operator%=(const lak::bigint &rhs);
		lak::bigint &operator<<=(const lak::bigint &rhs);
		lak::bigint &operator>>=(const lak::bigint &rhs);

		lak::bigint operator+(lak::bigint &&rhs) const &;
		lak::bigint operator*(lak::bigint &&rhs) const &;

		lak::bigint operator+(lak::bigint &&rhs) && { return operator+(rhs); }
		lak::bigint operator*(lak::bigint &&rhs) && { return operator*(rhs); }

		lak::bigint operator+(const lak::bigint &rhs) &&;
		lak::bigint operator-(const lak::bigint &rhs) &&;
		lak::bigint operator*(const lak::bigint &rhs) &&;
		lak::bigint operator/(const lak::bigint &rhs) &&;
		lak::bigint operator%(const lak::bigint &rhs) &&;
		lak::bigint operator<<(const lak::bigint &rhs) &&;
		lak::bigint operator>>(const lak::bigint &rhs) &&;

		lak::bigint operator+(const lak::bigint &rhs) const &;
		lak::bigint operator-(const lak::bigint &rhs) const &;
		lak::bigint operator*(const lak::bigint &rhs) const &;
		lak::bigint operator/(const lak::bigint &rhs) const &;
		lak::bigint operator%(const lak::bigint &rhs) const &;
		lak::bigint operator<<(const lak::bigint &rhs) const &;
		lak::bigint operator>>(const lak::bigint &rhs) const &;

		lak::strong_ordering operator<=>(const lak::bigint &rhs) const;
		bool operator<(const lak::bigint &) const  = default;
		bool operator>(const lak::bigint &) const  = default;
		bool operator==(const lak::bigint &) const = default;
		bool operator!=(const lak::bigint &) const = default;

		/* --- unary --- */

		lak::bigint operator-() const &;
		lak::bigint operator-() &&;

		friend std::ostream &operator<<(std::ostream &strm, const lak::bigint &val)
		{
			const auto old_width = strm.width();
			DEFER(strm.width(old_width));
			const auto old_base = strm.flags(std::ostream::basefield);
			DEFER(strm.setf(old_base, std::ostream::basefield));

			strm << (val.is_negative() ? '-' : '+');
			strm << std::hex << std::setfill('0');

			for (size_t i = val._data.size(); i-- > 0U;)
			{
				strm << std::setw(sizeof(uintmax_t) * 2U) << val._data[i];
				if (i != 0U) strm << '\'';
			}

			return strm;
		}
	};
}

#endif
