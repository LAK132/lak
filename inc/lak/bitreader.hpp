#ifndef LAK_BITREADER_HPP
#define LAK_BITREADER_HPP

#include "lak/memory.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"

namespace lak
{
  struct bitreader
  {
  private:
    lak::span<const uint8_t> _data;

    uintmax_t _bit_accum = 0;
    uint8_t _num_bits    = 0;
    uint8_t _unused_bits = 8;

    inline void flush_bits(const uint8_t bits);

  public:
    bitreader() = default;

    bitreader(const bitreader &) = default;

    bitreader &operator=(const bitreader &) = default;

    inline bitreader(lak::span<const uint8_t> data) : _data(data) {}

    inline lak::span<const uint8_t> get() const { return _data; }

    inline bool empty() const { return _data.empty(); }

    inline void reset_data(lak::span<const uint8_t> data) { _data = data; }

    inline void clear_data() { _data = {}; }

    inline void clear_bits()
    {
      _bit_accum   = 0;
      _num_bits    = 0;
      _unused_bits = 8;
    }

    inline void clear()
    {
      clear_data();
      clear_bits();
    }

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

#include "lak/bitreader.inl"

#endif