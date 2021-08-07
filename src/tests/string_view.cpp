#include "lak/test.hpp"

#include "lak/string_view.hpp"

BEGIN_TEST(string_view)
{
	[[maybe_unused]] lak::astring_view astr     = "Hello, World!"_view;
	[[maybe_unused]] lak::wstring_view wstr     = L"Hello, World!"_view;
	[[maybe_unused]] lak::u8string_view u8str   = u8"Hello, World!"_view;
	[[maybe_unused]] lak::u16string_view u16str = u"Hello, World!"_view;
	[[maybe_unused]] lak::u32string_view u32str = U"Hello, World!"_view;

	return 0;
}
END_TEST();
