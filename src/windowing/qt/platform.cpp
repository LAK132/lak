#include "lak/window.hpp"

bool lak::platform_init(lak::platform_instance *i)
{
  (void)i;
  return false;
}

bool lak::platform_quit(lak::platform_instance *i)
{
  (void)i;
  return false;
}

bool lak::get_clipboard(const lak::platform_instance &i, lak::u8string *s)
{
  (void)i;
  (void)s;
  return false;
}

bool lak::set_clipboard(const lak::platform_instance &i, lak::u8string_view s)
{
  (void)i;
  (void)s;
  return false;
}

bool lak::cursor_visible(const lak::platform_instance &i)
{
  (void)i;
  return false;
}

void lak::show_cursor(const lak::platform_instance &i)
{
  (void)i;
}

void lak::hide_cursor(const lak::platform_instance &i)
{
  (void)i;
}

void lak::set_cursor_pos(const lak::platform_instance &i, lak::vec2l_t p)
{
  (void)i;
  (void)p;
}

lak::vec2l_t lak::get_cursor_pos(const lak::platform_instance &i)
{
  (void)i;
  return {};
}

void lak::set_cursor(const lak::platform_instance &i, const lak::cursor &c)
{
  (void)i;
  (void)c;
}
