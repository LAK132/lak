#ifndef LAK_X3F_HPP
#define LAK_X3F_HPP

#include "lak/binary_reader.hpp"
#include "lak/binary_traits.hpp"
#include "lak/errors.hpp"
#include "lak/file/tiff.hpp"
#include "lak/format_traits.hpp"
#include "lak/image.hpp"

namespace lak
{
	namespace x3f
	{
		// always little endian

#define LAK_FOREACH_X3F_EXTENDED_DATA_TYPE(MACRO, ...)                        \
	MACRO(0, None, __VA_ARGS__)                                                 \
	MACRO(1, ExposureAdjust, __VA_ARGS__)                                       \
	MACRO(2, ContrastAdjust, __VA_ARGS__)                                       \
	MACRO(3, ShadowAdjust, __VA_ARGS__)                                         \
	MACRO(4, HighlightAdjust, __VA_ARGS__)                                      \
	MACRO(5, SaturationAdjust, __VA_ARGS__)                                     \
	MACRO(6, SharpnessAdjust, __VA_ARGS__)                                      \
	MACRO(7, RedAdjust, __VA_ARGS__)                                            \
	MACRO(8, GreenAdjust, __VA_ARGS__)                                          \
	MACRO(9, BlueAdjust, __VA_ARGS__)                                           \
	MACRO(10, FillLightAdjust, __VA_ARGS__)

		enum struct extended_data_type : uint8_t
		{
#define LAK_X3F_EXTENDED_DATA_TYPE_ENUM(VAL, NAME, ...) NAME = VAL,
			LAK_FOREACH_X3F_EXTENDED_DATA_TYPE(LAK_X3F_EXTENDED_DATA_TYPE_ENUM)
#undef LAK_X3F_EXTENDED_DATA_TYPE_ENUM
		};

		struct version_number
		{
			uint16_t major;
			uint16_t minor;
		};

		using fourcc_t = lak::array<char, 4U>;

		struct section_header
		{
			fourcc_t fourcc;
			lak::x3f::version_number version;
		};

		struct header_1_0
		{
			lak::array<uint8_t, 16U> unique_id;
			uint32_t mark_bits;
			uint32_t image_columns;
			uint32_t image_rows;
			uint32_t rotation;
		};

		struct header_2_0
		{
			lak::array<uint8_t, 16U> unique_id;
			uint32_t mark_bits;
			uint32_t image_columns;
			uint32_t image_rows;
			uint32_t rotation;
			lak::array<char, 32U> white_balance;
			lak::array<lak::x3f::extended_data_type, 32U> extended_data_types;
			lak::array<uint32_t, 32U> extended_data;
		};

		struct header_4_0
		{
			uint32_t unknown1;
			uint32_t unknown2;
			uint32_t unknown3;
			lak::array<uint8_t, 16U> unique_id;
			uint32_t mark_bits;
			uint32_t image_columns;
			uint32_t image_rows;
			uint32_t rotation;
			// lak::array<char, 32U> white_balance;
			// lak::array<lak::x3f::extended_data_type, 32U> extended_data_types;
			// lak::array<uint32_t, 32U> extended_data;
		};

		struct header
		{
			lak::x3f::section_header section;
			lak::variant<lak::x3f::header_1_0,
			             lak::x3f::header_2_0,
			             lak::x3f::header_4_0>
			  versioned;

			lak::error_code<lak::err::out_of_data> _read(lak::binary_reader &strm);

			template<lak::endian E>
			lak::error_code<lak::err::out_of_data> read(lak::binary_reader &strm)
			{
				return _read(strm);
			}
		};

		struct directory_entry_2_0
		{
			uint32_t offset;
			uint32_t size;
			fourcc_t type;
		};

		struct directory_2_0
		{
			lak::array<lak::x3f::directory_entry_2_0> entries;

			lak::error_code<lak::err::out_of_data> _read(lak::binary_reader &strm);

			template<lak::endian E>
			lak::error_code<lak::err::out_of_data> read(lak::binary_reader &strm)
			{
				return _read(strm);
			}
		};

		struct directory
		{
			lak::x3f::section_header section;
			lak::variant<lak::x3f::directory_2_0> versioned;

			lak::error_code<lak::err::out_of_data> _read(lak::binary_reader &strm);

