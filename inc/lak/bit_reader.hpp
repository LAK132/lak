#ifndef LAK_BIT_READER_HPP
#define LAK_BIT_READER_HPP

#include "lak/binary_reader.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"

namespace lak
{
  struct bit_reader
  {
  private:
    lak::span<const uint8_t> _data;

    uintmax_t _bit_accum  = 0;
    uint8_t _num_bits     = 0; // number of bits accumulated.
    uint8_t _unused_bits  = 8; // bits of _data[0] that are unaccumulated.
    uintmax_t _bytes_read = 0; // number of bytes flushed from _data.

    inline void flush_bits(const uint8_t bits);

  public:
    bit_reader() = default;

    bit_reader(const bit_reader &) = default;

    bit_reader &operator=(const bit_reader &) = default;

    // (bytes, bits)
    inline lak::pair<uintmax_t, uint8_t> bytes_read() const
    {
      return lak::pair<uintmax_t, uint8_t>(_bytes_read, 8 - _unused_bits);
    }

    inline bit_reader(lak::span<const uint8_t> data) : _data(data) {}

    inline lak::span<const uint8_t> get() const { return _data; }

    inline bool empty() const { return _data.empty(); }

    inline void reset_data(lak::span<const uint8_t> data) { _data = data; }

    enum struct error_t : uint8_t
    {
      // insufficent data left. this signifies that you should reset_data.
      out_of_data,

      // requested more than std::numeric_limits<uintmax_t>::digits bits.
      too_many_bits,
    };

    inline lak::result<uintmax_t, error_t> peek_bits(const uint8_t bits);

    inline lak::result<uintmax_t, error_t> read_bits(const uint8_t bits);

    inline lak::result<uint8_t, error_t> peek_byte();

    inline lak::result<uint8_t, error_t> read_byte();
  };
}

#include "lak/bit_reader.inl"

#endif