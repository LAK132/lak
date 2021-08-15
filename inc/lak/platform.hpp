#ifndef LAK_PLATFORM_HPP
#define LAK_PLATFORM_HPP

#include "lak/buffer.hpp"
#include "lak/compiler.hpp"
#include "lak/os.hpp"
#include "lak/span.hpp"
#include "lak/string.hpp"
#include "lak/vec.hpp"

namespace lak
{
	struct cursor;

	bool platform_init();
	bool platform_quit();

	bool get_clipboard(lak::u8string *s);
	bool set_clipboard(lak::u8string_view s);
	inline bool set_clipboard(const char s[])
	{
		return lak::set_clipboard(
		  lak::as_u8string(lak::astring_view::from_c_str(s)));
	}

	bool cursor_visible();
	void show_cursor();
	void hide_cursor();
	void set_cursor_pos(lak::vec2l_t p);
	lak::vec2l_t get_cursor_pos();
	void set_cursor(lak::cursor *c);
}

#endif
