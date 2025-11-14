#include "impl.hpp"

bool lak::platform_init() { return false; }

bool lak::platform_quit() { return false; }

bool lak::get_clipboard(lak::u8string *s)
{
	(void)s;
	return false;
}

bool lak::set_clipboard(lak::u8string_view s)
{
	(void)s;
	return false;
}

bool lak::cursor_visible() { return false; }

void lak::show_cursor() {}

void lak::hide_cursor() {}

void lak::set_cursor_pos(lak::vec2l_t p) { (void)p; }

lak::vec2l_t lak::get_cursor_pos() { return {}; }

void lak::set_cursor(lak::cursor *c) { (void)c; }

#include "../common/platform.inl"
