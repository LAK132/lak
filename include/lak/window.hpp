/*
Typical usage for an OpenGL program:

int main()
{
  lak::core_init();

  lak::window window(...);

  window.init_opengl(...);

  uint32_t framerate = 60;

  auto last_counter = lak::performance_counter();

  // main loop
  while(...)
  {
    // event handlers

    // update code

    // draw code

    window.swap();

    last_counter = lak::yield_frame(last_counter, framerate);
  }

  window.close();

  lak::core_quit();
}
*/

#ifndef LAK_WINDOW_HPP
#define LAK_WINDOW_HPP

#include "lak/platform.hpp"

#include "lak/events.hpp"

#include "lak/bank_ptr.hpp"
#include "lak/image.hpp"
#include "lak/memmanip.hpp"
#include "lak/result.hpp"
#include "lak/streamify.hpp"
#include "lak/string.hpp"
#include "lak/surface.hpp"
#include "lak/vec.hpp"

#include "lak/profile.hpp"

#include <ostream>

namespace lak
{
	enum struct graphics_mode
	{
		None     = 0,
		Software = 1,
		OpenGL   = 2,
		Vulkan   = 3,
		Metal    = 4,
	};

	inline std::ostream &operator<<(std::ostream &strm, lak::graphics_mode mode)
	{
		switch (mode)
		{
			case lak::graphics_mode::OpenGL:
				strm << "OpenGL";
				break;
			case lak::graphics_mode::Software:
				strm << "Software";
				break;
			case lak::graphics_mode::Vulkan:
				strm << "Vulkan";
				break;
			case lak::graphics_mode::Metal:
				strm << "Metal";
				break;
			default:
				strm << "None";
				break;
		}
		return strm;
	}

	static_assert(lak::is_streamable_v<lak::graphics_mode>);

	struct software_settings
	{
	};

	struct opengl_settings
	{
		bool double_buffered = false;
		uint8_t depth_size   = 24;
		uint8_t colour_size  = 8;
		uint8_t stencil_size = 8;
		int major            = 3;
		int minor            = 2;
	};

	struct vulkan_settings
	{
	};

	struct metal_settings
	{
	};

	struct software_context;

	struct opengl_context;

	struct vulkan_context;

	struct metal_context;

	struct window_handle;

	extern template struct lak::array<lak::window_handle, lak::dynamic_extent>;
	extern template struct lak::railcar<lak::window_handle>;
	extern template struct lak::bank<lak::window_handle>;
	extern template size_t lak::bank<lak::window_handle>::internal_create<
	  lak::window_handle>(lak::window_handle &&);
	using const_window_handle_ref = const lak::window_handle &;
	extern template size_t lak::bank<lak::window_handle>::internal_create<
	  lak::const_window_handle_ref>(lak::const_window_handle_ref &&);
	extern template struct lak::unique_bank_ptr<lak::window_handle>;
	extern template struct lak::shared_bank_ptr<lak::window_handle>;
	using window_handle_bank = lak::bank<lak::window_handle>;

	/* --- create/destroy window --- */

#ifdef LAK_ENABLE_SOFTRENDER
	lak::result<lak::window_handle *, lak::u8string> create_window(
	  const lak::software_settings &s);
#endif

#ifdef LAK_ENABLE_OPENGL
	lak::result<lak::window_handle *, lak::u8string> create_window(
	  const lak::opengl_settings &s);
#endif

#ifdef LAK_ENABLE_VULKAN
	lak::result<lak::window_handle *, lak::u8string> create_window(
	  const lak::vulkan_settings &s);
#endif

#ifdef LAK_ENABLE_METAL
	lak::result<lak::window_handle *, lak::u8string> create_window(
	  const lak::metal_settings &s);
#endif

	bool destroy_window(lak::window_handle *w);

	/* --- window state --- */

	lak::wstring window_title(const lak::window_handle *w);

	bool set_window_title(lak::window_handle *w, const lak::wstring &s);

	lak::vec2l_t window_size(const lak::window_handle *w);

	lak::vec2l_t window_drawable_size(const lak::window_handle *w);

	bool set_window_size(lak::window_handle *w, lak::vec2l_t s);

	bool set_window_cursor_pos(const lak::window_handle *w, lak::vec2l_t p);

	// :TODO:
	// bool set_window_drawable_size(lak::window_handle *w, lak::vec2l_t s);

	/* --- graphics control --- */

	lak::graphics_mode window_graphics_mode(const lak::window_handle *w);

#ifdef LAK_ENABLE_OPENGL
	// :TODO: This probably belongs in the platform header.
	bool set_opengl_swap_interval(const lak::opengl_context &c, int interval);
#endif

	bool swap_window(lak::window_handle *w);

	// Yield this thread until the target framerate is achieved.
	uint64_t yield_frame(const uint64_t last_counter,
	                     const uint32_t target_framerate);

	/* --- window wrapper class --- */

	struct window
	{
	private:
		lak::unique_bank_ptr<lak::window_handle> _handle;

		window(lak::unique_bank_ptr<lak::window_handle> &&handle);

	public:
		inline window(window &&w) : _handle(lak::move(w._handle)) {}

#ifdef LAK_ENABLE_SOFTRENDER
		static lak::result<window, lak::u8string> make(
		  const lak::software_settings &s);
#endif

#ifdef LAK_ENABLE_OPENGL
		static lak::result<window, lak::u8string> make(
		  const lak::opengl_settings &s);
#endif

#ifdef LAK_ENABLE_VULKAN
		static lak::result<window, lak::u8string> make(
		  const lak::vulkan_settings &s);
#endif

#ifdef LAK_ENABLE_METAL
		static lak::result<window, lak::u8string> make(
		  const lak::metal_settings &s);
#endif

		~window();

		inline lak::window_handle *handle() { return _handle.get(); }

		inline const lak::window_handle *handle() const { return _handle.get(); }

		inline lak::graphics_mode graphics() const
		{
			return lak::window_graphics_mode(handle());
		}

		inline lak::wstring title() const { return lak::window_title(handle()); }

		inline window &set_title(const lak::wstring &title)
		{
			ASSERT(lak::set_window_title(handle(), title));
			return *this;
		}

		inline lak::vec2l_t size() const { return lak::window_size(handle()); }

		inline lak::vec2l_t drawable_size() const
		{
			return lak::window_drawable_size(handle());
		}

		inline window &set_size(lak::vec2l_t size)
		{
			ASSERT(lak::set_window_size(handle(), size));
			return *this;
		}

		inline const window &set_cursor_pos(lak::vec2l_t pos) const
		{
			ASSERT(lak::set_window_cursor_pos(handle(), pos));
			return *this;
		}

		inline bool swap() { return lak::swap_window(handle()); }
	};
}

#endif
