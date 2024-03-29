#include "lak/window.hpp"

#include "impl.hpp"

#ifdef LAK_ENABLE_SOFTRENDER
lak::result<lak::window_handle *, lak::u8string> lak::create_window(
  const lak::software_settings &s)
{
	(void)s;
	return lak::ok_t{
	  lak::unique_bank_ptr<lak::window_handle>::create().release()};
}
#endif

#ifdef LAK_ENABLE_OPENGL
lak::result<lak::window_handle *, lak::u8string> lak::create_window(
  const lak::opengl_settings &s)
{
	(void)s;
	return lak::ok_t{
	  lak::unique_bank_ptr<lak::window_handle>::create().release()};
}
#endif

#ifdef LAK_ENABLE_VULKAN
lak::result<lak::window_handle *, lak::u8string> lak::create_window(
  const lak::vulkan_settings &s)
{
	(void)s;
	return lak::ok_t{
	  lak::unique_bank_ptr<lak::window_handle>::create().release()};
}
#endif

#ifdef LAK_ENABLE_METAL
lak::result<lak::window_handle *, lak::u8string> lak::create_window(
  const lak::metal_settings &s)
{
	(void)s;
	return lak::ok_t{
	  lak::unique_bank_ptr<lak::window_handle>::create().release()};
}
#endif

bool lak::destroy_window(lak::window_handle *w)
{
	lak::bank<lak::window_handle>::destroy(w);
	return true;
}

lak::wstring lak::window_title(const lak::window_handle *w)
{
	(void)w;
	return {};
}

bool lak::set_window_title(lak::window_handle *w, const lak::wstring &s)
{
	(void)w;
	(void)s;
	return false;
}

lak::vec2l_t lak::window_size(const lak::window_handle *w)
{
	(void)w;
	return {};
}

lak::vec2l_t lak::window_drawable_size(const lak::window_handle *w)
{
	(void)w;
	return {};
}

bool lak::set_window_size(lak::window_handle *w, lak::vec2l_t s)
{
	(void)w;
	(void)s;
	return false;
}

bool lak::set_window_cursor_pos(const lak::window_handle *w, lak::vec2l_t p)
{
	(void)w;
	(void)p;
	return false;
}

lak::graphics_mode lak::window_graphics_mode(const lak::window_handle *w)
{
	return w->graphics_mode();
}

#ifdef LAK_ENABLE_OPENGL
bool lak::set_opengl_swap_interval(const lak::opengl_context &c, int interval)
{
	(void)c;
	(void)interval;
	return false;
}
#endif

bool lak::swap_window(lak::window_handle *w)
{
	(void)w;
	return false;
}

#include "../common/window.inl"
