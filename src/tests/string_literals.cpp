#include "lak/string_literals.hpp"
#include "lak/test.hpp"

BEGIN_TEST(string_literals)
{
	[[maybe_unused]] constexpr lak::astring_view astr{"Hello, World!"_view};
	[[maybe_unused]] constexpr lak::wstring_view wstr{L"Hello, World!"_view};
	[[maybe_unused]] constexpr lak::u8string_view u8str{u8"Hello, World!"_view};
	[[maybe_unused]] constexpr lak::u16string_view u16str{u"Hello, World!"_view};
	[[maybe_unused]] constexpr lak::u32string_view u32str{U"Hello, World!"_view};

	[[maybe_unused]] constexpr lak::span<const char> aspan{"Hello, World!"_span};
	[[maybe_unused]] constexpr lak::span<const wchar_t> wspan{
	  L"Hello, World!"_span};
	[[maybe_unused]] constexpr lak::span<const char8_t> u8span{
	  u8"Hello, World!"_span};
	[[maybe_unused]] constexpr lak::span<const char16_t> u16span{
	  u"Hello, World!"_span};
	[[maybe_unused]] constexpr lak::span<const char32_t> u32span{
	  U"Hello, World!"_span};

	return 0;
}
END_TEST()
