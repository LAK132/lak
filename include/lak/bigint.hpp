#ifndef LAK_BIGINT_HPP
#define LAK_BIGINT_HPP

#include "lak/array.hpp"
#include "lak/compare.hpp"
#include "lak/defer.hpp"
#include "lak/format.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"
#include "lak/stdint.hpp"

#include <sstream>

namespace lak
{
	struct bigint
	{
#ifndef LAK_BIGINT_STANDALONE_HPP
		template<typename T>
		using _array = lak::array<T>;
		template<typename T, typename U>
		using _pair = lak::pair<T, U>;
#else
		template<typename T>
		using _array = std::vector<T>;
		template<typename T, typename U>
		using _pair = std::pair<T, U>;
#endif

		using div_rem_result = _pair<lak::bigint, lak::bigint>;

		using value_type = uint32_t;

	private:
		bool _negative = false;
		_array<value_type> _data;

		lak::bigint &negate();
		void reserve(size_t count);
		void normalise(size_t min_count = 0U);
		size_t min_size() const;

		lak::span<const value_type> min_span() const;

		// ignores _negative
		void add(uintmax_t value);
		void sub(uintmax_t value); // must not overflow
		[[nodiscard]] div_rem_result div_rem_impl(uintmax_t value,
		                                          bool negate_result) const;
		void add(const lak::bigint &value);
		void sub(const lak::bigint &value); // must not overflow
		[[nodiscard]] div_rem_result div_rem_impl(const lak::bigint &value,
		                                          bool negate_result) const;

		void add(lak::span<const value_type> value, size_t offset);
		void sub(lak::span<const value_type> value); // must not overflow
		void mul(value_type value, size_t offset);
		void mul(lak::span<const value_type> value);

		lak::result<uintmax_t> to_uintmax_ignore_sign() const;

		template<typename T, typename CHAR>
		friend struct format_traits;

	public:
		bigint()                                    = default;
		bigint(const lak::bigint &)                 = default;
		bigint(lak::bigint &&)                      = default;
		lak::bigint &operator=(const lak::bigint &) = default;
		lak::bigint &operator=(lak::bigint &&)      = default;

		bigint(uintmax_t value);
		bigint(intmax_t value);
		bigint(unsigned value) : bigint(uintmax_t(value)) {}
		bigint(signed value) : bigint(intmax_t(value)) {}

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

		// false = will fit in a uintmax_t
		// true = probably won't fit in a uintmax_t
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
	};

	template<typename CHAR>
	struct format_traits<lak::bigint, CHAR>
	{
		using format_args =
		  typename lak::format_traits<int32_t, CHAR>::format_args;

		static consteval format_args parse_args(lak::string_view<CHAR> args)
		{
			return lak::format_traits<int32_t, CHAR>::parse_args(args);
		}

		static constexpr lak::string<CHAR> to_string(const format_args &args,
		                                             const lak::bigint &val)
		{
			std::stringstream strm;
			const auto base = lak::numeric_base::hex;
			switch (base)
			{
				case lak::numeric_base::dec: strm << std::dec; break;
				case lak::numeric_base::hex: strm << std::hex; break;
				case lak::numeric_base::oct: strm << std::oct; break;
				default:                     break;
			}
			strm << std::noshowbase;
			if (args.uppercase) strm << std::uppercase;

			if (val.is_negative())
				strm << "-";
			else if (args.force_sign)
				strm << "+";
			else if (args.sign_pad)
				strm << " ";

			if (args.show_base)
			{
				switch (base)
				{
					case lak::numeric_base::dec: break;
					case lak::numeric_base::hex: strm << "0x"; break;
					case lak::numeric_base::oct: strm << "0"; break;
					case lak::numeric_base::bin: strm << "0b"; break;
				}
			}

			strm << std::setfill(args.fill);

			using value_type = lak::bigint::value_type;

			if (val._data.empty())
				strm << std::setw(args.precision) << value_type(0);
			else
			{
				strm << std::setw(args.precision -
				                  std::min<size_t>(args.precision,
				                                   sizeof(value_type) * 2U *
				                                     (val._data.size() - 1U)))
				     << val._data.back() << std::setfill('0');
				for (size_t i = val._data.size() - 1U; i-- > 0U;)
				{
					strm << '\'' << std::setw(sizeof(value_type) * 2U) << val._data[i];
				}
			}

			return lak::strconv<CHAR>(lak::astring_view(strm.view()));
		}
	};
}

#endif
