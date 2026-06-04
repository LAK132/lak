/*
Typical usage for an OpenGL program:

int main()
{
  lak::platform_init();
  DEFER(lak::platform_quit());

  {
    lak::window window = lak::window::make(lak::opengl_settings{...}).UNWRAP();

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

    // window closes when it window object is destroyed
  }
}
*/

#ifndef LAK_SYSTEM_WINDOWING_WINDOW_HPP
#define LAK_SYSTEM_WINDOWING_WINDOW_HPP

#include "lak/system/windowing/events.hpp"
#include "lak/system/windowing/platform.hpp"

#include "lak/system/profile.hpp"

#include "lak/bank_ptr.hpp"
#include "lak/format.hpp"
#include "lak/image.hpp"
#include "lak/memmanip.hpp"
#include "lak/result.hpp"
#include "lak/streamify.hpp"
#include "lak/string.hpp"
#include "lak/string_literals/view.hpp"
#include "lak/surface.hpp"
#include "lak/vec.hpp"

#ifdef LAK_ENABLE_COBALT
#	include "lak/system/cobalt/context.hpp"
#	include "lak/system/cobalt/renderers.hpp"

#	include <Logging/Logging.pkg>
#endif

#include <ostream>

namespace lak
{
	enum struct graphics_mode
	{
		None     = 0,
		Software = 1,
		OpenGL   = 2,
		Cobalt   = 3,
	};

	template<typename CHAR>
	struct format_traits<lak::graphics_mode, CHAR>
	{
		static constexpr lak::string<CHAR> to_string(
		  const lak::graphics_mode &mode)
		{
			switch (mode)
			{
				case lak::graphics_mode::OpenGL:
					return lak::strconv<CHAR>("OpenGL"_view);
				case lak::graphics_mode::Software:
					return lak::strconv<CHAR>("Software"_view);
				case lak::graphics_mode::Cobalt:
					return lak::strconv<CHAR>("Cobalt"_view);
				default: return lak::strconv<CHAR>("None"_view);
			}
		}
	};

	struct software_settings
	{
	};

	struct opengl_settings
	{
#ifdef LAK_OS_APPLE
		bool double_buffered = true;
#else
		bool double_buffered = false;
#endif
		uint8_t depth_size   = 24;
		uint8_t colour_size  = 8;
		uint8_t stencil_size = 8;
		int major            = 3;
		int minor            = 2;
	};

	struct cobalt_renderer_settings
	{
#ifdef LAK_ENABLE_COBALT
		::cobalt::graphics::RendererInfo renderer_info = {};
		::cobalt::graphics::IModuleHandle::unique_ptr module_handle;
		::cobalt::graphics::IGraphicsDeviceEnumerator::unique_ptr
		  device_enumerator                         = nullptr;
		::cobalt::graphics::IGraphicsDevice *device = nullptr;
		using feature_set_t =
		  std::set<::cobalt::graphics::IGraphicsDevice::Feature>;
		feature_set_t features                                   = {};
		std::set<::cobalt::graphics::IRenderer::Options> options = {};

		// the single preferred renderer
		static lak::result<cobalt_renderer_settings> preferred(
		  feature_set_t &&required_features = {});

		// preferred settings for each available renderer, in order of preference
		static lak::result<lak::array<cobalt_renderer_settings>> each_preferred(
		  feature_set_t &&required_features = {});

#	ifdef LAK_ENABLE_COBALT_OGL3
		static lak::result<cobalt_renderer_settings> preferred_ogl3(
		  feature_set_t &&required_features = {});
#	endif
#	ifdef LAK_ENABLE_COBALT_OGL4
		static lak::result<cobalt_renderer_settings> preferred_ogl4(
		  feature_set_t &&required_features = {});
#	endif
#	ifdef LAK_ENABLE_COBALT_D3D11
		static lak::result<cobalt_renderer_settings> preferred_d3d11(
		  feature_set_t &&required_features = {});
#	endif
#	ifdef LAK_ENABLE_COBALT_D3D12
		static lak::result<cobalt_renderer_settings> preferred_d3d12(
		  feature_set_t &&required_features = {});
#	endif
#	ifdef LAK_ENABLE_COBALT_VK
		static lak::result<cobalt_renderer_settings> preferred_vk(
		  feature_set_t &&required_features = {});
#	endif

#endif
	};

