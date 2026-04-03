#include "lak/test.hpp"

#include "lak/file/tiff.hpp"

#include "lak/string_literals/magic.hpp"

BEGIN_TEST(tiff)
{
	{
		SCOPED_CHECKPOINT("test small");
		lak::binary_array_writer expected;
		// ifh
		// byte order
		expected.write_u16le(u8"II"_magic_le).UNWRAP();
		// version
		expected.write_u16le(42).UNWRAP();
		// 1st ifd offset
		expected.write_u32le(expected.size() + sizeof(uint32_t)).UNWRAP();

		// ifd0
		// # of entries
		expected.write_u16le(0x0).UNWRAP();
		// next ifd
		expected.write_u32le(0x0).UNWRAP();

		lak::tiff::tiff tiff;
		tiff.ifh.version = 42;
		tiff.ifd.emplace_back();

		lak::binary_array_writer strm;
		strm.template write<lak::endian::little>(tiff).UNWRAP();

		ASSERT_ARRAY_EQUAL(expected.data, strm.data);
	}
	{
		SCOPED_CHECKPOINT("test large");
		lak::binary_array_writer expected;
		// ifh
		// byte order
		expected.write_u16le(u8"II"_magic_le).UNWRAP();
		// version
		expected.write_u16le(42).UNWRAP();
		// 1st ifd offset
		expected.write_u32le(expected.size() + sizeof(uint32_t)).UNWRAP();

		// ifd0
		// # of entries
		expected.write_u16le(0x6).UNWRAP();
		// NewSubfileType
		expected.write_u16le(0xFE).UNWRAP();
		expected.write_u16le(0x4).UNWRAP();
		expected.write_u32le(0x1).UNWRAP();
		expected.write_u32le(0x0).UNWRAP();
		// ImageWidth
		expected.write_u16le(0x100).UNWRAP();
		expected.write_u16le(0x4).UNWRAP();
		expected.write_u32le(0x1).UNWRAP();
		expected.write_u32le(0x7D0).UNWRAP();
		// ImageLength
		expected.write_u16le(0x101).UNWRAP();
		expected.write_u16le(0x4).UNWRAP();
		expected.write_u32le(0x1).UNWRAP();
		expected.write_u32le(0xBB8).UNWRAP();
		// Compression
		expected.write_u16le(0x103).UNWRAP();
		expected.write_u16le(0x3).UNWRAP();
		expected.write_u32le(0x1).UNWRAP();
		expected.write_u16le(0x8005).UNWRAP();
		expected.write_u16le(0x0).UNWRAP();
		// PhotometricInterpretation
		expected.write_u16le(0x106).UNWRAP();
		expected.write_u16le(0x3).UNWRAP();
		expected.write_u32le(0x1).UNWRAP();
		expected.write_u16le(0x1).UNWRAP();
		expected.write_u16le(0x0).UNWRAP();
		// rows per strip
		expected.write_u16le(0x116).UNWRAP();
		expected.write_u16le(0x4).UNWRAP();
		expected.write_u32le(0x1).UNWRAP();
		expected.write_u32le(0x10).UNWRAP();

		// next ifd
		expected.write_u32le(0x0).UNWRAP();

		{
			lak::tiff::tiff tiff;
			tiff.ifh.version = 42;
			auto &ifd        = tiff.ifd.emplace_back();
			ifd.push_NewSubfileType(lak::fixed_array(uint32_t(0U)));
			ifd.push_ImageWidth(lak::fixed_array(uint32_t(0x7D0U)));
			ifd.push_ImageLength(lak::fixed_array(uint32_t(0xBB8U)));
			ifd.push_Compression(lak::fixed_array(uint16_t(0x8005U)));
			ifd.push_PhotometricInterpretation(lak::fixed_array(uint16_t(1U)));
			ifd.push_RowsPerStrip(lak::fixed_array(uint32_t(0x10U)));

			lak::binary_array_writer strm;
			strm.template write<lak::endian::little>(tiff).UNWRAP();

			ASSERT_ARRAY_EQUAL(expected.data, strm.data);
		}
		{
			lak::binary_reader strm{expected.data};
			auto tiff = strm.read<lak::tiff::tiff, lak::endian::little>().UNWRAP();
			ASSERT_EQUAL(tiff.ifh.version, 42U);
			ASSERT_EQUAL(tiff.ifd.size(), 0x1U);

			ASSERT_EQUAL(tiff.ifd[0].strips.size(), 0x0U);

			ASSERT_EQUAL(tiff.ifd[0].tags.size(), 0x6U);

			auto unreachable = [](auto &&a) { ASSERT_UNREACHABLE(); };

			ASSERT_EQUAL(tiff.ifd[0].tags[0].id,
			             lak::tiff::tag_name::NewSubfileType);
			tiff.ifd[0].tags[0].data.visit(
			  lak::overloaded{[](lak::span<uint32_t> d)
			                  {
				                  ASSERT_EQUAL(d.size(), 0x1U);
				                  ASSERT_EQUAL(d[0], 0x0U);
			                  },
			                  unreachable});
			ASSERT_EQUAL(tiff.ifd[0].tags[1].id, lak::tiff::tag_name::ImageWidth);
			tiff.ifd[0].tags[1].data.visit(
			  lak::overloaded{[](lak::span<uint32_t> d)
			                  {
				                  ASSERT_EQUAL(d.size(), 0x1U);
				                  ASSERT_EQUAL(d[0], 0x7D0U);
			                  },
			                  unreachable});
			ASSERT_EQUAL(tiff.ifd[0].tags[2].id, lak::tiff::tag_name::ImageLength);
			tiff.ifd[0].tags[2].data.visit(
			  lak::overloaded{[](lak::span<uint32_t> d)
			                  {
				                  ASSERT_EQUAL(d.size(), 0x1U);
				                  ASSERT_EQUAL(d[0], 0xBB8U);
			                  },
			                  unreachable});
			ASSERT_EQUAL(tiff.ifd[0].tags[3].id, lak::tiff::tag_name::Compression);
			tiff.ifd[0].tags[3].data.visit(
			  lak::overloaded{[](lak::span<uint16_t> d)
			                  {
				                  ASSERT_EQUAL(d.size(), 0x1U);
				                  ASSERT_EQUAL(d[0], 0x8005U);
			                  },
			                  unreachable});
			ASSERT_EQUAL(tiff.ifd[0].tags[4].id,
			             lak::tiff::tag_name::PhotometricInterpretation);
			tiff.ifd[0].tags[4].data.visit(
			  lak::overloaded{[](lak::span<uint16_t> d)
			                  {
				                  ASSERT_EQUAL(d.size(), 0x1U);
				                  ASSERT_EQUAL(d[0], 0x1U);
			                  },
			                  unreachable});
			ASSERT_EQUAL(tiff.ifd[0].tags[5].id, lak::tiff::tag_name::RowsPerStrip);
			tiff.ifd[0].tags[5].data.visit(
			  lak::overloaded{[](lak::span<uint32_t> d)
			                  {
				                  ASSERT_EQUAL(d.size(), 0x1U);
				                  ASSERT_EQUAL(d[0], 0x10U);
			                  },
			                  unreachable});
		}
	}
	{
		SCOPED_CHECKPOINT("test strips")
		lak::binary_array_writer strm;
		{
			lak::tiff::tiff tiff;

			auto &ifd = tiff.ifd.emplace_back();
			ifd.rows  = 1;
			for (uint8_t i = 0; i < 10; ++i)
			{
				auto &strip = ifd.push_strip();
				strip.data.resize(0x10);
				lak::fill<byte_t>(strip.data, byte_t(i));
			}

			strm.write_be(tiff).UNWRAP();
		}
		{
			auto tiff = lak::binary_reader{strm.data}
			              .read<lak::tiff::tiff, lak::endian::big>()
			              .UNWRAP();
			ASSERT_EQUAL(tiff.ifd.size(), 0x1U);
			ASSERT_EQUAL(tiff.ifd[0].tags.size(), 0x3U);
			ASSERT_EQUAL(tiff.ifd[0].tags[0].id, lak::tiff::tag_name::StripOffsets);
			ASSERT_EQUAL(tiff.ifd[0].tags[1].id, lak::tiff::tag_name::RowsPerStrip);
			ASSERT_EQUAL(tiff.ifd[0].tags[2].id,
			             lak::tiff::tag_name::StripByteCounts);
		}
	}
	return 0;
}
END_TEST()
