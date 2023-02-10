#include "lak/compression/deflate.hpp"

const char *lak::deflate_iterator::error_name(error_t error)
{
	switch (error)
	{
		case error_t::ok:
			return "Ok";
		case error_t::no_data:
			return "No data";
		case error_t::invalid_parameter:
			return "Invalid parameter";
		case error_t::custom_dictionary:
			return "Custom dictionary";
		case error_t::invalid_state:
			return "Invalid state";
		case error_t::invalid_block_code:
			return "Invalid block code";
		case error_t::out_of_data:
			return "Out of data";
		case error_t::corrupt_stream:
			return "Corrupt stream";
		case error_t::huffman_table_gen_failed:
			return "Huffman table gen failed";
		case error_t::invalid_symbol:
			return "Invalid symbol";
		case error_t::invalid_distance:
			return "Invalid distance";
		case error_t::no_symbols:
			return "No symbols";
		case error_t::too_many_symbols:
			return "Too many symbols";
		case error_t::incomplete_tree:
			return "Incomplete tree";
		default:
			return "Not a deflate error";
	}
}

template<typename T>
bool lak::deflate_iterator::get_huff(lak::span<int16_t> table, T *out)
{
	uint8_t bits_used = 0;
	uint32_t index    = 0;

	for (bool looping = true; looping;)
	{
		if_let_ok (uintmax_t v, _compressed.peek_bits(bits_used + 1))
		{
			index += (v >> bits_used) & 1;
			++bits_used;
			if (table[index] >= 0)
				looping = false;
			else
				index = ~(table[index]);
			continue;
		}
		return true;
	}

	*out = table[index];

	_compressed.read_bits(bits_used).UNWRAP();

	return false;
}

lak::deflate_iterator::error_t lak::deflate_iterator::gen_huffman_table(
  lak::span<const uint8_t> lengths,
  bool allow_no_symbols,
  lak::span<int16_t> table)
{
	uint16_t length_count[16] = {0};
	uint16_t total_count      = 0;
	uint16_t first_code[16];

	for (uint32_t i = 0; i < lengths.size(); ++i)
		if (lengths[i] > 0) ++length_count[lengths[i]];

	for (uint32_t i = 1; i < 16; ++i) total_count += length_count[i];

	if (total_count == 0)
	{
		return allow_no_symbols ? error_t::ok : error_t::no_symbols;
	}
	else if (total_count == 1)
	{
		for (uint32_t i = 0; i < lengths.size(); ++i)
			if (lengths[i] != 0) table[0] = table[1] = uint16_t(i);
		return error_t::ok;
	}

	first_code[0] = 0;

	for (uint32_t i = 1; i < 16; ++i)
	{
		first_code[i] = (first_code[i - 1] + length_count[i - 1]) << 1;
		if (first_code[i] + length_count[i] > uint16_t(1 << i))
			return error_t::too_many_symbols;
	}

	if (first_code[15] + length_count[15] != uint16_t(1 << 15))
		return error_t::incomplete_tree;

	for (uint32_t index = 0, i = 1; i < 16; ++i)
	{
		uint32_t code_limit = 1U << i;
		uint32_t next_code  = first_code[i] + length_count[i];
		uint32_t next_index = index + (code_limit - first_code[i]);

		for (uint32_t j = 0; j < lengths.size(); ++j)
			if (lengths[j] == i) table[index++] = uint16_t(j);

		for (uint32_t j = next_code; j < code_limit; ++j)
		{
			table[index++] = uint16_t(~next_index);
			next_index += 2;
		}
	}

	return error_t::ok;
}

lak::deflate_iterator::deflate_iterator(
  lak::span<const byte_t> compressed,
  lak::span<byte_t, 0x8000> output_buffer,
  bool parse_header,
  bool anaconda)
: _value(value_type::make_err(error_t::invalid_state)),
  _compressed(compressed),
  _output_buffer(output_buffer, false),
  _anaconda(anaconda)
{
	if (parse_header)
	{
		if_let_ok (uintmax_t read, _compressed.peek_bits(16))
		{
			uint16_t zlib_header = static_cast<uint16_t>(read);
			zlib_header          = (zlib_header >> 8) | ((zlib_header & 0xFF) << 8);

			if ((zlib_header & 0x8F00) == 0x0800 && (zlib_header % 31) == 0)
			{
				if (zlib_header & 0x0020)
				{
					_state = state_t::custom_dictionary;
					return;
				}
				_compressed.read_bits(16).discard();
			}
		}
		else
		{
			_state = state_t::out_of_data;
			return;
		}
	}
	_state = state_t::header;
}

bool lak::deflate_iterator::is_final_block() const { return _final; }

lak::span<const byte_t> lak::deflate_iterator::compressed() const
{
	return _compressed.get();
}

void lak::deflate_iterator::replace_compressed(
  lak::span<const byte_t> compressed)
{
	if (!_compressed.empty())
		WARNING(
		  "replacing compressed data buffer before the previous "
		  "compressed data buffer was empty");
	_compressed.reset_data(compressed);
}

lak::deflate_iterator &lak::deflate_iterator::operator++() { return step(); }

lak::deflate_iterator::value_type lak::deflate_iterator::operator*()
{
	return _value;
}
