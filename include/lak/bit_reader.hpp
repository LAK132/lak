#ifndef LAK_BIT_READER_HPP
#define LAK_BIT_READER_HPP

#include "lak/binary_reader.hpp"
#include "lak/bit_count.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"

namespace lak
{
	template<typename T = lak::monostate>
	using bit_reader_result = lak::result<
	  T,
	  lak::variant<lak::err::out_of_data, lak::err::value_out_of_range>>;

	template<lak::endian ENDIAN = lak::endian::little>
	struct bit_reader
	{
	private:
		lak::span<const byte_t> _data;

		uintmax_t _bit_accum  = 0;
		uint8_t _num_bits     = 0; // number of bits accumulated.
		uint8_t _unused_bits  = 8; // bits of _data[0] that are unaccumulated.
		uintmax_t _bytes_read = 0; // number of bytes flushed from _data.

		inline void flush_bits(const uint8_t bits);

		inline void accumulate_to(const uint8_t bits);

		inline void accumulate_bits(const uint8_t bits);

	public:
		bit_reader() = default;

		bit_reader(const bit_reader &) = default;

		bit_reader &operator=(const bit_reader &) = default;

		// (bytes, bits)
		inline lak::bit_count bytes_read() const
		{
			return lak::bit_count{.bytes = size_t(_bytes_read),
			                      .bits  = uint8_t(8 - _unused_bits)};
		}

		inline lak::bit_count bytes_remaining() const
		{
			return (lak::bit_count::from_bytes(_data.size()) -
			        lak::bit_count::from_bits(8U - _unused_bits)) +
			       lak::bit_count::from_bits(_num_bits);
		}

		inline bit_reader(lak::span<const byte_t> data) : _data(data) {}

		inline lak::span<const byte_t> get() const { return _data; }

		inline bool empty() const { return _data.empty(); }

		inline void reset_data(lak::span<const byte_t> data) { _data = data; }

		inline lak::bit_reader_result<uintmax_t> peek_bits(const uint8_t bits);

		inline lak::bit_reader_result<uintmax_t> read_bits(const uint8_t bits);

		inline lak::bit_reader_result<byte_t> peek_byte();

		inline lak::bit_reader_result<byte_t> read_byte();

		inline lak::bit_reader_result<> skip(lak::bit_count bits);

		inline lak::bit_reader_result<> skip_bits(const size_t bits);

		inline lak::bit_reader_result<> skip_bytes(const size_t bytes);
	};
}

#include "lak/bit_reader.inl"

#endif
