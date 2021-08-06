#include "lak/platform.hpp"
#include "lak/debug.hpp"

bool lak::platform_init(lak::platform_instance *)
{
  SDL_SetMainReady();
  return SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) == 0;
}

bool lak::platform_quit(lak::platform_instance *)
{
  SDL_Quit();
  return true;
}

bool lak::get_clipboard(const lak::platform_instance &i, lak::u8string *s)
{
  LAK_UNUSED(i);
  LAK_UNUSED(s);
  ASSERT_NYI();
}

bool lak::set_clipboard(const lak::platform_instance &i, lak::u8string_view s)
{
  LAK_UNUSED(i);
  LAK_UNUSED(s);
  ASSERT_NYI();
}

bool lak::cursor_visible(const lak::platform_instance &i)
{
  LAK_UNUSED(i);
  ASSERT_NYI();
}

void lak::show_cursor(const lak::platform_instance &i)
{
  LAK_UNUSED(i);
  ASSERT_NYI();
}

void lak::hide_cursor(const lak::platform_instance &i)
{
  LAK_UNUSED(i);
  ASSERT_NYI();
}

void lak::set_cursor_pos(const lak::platform_instance &i, lak::vec2l_t p)
{
  LAK_UNUSED(i);
  ASSERT(p.x < INT_MAX && p.x > INT_MIN);
  ASSERT(p.y < INT_MAX && p.y > INT_MIN);
  // :TODO: clamp instead of cast
  ASSERT_EQUAL(SDL_WarpMouseGlobal((int)p.x, (int)p.y), 0);
}

lak::vec2l_t lak::get_cursor_pos(const lak::platform_instance &i)
{
  LAK_UNUSED(i);
  ASSERT_NYI();
}

void lak::set_cursor(const lak::platform_instance &i, const lak::cursor &c)
{
  LAK_UNUSED(i);
  LAK_UNUSED(c);
  ASSERT_NYI();
}