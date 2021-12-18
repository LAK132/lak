// Based on tinflate by Andrew Church

#ifndef LAK_COMPRESSION_DEFLATE_HPP
#define LAK_COMPRESSION_DEFLATE_HPP

#include "lak/array.hpp"
#include "lak/bit_reader.hpp"
#include "lak/buffer_span.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"

namespace lak
{
	struct deflate_iterator
	{
		enum class state_t : uint8_t
		{
			out_of_data,
			block_complete,
			custom_dictionary,
			header,
			uncompressed_len,
			uncompressed_ilen,
			uncompressed_data,
			literal_count,
			distance_count,
			codelen_count,
			read_code_lengths,
			read_lengths,
			read_lengths_16,
			read_lengths_17,
			read_lengths_18,
			read_symbol,
			push_symbol,
			read_length,
			read_distance,
			read_distance_extra,
			write_repeat,
		};

		enum class error_t : uint8_t
		{
			ok,

			no_data,

			invalid_parameter,
			custom_dictionary,

			invalid_state,
			invalid_block_code,
			out_of_data,
			corrupt_stream,
			huffman_table_gen_failed,
			invalid_symbol,
			invalid_distance,

			no_symbols,
			too_many_symbols,
			incomplete_tree,
		};

		static const char *error_name(error_t error);

	private:
		using value_type = lak::result<lak::span<byte_t>, error_t>;
		value_type _value;

		lak::bit_reader<lak::endian::little> _compressed;
		lak::buffer_span<byte_t, 0x8000> _output_buffer;
		size_t _unflushed = 0;

		state_t _state = state_t::out_of_data;
		uint32_t _crc  = 0;
		uint32_t _icrc = 0;
		bool _final    = false;
		bool _anaconda = false;

		uint8_t _block_type     = 0;
		uint32_t _counter       = 0;
		uint32_t _symbol        = 0;
		uint32_t _last_value    = 0;
		uint32_t _repeat_length = 0;
		uint32_t _repeat_count  = 0;
		uint32_t _distance      = 0;

		uint32_t _len   = 0;
		uint32_t _ilen  = 0;
		uint32_t _nread = 0;

		lak::array<int16_t, 0x23E> _literal_table = {};
		lak::array<uint8_t, 0x120> _literal_len   = {};
		uint32_t _literal_count                   = 0;

		lak::array<int16_t, 0x3E> _distance_table = {};
		lak::array<uint8_t, 0x20> _distance_len   = {};
		uint32_t _distance_count                  = 0;

		lak::array<int16_t, 0x24> _codelen_table = {};
		lak::array<uint8_t, 0x13> _codelen_len   = {};
		uint32_t _codelen_count                  = 0;

		template<typename T>
		bool get_huff(lak::span<int16_t> table, T *out);

		force_inline deflate_iterator &fail(error_t reason);

		force_inline deflate_iterator &success();

		// returns true when need to flush
		[[nodiscard]] force_inline bool push_value(byte_t value);

		force_inline deflate_iterator &finished();

		error_t gen_huffman_table(lak::span<const uint8_t> lengths,
		                          bool allow_no_symbols,
		                          lak::span<int16_t> table);

	public:
		deflate_iterator(lak::span<const byte_t> compressed,
		                 lak::span<byte_t, 0x8000> output_buffer,
		                 bool parse_header,
		                 bool anaconda = false);

		bool is_final_block() const;

		// (bytes, bits)
		auto bytes_read() const { return _compressed.bytes_read(); }

		lak::span<const byte_t> compressed() const;
		void replace_compressed(lak::span<const byte_t> compressed);

		force_inline deflate_iterator &step();

		deflate_iterator &operator++();

		value_type operator*();

		template<typename FUNC>
		lak::error_code<error_t> read(FUNC &&func);
	};
}

#include "lak/compression/deflate.inl"

#endif