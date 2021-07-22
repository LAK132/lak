#include "lak/bit_reader.hpp"

inline void lak::bit_reader::flush_bits(const uint8_t bits)
{
  _bit_accum >>= bits;
  _num_bits -= bits;
}

inline lak::result<uintmax_t, lak::bit_reader::error_t>
lak::bit_reader::peek_bits(const uint8_t bits)
{
  if (bits > std::numeric_limits<uintmax_t>::digits)
    return lak::err_t{lak::bit_reader::error_t::too_many_bits};

  while (_num_bits < bits)
  {
    if (_data.empty())
      return lak::err_t{lak::bit_reader::error_t::out_of_data};

    if (uint8_t bits_needed = bits - _num_bits; bits_needed >= _unused_bits)
    {
      // there are just enough bits unused or we need more bits than are unused
      uint8_t unused = _data[0] >> (8 - _unused_bits);
      _bit_accum |= unused << _num_bits;
      _num_bits += _unused_bits;
      _unused_bits = 8;
      _data        = _data.subspan(1);
      ++_bytes_read;
    }
    else
    {
      // there are more bits unused than we need
      uint8_t unused =
        (_data[0] >> (8 - _unused_bits)) & ~(UINTMAX_MAX << bits_needed);
      _bit_accum |= unused << _num_bits;
      _num_bits += bits_needed;
      _unused_bits -= bits_needed;
    }
  }

  return lak::ok_t{uintmax_t(_bit_accum & ~(UINTMAX_MAX << bits))};
}

inline lak::result<uintmax_t, lak::bit_reader::error_t>
lak::bit_reader::read_bits(const uint8_t bits)
{
  return peek_bits(bits).if_ok([&](auto &&) { flush_bits(bits); });
}

inline lak::result<uint8_t, lak::bit_reader::error_t>
lak::bit_reader::peek_byte()
{
  return peek_bits(8).map([](uintmax_t v) -> uint8_t { return uint8_t(v); });
}

inline lak::result<uint8_t, lak::bit_reader::error_t>
lak::bit_reader::read_byte()
{
  return read_bits(8).map([](uintmax_t v) -> uint8_t { return uint8_t(v); });
}
