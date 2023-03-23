#include "lak/bigint.hpp"

#include "lak/debug.hpp"

#include "lak/array.hpp"
#include "lak/span_manip.hpp"
#include "lak/wide_math.hpp"

#include <algorithm>
#include <bit>
#include <cmath>

bool is_negative(uintmax_t v) { return (v & (~(UINTMAX_MAX >> 1U))) != 0U; }

lak::stack_array<lak::bigint::value_type,
                 lak::ceil_div<size_t>(sizeof(uintmax_t),
                                       sizeof(lak::bigint::value_type))>
split_umax(uintmax_t v)
{
	lak::stack_array<lak::bigint::value_type,
	                 lak::ceil_div<size_t>(sizeof(uintmax_t),
	                                       sizeof(lak::bigint::value_type))>
	  result;

	for (size_t i = 0; i < result.max_size(); ++i)
	{
		result.push_back(static_cast<lak::bigint::value_type>(
		  v >> (sizeof(lak::bigint::value_type) * CHAR_BIT * i)));
	}
	while (!result.empty() && result.back() == 0U) result.pop_back();

	return result;
}

lak::bigint &lak::bigint::negate()
{
	_negative = !_negative;
	return *this;
}

void lak::bigint::reserve(size_t count)
{
	if (_data.size() >= count)
		normalise(count);
	else
		_data.resize(count, 0U);
}

void lak::bigint::normalise(size_t min_count)
{
	if (_data.empty()) return;
	if (min_count < 1U) min_count = 1U;
	while (_data.size() > min_count && _data.back() == 0U) _data.pop_back();
}

size_t lak::bigint::min_size() const
{
	size_t result = _data.size();
	while (result > 0U && _data[result - 1U] == 0U) --result;
	return result;
}

lak::span<const lak::bigint::value_type> lak::bigint::min_span() const
{
	return lak::span(_data).first(min_size());
}

void lak::bigint::add(uintmax_t value) { add(split_umax(value), 0U); }

void lak::bigint::sub(uintmax_t value) { sub(split_umax(value)); }

lak::bigint::div_rem_result lak::bigint::div_rem_impl(uintmax_t value,
                                                      bool negate_output) const
{
	ASSERT_NOT_EQUAL(value, 0U);

	lak::bigint::div_rem_result result;

	const auto bits{min_bit_count()};
	for (auto i{bits}; i-- > 0U;)
	{
		result.second <<= 1U;
		result.second.set_bit(0U, bit(i));
		if (result.second >= value)
		{
			result.second -= value;
			result.first.set_bit(i, 1U);
		}
	}

	while (result.second >= value)
	{
		result.second -= value;
		result.first += 1U;
	}

	result.first.normalise();
	result.second.normalise();

	if (negate_output)
	{
		result.first.negate();
		result.second.negate();
	}

	return result;
}

void lak::bigint::add(const lak::bigint &value) { add(value.min_span(), 0U); }

void lak::bigint::sub(const lak::bigint &value) { sub(value.min_span()); }

lak::bigint::div_rem_result lak::bigint::div_rem_impl(const lak::bigint &value,
                                                      bool negate_output) const
{
	ASSERT_NOT_EQUAL(value, 0U);

	lak::bigint::div_rem_result result;

	for (uintmax_t i = bit_count(); i-- > 0U;)
	{
		result.second <<= 1U;
		result.second.set_bit(0U, bit(i));
		if (result.second >= value)
		{
			result.second -= value;
			result.first.set_bit(i, 1U);
		}
	}

	result.first.normalise();
	result.second.normalise();

	if (negate_output)
	{
		result.first.negate();
		result.second.negate();
	}

	return result;
}

void lak::bigint::add(lak::span<const value_type> value, size_t offset)
{
	reserve(value.size() + offset);

	const auto carry{
	  lak::add_carry_u32(lak::span(_data).subspan(offset), value)};

	if (carry != 0U) _data.push_back(carry);
}

