#include "lak/platform.hpp"
#include "lak/debug.hpp"

bool lak::platform_init(lak::platform_instance *handle)
{
  SDL_SetMainReady();
  return SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) == 0;
}

bool lak::platform_quit(lak::platform_instance *handle)
{
  SDL_Quit();
  return true;
}

bool lak::get_clipboard(const lak::platform_instance &i, lak::u8string *s)
{
  ASSERT_NYI();
  return false;
}

bool lak::set_clipboard(const lak::platform_instance &i,
                        const lak::u8string &s)
{
  ASSERT_NYI();
  return false;
}

bool lak::set_clipboard(const lak::platform_instance &i,
                        lak::span<const char8_t> s)
{
  ASSERT_NYI();
  return false;
}

bool lak::cursor_visible(const lak::platform_instance &i)
{
  ASSERT_NYI();
  return false;
}

void lak::show_cursor(const lak::platform_instance &i)
{
  ASSERT_NYI();
}

void lak::hide_cursor(const lak::platform_instance &i)
{
  ASSERT_NYI();
}

void lak::set_cursor_pos(const lak::platform_instance &i, lak::vec2l_t p)
{
  ASSERT(p.x < INT_MAX && p.x > INT_MIN);
  ASSERT(p.y < INT_MAX && p.y > INT_MIN);
  // :TODO: clamp instead of cast
  ASSERT_EQUAL(SDL_WarpMouseGlobal((int)p.x, (int)p.y), 0);
}

lak::vec2l_t lak::get_cursor_pos(const lak::platform_instance &i)
{
  ASSERT_NYI();
  return {};
}

void lak::set_cursor(const lak::platform_instance &i, const lak::cursor &c)
{
  ASSERT_NYI();
}