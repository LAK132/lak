#include "lak/test.hpp"

#include "lak/strconv.hpp"
#include "lak/string_literals/string.hpp"
#include "lak/string_literals/view.hpp"

template<typename CHAR>
lak::string<CHAR> asciiify(lak::string_view<CHAR> str)
{
	lak::string<CHAR> result;
	lak::foreach_char<CHAR>(
		str, [&](CHAR c) { result += lak::fmt<CHAR, "{:A}">(c); });
	return result;
}

BEGIN_TEST(strconv_ascii)
{
	lak::astring str = "Hello, World!";

	{
		auto astr   = lak::to_astring(str);
		auto wstr   = lak::to_wstring(str);
		auto u8str  = lak::to_u8string(str);
		auto u16str = lak::to_u16string(str);
		auto u32str = lak::to_u32string(str);

		ASSERT_EQUAL(astr, "Hello, World!"_view);
		ASSERT_EQUAL(wstr, L"Hello, World!"_view);
		ASSERT_EQUAL(u8str, u8"Hello, World!"_view);
		ASSERT_EQUAL(u16str, u"Hello, World!"_view);
		ASSERT_EQUAL(u32str, U"Hello, World!"_view);
	}

	{
		auto view   = lak::string_view(str);
		auto astr   = lak::to_astring(view);
		auto wstr   = lak::to_wstring(view);
		auto u8str  = lak::to_u8string(view);
		auto u16str = lak::to_u16string(view);
		auto u32str = lak::to_u32string(view);

		ASSERT_EQUAL(astr, "Hello, World!"_view);
		ASSERT_EQUAL(wstr, L"Hello, World!"_view);
		ASSERT_EQUAL(u8str, u8"Hello, World!"_view);
		ASSERT_EQUAL(u16str, u"Hello, World!"_view);
		ASSERT_EQUAL(u32str, U"Hello, World!"_view);
	}

	{
		auto c_str  = str.c_str();
		auto astr   = lak::to_astring(c_str);
		auto wstr   = lak::to_wstring(c_str);
		auto u8str  = lak::to_u8string(c_str);
		auto u16str = lak::to_u16string(c_str);
		auto u32str = lak::to_u32string(c_str);

		ASSERT_EQUAL(astr, "Hello, World!"_view);
		ASSERT_EQUAL(wstr, L"Hello, World!"_view);
		ASSERT_EQUAL(u8str, u8"Hello, World!"_view);
		ASSERT_EQUAL(u16str, u"Hello, World!"_view);
		ASSERT_EQUAL(u32str, U"Hello, World!"_view);
	}

	return 0;
}
END_TEST()

BEGIN_TEST(strconv_unicode)
{
	lak::u32string str = U"Hello, World!\U0001FAEA\0\0\U0001FAEA"_str;

	{
		auto u8str  = lak::to_u8string(str);
		auto u16str = lak::to_u16string(str);
		auto u32str = lak::to_u32string(str);

		DEBUG(asciiify(lak::string_view(u8str)));
		DEBUG(asciiify(lak::string_view(u16str)));
		DEBUG(asciiify(lak::string_view(u32str)));

		ASSERT_EQUAL(u8str,
		             u8"Hello, World!\xF0\x9F\xAB\xAA\0\0\xF0\x9F\xAB\xAA"_view);
		ASSERT_EQUAL(u16str, u"Hello, World!\xD83E\xDEEA\0\0\xD83E\xDEEA"_view);
		ASSERT_EQUAL(u32str, U"Hello, World!\x0001FAEA\0\0\x0001FAEA"_view);
	}

	{
		auto view   = lak::string_view(str);
		auto u8str  = lak::to_u8string(view);
		auto u16str = lak::to_u16string(view);
		auto u32str = lak::to_u32string(view);

		DEBUG(asciiify(lak::string_view(u8str)));
		DEBUG(asciiify(lak::string_view(u16str)));
		DEBUG(asciiify(lak::string_view(u32str)));

		ASSERT_EQUAL(u8str,
		             u8"Hello, World!\xF0\x9F\xAB\xAA\0\0\xF0\x9F\xAB\xAA"_view);
		ASSERT_EQUAL(u16str, u"Hello, World!\xD83E\xDEEA\0\0\xD83E\xDEEA"_view);
		ASSERT_EQUAL(u32str, U"Hello, World!\x0001FAEA\0\0\x0001FAEA"_view);
	}

	{
		auto c_str  = str.c_str();
		auto u8str  = lak::to_u8string(c_str);
		auto u16str = lak::to_u16string(c_str);
		auto u32str = lak::to_u32string(c_str);

		DEBUG(asciiify(lak::string_view(u8str)));
		DEBUG(asciiify(lak::string_view(u16str)));
		DEBUG(asciiify(lak::string_view(u32str)));

		ASSERT_EQUAL(u8str, u8"Hello, World!\xF0\x9F\xAB\xAA"_view);
		ASSERT_EQUAL(u16str, u"Hello, World!\xD83E\xDEEA"_view);
		ASSERT_EQUAL(u32str, U"Hello, World!\x0001FAEA"_view);
	}

	return 0;
}
END_TEST()
