#include "lak/compression/lz4.hpp"

#include "lak/binary_writer.hpp"

const char *lak::lz4_error_name(lak::lz4_decode_error err)
{
	switch (err)
	{
		case lak::lz4_decode_error::too_many_literals: return "too many literals";
		case lak::lz4_decode_error::out_of_data: return "out of data";
		case lak::lz4_decode_error::zero_offset: return "zero offset";
		case lak::lz4_decode_error::offset_too_large: return "offset too large";
		case lak::lz4_decode_error::match_too_long: return "match too long";
		default: return "invalid error code";
	}
}

lak::result<lak::array<byte_t>, lak::lz4_decode_error> lak::decode_lz4_block(
  lak::binary_reader &strm, size_t output_size, bool allow_partial_read)
{
	lak::array<byte_t> output(output_size);
	lak::binary_span_writer writer{lak::span<byte_t>(lak::span(output))};

	auto out_of_data = [](auto &&)
	{ return lak::lz4_decode_error::out_of_data; };

	for (;;)
	{
		RES_TRY_ASSIGN(const uint8_t token =, strm.read_u8().map_err(out_of_data));
		size_t match_length = (token >> 0) & 0xF;
		size_t length       = (token >> 4) & 0xF;

		if (length == 15)
		{
			// read more bytes

			for (;;)
			{
				RES_TRY_ASSIGN(uint8_t another =, strm.read_u8().map_err(out_of_data));
				if (length + another < length)
					return lak::err_t{lak::lz4_decode_error::too_many_literals};
				length += another;
				if (another != 255) break;
			}
		}

		// there are <length> literals

		if (writer.remaining().size() < length)
		{
			if (allow_partial_read)
				length = writer.remaining().size();
			else
				return lak::err_t{lak::lz4_decode_error::output_full};
		}
		if (strm.remaining().size() < length)
			return lak::err_t{lak::lz4_decode_error::out_of_data};
		writer.write(strm.remaining().first(length)).unwrap();
		RES_TRY(strm.skip(length).map_err(out_of_data));

		if (strm.empty()) break; // reached the end of the last block

		RES_TRY_ASSIGN(const auto offset =, strm.read_u16().map_err(out_of_data));
		if (offset == 0) return lak::err_t{lak::lz4_decode_error::zero_offset};

		if (match_length == 15)
		{
			for (;;)
			{
				RES_TRY_ASSIGN(uint8_t another =, strm.read_u8().map_err(out_of_data));
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
		if (writer.position() < offset)
			return lak::err_t{lak::lz4_decode_error::offset_too_large};
		if (writer.remaining().size() < match_length)
		{
			if (allow_partial_read)
				match_length = writer.remaining().size();
			else
				return lak::err_t{lak::lz4_decode_error::output_full};
		}
		for (const auto &v : lak::span<const byte_t>(
		       writer.remaining().begin() - offset, match_length))
			writer.write_u8(uint8_t(v)).unwrap();

		if (allow_partial_read && writer.empty()) break;
	}

	if (!writer.empty()) WARNING("Expected More Output Data");

	return lak::move_ok(output);
}