void lak::bigint::sub(lak::span<const value_type> value)
{
	const size_t l_max = min_size();
	const size_t r_max = value.size();
	ASSERT_GREATER_OR_EQUAL(l_max, r_max);

	for (size_t i = 0U; i < r_max; ++i)
	{
		unsigned char carry = lak::sub_carry_u32(_data[i], value[i], &_data[i]);

		for (size_t carry_i = i + 1U; carry_i < l_max && carry != 0U; ++carry_i)
			carry = lak::sub_carry_u32(_data[carry_i], 0U, &_data[carry_i], carry);

		ASSERT_EQUAL(carry, 0U);
	}

	normalise();
}

void lak::bigint::mul(value_type value, size_t offset)
{
	if (offset != 0U)
	{
		_data.resize(_data.size() + offset);
		lak::shift_right<value_type>(_data, offset);
		lak::fill<value_type>(lak::span(_data).first(offset), 0U);
	}

	value_type carry = 0U;
	for (size_t i = offset; i < _data.size(); ++i)
		carry = lak::mul_carry_u32(_data[i], value, &_data[i], carry);
	if (carry != 0) _data.push_back(carry);
}

void lak::bigint::mul(lak::span<const value_type> value)
{
	if (value.empty() || is_zero())
	{
		_data.clear();
		return;
	}

	_data.reserve(_data.size() + value.size());

	auto mspan{min_span()};

	lak::bigint result;
	{
		lak::bigint copy;
		copy._data.reserve(mspan.size() + value.size());

		for (size_t i = 0U; i < value.size(); ++i)
		{
			copy._data.resize(mspan.size() + i);
			lak::fill(lak::span(copy._data).first(i), 0U);
			lak::copy(_data.begin(), _data.end(), copy._data.begin() + i);

			copy.mul(value[i], i);
			result += copy;
		}
	}

	lak::swap(_data, result._data);
}

lak::result<uintmax_t> lak::bigint::to_uintmax_ignore_sign() const
{
	auto mspan{min_span()};
	if (mspan.size() * sizeof(value_type) > sizeof(uintmax_t))
		return lak::err_t{};

	uintmax_t result = 0U;
	size_t i         = 0U;
	for (const auto &v : mspan)
		result |= static_cast<uintmax_t>(v)
		          << (sizeof(value_type) * CHAR_BIT * i++);

	return lak::ok_t<uintmax_t>{result};
}

lak::bigint::bigint(uintmax_t value)
{
	if (value != 0U)
	{
		auto split{split_umax(value)};
		_data.resize(split.size());
		for (size_t i = 0U; i < _data.size(); ++i) _data[i] = split[i];
	}
}

lak::bigint::bigint(intmax_t value)
{
	if (value >= 0U)
		*this = static_cast<uintmax_t>(value);
	else
		(*this = static_cast<uintmax_t>(-value)).negate();
}

lak::bigint &lak::bigint::operator=(uintmax_t value)
{
	if (value != 0U)
	{
		auto split{split_umax(value)};
		_data.resize(split.size());
		for (size_t i = 0U; i < _data.size(); ++i) _data[i] = split[i];
	}
	else
		_data.clear();
	return *this;
}

lak::bigint &lak::bigint::operator=(intmax_t value)
{
	if (value >= 0U)
		*this = static_cast<uintmax_t>(value);
	else
		(*this = static_cast<uintmax_t>(-value)).negate();
	return *this;
}

lak::result<uintmax_t> lak::bigint::to_uintmax() const
{
	if (is_negative()) return lak::err_t{};
	return to_uintmax_ignore_sign();
}

lak::result<intmax_t> lak::bigint::to_intmax() const
{
	if_let_ok (uintmax_t uvalue, to_uintmax_ignore_sign())
	{
		if (uvalue > INTMAX_MAX) return lak::err_t{};
		intmax_t result = static_cast<intmax_t>(uvalue);
		return lak::ok_t<intmax_t>{is_negative() ? -result : result};
	}
	return lak::err_t{};
}

double lak::bigint::to_double() const
{
	const auto bits{min_bit_count()};
	uintmax_t v             = 0U;
	constexpr size_t v_bits = sizeof(v) * CHAR_BIT;
	const auto max_bits{std::max<uintmax_t>(bits, v_bits)};
	const auto min_bits{static_cast<size_t>(std::min<uintmax_t>(bits, v_bits))};

	for (size_t i = 0U; i < min_bits; ++i)
		v = (v << 1U) | (bit(bits - (i + 1U)));

	double result = static_cast<double>(v) * std::pow(2.0, max_bits - v_bits);

	return is_negative() ? -result : result;
}

