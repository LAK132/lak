#include "lak/test.hpp"

#include "lak/string_literals/string.hpp"
#include "lak/string_literals/view.hpp"
#include "lak/unicode.hpp"

BEGIN_TEST(converted_string_length)
{
	{
		auto str = U"abcd"_view;

		ASSERT_EQUAL(lak::converted_string_length<char8_t>(str), 4U);
		ASSERT_EQUAL(lak::converted_string_length<char16_t>(str), 4U);
		ASSERT_EQUAL(lak::converted_string_length<char32_t>(str), 4U);
	}

	{
		auto str = U"ab\U0001FAEAcd"_view;

		ASSERT_EQUAL(lak::converted_string_length<char8_t>(str), 8U);
		ASSERT_EQUAL(lak::converted_string_length<char16_t>(str), 6U);
		ASSERT_EQUAL(lak::converted_string_length<char32_t>(str), 5U);
	}

	return 0;
}
END_TEST()

BEGIN_TEST(character_length)
{
	{
		ASSERT_EQUAL(lak::character_length(u8"abcd"_view), 1U);
		ASSERT_EQUAL(lak::character_length(u"abcd"_view), 1U);
		ASSERT_EQUAL(lak::character_length(U"abcd"_view), 1U);
	}

	{
		ASSERT_EQUAL(lak::character_length(u8"\U0001FAEA"_view), 4U);
		ASSERT_EQUAL(lak::character_length(u"\U0001FAEA"_view), 2U);
		ASSERT_EQUAL(lak::character_length(U"\U0001FAEA"_view), 1U);
	}

	return 0;
}
END_TEST()

BEGIN_TEST(codepoint)
{
	ASSERT_EQUAL(lak::codepoint(L"\U0001FAEA"_view), char32_t(0x1FAEA));
	ASSERT_EQUAL(lak::codepoint(u8"\U0001FAEA"_view), char32_t(0x1FAEA));
	ASSERT_EQUAL(lak::codepoint(u"\U0001FAEA"_view), char32_t(0x1FAEA));
	ASSERT_EQUAL(lak::codepoint(U"\U0001FAEA"_view), char32_t(0x1FAEA));

	return 0;
}
END_TEST()

BEGIN_TEST(codepoint_length)
{
	ASSERT_EQUAL(lak::codepoint_length<char8_t>(char32_t(0x1FAEA)), 4U);
	ASSERT_EQUAL(lak::codepoint_length<char16_t>(char32_t(0x1FAEA)), 2U);
	ASSERT_EQUAL(lak::codepoint_length<char32_t>(char32_t(0x1FAEA)), 1U);

	return 0;
}
END_TEST()

BEGIN_TEST(from_codepoint)
{
	{
		lak::codepoint_buffer_t<char8_t> buffer;
		auto span = lak::from_codepoint(buffer, char32_t(0x1FAEA));
		ASSERT_EQUAL(lak::string_view(span), u8"\xF0\x9F\xAB\xAA"_view);
	}
	{
		lak::codepoint_buffer_t<char16_t> buffer;
		auto span = lak::from_codepoint(buffer, char32_t(0x1FAEA));
		ASSERT_EQUAL(lak::string_view(span), u"\xD83E\xDEEA"_view);
	}
	{
		lak::codepoint_buffer_t<char32_t> buffer;
		auto span = lak::from_codepoint(buffer, char32_t(0x1FAEA));
		ASSERT_EQUAL(lak::string_view(span), U"\x0001FAEA"_view);
	}

	return 0;
}
END_TEST()

// BEGIN_TEST(append_codepoint)
// {
// 	//
// 	return 0;
// }
// END_TEST()

// BEGIN_TEST(codepoint_iterator)
// {
// 	//
// 	return 0;
// }
// END_TEST()

// BEGIN_TEST(codepoint_range)
// {
// 	//
// 	return 0;
// }
// END_TEST()

// BEGIN_TEST(foreach_char)
// {
// 	//
// 	return 0;
// }
// END_TEST()