	struct cobalt_settings
	{
#ifdef LAK_ENABLE_COBALT
		::cobalt::graphics::IFrameBuffer::WindowDepthStencilMode depth_mode =
		  ::cobalt::graphics::IFrameBuffer::WindowDepthStencilMode::
		    DepthUNorm24StencilUInt8;
		::cobalt::graphics::IFrameBuffer::WindowColorSpaceMode colour_mode =
		  ::cobalt::graphics::IFrameBuffer::WindowColorSpaceMode::Default;
#endif
	};

	struct software_context;

	struct opengl_context;

	struct cobalt_context;

	struct window_handle;

	extern template struct lak::uninit_array<lak::window_handle>;
	extern template struct lak::uninit_railcar<lak::window_handle>;
	extern template struct lak::array<lak::window_handle, lak::dynamic_extent>;
	extern template struct lak::bank<lak::window_handle>;
	extern template size_t lak::bank<lak::window_handle>::internal_create<
	  lak::window_handle>(lak::window_handle &&);
	using const_window_handle_ref = const lak::window_handle &;
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

#ifdef LAK_ENABLE_COBALT
	lak::result<lak::window_handle *, lak::u8string> create_window(
	  const lak::cobalt_settings &s, const lak::cobalt_renderer_settings &r);
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
	enum struct window_swap_interval : int
	{
		vsync_off      = 0,
		vsync_on       = 1,
		vsync_adaptive = -1,
	};
	bool set_opengl_swap_interval(const lak::opengl_context &c,
	                              lak::window_swap_interval interval);
#endif

#ifdef LAK_ENABLE_COBALT
	lak::unique_ptr<::cobalt::graphics::IRenderer::WindowSystemInfoBase>
	cobalt_window_system_info(const lak::window_handle *w);

	lak::unique_ptr<::cobalt::graphics::IFrameBuffer::WindowInfoBase>
	cobalt_window_info(const lak::window_handle *w);

	lak::result<const lak::cobalt::graphics_context &> cobalt_graphics_context(
	  const lak::window_handle *w);

	::cobalt::graphics::IRenderPassNode *cobalt_create_render_pass(
	  const lak::cobalt_context &c);
	lak::result<::cobalt::graphics::IRenderPassNode *> cobalt_create_render_pass(
	  const lak::window_handle *w);

	void cobalt_append_render_pass(const lak::cobalt_context &c,
	                               ::cobalt::graphics::IRenderPassNode *pass);
	void cobalt_append_render_pass(
	  const lak::cobalt_context &c,
	  ::cobalt::graphics::IRenderPassNode::unique_ptr &&pass);
	lak::result<lak::monostate> cobalt_append_render_pass(
	  const lak::window_handle *w, ::cobalt::graphics::IRenderPassNode *pass);
	lak::result<lak::monostate> cobalt_append_render_pass(
	  const lak::window_handle *w,
	  ::cobalt::graphics::IRenderPassNode::unique_ptr &&pass);
#endif

	bool set_active_window(const lak::window_handle *w);

	bool swap_window(lak::window_handle *w);

	// Yield this thread until the target framerate is achieved.
	uint64_t yield_frame(const uint64_t last_counter,
	                     const uint32_t target_framerate);

	void window_handle_resize(const lak::window_handle *w);

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

#ifdef LAK_ENABLE_COBALT
		// use preferred renderer settings
		static lak::result<lak::pair<window, lak::cobalt_renderer_settings>,
		                   lak::u8string>
		make(const lak::cobalt_settings &s);

		// use specific renderer settings
		static lak::result<window, lak::u8string> make(
		  const lak::cobalt_settings &s, const lak::cobalt_renderer_settings &r);
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

		inline const window &set_active() const
		{
			ASSERT(lak::set_active_window(handle()));
			return *this;
		}

		inline bool swap() { return lak::swap_window(handle()); }
	};
}

#endif
