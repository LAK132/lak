#ifndef LAK_COMPRESSION_LZ4_HPP
#define LAK_COMPRESSION_LZ4_HPP

#include "lak/array.hpp"
#include "lak/binary_reader.hpp"
#include "lak/buffer_span.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"

namespace lak
{
	enum class lz4_decode_error : uint8_t
	{
		too_many_literals,

		out_of_data,
		output_full,

		zero_offset,
		offset_too_large,

		match_too_long,
	};

	const char *lz4_error_name(lak::lz4_decode_error err);

	// allow_partial_reads disables the output_full error, but in the event of a
	// partial read, it means the input stream is left in a potentially bad state
	// for further block decoding.

	lak::result<lak::array<byte_t>, lak::lz4_decode_error> decode_lz4_block(
	  lak::binary_reader &strm,
	  size_t output_size,
	  bool allow_partial_read = false);

	inline lak::result<lak::array<byte_t>, lak::lz4_decode_error>
	decode_lz4_block(lak::binary_reader &&strm,
	                 size_t output_size,
	                 bool allow_partial_read = false)
	{
		return decode_lz4_block(strm, output_size, allow_partial_read);
	}
}

#endif