			template<lak::endian E>
			lak::error_code<lak::err::out_of_data> read(lak::binary_reader &strm)
			{
				return _read(strm);
			}
		};

#define LAK_FOREACH_X3F_IMAGE_TYPE(MACRO, ...)                                \
	MACRO(1, One, __VA_ARGS__)                                                  \
	MACRO(3, Three, __VA_ARGS__)

		enum struct image_type : uint32_t
		{
#define LAK_X3F_IMAGE_TYPE_ENUM(VAL, NAME, ...) NAME = VAL,
			LAK_FOREACH_X3F_IMAGE_TYPE(LAK_X3F_IMAGE_TYPE_ENUM)
#undef LAK_X3F_IMAGE_TYPE_ENUM
		};

#define LAK_FOREACH_X3F_IMAGE_FORMAT(MACRO, ...)                              \
	MACRO(0x06, SD9_SD10_SD14, __VA_ARGS__)                                     \
	MACRO(0x0B, Huffman_DCPM_888_RGB, __VA_ARGS__)                              \
	MACRO(0x12, JPEG_888_RGB, __VA_ARGS__)                                      \
	MACRO(0x1E, DP1_DP1S_DP2_Merril, __VA_ARGS__)                               \
	MACRO(0x23, DP2_Quattro, __VA_ARGS__)                                       \
	MACRO(0x25, SD_Quattro, __VA_ARGS__)                                        \
	MACRO(0x27, SD_Quattro_H, __VA_ARGS__)                                      \
	MACRO(0x29, SD_Quattro_H2, __VA_ARGS__)

		// 3 + 5 = huffman x530
		// 3 + 6 = huffman 10bit
		// 3 + 1e = huffman true
		// 1 + 1e = huffman true (merril)
		// 1 + ?? = huffman true (quattro)

		enum struct image_format : uint32_t
		{
#define LAK_X3F_IMAGE_FORMAT_ENUM(VAL, NAME, ...) NAME = VAL,
			LAK_FOREACH_X3F_IMAGE_FORMAT(LAK_X3F_IMAGE_FORMAT_ENUM)
#undef LAK_X3F_IMAGE_FORMAT_ENUM
		};

		struct image_data_2_0
		{
			lak::x3f::image_type type;
			lak::x3f::image_format format;
			uint32_t columns;
			uint32_t rows;
			uint32_t row_bytes;
		};

		struct image_data
		{
			lak::x3f::section_header section;
			lak::variant<lak::x3f::image_data_2_0> versioned;
			uint32_t offset = uint32_t(-1);
			lak::array<byte_t> data;
			lak::image<lak::vec4u16_t> image;

			lak::error_codes<lak::err::out_of_data, lak::err::value_out_of_range>
			_read(lak::binary_reader &strm);

			template<lak::endian E>
			lak::error_codes<lak::err::out_of_data, lak::err::value_out_of_range>
			read(lak::binary_reader &strm)
			{
				return _read(strm);
			}
		};

		struct camf_data_header
		{
			uint32_t type;
			uint64_t unknown;
			uint32_t columns;
			uint32_t rows;
		};

		struct camf_data
		{
			lak::x3f::section_header section;
			lak::x3f::camf_data_header header;
			lak::array<byte_t> data;

			lak::error_codes<lak::err::out_of_data, lak::err::value_out_of_range>
			_read(lak::binary_reader &strm);

			template<lak::endian E>
			lak::error_codes<lak::err::out_of_data, lak::err::value_out_of_range>
			read(lak::binary_reader &strm)
			{
				ASSERT_EQUAL(strm.position(), 0U);
				return _read(strm);
			}
		};

		struct x3f
		{
			lak::x3f::header header;
			lak::x3f::directory directory;
			lak::array<lak::x3f::image_data> image_entries;
			lak::array<lak::x3f::camf_data> camf_entries;
			lak::array<lak::tiff::tiff> format_12s;

			lak::error_codes<lak::err::out_of_data, lak::err::value_out_of_range>
			_read(lak::binary_reader &strm);

			template<lak::endian E>
			lak::error_codes<lak::err::out_of_data, lak::err::value_out_of_range>
			read(lak::binary_reader &strm)
			{
				ASSERT_EQUAL(strm.position(), 0U);
				return _read(strm);
			}
		};
	}
}

#include "lak/file/x3f.inl"

#endif
