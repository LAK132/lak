#ifndef LAK_COMPRESSION_LZ4_HPP
#define LAK_COMPRESSION_LZ4_HPP

#include "lak/array.hpp"
#include "lak/bitreader.hpp"
#include "lak/buffer_span.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"

namespace lak
{
  enum class lz4_decode_error : uint8_t
  {
    too_many_literals,

    out_of_data,

    zero_offset,
    offset_too_large,

    match_too_long,
  };

  lak::result<lak::memory, lak::lz4_decode_error> decode_lz4_block(
    lak::span_memory &strm, size_t output_size);

  inline lak::result<lak::memory, lak::lz4_decode_error> decode_lz4_block(
    lak::span_memory &&strm, size_t output_size)
  {
    return decode_lz4_block(strm, output_size);
  }
}

#endif