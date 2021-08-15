#include "lak/debug.hpp"

#include "impl.hpp"

bool lak::platform_init()
{
	SDL_SetMainReady();
	return SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) == 0;
}

bool lak::platform_quit()
{
	SDL_Quit();
	return true;
}

bool lak::get_clipboard(lak::u8string *s)
{
	LAK_UNUSED(s);
	ASSERT_NYI();
}

bool lak::set_clipboard(lak::u8string_view s)
{
	LAK_UNUSED(s);
	ASSERT_NYI();
}

bool lak::cursor_visible()
{
	ASSERT_NYI();
}

void lak::show_cursor()
{
	ASSERT_NYI();
}

void lak::hide_cursor()
{
	ASSERT_NYI();
}

void lak::set_cursor_pos(lak::vec2l_t p)
{
	ASSERT(p.x < INT_MAX && p.x > INT_MIN);
	ASSERT(p.y < INT_MAX && p.y > INT_MIN);
	// :TODO: clamp instead of cast
	ASSERT_EQUAL(SDL_WarpMouseGlobal((int)p.x, (int)p.y), 0);
}

lak::vec2l_t lak::get_cursor_pos()
{
	ASSERT_NYI();
}

void lak::set_cursor(lak::cursor *c)
{
	LAK_UNUSED(c);
	ASSERT_NYI();
}

#include "../common/platform.inl"
