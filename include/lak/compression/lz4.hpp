#ifndef LAK_COMPRESSION_LZ4_HPP
#define LAK_COMPRESSION_LZ4_HPP

#include "lak/array.hpp"
#include "lak/binary_reader.hpp"
#include "lak/buffer_span.hpp"
#include "lak/errors.hpp"
#include "lak/format.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"

namespace lak
{
	namespace err
	{
		enum class lz4_decode : uint8_t
		{
			too_many_literals,

			output_full,

			zero_offset,
			offset_too_large,

			match_too_long,
		};
	}

	// allow_partial_reads disables the output_full error, but in the event of a
	// partial read, it means the input stream is left in a potentially bad state
	// for further block decoding.

	lak::result<lak::array<byte_t>,
	            lak::variant<lak::err::out_of_data, lak::err::lz4_decode>>
	decode_lz4_block(lak::binary_reader &strm,
	                 size_t output_size,
	                 bool allow_partial_read = false);

	inline lak::result<lak::array<byte_t>,
	                   lak::variant<lak::err::out_of_data, lak::err::lz4_decode>>
	decode_lz4_block(lak::binary_reader &&strm,
	                 size_t output_size,
	                 bool allow_partial_read = false)
	{
		return decode_lz4_block(strm, output_size, allow_partial_read);
	}
}

template<typename CHAR>
struct lak::format_traits<lak::err::lz4_decode, CHAR>
{
	static lak::string<CHAR> to_string(const lak::err::lz4_decode &value)
	{
		switch (value)
		{
			case lak::err::lz4_decode::too_many_literals:
				return lak::strconv<CHAR>("too many literals"_view);
			case lak::err::lz4_decode::zero_offset:
				return lak::strconv<CHAR>("zero offset"_view);
			case lak::err::lz4_decode::offset_too_large:
				return lak::strconv<CHAR>("offset too large"_view);
			case lak::err::lz4_decode::match_too_long:
				return lak::strconv<CHAR>("match too long"_view);
			default:
				BOUNDS_ASSERT_UNREACHABLE(
				  return lak::strconv<CHAR>("invalid error code"_view));
		}
	}
};

#endif
