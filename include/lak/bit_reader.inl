#include "lak/bit_reader.hpp"

template<lak::endian ENDIAN>
inline void lak::bit_reader<ENDIAN>::flush_bits(const uint8_t bits)
{
	static_assert(ENDIAN == lak::endian::little || ENDIAN == lak::endian::big);
	if constexpr (ENDIAN == lak::endian::little)
		_bit_accum >>= bits;
	else if constexpr (ENDIAN == lak::endian::big)
		_bit_accum &= ~(UINTMAX_MAX << (_num_bits - bits));
	_num_bits -= bits;
}

template<lak::endian ENDIAN>
inline void lak::bit_reader<ENDIAN>::accumulate_to(const uint8_t bits)
{
	while (_num_bits < bits)
	{
		if (uint8_t bits_needed = bits - _num_bits; bits_needed >= _unused_bits)
		{
			// there are just enough bits unused or we need more bits than are
			// unused
			if constexpr (ENDIAN == lak::endian::little)
			{
				_bit_accum |= uintmax_t(uint8_t(_data[0]) >> (8U - _unused_bits))
				              << _num_bits;
			}
			else if constexpr (ENDIAN == lak::endian::big)
			{
				_bit_accum <<= _unused_bits;
				_bit_accum |=
				  uint8_t(uint8_t(_data[0]) & (0xFFU >> (8U - _unused_bits)));
			}
			_num_bits += _unused_bits;
			_unused_bits = 8;
			_data        = _data.subspan(1);
			++_bytes_read;
		}
		else
		{
			// there are more bits unused than we need
			if constexpr (ENDIAN == lak::endian::little)
			{
				_bit_accum |= uintmax_t(uint8_t(_data[0]) >> (8U - _unused_bits) &
				                        ~(UINTMAX_MAX << bits_needed))
				              << _num_bits;
			}
			else if constexpr (ENDIAN == lak::endian::big)
			{
				_bit_accum <<= bits_needed;
				_bit_accum |=
				  uint8_t(uint8_t(_data[0]) & (0xFFU >> (8U - _unused_bits))) >>
				  (_unused_bits - bits_needed);
			}
			_num_bits += bits_needed;
			_unused_bits -= bits_needed;
		}
	}
}

template<lak::endian ENDIAN>
inline void lak::bit_reader<ENDIAN>::accumulate_bits(const uint8_t bits)
{
	accumulate_to(_num_bits + bits);
}

template<lak::endian ENDIAN>
inline lak::result<uintmax_t, lak::bit_reader_error_t>
lak::bit_reader<ENDIAN>::peek_bits(const uint8_t bits)
{
	static_assert(ENDIAN == lak::endian::little || ENDIAN == lak::endian::big);

	if (bits > std::numeric_limits<uintmax_t>::digits)
		return lak::err_t{lak::bit_reader_error_t::too_many_bits};

	if (bits > _num_bits)
		if (const uint8_t bytes = (bits - _num_bits) / 8U,
		    frac_bits           = (bits - _num_bits) % 8U;
		    !((_data.size() > bytes) ||
		      ((_data.size() == bytes) && (frac_bits == 0U))))
			return lak::err_t{lak::bit_reader_error_t::out_of_data};

	accumulate_to(bits);

	if constexpr (ENDIAN == lak::endian::little)
		return lak::ok_t{uintmax_t(_bit_accum & ~(UINTMAX_MAX << bits))};
	else if constexpr (ENDIAN == lak::endian::big)
		return lak::ok_t{uintmax_t(_bit_accum >> (_num_bits - bits))};
}

template<lak::endian ENDIAN>
inline lak::result<uintmax_t, lak::bit_reader_error_t>
lak::bit_reader<ENDIAN>::read_bits(const uint8_t bits)
{
	return peek_bits(bits).if_ok([&](auto &&) { flush_bits(bits); });
}

template<lak::endian ENDIAN>
inline lak::result<byte_t, lak::bit_reader_error_t>
lak::bit_reader<ENDIAN>::peek_byte()
{
	return peek_bits(8).map([](uintmax_t v) -> byte_t { return byte_t(v); });
}

template<lak::endian ENDIAN>
inline lak::result<byte_t, lak::bit_reader_error_t>
lak::bit_reader<ENDIAN>::read_byte()
{
	return read_bits(8).map([](uintmax_t v) -> byte_t { return byte_t(v); });
}

template<lak::endian ENDIAN>
inline lak::error_code<lak::bit_reader_error_t> lak::bit_reader<ENDIAN>::skip(
  size_t bytes, size_t bits)
{
	bytes += bits / 8U;
	bits = bits % 8U;

	{
		const uint8_t acc_num_bits  = _num_bits % 8U,
		              acc_num_bytes = _num_bits / 8U;
		if ((acc_num_bytes > bytes) ||
		    ((acc_num_bytes == bytes) && (acc_num_bits > bits)))
		{
			flush_bits(uint8_t((bytes * 8U) + bits));
			return lak::ok_t{};
		}

		if (bits >= acc_num_bits)
		{
			bytes -= acc_num_bytes;
			bits -= acc_num_bits;
		}
		else
		{
			bytes -= acc_num_bytes + 1U;
			bits = (bits + 8U) - acc_num_bits;
		}
	}

	if (!((_data.size() > bytes) || ((_data.size() == bytes) && (bits == 0U))))
		return lak::err_t{lak::bit_reader_error_t::out_of_data};

	_bit_accum = 0U;
	_num_bits  = 0U;

	if (bits <= _unused_bits)
	{
		_data = _data.subspan(bytes);
		_unused_bits -= uint8_t(bits);
	}
	else
	{
		_data        = _data.subspan(bytes + 1U);
		_unused_bits = uint8_t(8U - (bits - _unused_bits));
	}

	return lak::ok_t{};
}

template<lak::endian ENDIAN>
inline lak::error_code<lak::bit_reader_error_t>
lak::bit_reader<ENDIAN>::skip_bits(const size_t bits)
{
	return skip(0, bits);
}

template<lak::endian ENDIAN>
inline lak::error_code<lak::bit_reader_error_t>
lak::bit_reader<ENDIAN>::skip_bytes(const size_t bytes)
{
	return skip(bytes, 0);
}
