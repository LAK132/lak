#include "lak/test.hpp"

#include "lak/format.hpp"
#include "lak/string_literals/string.hpp"
#include "lak/string_literals/view.hpp"

BEGIN_TEST(format)
{
	{
		constexpr auto fmt = lak::format_string<"asdf">{};
		ASSERT_EQUAL(fmt.specifiers.size(), 0U);
		ASSERT_EQUAL(fmt.prefixes.size(), fmt.specifiers.size() + 1U);
		ASSERT_EQUAL(fmt.prefixes_buffer().size(), 4U);
		ASSERT_EQUAL(fmt.specifiers_buffer().size(), 0U);
		ASSERT_EQUAL(fmt.prefixes_buffer(), "asdf"_view);
		ASSERT_EQUAL(fmt.specifiers_buffer(), ""_view);
	}
	{
		constexpr auto fmt = lak::format_string<"{}">{};
		ASSERT_EQUAL(fmt.specifiers.size(), 1U);
		ASSERT_EQUAL(fmt.prefixes.size(), fmt.specifiers.size() + 1U);
		ASSERT_EQUAL(fmt.specifiers[0].first, 0U);
		ASSERT_EQUAL(fmt.prefixes_buffer().size(), 0U);
		ASSERT_EQUAL(fmt.specifiers_buffer().size(), 0U);
		ASSERT_EQUAL(fmt.prefixes_buffer(), ""_view);
		ASSERT_EQUAL(fmt.specifiers_buffer(), ""_view);
	}
	{
		constexpr auto fmt = lak::format_string<"{{}}">{};
		ASSERT_EQUAL(fmt.specifiers.size(), 0U);
		ASSERT_EQUAL(fmt.prefixes.size(), fmt.specifiers.size() + 1U);
		ASSERT_EQUAL(fmt.prefixes_buffer().size(), 2U);
		ASSERT_EQUAL(fmt.specifiers_buffer().size(), 0U);
		ASSERT_EQUAL(fmt.prefixes_buffer(), "{}"_view);
		ASSERT_EQUAL(fmt.specifiers_buffer(), ""_view);
	}
	{
		constexpr auto fmt = lak::format_string<"{{{}}}">{};
		ASSERT_EQUAL(fmt.specifiers.size(), 1U);
		ASSERT_EQUAL(fmt.prefixes.size(), fmt.specifiers.size() + 1U);
		ASSERT_EQUAL(fmt.specifiers[0].first, 0U);
		ASSERT_EQUAL(fmt.prefixes_buffer().size(), 2U);
		ASSERT_EQUAL(fmt.specifiers_buffer().size(), 0U);
		ASSERT_EQUAL(fmt.prefixes_buffer(), "{}"_view);
		ASSERT_EQUAL(fmt.specifiers_buffer(), ""_view);
	}
	{
		constexpr auto fmt = lak::format_string<"{1}">{};
		ASSERT_EQUAL(fmt.specifiers.size(), 1U);
		ASSERT_EQUAL(fmt.prefixes.size(), fmt.specifiers.size() + 1U);
		ASSERT_EQUAL(fmt.specifiers[0].first, 1U);
		ASSERT_EQUAL(fmt.prefixes_buffer().size(), 0U);
		ASSERT_EQUAL(fmt.specifiers_buffer().size(), 0U);
		ASSERT_EQUAL(fmt.prefixes_buffer(), ""_view);
		ASSERT_EQUAL(fmt.specifiers_buffer(), ""_view);
	}
	{
		constexpr auto fmt = lak::format_string<"{:}">{};
		ASSERT_EQUAL(fmt.specifiers.size(), 1U);
		ASSERT_EQUAL(fmt.prefixes.size(), fmt.specifiers.size() + 1U);
		ASSERT_EQUAL(fmt.specifiers[0].first, 0U);
		ASSERT_EQUAL(fmt.prefixes_buffer().size(), 0U);
		ASSERT_EQUAL(fmt.specifiers_buffer().size(), 0U);
		ASSERT_EQUAL(fmt.prefixes_buffer(), ""_view);
		ASSERT_EQUAL(fmt.specifiers_buffer(), ""_view);
	}
	{
		constexpr auto fmt = lak::format_string<"{:asdf}">{};
		ASSERT_EQUAL(fmt.specifiers.size(), 1U);
		ASSERT_EQUAL(fmt.prefixes.size(), fmt.specifiers.size() + 1U);
		ASSERT_EQUAL(fmt.specifiers[0].first, 0U);
		ASSERT_EQUAL(fmt.prefixes_buffer().size(), 0U);
		ASSERT_EQUAL(fmt.specifiers_buffer().size(), 4U);
		ASSERT_EQUAL(fmt.prefixes_buffer(), ""_view);
		ASSERT_EQUAL(fmt.specifiers_buffer(), "asdf"_view);
	}
	{
		constexpr auto fmt = lak::format_string<"{{{:asdf}{:hello}}}">{};
		ASSERT_EQUAL(fmt.specifiers.size(), 2U);
		ASSERT_EQUAL(fmt.prefixes.size(), fmt.specifiers.size() + 1U);
		ASSERT_EQUAL(fmt.specifiers[0].first, 0U);
		ASSERT_EQUAL(fmt.specifiers[1].first, 1U);
		ASSERT_EQUAL(fmt.prefixes_buffer().size(), 2U);
		ASSERT_EQUAL(fmt.specifiers_buffer().size(), 9U);
		ASSERT_EQUAL(fmt.prefixes_buffer(), "{}"_view);
		ASSERT_EQUAL(fmt.specifiers_buffer(), "asdfhello"_view);
	}
	{
		constexpr auto fmt = lak::format_string<"123{4:hello}asdf">{};
		ASSERT_EQUAL(fmt.specifiers.size(), 1U);
		ASSERT_EQUAL(fmt.prefixes.size(), fmt.specifiers.size() + 1U);
		ASSERT_EQUAL(fmt.specifiers[0].first, 4U);
		ASSERT_EQUAL(fmt.prefixes_buffer().size(), 7U);
		ASSERT_EQUAL(fmt.specifiers_buffer().size(), 5U);
		ASSERT_EQUAL(fmt.prefixes_buffer(), "123asdf"_view);
		ASSERT_EQUAL(fmt.specifiers_buffer(), "hello"_view);
	}
	{
		constexpr auto fmt = lak::format_string<"1{1}2{2}3{3}4">{};
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
		constexpr auto fmt = lak::format_string<"LAK{}{2}{}!">{};
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

	DEBUG_EXPR(lak::fmt<"{0}, {0:+#08.4}, {0:+#08.4d}">(-.01));

	ASSERT_EQUAL(lak::fmt<"{:H}">(char(0x40)), "\\x40"_view);

	ASSERT_EQUAL(lak::fmt<"{:A}">(char(0x40)), "\x40"_view);

	ASSERT_EQUAL(lak::fmt<"{:A}">(char(0xFF)), "\\xFF"_view);

	ASSERT_EQUAL(lak::fmt<"{:C}">(char(0xFF)), ""_str + char(0xFF));

	ASSERT_EQUAL(
	  lak::fmt<u8"{:A}">(u8"hello "_str + char8_t(0x40) + u8" world"_str),
	  u8"hello \x40 world"_view);

	ASSERT_EQUAL(
	  lak::fmt<u8"{:A}">(u8"hello "_str + char8_t(0xFF) + u8" world"_str),
	  u8"hello \\xFF world"_view);

	ASSERT_EQUAL(
	  lak::fmt<u8"{:Ax}">(u8"hello "_str + char8_t(0xFF) + u8" world"_str),
	  u8"hello \\xFF world"_view);

	ASSERT_EQUAL(
	  lak::fmt<u8"{:AU}">(u8"hello "_str + char8_t(0xFF) + u8" world"_str),
	  u8"hello \\UFF world"_view);

	return 0;
}
END_TEST()