bool lak::bigint::is_negative() const { return _negative; }

bool lak::bigint::is_zero() const
{
	for (const auto &v : _data)
		if (v != 0U) return false;
	return true;
}

bool lak::bigint::is_big() const
{
	return min_size() * sizeof(value_type) > sizeof(uintmax_t);
}

uintmax_t lak::bigint::bit_count() const
{
	return _data.size() * sizeof(value_type) * CHAR_BIT;
}

uintmax_t lak::bigint::min_bit_count() const
{
	size_t min_sz = min_size();
	if (min_sz == 0U) return 0U;
	return (min_sz * sizeof(value_type) * CHAR_BIT) -
	       std::countl_zero(_data[min_sz - 1U]);
}

unsigned lak::bigint::bit(uintmax_t index) const
{
	const size_t uint_index =
	  static_cast<size_t>(index / (sizeof(value_type) * CHAR_BIT));
	const size_t offset =
	  static_cast<size_t>(index % (sizeof(value_type) * CHAR_BIT));
	return (_data[uint_index] >> offset) & 1U;
}

void lak::bigint::set_bit(uintmax_t index, unsigned value)
{
	const uintmax_t bit_index = index % (sizeof(value_type) * CHAR_BIT);
	const uintmax_t bit_mask  = ~(uintmax_t(1U) << bit_index);
	const size_t uint_index =
	  static_cast<size_t>(index / (sizeof(value_type) * CHAR_BIT));

	if (uint_index >= _data.size()) reserve(uint_index + 1U);

	value_type &v = _data[uint_index];

	v = (v & bit_mask) | (value_type(value & 1U) << bit_index);
}

/* --- uintmax_t --- */

lak::bigint::div_rem_result lak::bigint::div_rem(uintmax_t rhs) const
{
	return div_rem_impl(rhs, is_negative());
}

lak::bigint &lak::bigint::operator+=(uintmax_t rhs)
{
	normalise(1U);

	if (is_negative())
	{
		if_let_ok (uintmax_t v, to_uintmax_ignore_sign())
		{
			if (v > rhs)
				sub(rhs);
			else
				*this = static_cast<uintmax_t>(rhs - v);
		}
		else
		{
			ASSERT(is_big());
			sub(rhs);
		}
	}
	else
		add(rhs);

	return *this;
}

lak::bigint &lak::bigint::operator-=(uintmax_t rhs)
{
	normalise(1U);

	if (is_positive())
	{
		if_let_ok (uintmax_t v, to_uintmax_ignore_sign())
		{
			if (v >= rhs)
				sub(rhs);
			else
			{
				*this     = static_cast<uintmax_t>(rhs - v);
				_negative = true;
			}
		}
		else
		{
			ASSERT(is_big());
			sub(rhs);
		}
	}
	else
		add(rhs);

	return *this;
}

lak::bigint &lak::bigint::operator*=(uintmax_t rhs)
{
	normalise();

	mul(split_umax(rhs));

	normalise();

	return *this;
}

lak::bigint &lak::bigint::operator/=(uintmax_t rhs)
{
	*this = div_rem(rhs).first;
	return *this;
}

lak::bigint &lak::bigint::operator%=(uintmax_t rhs)
{
	*this = div_rem(rhs).second;
	return *this;
}

lak::bigint &lak::bigint::operator<<=(uintmax_t rhs)
{
	if (rhs == 0) return *this;

	const size_t whole_shift =
	  static_cast<size_t>(rhs / (sizeof(value_type) * CHAR_BIT));
	const uintmax_t bit_shift = rhs % (sizeof(value_type) * CHAR_BIT);
	const size_t old_size     = _data.size();

	reserve(old_size + whole_shift + 1U);

	if (bit_shift == 0)
	{
		// simple memmove
		lak::memmove(reinterpret_cast<byte_t *>(_data.data() + whole_shift),
		             reinterpret_cast<const byte_t *>(_data.data()),
		             old_size);
	}
	else
	{
		const uintmax_t high_mask = UINTMAX_MAX << bit_shift;
		const uintmax_t low_mask  = ~high_mask;
		for (size_t i = old_size; i-- > 0;)
		{
			value_type value       = _data[i];
			_data[i + whole_shift] = value << bit_shift;
			_data[i + whole_shift + 1U] =
			  (_data[i + whole_shift + 1U] & high_mask) |
			  (value >> ((sizeof(value_type) * CHAR_BIT) - bit_shift));
		}
	}

	lak::fill(lak::span(_data).first(whole_shift), value_type(0));

	normalise();

	return *this;
}

