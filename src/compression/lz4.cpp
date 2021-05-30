#include "lak/compression/lz4.hpp"

lak::result<lak::memory, lak::lz4_decode_error> lak::decode_lz4_block(
  lak::span_memory &strm, size_t output_size)
{
  lak::memory output = lak::memory(std::vector<uint8_t>(output_size));

  for (;;)
  {
    if (strm.remaining() < 1)
      return lak::err_t{lak::lz4_decode_error::out_of_data};
    const uint8_t token = strm.read_u8();
    size_t match_length = (token >> 0) & 0xF;
    size_t length       = (token >> 4) & 0xF;

    if (length == 15)
    {
      // read more bytes

      for (;;)
      {
        if (strm.remaining() < 1)
          return lak::err_t{lak::lz4_decode_error::out_of_data};
        uint8_t another = strm.read_u8();
        if (length + another < length)
          return lak::err_t{lak::lz4_decode_error::too_many_literals};
        length += another;
        if (another != 255) break;
      }
    }

    // there are length literals

    if (strm.remaining() < length)
      return lak::err_t{lak::lz4_decode_error::out_of_data};
    output.write(lak::span<const uint8_t>(strm.cursor(), length));
    strm.skip(length);

    if (strm.remaining() == 0) break; // reached the end of the last block

    if (strm.remaining() < 2)
      return lak::err_t{lak::lz4_decode_error::out_of_data};
    const auto offset = strm.read_u16();
    if (offset == 0) return lak::err_t{lak::lz4_decode_error::zero_offset};

    if (match_length == 15)
    {
      for (;;)
      {
        if (strm.remaining() < 1)
          return lak::err_t{lak::lz4_decode_error::out_of_data};
        uint8_t another = strm.read_u8();
        if (match_length + another < match_length)
          return lak::err_t{lak::lz4_decode_error::match_too_long};
        match_length += another;
        if (another != 255) break;
      }
    }

    // match_length is always offset by 4 (the minimum match length)
    if (match_length + 4 < match_length)
      return lak::err_t{lak::lz4_decode_error::match_too_long};
    match_length += 4;

    // this may be an aliasing copy!
    if (output.cursor() - output.begin() < offset)
      return lak::err_t{lak::lz4_decode_error::offset_too_large};
    if (output.remaining() + offset < match_length)
      return lak::err_t{lak::lz4_decode_error::out_of_data};
    for (const auto &v :
         lak::span<const uint8_t>(output.cursor() - offset, match_length))
      output.write_u8(v);
  }

  if (output.remaining() != 0) WARNING("Expected More Output Data");

  return lak::ok_t{lak::move(output)};
}
