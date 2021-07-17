#include "lak/bit_reader.hpp"
#include "lak/span.hpp"
#include "lak/string_view.hpp"

#include "lak/test.hpp"

BEGIN_TEST(bit_reader)
{
  auto data = lak::span<const uint8_t>(lak::string_view("ABCD "));

  lak::bit_reader reader;

  ASSERT(reader.empty());

  reader.peek_bits(0).EXPECT(
    "reading 0 bits should succeed even when the reader is empty");
  reader.peek_bits(1).EXPECT_ERR(
    "reading more than 0 bits should fail when the reader is empty");

  reader.reset_data(data);
  ASSERT(!reader.empty());

  ASSERT_EQUAL(reader.peek_bits(8).unwrap(), uint8_t('A'));
  ASSERT_EQUAL(reader.read_bits(8).unwrap(), uint8_t('A'));

  ASSERT_EQUAL(reader.peek_bits(8).unwrap(), uint8_t('B'));
  ASSERT_EQUAL(reader.read_bits(8).unwrap(), uint8_t('B'));

  ASSERT_EQUAL(reader.read_bits(8).unwrap(), uint8_t('C'));
  ASSERT_EQUAL(reader.read_bits(8).unwrap(), uint8_t('D'));

  ASSERT_EQUAL(int(reader.read_bits(4).unwrap()), 0x0);
  ASSERT_EQUAL(int(reader.read_bits(4).unwrap()), 0x2);

  ASSERT(reader.empty());

  for (uint8_t i = 0; i < 4; ++i)
  {
    reader.reset_data(data.subspan(i, 1));
    for (uint8_t j = 0; j <= (i + 1) * 8; ++j)
    {
      reader.peek_bits(j).UNWRAP();
    }
  }
  ASSERT_EQUAL(reader.peek_bits(4 * 8).unwrap(), 'DCBA');

  return 0;
}
END_TEST()