lak::bigint &lak::bigint::operator>>=(uintmax_t rhs)
{
	if (rhs == 0) return *this;

	const size_t whole_shift =
	  static_cast<size_t>(rhs / (sizeof(value_type) * CHAR_BIT));
	const uintmax_t bit_shift = rhs % (sizeof(value_type) * CHAR_BIT);
	const size_t new_size     = _data.size() - whole_shift;

	reserve(new_size + 1U);

	if (bit_shift == 0)
	{
		// simple memmove
		lak::memmove(reinterpret_cast<byte_t *>(_data.data()),
		             reinterpret_cast<const byte_t *>(_data.data() + whole_shift),
		             new_size);
	}
	else
	{
		const uintmax_t low_mask  = UINTMAX_MAX >> bit_shift;
		const uintmax_t high_mask = ~low_mask;
		for (size_t i = 0; i < new_size; ++i)
		{
			value_type value = _data[i + whole_shift];
			_data[i]         = (_data[i] & high_mask) | (value >> bit_shift);
			_data[i + 1U] = (value << ((sizeof(value_type) * CHAR_BIT) - bit_shift));
		}
	}

	_data.resize(new_size);

	normalise();

	return *this;
}

lak::bigint lak::bigint::operator+(uintmax_t rhs) && { return *this += rhs; }

lak::bigint lak::bigint::operator-(uintmax_t rhs) && { return *this -= rhs; }

lak::bigint lak::bigint::operator*(uintmax_t rhs) && { return *this *= rhs; }

lak::bigint lak::bigint::operator/(uintmax_t rhs) && { return *this /= rhs; }

lak::bigint lak::bigint::operator%(uintmax_t rhs) && { return *this %= rhs; }

lak::bigint lak::bigint::operator<<(uintmax_t rhs) && { return *this <<= rhs; }

lak::bigint lak::bigint::operator>>(uintmax_t rhs) && { return *this >>= rhs; }

lak::bigint lak::bigint::operator+(uintmax_t rhs) const &
{
	lak::bigint result = *this;
	result += rhs;
	return result;
}

lak::bigint lak::bigint::operator-(uintmax_t rhs) const &
{
	lak::bigint result = *this;
	result -= rhs;
	return result;
}

lak::bigint lak::bigint::operator*(uintmax_t rhs) const &
{
	lak::bigint result = *this;
	result *= rhs;
	return result;
}

lak::bigint lak::bigint::operator/(uintmax_t rhs) const &
{
	lak::bigint result = *this;
	result /= rhs;
	return result;
}

lak::bigint lak::bigint::operator%(uintmax_t rhs) const &
{
	lak::bigint result = *this;
	result %= rhs;
	return result;
}

lak::bigint lak::bigint::operator<<(uintmax_t rhs) const &
{
	lak::bigint result = *this;
	result <<= rhs;
	return result;
}

lak::bigint lak::bigint::operator>>(uintmax_t rhs) const &
{
	lak::bigint result = *this;
	result >>= rhs;
	return result;
}

lak::strong_ordering lak::bigint::operator<=>(uintmax_t rhs) const
{
	if_let_ok (uintmax_t lhs_value, to_uintmax())
	{
		if (lhs_value < rhs) return lak::strong_ordering::less;
		if (lhs_value > rhs) return lak::strong_ordering::greater;
		return lak::strong_ordering::equal;
	}
	else
		return is_negative() ? lak::strong_ordering::less
		                     : lak::strong_ordering::greater;
}

/* --- intmax_t --- */

lak::bigint::div_rem_result lak::bigint::div_rem(intmax_t rhs) const
{
	return div_rem_impl(rhs >= 0 ? static_cast<uintmax_t>(rhs)
	                             : static_cast<uintmax_t>(-rhs),
	                    is_negative() != (rhs < 0));
}

