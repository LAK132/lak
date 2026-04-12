#include "lak/bit_reader.hpp"

template<lak::endian BYTE_ENDIAN, lak::endian BIT_ENDIAN>
inline void lak::bit_reader<BYTE_ENDIAN, BIT_ENDIAN>::flush_bits(
  const uint8_t bits)
{
	static_assert(BYTE_ENDIAN == lak::endian::little ||
	              BYTE_ENDIAN == lak::endian::big);
	if constexpr (BYTE_ENDIAN == lak::endian::little)
		_bit_accum >>= bits;
	else if constexpr (BYTE_ENDIAN == lak::endian::big)
		_bit_accum &= ~(UINTMAX_MAX << (_num_bits - bits));
	_num_bits -= bits;
}

template<lak::endian BYTE_ENDIAN, lak::endian BIT_ENDIAN>
inline void lak::bit_reader<BYTE_ENDIAN, BIT_ENDIAN>::accumulate_to(
  const uint8_t bits)
{
	static_assert(BYTE_ENDIAN == lak::endian::little ||
	              BYTE_ENDIAN == lak::endian::big);
	static_assert(BIT_ENDIAN == lak::endian::little ||
	              BIT_ENDIAN == lak::endian::big);

	auto get_bits = [&](uint8_t count) -> uintmax_t
	{
		auto get_lsb = [&]() -> uint8_t
		{
			return (uint8_t(_data[0U]) >> (8U - _unused_bits)) &
			       uint8_t(0xFF >> (8U - count));
		};

		auto get_msb = [&]() -> uint8_t
		{
			return (uint8_t(_data[0U]) & uint8_t(0xFF >> (8U - _unused_bits))) >>
			       (_unused_bits - count);
		};

		auto reverse = [&](uint8_t value) -> uint8_t
		{
			uint8_t result = 0U;
			for (uint8_t i = 0U; i < count; ++i)
			{
				result <<= 1U;
				result |= (value >> i) & 1U;
			}
			return result;
		};

		if constexpr (BIT_ENDIAN == lak::endian::little)
			return get_lsb();
		else if constexpr (BIT_ENDIAN == lak::endian::big)
			return reverse(get_msb());
	};

	auto next_byte = [&]()
	{
		_data = _data.subspan(1U);
		++_bytes_read;
		_unused_bits = 8U;
	};

	if (_unused_bits == 0U) next_byte();

	while (_num_bits < bits)
	{
		uint8_t bits_needed = bits - _num_bits;
		uint8_t bits_get    = std::min<uint8_t>(bits_needed, _unused_bits);
		if constexpr (BYTE_ENDIAN == lak::endian::little)
			_bit_accum = _bit_accum | (get_bits(bits_get) << _num_bits);
		else if constexpr (BYTE_ENDIAN == lak::endian::big)
			_bit_accum = (_bit_accum << bits_get) | get_bits(bits_get);
		_num_bits += bits_get;
		if (bits_needed >= _unused_bits)
			next_byte();
		else
			_unused_bits -= bits_needed;
	}
}

template<lak::endian BYTE_ENDIAN, lak::endian BIT_ENDIAN>
inline void lak::bit_reader<BYTE_ENDIAN, BIT_ENDIAN>::accumulate_bits(
  const uint8_t bits)
{
	accumulate_to(_num_bits + bits);
}

