#include "lak/bigint.hpp"

#include "lak/debug.hpp"

#include "lak/span_manip.hpp"
#include "lak/wide_math.hpp"

bool is_negative(uintmax_t v) { return (v & (~(UINTMAX_MAX >> 1U))) != 0U; }

lak::bigint &lak::bigint::negate()
{
	_negative = !_negative;
	return *this;
}

void lak::bigint::reserve(size_t count)
{
	if (_data.size() >= count) return normalise(count);
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

lak::bigint::_span<const uintmax_t> lak::bigint::min_span() const
{
	return lak::span(_data).first(min_size());
}

void lak::bigint::add(uintmax_t value)
{
	reserve(1U);

	uintmax_t carry = 0U;
	{
		lak::uintmax2_t result = lak::add_uintmax2(_data[0U], value);
		_data[0U]              = result.low;
		carry                  = result.high;
	}
	for (size_t i = 1U; i < _data.size() && carry != 0U; ++i)
	{
		lak::uintmax2_t result = lak::add_uintmax2(_data[i], carry);
		_data[i]               = result.low;
		carry                  = result.high;
	}
	if (carry != 0U) _data.push_back(carry);

	normalise();
}

void lak::bigint::sub(uintmax_t value)
{
	uintmax_t carry = 0U;
	{
		lak::uintmax2_t result = lak::sub_uintmax2(_data[0U], value);
		_data[0U]              = result.low;
		carry                  = result.high;
	}
	for (size_t i = 1U; i < _data.size() && carry != 0U; ++i)
	{
		lak::uintmax2_t result = lak::sub_uintmax2(_data[i], 0U, carry);
		_data[i]               = result.low;
		carry                  = result.high;
	}
	ASSERT_EQUAL(carry, 0U);

	normalise();
}

lak::bigint::div_rem_result lak::bigint::div_rem_impl(uintmax_t value,
                                                      bool negate_output) const
{
	ASSERT_NOT_EQUAL(value, 0U);

	lak::bigint::div_rem_result result;

	for (uintmax_t i = min_bit_count(); i-- > 0U;)
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

void lak::bigint::add(const lak::bigint &value)
{
	reserve(value._data.size());

	uintmax_t carry = 0U;
	for (size_t i = 0; i < value._data.size(); ++i)
	{
		lak::uintmax2_t result =
		  lak::add_uintmax2(_data[i], value._data[i], carry);
		_data[i] = result.low;
		carry    = result.high;
	}
	for (size_t i = value._data.size(); i < _data.size(); ++i)
	{
		lak::uintmax2_t result = lak::add_uintmax2(_data[i], carry);
		_data[i]               = result.low;
		carry                  = result.high;
	}
	if (carry != 0U) _data.push_back(carry);

	normalise();
}

void lak::bigint::sub(const lak::bigint &value)
{
	const size_t l_max = min_size();
	const size_t r_max = value.min_size();
	ASSERT_GREATER_OR_EQUAL(l_max, r_max);

	for (size_t i = 0U; i < r_max; ++i)
	{
		uintmax_t carry = 0U;
		{
			lak::uintmax2_t result = lak::sub_uintmax2(_data[i], value._data[i]);
			_data[i]               = result.low;
			carry                  = result.high;
		}
		for (size_t carry_i = i; carry_i < l_max && carry != 0U; ++carry_i)
		{
			lak::uintmax2_t result = lak::sub_uintmax2(_data[carry_i], 0U, carry);
			_data[carry_i]         = result.low;
			carry                  = result.high;
		}
		ASSERT_EQUAL(carry, 0U);
	}

	normalise();
}

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

lak::bigint::bigint(uintmax_t value)
{
	if (value != 0U)
	{
		_data.resize(1U);
		_data[0U] = value;
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
		_data.resize(1U);
		_data[0U] = value;
	}
	else
		_data.empty();
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
	if (is_negative() || min_size() > 1U) return lak::err_t{};
	return lak::ok_t<uintmax_t>{_data.empty() ? 0U : _data[0U]};
}

lak::result<intmax_t> lak::bigint::to_intmax() const
{
	if (min_size() > 1U) return lak::err_t{};
	if (_data[0U] > static_cast<uintmax_t>(INTMAX_MAX)) return lak::err_t{};

	intmax_t result = static_cast<intmax_t>(_data.empty() ? 0U : _data[0U]);
	return lak::ok_t<intmax_t>{is_negative() ? -result : result};
}

double lak::bigint::to_double() const
{
	size_t i = _data.size();
	while (i > 0 && _data[--i] == 0)
		;
	uintmax_t v = _data[i];
	int lz      = 0;
	if (i > 0)
	{
		lz = std::countl_zero(v);
		if (lz > 0)
		{
			v <<= lz;
			v |= _data[i - 1] >> ((sizeof(uintmax_t) * CHAR_BIT) - lz);
		}
	}
	double result = static_cast<double>(v) *
	                std::pow(2.0, (i * sizeof(uintmax_t) * CHAR_BIT) - lz);
	return is_negative() ? -result : result;
}

bool lak::bigint::is_negative() const { return _negative; }

bool lak::bigint::is_zero() const
{
	for (const uintmax_t &v : _data)
		if (v != 0U) return false;
	return true;
}

bool lak::bigint::is_big() const { return min_size() > 1U; }

uintmax_t lak::bigint::bit_count() const
{
	return _data.size() * sizeof(uintmax_t) * CHAR_BIT;
}

uintmax_t lak::bigint::min_bit_count() const
{
	size_t min_sz = min_size();
	if (min_sz == 0U) return 0U;
	return (min_sz * sizeof(uintmax_t) * CHAR_BIT) -
	       std::countl_zero(_data[min_sz - 1U]);
}

unsigned lak::bigint::bit(uintmax_t index) const
{
	return (_data[index / (sizeof(uintmax_t) * CHAR_BIT)] >>
	        (index % (sizeof(uintmax_t) * CHAR_BIT))) &
	       1U;
}

void lak::bigint::set_bit(uintmax_t index, unsigned value)
{
	const uintmax_t bit_index = index % (sizeof(uintmax_t) * CHAR_BIT);
	const uintmax_t bit_mask  = ~(uintmax_t(1U) << bit_index);
	const size_t uint_index   = index / (sizeof(uintmax_t) * CHAR_BIT);

	if (uint_index >= _data.size()) reserve(uint_index + 1U);

	uintmax_t &v = _data[uint_index];

	v = (v & bit_mask) | (uintmax_t(value & 1U) << bit_index);
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
		if (_data.size() > 1 || _data[0U] >= rhs)
			sub(rhs);
		else
			*this = static_cast<uintmax_t>(rhs - _data[0U]);
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
		if (_data.size() > 1 || _data[0U] >= rhs)
			sub(rhs);
		else
		{
			*this     = static_cast<uintmax_t>(rhs - _data[0U]);
			_negative = true;
		}
	}
	else
		add(rhs);

	return *this;
}

lak::bigint &lak::bigint::operator*=(uintmax_t rhs)
{
	normalise();

	lak::uintmax2_t carry = {.high = 0U, .low = 0U};
	for (uintmax_t &v : _data)
	{
		lak::uintmax2_t mul = lak::mul_uintmax2(v, rhs);
		lak::uintmax2_t add = lak::add_uintmax2(mul.low, carry.low);
		v                   = add.low;
		carry               = lak::add_uintmax2(add.high, mul.high, carry.high);
	}

	if (carry.high != 0U || carry.low != 0U) _data.push_back(carry.low);
	if (carry.high != 0U) _data.push_back(carry.high);

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

	const size_t whole_shift  = rhs / (sizeof(uintmax_t) * CHAR_BIT);
	const uintmax_t bit_shift = rhs % (sizeof(uintmax_t) * CHAR_BIT);
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
			uintmax_t value        = _data[i];
			_data[i + whole_shift] = value << bit_shift;
			_data[i + whole_shift + 1U] =
			  (_data[i + whole_shift + 1U] & high_mask) |
			  (value >> ((sizeof(uintmax_t) * CHAR_BIT) - bit_shift));
		}
	}

	lak::fill(lak::span(_data).first(whole_shift), uintmax_t(0));

	normalise();

	return *this;
}

lak::bigint &lak::bigint::operator>>=(uintmax_t rhs)
{
	if (rhs == 0) return *this;

	const size_t whole_shift  = rhs / (sizeof(uintmax_t) * CHAR_BIT);
	const uintmax_t bit_shift = rhs % (sizeof(uintmax_t) * CHAR_BIT);
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
			uintmax_t value = _data[i + whole_shift];
			_data[i]        = (_data[i] & high_mask) | (value >> bit_shift);
			_data[i + 1U] = (value << ((sizeof(uintmax_t) * CHAR_BIT) - bit_shift));
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

	const lak::bigint::_span<const uintmax_t> rhs_span = rhs.min_span();
	const size_t rhs_size                              = rhs_span.size();
	const bool negative_result = is_negative() != rhs.is_negative();

	_negative = false;

	if (rhs_size == 0U || is_zero() || rhs.is_zero())
	{
		_data.clear();
		return *this;
	}

	_data.reserve(_data.size() + rhs_size);

	lak::bigint result;

	for (const uintmax_t &v : rhs_span)
	{
		result += *this * v;
		_data.insert(_data.begin(), 0U);
	}

	lak::swap(*this, result);

	_negative = negative_result;

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