lak::bigint &lak::bigint::operator+=(intmax_t rhs)
{
	return rhs >= 0 ? *this += static_cast<uintmax_t>(rhs)
	                : *this -= static_cast<uintmax_t>(-rhs);
}

lak::bigint &lak::bigint::operator-=(intmax_t rhs)
{
	return rhs >= 0 ? *this -= static_cast<uintmax_t>(rhs)
	                : *this += static_cast<uintmax_t>(-rhs);
}

lak::bigint &lak::bigint::operator*=(intmax_t rhs)
{
	return rhs >= 0 ? *this *= static_cast<uintmax_t>(rhs)
	                : (*this *= static_cast<uintmax_t>(-rhs)).negate();
}

lak::bigint &lak::bigint::operator/=(intmax_t rhs)
{
	*this = div_rem(rhs).first;
	return *this;
}

lak::bigint &lak::bigint::operator%=(intmax_t rhs)
{
	*this = div_rem(rhs).second;
	return *this;
}

lak::bigint lak::bigint::operator+(intmax_t rhs) && { return *this += rhs; }

lak::bigint lak::bigint::operator-(intmax_t rhs) && { return *this -= rhs; }

lak::bigint lak::bigint::operator*(intmax_t rhs) && { return *this *= rhs; }

lak::bigint lak::bigint::operator/(intmax_t rhs) && { return *this /= rhs; }

lak::bigint lak::bigint::operator%(intmax_t rhs) && { return *this %= rhs; }

lak::bigint lak::bigint::operator+(intmax_t rhs) const &
{
	lak::bigint result = *this;
	result += rhs;
	return result;
}

lak::bigint lak::bigint::operator-(intmax_t rhs) const &
{
	lak::bigint result = *this;
	result -= rhs;
	return result;
}

lak::bigint lak::bigint::operator*(intmax_t rhs) const &
{
	lak::bigint result = *this;
	result *= rhs;
	return result;
}

lak::bigint lak::bigint::operator/(intmax_t rhs) const &
{
	lak::bigint result = *this;
	result /= rhs;
	return result;
}

lak::bigint lak::bigint::operator%(intmax_t rhs) const &
{
	lak::bigint result = *this;
	result %= rhs;
	return result;
}

lak::strong_ordering lak::bigint::operator<=>(intmax_t rhs) const
{
	if_let_ok (intmax_t lhs_value, to_intmax())
	{
		if (lhs_value < rhs) return lak::strong_ordering::less;
		if (lhs_value > rhs) return lak::strong_ordering::greater;
		return lak::strong_ordering::equal;
	}
	else
		return is_negative() ? lak::strong_ordering::less
		                     : lak::strong_ordering::greater;
}

/* --- bigint --- */

lak::bigint::div_rem_result lak::bigint::div_rem(const lak::bigint &rhs) const
{
	return div_rem_impl(rhs, is_negative() != rhs.is_negative());
}

lak::bigint &lak::bigint::operator+=(const lak::bigint &rhs)
{
	if (is_negative() == rhs.is_negative())
	{
		add(rhs);
	}
	else
	{
		negate(); // signs have to be the same for the comparison to work
		if (is_negative() ? *this <= rhs : *this >= rhs)
		{
			negate();
			sub(rhs);
		}
		else
			*this = rhs - *this;
	}

	return *this;
}

lak::bigint &lak::bigint::operator-=(const lak::bigint &rhs)
{
	if (is_negative() == rhs.is_negative())
	{
		if (is_negative() ? *this <= rhs : *this >= rhs)
			sub(rhs);
		else
			*this = rhs - *this;
	}
	else
		add(rhs);

	return *this;
}

lak::bigint &lak::bigint::operator*=(const lak::bigint &rhs)
{
	normalise();

	_negative = is_negative() != rhs.is_negative();

	mul(rhs.min_span());

	normalise();

	return *this;
}

lak::bigint &lak::bigint::operator/=(const lak::bigint &rhs)
{
	*this = lak::move(*this).div_rem(rhs).first;
	return *this;
}

lak::bigint &lak::bigint::operator%=(const lak::bigint &rhs)
{
	*this = lak::move(*this).div_rem(rhs).second;
	return *this;
}

