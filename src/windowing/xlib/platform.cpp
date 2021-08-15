#include "lak/platform.hpp"
#include "lak/debug.hpp"

#include "impl.hpp"

bool lak::platform_init()
{
	Display *handle = XOpenDisplay(NULL);
	if (!handle) return false;
	lak::_platform_instance         = new lak::platform_instance();
	lak::_platform_instance->handle = handle;
	lak::_platform_instance->wm_delete_window =
	  XInterAtom(lak::_platform_instance->handle, "WM_DELETE_WINDOW", False);
	// When we construct a window we need to `XSetWMProtocols(display, window,
	// &wm_delete_window, 1);` on it.
	return true;
}

bool lak::platform_quit()
{
	XCloseDisplay(lak::_platform_instance->handle);
	delete lak::_platform_instance;
	return true;
}

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

bool lak::cursor_visible()
{
	return false;
}

void lak::show_cursor() {}

void lak::hide_cursor() {}

void lak::set_cursor_pos(lak::vec2l_t p)
{
	(void)p;
}

lak::vec2l_t lak::get_cursor_pos()
{
	return {};
}

void lak::set_cursor(lak::cursor *c)
{
	(void)c;
}

#include "../common/platform.inl"
