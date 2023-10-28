#include "lak/test.hpp"

#include "lak/structure/nbt.hpp"

#include "lak/compression/deflate.hpp"

#include "lak/binary_writer.hpp"
#include "lak/string_literals.hpp"

BEGIN_TEST(nbt)
{
	{
		lak::array<byte_t> expected = {
		  // tagType
		  byte_t(lak::nbt::tag_type::Compound),

		  // name.length
		  byte_t(0x00),
		  byte_t(0x05),

		  // name.payload
		  byte_t('h'),
		  byte_t('e'),
		  byte_t('l'),
		  byte_t('l'),
		  byte_t('o'),

		  // payload
		  byte_t(lak::nbt::tag_type::End),
		};

		auto root = lak::nbt::named_tag{
		  .name    = {.value = u8"hello"_view},
		  .payload = {.value = lak::nbt::TAG_Compound{}},
		};
		DEBUG_EXPR(root);

		lak::binary_array_writer strm;
		strm.write(root);

		ASSERT_ARRAY_EQUAL(expected, strm.data);
	}

	{
		auto func = [](size_t count)
		{
			lak::array<lak::nbt::TAG_Byte::value_type> result;
			result.reserve(count);
			for (auto n : lak::size_range_count(count))
			{
				result.push_back((n * n * 255 + n * 7) % 100);
			}
			return result;
		};

		lak::nbt::named_tag root = lak::nbt::make_compound(
		  u8"Level"_str,
		  {
		    lak::nbt::make_long(u8"longTest"_str, 9223372036854775807),
		    lak::nbt::make_short(u8"shortTest"_str, 32767),
		    lak::nbt::make_string(
		      u8"stringTest"_str,
		      u8"HELLO WORLD THIS IS A TEST STRING \u00C5\u00C4\u00D6!"_str),
		    lak::nbt::make_float(u8"floatTest"_str, 0.49823147f),
		    lak::nbt::make_int(u8"intTest"_str, 2147483647),
		    lak::nbt::make_compound(
		      u8"nested compound test"_str,
		      {
		        lak::nbt::make_compound(
		          u8"ham"_str,
		          {
		            lak::nbt::make_string(u8"name"_str, u8"Hampus"_str),
		            lak::nbt::make_float(u8"value"_str, 0.75f),
		          }),
		        lak::nbt::make_compound(
		          u8"egg"_str,
		          {
		            lak::nbt::make_string(u8"name"_str, u8"Eggbert"_str),
		            lak::nbt::make_float(u8"value"_str, 0.5f),
		          }),
		      }),
		    lak::nbt::make_list<lak::nbt::TAG_Long>(
		      u8"listTest (long)"_str, {{11}, {12}, {13}, {14}, {15}}),
		    lak::nbt::make_list<lak::nbt::TAG_Compound>(
		      u8"listTest (compound)"_str,
		      {
		        {{
		          lak::nbt::make_string(u8"name"_str, u8"Compound tag #0"_str),
		          lak::nbt::make_long(u8"created-on"_str, 1264099775885),
		        }},
		        {{
		          lak::nbt::make_string(u8"name"_str, u8"Compound tag #1"_str),
		          lak::nbt::make_long(u8"created-on"_str, 1264099775885),
		        }},
		      }),
		    lak::nbt::make_byte(u8"byteTest"_str, 127),
		    lak::nbt::make_byte_array(
		      u8"byteArrayTest (the first 1000 values of (n*n*255+n*7)%100, "
		      "starting with n=0 (0, 62, 34, 16, 8, ...))"_str,
		      func(1000)),
		    lak::nbt::make_double(u8"doubleTest"_str, 0.4931287132182315),
		  });

		lak::binary_array_writer strm;
		strm.write(root);

		const lak::array<byte_t> compressed = {
		  byte_t(0x1F), byte_t(0x8B), byte_t(0x08), byte_t(0x00), byte_t(0x00),
		  byte_t(0x00), byte_t(0x00), byte_t(0x00), byte_t(0x00), byte_t(0x00),
		  byte_t(0xED), byte_t(0x54), byte_t(0xCF), byte_t(0x4F), byte_t(0x1A),
		  byte_t(0x41), byte_t(0x14), byte_t(0x7E), byte_t(0xC2), byte_t(0x02),
		  byte_t(0xCB), byte_t(0x96), byte_t(0x82), byte_t(0xB1), byte_t(0xC4),
		  byte_t(0x10), byte_t(0x63), byte_t(0xCC), byte_t(0xAB), byte_t(0xB5),
		  byte_t(0x84), byte_t(0xA5), byte_t(0xDB), byte_t(0xCD), byte_t(0x42),
		  byte_t(0x11), byte_t(0x89), byte_t(0xB1), byte_t(0x88), byte_t(0x16),
		  byte_t(0x2C), byte_t(0x9A), byte_t(0x0D), byte_t(0x1A), byte_t(0xD8),
		  byte_t(0xA8), byte_t(0x31), byte_t(0x86), byte_t(0xB8), byte_t(0x2B),
		  byte_t(0xC3), byte_t(0x82), byte_t(0x2E), byte_t(0xBB), byte_t(0x66),
		  byte_t(0x77), byte_t(0xB0), byte_t(0xF1), byte_t(0xD4), byte_t(0x4B),
		  byte_t(0x7B), byte_t(0x6C), byte_t(0x7A), byte_t(0xEB), byte_t(0x3F),
		  byte_t(0xD3), byte_t(0x23), byte_t(0x7F), byte_t(0x43), byte_t(0xCF),
		  byte_t(0xBD), byte_t(0xF6), byte_t(0xBF), byte_t(0xA0), byte_t(0xC3),
		  byte_t(0x2F), byte_t(0x7B), byte_t(0x69), byte_t(0xCF), byte_t(0xBD),
		  byte_t(0xF0), byte_t(0x32), byte_t(0xC9), byte_t(0xF7), byte_t(0xE6),
		  byte_t(0xBD), byte_t(0x6F), byte_t(0xE6), byte_t(0x7B), byte_t(0x6F),
		  byte_t(0x26), byte_t(0x79), byte_t(0x02), byte_t(0x04), byte_t(0x54),
		  byte_t(0x72), byte_t(0x4F), byte_t(0x2C), byte_t(0x0E), byte_t(0x78),
		  byte_t(0xCB), byte_t(0xB1), byte_t(0x4D), byte_t(0x8D), byte_t(0x78),
		  byte_t(0xF4), byte_t(0xE3), byte_t(0x70), byte_t(0x62), byte_t(0x3E),
		  byte_t(0x08), byte_t(0x7B), byte_t(0x1D), byte_t(0xC7), byte_t(0xA5),
		  byte_t(0x93), byte_t(0x18), byte_t(0x0F), byte_t(0x82), byte_t(0x47),
		  byte_t(0xDD), byte_t(0xEE), byte_t(0x84), byte_t(0x02), byte_t(0x62),
		  byte_t(0xB5), byte_t(0xA2), byte_t(0xAA), byte_t(0xC7), byte_t(0x78),
		  byte_t(0x76), byte_t(0x5C), byte_t(0x57), byte_t(0xCB), byte_t(0xA8),
		  byte_t(0x55), byte_t(0x0F), byte_t(0x1B), byte_t(0xC8), byte_t(0xD6),
		  byte_t(0x1E), byte_t(0x6A), byte_t(0x95), byte_t(0x86), byte_t(0x86),
		  byte_t(0x0D), byte_t(0xAD), byte_t(0x7E), byte_t(0x58), byte_t(0x7B),
		  byte_t(0x8F), byte_t(0x83), byte_t(0xCF), byte_t(0x83), byte_t(0x4F),
		  byte_t(0x83), byte_t(0x6F), byte_t(0xCF), byte_t(0x03), byte_t(0x10),
		  byte_t(0x6E), byte_t(0x5B), byte_t(0x8E), byte_t(0x3E), byte_t(0xBE),
		  byte_t(0xA5), byte_t(0x38), byte_t(0x4C), byte_t(0x64), byte_t(0xFD),
		  byte_t(0x10), byte_t(0xEA), byte_t(0xDA), byte_t(0x74), byte_t(0xA6),
		  byte_t(0x23), byte_t(0x40), byte_t(0xDC), byte_t(0x66), byte_t(0x2E),
		  byte_t(0x69), byte_t(0xE1), byte_t(0xB5), byte_t(0xD3), byte_t(0xBB),
		  byte_t(0x73), byte_t(0xFA), byte_t(0x76), byte_t(0x0B), byte_t(0x29),
		  byte_t(0xDB), byte_t(0x0B), byte_t(0xE0), byte_t(0xEF), byte_t(0xE8),
		  byte_t(0x3D), byte_t(0x1E), byte_t(0x38), byte_t(0x5B), byte_t(0xEF),
		  byte_t(0x11), byte_t(0x08), byte_t(0x56), byte_t(0xF5), byte_t(0xDE),
		  byte_t(0x5D), byte_t(0xDF), byte_t(0x0B), byte_t(0x40), byte_t(0xE0),
		  byte_t(0x5E), byte_t(0xB7), byte_t(0xFA), byte_t(0x64), byte_t(0xB7),
		  byte_t(0x04), byte_t(0x00), byte_t(0x8C), byte_t(0x41), byte_t(0x4C),
		  byte_t(0x73), byte_t(0xC6), byte_t(0x08), byte_t(0x55), byte_t(0x4C),
		  byte_t(0xD3), byte_t(0x20), byte_t(0x2E), byte_t(0x7D), byte_t(0xA4),
		  byte_t(0xC0), byte_t(0xC8), byte_t(0xC2), byte_t(0x10), byte_t(0xB3),
		  byte_t(0xBA), byte_t(0xDE), byte_t(0x58), byte_t(0x0B), byte_t(0x53),
		  byte_t(0xA3), byte_t(0xEE), byte_t(0x44), byte_t(0x8E), byte_t(0x45),
		  byte_t(0x03), byte_t(0x30), byte_t(0xB1), byte_t(0x27), byte_t(0x53),
		  byte_t(0x8C), byte_t(0x4C), byte_t(0xF1), byte_t(0xE9), byte_t(0x14),
		  byte_t(0xA3), byte_t(0x53), byte_t(0x8C), byte_t(0x85), byte_t(0xE1),
		  byte_t(0xD9), byte_t(0x9F), byte_t(0xE3), byte_t(0xB3), byte_t(0xF2),
		  byte_t(0x44), byte_t(0x81), byte_t(0xA5), byte_t(0x7C), byte_t(0x33),
		  byte_t(0xDD), byte_t(0xD8), byte_t(0xBB), byte_t(0xC7), byte_t(0xAA),
		  byte_t(0x75), byte_t(0x13), byte_t(0x5F), byte_t(0x28), byte_t(0x1C),
		  byte_t(0x08), byte_t(0xD7), byte_t(0x2E), byte_t(0xD1), byte_t(0x59),
		  byte_t(0x3F), byte_t(0xAF), byte_t(0x1D), byte_t(0x1B), byte_t(0x60),
		  byte_t(0x21), byte_t(0x59), byte_t(0xDF), byte_t(0xFA), byte_t(0xF1),
		  byte_t(0x05), byte_t(0xFE), byte_t(0xC1), byte_t(0xCE), byte_t(0xFC),
		  byte_t(0x9D), byte_t(0xBD), byte_t(0x00), byte_t(0xBC), byte_t(0xF1),
		  byte_t(0x40), byte_t(0xC9), byte_t(0xF8), byte_t(0x85), byte_t(0x42),
		  byte_t(0x40), byte_t(0x46), byte_t(0xFE), byte_t(0x9E), byte_t(0xEB),
		  byte_t(0xEA), byte_t(0x0F), byte_t(0x93), byte_t(0x3A), byte_t(0x68),
		  byte_t(0x87), byte_t(0x60), byte_t(0xBB), byte_t(0xEB), byte_t(0x32),
		  byte_t(0x37), byte_t(0xA3), byte_t(0x28), byte_t(0x0A), byte_t(0x8E),
		  byte_t(0xBB), byte_t(0xF5), byte_t(0xD0), byte_t(0x69), byte_t(0x63),
		  byte_t(0xCA), byte_t(0x4E), byte_t(0xDB), byte_t(0xE9), byte_t(0xEC),
		  byte_t(0xE6), byte_t(0xE6), byte_t(0x2B), byte_t(0x3B), byte_t(0xBD),
		  byte_t(0x25), byte_t(0xBE), byte_t(0x64), byte_t(0x49), byte_t(0x09),
		  byte_t(0x3D), byte_t(0xAA), byte_t(0xBB), byte_t(0x94), byte_t(0xFD),
		  byte_t(0x18), byte_t(0x7E), byte_t(0xE8), byte_t(0xD2), byte_t(0x0E),
		  byte_t(0xDA), byte_t(0x6F), byte_t(0x15), byte_t(0x4C), byte_t(0xB1),
		  byte_t(0x68), byte_t(0x3E), byte_t(0x2B), byte_t(0xE1), byte_t(0x9B),
		  byte_t(0x9C), byte_t(0x84), byte_t(0x99), byte_t(0xBC), byte_t(0x84),
		  byte_t(0x05), byte_t(0x09), byte_t(0x65), byte_t(0x59), byte_t(0x16),
		  byte_t(0x45), byte_t(0x00), byte_t(0xFF), byte_t(0x2F), byte_t(0x28),
		  byte_t(0xAE), byte_t(0x2F), byte_t(0xF2), byte_t(0xC2), byte_t(0xB2),
		  byte_t(0xA4), byte_t(0x2E), byte_t(0x1D), byte_t(0x20), byte_t(0x77),
		  byte_t(0x5A), byte_t(0x3B), byte_t(0xB9), byte_t(0x8C), byte_t(0xCA),
		  byte_t(0xE7), byte_t(0x29), byte_t(0xDF), byte_t(0x51), byte_t(0x41),
		  byte_t(0xC9), byte_t(0x16), byte_t(0xB5), byte_t(0xC5), byte_t(0x6D),
		  byte_t(0xA1), byte_t(0x2A), byte_t(0xAD), byte_t(0x2C), byte_t(0xC5),
		  byte_t(0x31), byte_t(0x7F), byte_t(0xBA), byte_t(0x7A), byte_t(0x92),
		  byte_t(0x8E), byte_t(0x5E), byte_t(0x9D), byte_t(0x5F), byte_t(0xF8),
		  byte_t(0x12), byte_t(0x05), byte_t(0x23), byte_t(0x1B), byte_t(0xD1),
		  byte_t(0xF6), byte_t(0xB7), byte_t(0x77), byte_t(0xAA), byte_t(0xCD),
		  byte_t(0x95), byte_t(0x72), byte_t(0xBC), byte_t(0x9E), byte_t(0xDF),
		  byte_t(0x58), byte_t(0x5D), byte_t(0x4B), byte_t(0x97), byte_t(0xAE),
		  byte_t(0x92), byte_t(0x17), byte_t(0xB9), byte_t(0x44), byte_t(0xD0),
		  byte_t(0x80), byte_t(0xC8), byte_t(0xFA), byte_t(0x3E), byte_t(0xBF),
		  byte_t(0xB3), byte_t(0xDC), byte_t(0x54), byte_t(0xCB), byte_t(0x07),
		  byte_t(0x75), byte_t(0x6E), byte_t(0xA3), byte_t(0xB6), byte_t(0x76),
		  byte_t(0x59), byte_t(0x92), byte_t(0x93), byte_t(0xA9), byte_t(0xDC),
		  byte_t(0x51), byte_t(0x50), byte_t(0x99), byte_t(0x6B), byte_t(0xCC),
		  byte_t(0x35), byte_t(0xE6), byte_t(0x1A), byte_t(0xFF), byte_t(0x57),
		  byte_t(0x23), byte_t(0x08), byte_t(0x42), byte_t(0xCB), byte_t(0xE9),
		  byte_t(0x1B), byte_t(0xD6), byte_t(0x78), byte_t(0xC2), byte_t(0xEC),
		  byte_t(0xFE), byte_t(0xFC), byte_t(0x7A), byte_t(0xFB), byte_t(0x7D),
		  byte_t(0x78), byte_t(0xD3), byte_t(0x84), byte_t(0xDF), byte_t(0xD4),
		  byte_t(0xF2), byte_t(0xA4), byte_t(0xFB), byte_t(0x08), byte_t(0x06),
		  byte_t(0x00), byte_t(0x00)};

		lak::array<byte_t, 0x8000> buff;
		lak::deflate_iterator iter{
		  compressed, buff, lak::deflate_iterator::header_t::gzip, false};
		lak::array<byte_t> out;
		out.reserve(buff.size());
		iter
		  .read(
		    [&](lak::span<byte_t> v)
		    {
			    out.resize(out.size() + v.size());
			    lak::memcpy(lak::span(out).last(v.size()), v);
			    return true;
		    })
		  .UNWRAP();

		lak::binary_reader strm2{out};
		[[maybe_unused]] auto tag =
		  strm2.template read<lak::nbt::named_tag>().UNWRAP();

		ASSERT_ARRAY_EQUAL(strm.data, out);
	}

	return 0;
}
END_TEST()