template<lak::endian BYTE_ENDIAN, lak::endian BIT_ENDIAN>
inline lak::bit_reader_result<uintmax_t>
lak::bit_reader<BYTE_ENDIAN, BIT_ENDIAN>::peek_bits(const uint8_t bits)
{
	static_assert(BYTE_ENDIAN == lak::endian::little ||
	              BYTE_ENDIAN == lak::endian::big);
	static_assert(BIT_ENDIAN == lak::endian::little ||
	              BIT_ENDIAN == lak::endian::big);

	if (bits > std::numeric_limits<uintmax_t>::digits)
		return lak::err_t{lak::err::value_out_of_range{}};

	if (lak::bit_count::from_bits(bits) > bytes_remaining())
		return lak::err_t<lak::err::out_of_data>{};

	accumulate_to(bits);

	auto reverse = [&](uintmax_t value) -> uintmax_t
	{
		uintmax_t result = 0U;
		for (uint8_t i = 0U; i < bits; ++i)
		{
			result <<= 1U;
			result |= (value >> i) & 1U;
		}
		return result;
	};

	if constexpr (BYTE_ENDIAN == lak::endian::little)
	{
		auto res = uintmax_t(_bit_accum & ~(UINTMAX_MAX << bits));
		if constexpr (BIT_ENDIAN == lak::endian::little)
			return lak::ok_t{res};
		else if constexpr (BIT_ENDIAN == lak::endian::big)
			return lak::ok_t{reverse(res)};
	}
	else if constexpr (BYTE_ENDIAN == lak::endian::big)
	{
		auto res = uintmax_t(_bit_accum >> (_num_bits - bits));
		if constexpr (BIT_ENDIAN == lak::endian::little)
			return lak::ok_t{res};
		else if constexpr (BIT_ENDIAN == lak::endian::big)
			return lak::ok_t{reverse(res)};
	}
}

template<lak::endian BYTE_ENDIAN, lak::endian BIT_ENDIAN>
inline lak::bit_reader_result<uintmax_t>
lak::bit_reader<BYTE_ENDIAN, BIT_ENDIAN>::read_bits(const uint8_t bits)
{
	return peek_bits(bits).if_ok([&](auto &&) { flush_bits(bits); });
}

template<lak::endian BYTE_ENDIAN, lak::endian BIT_ENDIAN>
inline lak::bit_reader_result<byte_t> lak::bit_reader<BYTE_ENDIAN,
                                                      BIT_ENDIAN>::peek_byte()
{
	return peek_bits(8).map([](uintmax_t v) -> byte_t { return byte_t(v); });
}

template<lak::endian BYTE_ENDIAN, lak::endian BIT_ENDIAN>
inline lak::bit_reader_result<byte_t> lak::bit_reader<BYTE_ENDIAN,
                                                      BIT_ENDIAN>::read_byte()
{
	return read_bits(8).map([](uintmax_t v) -> byte_t { return byte_t(v); });
}

template<lak::endian BYTE_ENDIAN, lak::endian BIT_ENDIAN>
inline lak::bit_reader_result<> lak::bit_reader<BYTE_ENDIAN, BIT_ENDIAN>::skip(
  lak::bit_count count)
{
	count.normalise();

	auto acc = lak::bit_count::from_bits(_num_bits);
	if (acc >= count)
	{
		flush_bits(uint8_t(count.to_bits()));
		return lak::ok_t{};
	}

	if (count > bytes_remaining()) return lak::err_t<lak::err::out_of_data>{};

	count -= acc;
	_bit_accum = 0U;
	_num_bits  = 0U;
	_data      = _data.subspan(count.bytes);
	_bytes_read += count.bytes;
	count.bytes = 0U;

	ASSERT_LESS(count.bits, 8U);

	if (count.bits >= _unused_bits)
	{
		_data = _data.subspan(1U);
		_bytes_read += 1U;
		_unused_bits += 8U;
		_unused_bits -= count.bits;
	}
	else
		_unused_bits -= count.bits;

	return lak::ok_t{};
}

template<lak::endian BYTE_ENDIAN, lak::endian BIT_ENDIAN>
inline lak::bit_reader_result<>
lak::bit_reader<BYTE_ENDIAN, BIT_ENDIAN>::skip_bits(const size_t bits)
{
	return skip(lak::bit_count::from_bits(bits));
}

template<lak::endian BYTE_ENDIAN, lak::endian BIT_ENDIAN>
inline lak::bit_reader_result<>
lak::bit_reader<BYTE_ENDIAN, BIT_ENDIAN>::skip_bytes(const size_t bytes)
{
	return skip(lak::bit_count::from_bytes(bytes));
}
