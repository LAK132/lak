#include "lak/test.hpp"

#include "lak/format.hpp"

BEGIN_TEST(format)
{
	{
		constexpr auto fmt = "asdf"_afmt;
		ASSERT_EQUAL(fmt.specifiers.size(), 0U);
		ASSERT_EQUAL(fmt.prefixes.size(), fmt.specifiers.size() + 1U);
		ASSERT_EQUAL(fmt.prefixes_buffer().size(), 4U);
		ASSERT_EQUAL(fmt.specifiers_buffer().size(), 0U);
		ASSERT_EQUAL(fmt.prefixes_buffer(), "asdf"_view);
		ASSERT_EQUAL(fmt.specifiers_buffer(), ""_view);
	}
	{
		constexpr auto fmt = "{}"_afmt;
		ASSERT_EQUAL(fmt.specifiers.size(), 1U);
		ASSERT_EQUAL(fmt.prefixes.size(), fmt.specifiers.size() + 1U);
		ASSERT_EQUAL(fmt.specifiers[0].first, 0U);
		ASSERT_EQUAL(fmt.prefixes_buffer().size(), 0U);
		ASSERT_EQUAL(fmt.specifiers_buffer().size(), 0U);
		ASSERT_EQUAL(fmt.prefixes_buffer(), ""_view);
		ASSERT_EQUAL(fmt.specifiers_buffer(), ""_view);
	}
	{
		constexpr auto fmt = "{1}"_afmt;
		ASSERT_EQUAL(fmt.specifiers.size(), 1U);
		ASSERT_EQUAL(fmt.prefixes.size(), fmt.specifiers.size() + 1U);
		ASSERT_EQUAL(fmt.specifiers[0].first, 1U);
		ASSERT_EQUAL(fmt.prefixes_buffer().size(), 0U);
		ASSERT_EQUAL(fmt.specifiers_buffer().size(), 0U);
		ASSERT_EQUAL(fmt.prefixes_buffer(), ""_view);
		ASSERT_EQUAL(fmt.specifiers_buffer(), ""_view);
	}
	{
		constexpr auto fmt = "{:}"_afmt;
		ASSERT_EQUAL(fmt.specifiers.size(), 1U);
		ASSERT_EQUAL(fmt.prefixes.size(), fmt.specifiers.size() + 1U);
		ASSERT_EQUAL(fmt.specifiers[0].first, 0U);
		ASSERT_EQUAL(fmt.prefixes_buffer().size(), 0U);
		ASSERT_EQUAL(fmt.specifiers_buffer().size(), 0U);
		ASSERT_EQUAL(fmt.prefixes_buffer(), ""_view);
		ASSERT_EQUAL(fmt.specifiers_buffer(), ""_view);
	}
	{
		constexpr auto fmt = "{:asdf}"_afmt;
		ASSERT_EQUAL(fmt.specifiers.size(), 1U);
		ASSERT_EQUAL(fmt.prefixes.size(), fmt.specifiers.size() + 1U);
		ASSERT_EQUAL(fmt.specifiers[0].first, 0U);
		ASSERT_EQUAL(fmt.prefixes_buffer().size(), 0U);
		ASSERT_EQUAL(fmt.specifiers_buffer().size(), 4U);
		ASSERT_EQUAL(fmt.prefixes_buffer(), ""_view);
		ASSERT_EQUAL(fmt.specifiers_buffer(), "asdf"_view);
	}
	{
		constexpr auto fmt = "123{4:hello}asdf"_afmt;
		ASSERT_EQUAL(fmt.specifiers.size(), 1U);
		ASSERT_EQUAL(fmt.prefixes.size(), fmt.specifiers.size() + 1U);
		ASSERT_EQUAL(fmt.specifiers[0].first, 4U);
		ASSERT_EQUAL(fmt.prefixes_buffer().size(), 7U);
		ASSERT_EQUAL(fmt.specifiers_buffer().size(), 5U);
		ASSERT_EQUAL(fmt.prefixes_buffer(), "123asdf"_view);
		ASSERT_EQUAL(fmt.specifiers_buffer(), "hello"_view);
	}
	{
		constexpr auto fmt = "1{1}2{2}3{3}4"_afmt;
		ASSERT_EQUAL(fmt.specifiers.size(), 3U);
		ASSERT_EQUAL(fmt.prefixes.size(), fmt.specifiers.size() + 1U);
		ASSERT_EQUAL(fmt.specifiers[0].first, 1U);
		ASSERT_EQUAL(fmt.specifiers[1].first, 2U);
		ASSERT_EQUAL(fmt.specifiers[2].first, 3U);
		ASSERT_EQUAL(fmt.prefixes_buffer().size(), 4U);
		ASSERT_EQUAL(fmt.specifiers_buffer().size(), 0U);
		ASSERT_EQUAL(fmt.prefixes_buffer(), "1234"_view);
		ASSERT_EQUAL(fmt.specifiers_buffer(), ""_view);
	}
	{
		constexpr auto fmt = "LAK{}{2}{}!"_afmt;
		ASSERT_EQUAL(fmt.specifiers.size(), 3U);
		ASSERT_EQUAL(fmt.prefixes.size(), fmt.specifiers.size() + 1U);
		ASSERT_EQUAL(fmt.specifiers[0].first, 0U);
		ASSERT_EQUAL(fmt.specifiers[1].first, 2U);
		ASSERT_EQUAL(fmt.specifiers[2].first, 1U);
		ASSERT_EQUAL(fmt.prefixes_buffer().size(), 4U);
		ASSERT_EQUAL(fmt.specifiers_buffer().size(), 0U);
		ASSERT_EQUAL(fmt.prefixes_buffer(), "LAK!"_view);
		ASSERT_EQUAL(fmt.specifiers_buffer(), ""_view);
	}

	ASSERT_EQUAL(lak::fmt<"LAK{}{2}{}!">(1, 2, 3), "LAK132!"_view);

	ASSERT_EQUAL(lak::fmt<"hello {}!">("world"), "hello world!"_view);

	ASSERT_EQUAL(lak::fmt<"{:#0.4X}">(0x132U), "0x0132"_view);

	return 0;
}
END_TEST()
