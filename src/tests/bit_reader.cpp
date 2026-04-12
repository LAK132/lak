#include "lak/bit_reader.hpp"
#include "lak/span.hpp"
#include "lak/string_view.hpp"

#include "lak/test.hpp"

BEGIN_TEST(bit_reader)
{
	{
		auto data = lak::span<const byte_t>(lak::span(lak::string_view("ABCD ")));

		lak::bit_reader strm;

		ASSERT(strm.empty());

		strm.peek_bits(0).EXPECT(
		  "reading 0 bits should succeed even when the strm is empty");
		strm.peek_bits(1).EXPECT_ERR(
		  "reading more than 0 bits should fail when the strm is empty");

		strm.reset_data(data);
		DEBUG_EXPR(strm.bytes_remaining());
		ASSERT(!strm.empty());

		ASSERT_EQUAL(strm.peek_bits(8).UNWRAP(), uint8_t('A'));
		ASSERT_EQUAL(strm.read_bits(8).UNWRAP(), uint8_t('A'));

		ASSERT_EQUAL(strm.peek_bits(8).UNWRAP(), uint8_t('B'));
		ASSERT_EQUAL(strm.read_bits(8).UNWRAP(), uint8_t('B'));

		ASSERT_EQUAL(strm.read_bits(8).UNWRAP(), uint8_t('C'));
		ASSERT_EQUAL(strm.read_bits(8).UNWRAP(), uint8_t('D'));

		ASSERT_EQUAL(int(strm.read_bits(4).UNWRAP()), 0x0);
		ASSERT_EQUAL(int(strm.read_bits(4).UNWRAP()), 0x2);

		ASSERT(strm.empty());

		for (uint8_t i = 0; i < 4; ++i)
		{
			strm.reset_data(data.subspan(i, 1));
			for (uint8_t j = 0; j <= (i + 1) * 8; ++j)
			{
				strm.peek_bits(j).UNWRAP();
			}
		}
		// ASSERT_EQUAL(strm.peek_bits(4 * 8).unwrap(), 'DCBA');
		// :TODO: multichar warning on gcc
	}

	{
		lak::array<byte_t> data{
		  byte_t(0b1010'1010),
		  byte_t(0b1010'0000),
		  byte_t(0b0000'1010),
		  byte_t(0b1111'0000),
		  byte_t(0b0101'0101),
		  byte_t(0b0101'0000),
		  byte_t(0b0110'0101),
		  byte_t(0b1001'1010),
		};
		{
			lak::bit_reader<lak::endian::little, lak::endian::little> strm{
			  lak::span(data)};
			ASSERT_EQUAL(strm.read_bits(16U).UNWRAP(),
			             uint16_t(0b1010'0000'1010'1010));
			ASSERT_EQUAL(strm.read_bits(16U).UNWRAP(),
			             uint16_t(0b1111'0000'0000'1010));
			ASSERT_EQUAL(strm.read_bits(16U).UNWRAP(),
			             uint16_t(0b0101'0000'0101'0101));
			ASSERT_EQUAL(strm.read_bits(4U).UNWRAP(), uint8_t(0b0101));
			ASSERT_EQUAL(strm.read_bits(4U).UNWRAP(), uint8_t(0b0110));
			ASSERT_EQUAL(strm.read_bits(4U).UNWRAP(), uint8_t(0b1010));
			ASSERT_EQUAL(strm.read_bits(4U).UNWRAP(), uint8_t(0b1001));
			ASSERT(strm.empty());
		}
		{
			lak::bit_reader<lak::endian::little, lak::endian::big> strm{
			  lak::span(data)};
			ASSERT_EQUAL(strm.read_bits(16U).UNWRAP(),
			             uint16_t(0b1010'1010'1010'0000));
			ASSERT_EQUAL(strm.read_bits(16U).UNWRAP(),
			             uint16_t(0b0000'1010'1111'0000));
			ASSERT_EQUAL(strm.read_bits(16U).UNWRAP(),
			             uint16_t(0b0101'0101'0101'0000));
			ASSERT_EQUAL(strm.read_bits(4U).UNWRAP(), uint8_t(0b0110));
			ASSERT_EQUAL(strm.read_bits(4U).UNWRAP(), uint8_t(0b0101));
			ASSERT_EQUAL(strm.read_bits(4U).UNWRAP(), uint8_t(0b1001));
			ASSERT_EQUAL(strm.read_bits(4U).UNWRAP(), uint8_t(0b1010));
			ASSERT(strm.empty());
		}
		{
			lak::bit_reader<lak::endian::big, lak::endian::little> strm{
			  lak::span(data)};
			ASSERT_EQUAL(strm.read_bits(16U).UNWRAP(),
			             uint16_t(0b1010'1010'1010'0000));
			ASSERT_EQUAL(strm.read_bits(16U).UNWRAP(),
			             uint16_t(0b0000'1010'1111'0000));
			ASSERT_EQUAL(strm.read_bits(16U).UNWRAP(),
			             uint16_t(0b0101'0101'0101'0000));
			ASSERT_EQUAL(strm.read_bits(4U).UNWRAP(), uint8_t(0b0101));
			ASSERT_EQUAL(strm.read_bits(4U).UNWRAP(), uint8_t(0b0110));
			ASSERT_EQUAL(strm.read_bits(4U).UNWRAP(), uint8_t(0b1010));
			ASSERT_EQUAL(strm.read_bits(4U).UNWRAP(), uint8_t(0b1001));
			ASSERT(strm.empty());
		}
		{
			lak::bit_reader<lak::endian::big, lak::endian::big> strm{
			  lak::span(data)};
			ASSERT_EQUAL(strm.read_bits(16U).UNWRAP(),
			             uint16_t(0b1010'0000'1010'1010));
			ASSERT_EQUAL(strm.read_bits(16U).UNWRAP(),
			             uint16_t(0b1111'0000'0000'1010));
			ASSERT_EQUAL(strm.read_bits(16U).UNWRAP(),
			             uint16_t(0b0101'0000'0101'0101));
			ASSERT_EQUAL(strm.read_bits(4U).UNWRAP(), uint8_t(0b0110));
			ASSERT_EQUAL(strm.read_bits(4U).UNWRAP(), uint8_t(0b0101));
			ASSERT_EQUAL(strm.read_bits(4U).UNWRAP(), uint8_t(0b1001));
			ASSERT_EQUAL(strm.read_bits(4U).UNWRAP(), uint8_t(0b1010));
			ASSERT(strm.empty());
		}
		{
			lak::bit_reader<lak::endian::little, lak::endian::big> strm{
			  lak::span(data)};
			strm.skip_bits(4U).UNWRAP();
			ASSERT_EQUAL(strm.read_bits(12U).UNWRAP(), uint16_t(0b1010'1010'0000));
			ASSERT_EQUAL(strm.read_bits(8U).UNWRAP(), uint8_t(0b0000'1010));
			ASSERT_EQUAL(strm.read_bits(8U).UNWRAP(), uint8_t(0b1111'0000));
			ASSERT_EQUAL(strm.read_bits(8U).UNWRAP(), uint8_t(0b0101'0101));
			ASSERT_EQUAL(strm.read_bits(8U).UNWRAP(), uint8_t(0b0101'0000));
			ASSERT_EQUAL(strm.read_bits(4U).UNWRAP(), uint8_t(0b0110));
			ASSERT_EQUAL(strm.read_bits(4U).UNWRAP(), uint8_t(0b0101));
			ASSERT_EQUAL(strm.read_bits(4U).UNWRAP(), uint8_t(0b1001));
			ASSERT_EQUAL(strm.read_bits(4U).UNWRAP(), uint8_t(0b1010));
			ASSERT(strm.empty());
		}
	}

	return EXIT_SUCCESS;
}
END_TEST()