lak::bigint &lak::bigint::operator<<=(const lak::bigint &rhs)
{
	if (rhs.is_zero()) return *this;
	if (rhs.is_negative()) return *this >>= -rhs;
	return *this <<= to_uintmax().UNWRAP();
}

lak::bigint &lak::bigint::operator>>=(const lak::bigint &rhs)
{
	if (rhs.is_zero()) return *this;
	if (rhs.is_negative()) return *this <<= -rhs;
	return *this >>= to_uintmax().UNWRAP();
}

lak::bigint lak::bigint::operator+(lak::bigint &&rhs) const &
{
	return rhs += *this;
}

lak::bigint lak::bigint::operator*(lak::bigint &&rhs) const &
{
	return rhs *= *this;
}

lak::bigint lak::bigint::operator+(const lak::bigint &rhs) &&
{
	return *this += rhs;
}

lak::bigint lak::bigint::operator-(const lak::bigint &rhs) &&
{
	return *this -= rhs;
}

lak::bigint lak::bigint::operator*(const lak::bigint &rhs) &&
{
	return *this *= rhs;
}

lak::bigint lak::bigint::operator/(const lak::bigint &rhs) &&
{
	return *this /= rhs;
}

lak::bigint lak::bigint::operator%(const lak::bigint &rhs) &&
{
	return *this %= rhs;
}

lak::bigint lak::bigint::operator<<(const lak::bigint &rhs) &&
{
	return *this <<= rhs;
}

lak::bigint lak::bigint::operator>>(const lak::bigint &rhs) &&
{
	return *this >>= rhs;
}

lak::bigint lak::bigint::operator+(const lak::bigint &rhs) const &
{
	lak::bigint result = *this;
	result += rhs;
	return result;
}

lak::bigint lak::bigint::operator-(const lak::bigint &rhs) const &
{
	lak::bigint result = *this;
	result -= rhs;
	return result;
}

lak::bigint lak::bigint::operator*(const lak::bigint &rhs) const &
{
	lak::bigint result = *this;
	result *= rhs;
	return result;
}

lak::bigint lak::bigint::operator/(const lak::bigint &rhs) const &
{
	lak::bigint result = *this;
	result /= rhs;
	return result;
}

lak::bigint lak::bigint::operator%(const lak::bigint &rhs) const &
{
	lak::bigint result = *this;
	result %= rhs;
	return result;
}

lak::bigint lak::bigint::operator<<(const lak::bigint &rhs) const &
{
	lak::bigint result = *this;
	result <<= rhs;
	return result;
}

lak::bigint lak::bigint::operator>>(const lak::bigint &rhs) const &
{
	lak::bigint result = *this;
	result >>= rhs;
	return result;
}

lak::strong_ordering lak::bigint::operator<=>(const lak::bigint &rhs) const
{
	if (is_negative())
	{
		if (rhs.is_negative())
		{
			auto l_span = min_span();
			auto r_span = rhs.min_span();
			if (l_span.size() > r_span.size()) return lak::strong_ordering::less;
			if (l_span.size() < r_span.size()) return lak::strong_ordering::greater;
			for (size_t i = l_span.size(); i-- > 0;)
			{
				if (l_span[i] < r_span[i]) return lak::strong_ordering::less;
				if (l_span[i] > r_span[i]) return lak::strong_ordering::greater;
			}
			return lak::strong_ordering::equal;
		}
		else
		{
			return lak::strong_ordering::less;
		}
	}
	else
	{
		if (rhs.is_negative())
		{
			return lak::strong_ordering::greater;
		}
		else
		{
			auto l_span = min_span();
			auto r_span = rhs.min_span();
			if (l_span.size() < r_span.size()) return lak::strong_ordering::less;
			if (l_span.size() > r_span.size()) return lak::strong_ordering::greater;
			for (size_t i = l_span.size(); i-- > 0;)
			{
				if (l_span[i] < r_span[i]) return lak::strong_ordering::less;
				if (l_span[i] > r_span[i]) return lak::strong_ordering::greater;
			}
			return lak::strong_ordering::equal;
		}
	}
}

/* --- unary --- */

lak::bigint lak::bigint::operator-() const &
{
	lak::bigint result = *this;
	result.negate();
	return result;
}

lak::bigint lak::bigint::operator-() && { return negate(); }
