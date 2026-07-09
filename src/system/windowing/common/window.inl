#include "lak/system/windowing/window.hpp"

#include "lak/string_literals/string.hpp"

#ifdef LAK_ENABLE_COBALT
#	include "lak/system/cobalt/result.hpp"
#	include "lak/system/cobalt/log_target.hpp"
#endif

#include <thread>

template struct lak::uninit_array<lak::window_handle>;
template struct lak::uninit_railcar<lak::window_handle>;
template struct lak::array<lak::window_handle, lak::dynamic_extent>;
template struct lak::bank<lak::window_handle>;
template size_t lak::bank<lak::window_handle>::internal_create<
  lak::window_handle>(lak::window_handle &&);
template struct lak::unique_bank_ptr<lak::window_handle>;
template struct lak::shared_bank_ptr<lak::window_handle>;

#ifdef LAK_ENABLE_COBALT
lak::result<lak::cobalt_renderer_settings>
lak::cobalt_renderer_settings::preferred(feature_set_t &&required_features)
{
#	ifdef LAK_ENABLE_COBALT_D3D12
	if_let_ok (auto settings,
	           lak::cobalt_renderer_settings::preferred_d3d12(
	             lak::move(required_features)))
		return lak::move_ok(settings);
#	endif

#	ifdef LAK_ENABLE_COBALT_D3D11
	if_let_ok (auto settings,
	           lak::cobalt_renderer_settings::preferred_d3d11(
	             lak::move(required_features)))
		return lak::move_ok(settings);
#	endif

#	ifdef LAK_ENABLE_COBALT_VK
	if_let_ok (auto settings,
	           lak::cobalt_renderer_settings::preferred_vk(
	             lak::move(required_features)))
		return lak::move_ok(settings);
#	endif

#	ifdef LAK_ENABLE_COBALT_OGL4
	if_let_ok (auto settings,
	           lak::cobalt_renderer_settings::preferred_ogl4(
	             lak::move(required_features)))
		return lak::move_ok(settings);
#	endif

#	ifdef LAK_ENABLE_COBALT_OGL3
	if_let_ok (auto settings,
	           lak::cobalt_renderer_settings::preferred_ogl3(
	             lak::move(required_features)))
		return lak::move_ok(settings);
#	endif

	return lak::err_t{};
}

lak::result<lak::array<lak::cobalt_renderer_settings>> lak::
  cobalt_renderer_settings::each_preferred(feature_set_t &&required_features)
{
	lak::array<lak::cobalt_renderer_settings> result;

#	ifdef LAK_ENABLE_COBALT_D3D12
	if_let_ok (auto settings,
	           lak::cobalt_renderer_settings::preferred_d3d12(
	             lak::move(required_features)))
		result.push_back(lak::move(settings));
#	endif

#	ifdef LAK_ENABLE_COBALT_D3D11
	if_let_ok (auto settings,
	           lak::cobalt_renderer_settings::preferred_d3d11(
	             lak::move(required_features)))
		result.push_back(lak::move(settings));
#	endif

#	ifdef LAK_ENABLE_COBALT_VK
	if_let_ok (auto settings,
	           lak::cobalt_renderer_settings::preferred_vk(
	             lak::move(required_features)))
		result.push_back(lak::move(settings));
#	endif

#	ifdef LAK_ENABLE_COBALT_OGL4
	if_let_ok (auto settings,
	           lak::cobalt_renderer_settings::preferred_ogl4(
	             lak::move(required_features)))
		result.push_back(lak::move(settings));
#	endif

#	ifdef LAK_ENABLE_COBALT_OGL3
	if_let_ok (auto settings,
	           lak::cobalt_renderer_settings::preferred_ogl3(
	             lak::move(required_features)))
		result.push_back(lak::move(settings));
#	endif

	if (result.empty()) return lak::err_t{};

	return lak::move_ok(result);
}

namespace
{
	namespace local
	{
		lak::result<lak::cobalt_renderer_settings> cobalt_try_renderer(
		  lak::cobalt::renderer_plugin_func info,
		  ::cobalt::logging::ILogger::unique_ptr log,
		  lak::cobalt_renderer_settings::feature_set_t &&required_features)
		{
			lak::cobalt_renderer_settings result;

			info(0, result.renderer_plugin);

			result.device_enumerator =
			  result.renderer_plugin.CreateGraphicsDeviceEnumerator(
			    log->CloneLogger());
			RES_TRY(
			  lak::cobalt::as_result(result.device_enumerator->EnumerateDevices()));

			result.device = result.device_enumerator->GetPreferredDevice();
			if (!result.device)
			{
				log->Error("Failed to get preferred device");
				return lak::err_t{};
			}
			if (!result.device->AreAllFeaturesSupported(required_features))
			{
				log->Error("Not all features are supported");
				return lak::err_t{};
			}

			result.features = lak::move(required_features);

			return lak::move_ok(result);
		}
	}
}

#	ifdef LAK_ENABLE_COBALT_OGL3
lak::result<lak::cobalt_renderer_settings> lak::cobalt_renderer_settings::
  preferred_ogl3(feature_set_t &&required_features)
{
	return local::cobalt_try_renderer(
	  lak::cobalt::ogl3_get_renderer_plugin(),
	  lak::cobalt::log_manager->GetLogger("OpenGL3"),
	  lak::move(required_features));
}
#	endif

#	ifdef LAK_ENABLE_COBALT_OGL4
lak::result<lak::cobalt_renderer_settings> lak::cobalt_renderer_settings::
  preferred_ogl4(feature_set_t &&required_features)
{
	return local::cobalt_try_renderer(
	  lak::cobalt::ogl4_get_renderer_plugin(),
	  lak::cobalt::log_manager->GetLogger("OpenGL4"),
	  lak::move(required_features));
}
#	endif

#	ifdef LAK_ENABLE_COBALT_D3D11
lak::result<lak::cobalt_renderer_settings> lak::cobalt_renderer_settings::
  preferred_d3d11(feature_set_t &&required_features)
{
	return local::cobalt_try_renderer(
	  lak::cobalt::d3d11_get_renderer_plugin(),
	  lak::cobalt::log_manager->GetLogger("D3D11"),
	  lak::move(required_features));
}
#	endif

#	ifdef LAK_ENABLE_COBALT_D3D12
lak::result<lak::cobalt_renderer_settings> lak::cobalt_renderer_settings::
  preferred_d3d12(feature_set_t &&required_features)
{
	return local::cobalt_try_renderer(
	  lak::cobalt::d3d12_get_renderer_plugin(),
	  lak::cobalt::log_manager->GetLogger("D3D12"),
	  lak::move(required_features));
}
#	endif

#	ifdef LAK_ENABLE_COBALT_VK
lak::result<lak::cobalt_renderer_settings>
lak::cobalt_renderer_settings::preferred_vk(feature_set_t &&required_features)
{
	return local::cobalt_try_renderer(
	  lak::cobalt::vk_get_renderer_plugin(),
	  lak::cobalt::log_manager->GetLogger("Vulkan"),
	  lak::move(required_features));
}
#	endif

lak::result<const lak::cobalt::graphics_context &>
lak::cobalt_graphics_context(const lak::window_handle *w)
{
	RES_TRY_ASSIGN(auto &ctx =,
	               lak::result_from_pointer(w->gc.get<lak::cobalt_context>()));
	return lak::ok_t<const lak::cobalt::graphics_context &>{
	  *ctx.platform_handle};
}

::cobalt::graphics::IRenderPassNode *lak::cobalt_create_render_pass(
  const lak::cobalt_context &c)
{
	c.platform_handle->owned_render_passes.reserve(
	  c.platform_handle->owned_render_passes.size() + 1U);
	c.platform_handle->render_passes.reserve(
	  c.platform_handle->render_passes.size() + 1U);

	auto p = c.platform_handle->renderer->CreateRenderPassNode();
	p->BindFrameBuffer(c.platform_handle->frame_buffer.get());

	auto res =
	  c.platform_handle->owned_render_passes.push_back(lak::move(p)).get();
	c.platform_handle->render_passes.push_back(res);

	return res;
}

lak::result<::cobalt::graphics::IRenderPassNode *>
lak::cobalt_create_render_pass(const lak::window_handle *w)
{
	RES_TRY_ASSIGN(auto &ctx =,
	               lak::result_from_pointer(w->gc.get<lak::cobalt_context>()));
	return lak::ok_t{lak::cobalt_create_render_pass(ctx)};
}

void lak::cobalt_append_render_pass(const lak::cobalt_context &c,
                                    ::cobalt::graphics::IRenderPassNode *pass)
{
	c.platform_handle->render_passes.push_back(pass);
}

void lak::cobalt_append_render_pass(
  const lak::cobalt_context &c,
  ::cobalt::graphics::IRenderPassNode::unique_ptr &&pass)
{
	c.platform_handle->owned_render_passes.reserve(
	  c.platform_handle->owned_render_passes.size() + 1U);
	c.platform_handle->render_passes.reserve(
	  c.platform_handle->render_passes.size() + 1U);

	auto p =
	  c.platform_handle->owned_render_passes.push_back(lak::move(pass)).get();
	c.platform_handle->render_passes.push_back(p);
}

lak::result<lak::monostate> lak::cobalt_append_render_pass(
  const lak::window_handle *w, ::cobalt::graphics::IRenderPassNode *pass)
{
	RES_TRY_ASSIGN(auto &ctx =,
	               lak::result_from_pointer(w->gc.get<lak::cobalt_context>()));
	lak::cobalt_append_render_pass(ctx, pass);
	return lak::ok_t{};
}

lak::result<lak::monostate> lak::cobalt_append_render_pass(
  const lak::window_handle *w,
  ::cobalt::graphics::IRenderPassNode::unique_ptr &&pass)
{
	RES_TRY_ASSIGN(auto &ctx =,
	               lak::result_from_pointer(w->gc.get<lak::cobalt_context>()));
	lak::cobalt_append_render_pass(ctx, lak::move(pass));
	return lak::ok_t{};
}

#	ifdef LAK_OS_APPLE
#		define LAK_COMPUTE_PASSES compute_passes
#	else
#		define LAK_COMPUTE_PASSES render_passes
#	endif

::cobalt::graphics::IRenderPassNode *lak::cobalt_create_compute_pass(
  const lak::cobalt_context &c)
{
	c.platform_handle->owned_render_passes.reserve(
	  c.platform_handle->owned_render_passes.size() + 1U);
	c.platform_handle->LAK_COMPUTE_PASSES.reserve(
	  c.platform_handle->LAK_COMPUTE_PASSES.size() + 1U);

	auto p = c.platform_handle->renderer->CreateRenderPassNode();
	p->BindFrameBuffer(c.platform_handle->frame_buffer.get());

	auto res =
	  c.platform_handle->owned_render_passes.push_back(lak::move(p)).get();
	c.platform_handle->LAK_COMPUTE_PASSES.push_back(res);

	return res;
}

lak::result<::cobalt::graphics::IRenderPassNode *>
lak::cobalt_create_compute_pass(const lak::window_handle *w)
{
	RES_TRY_ASSIGN(auto &ctx =,
	               lak::result_from_pointer(w->gc.get<lak::cobalt_context>()));
	return lak::ok_t{lak::cobalt_create_render_pass(ctx)};
}

void lak::cobalt_append_compute_pass(const lak::cobalt_context &c,
                                    ::cobalt::graphics::IRenderPassNode *pass)
{
	c.platform_handle->LAK_COMPUTE_PASSES.push_back(pass);
}

void lak::cobalt_append_compute_pass(
  const lak::cobalt_context &c,
  ::cobalt::graphics::IRenderPassNode::unique_ptr &&pass)
{
	c.platform_handle->owned_render_passes.reserve(
	  c.platform_handle->owned_render_passes.size() + 1U);
	c.platform_handle->LAK_COMPUTE_PASSES.reserve(
	  c.platform_handle->LAK_COMPUTE_PASSES.size() + 1U);

	auto p =
	  c.platform_handle->owned_render_passes.push_back(lak::move(pass)).get();
	c.platform_handle->LAK_COMPUTE_PASSES.push_back(p);
}

#	undef LAK_COMPUTE_PASSES

lak::result<lak::monostate> lak::cobalt_append_compute_pass(
  const lak::window_handle *w, ::cobalt::graphics::IRenderPassNode *pass)
{
	RES_TRY_ASSIGN(auto &ctx =,
	               lak::result_from_pointer(w->gc.get<lak::cobalt_context>()));
	lak::cobalt_append_compute_pass(ctx, pass);
	return lak::ok_t{};
}

lak::result<lak::monostate> lak::cobalt_append_compute_pass(
  const lak::window_handle *w,
  ::cobalt::graphics::IRenderPassNode::unique_ptr &&pass)
{
	RES_TRY_ASSIGN(auto &ctx =,
	               lak::result_from_pointer(w->gc.get<lak::cobalt_context>()));
	lak::cobalt_append_compute_pass(ctx, lak::move(pass));
	return lak::ok_t{};
}

#endif

uint64_t lak::yield_frame(const uint64_t last_counter,
                          const uint32_t target_framerate)
{
	const uint64_t target_count =
	  (lak::performance_frequency() / target_framerate) + last_counter;

	for (uint64_t count = lak::performance_counter();
	     target_count > last_counter
	       ? (count < target_count)
	       : (count < target_count || count > last_counter);
	     count = lak::performance_counter())
		std::this_thread::yield();

	return lak::performance_counter();
}

lak::window::window(lak::unique_bank_ptr<lak::window_handle> &&handle)
: _handle(lak::move(handle))
{
	ASSERT(_handle);
}

#ifdef LAK_ENABLE_SOFTRENDER
lak::result<lak::window, lak::u8string> lak::window::make(
  const lak::software_settings &s)
{
	if (auto maybe_handle{lak::create_window(s)}; maybe_handle.is_err())
		return lak::err_t<lak::u8string>{
		  lak::move(maybe_handle.unsafe_unwrap_err())};
	else if (auto handle{
	           lak::unique_bank_ptr<lak::window_handle>::from_raw_bank_ptr(
	             maybe_handle.unsafe_unwrap())};
	         !handle)
		return lak::err_t<lak::u8string>{u8"Failed to create bank ptr"_str};
	else
		return lak::ok_t{lak::window(lak::move(handle))};
}
#endif

#ifdef LAK_ENABLE_OPENGL
lak::result<lak::window, lak::u8string> lak::window::make(
  const lak::opengl_settings &s)
{
	if (auto maybe_handle{lak::create_window(s)}; maybe_handle.is_err())
		return lak::err_t<lak::u8string>{
		  lak::move(maybe_handle.unsafe_unwrap_err())};
	else if (auto handle{
	           lak::unique_bank_ptr<lak::window_handle>::from_raw_bank_ptr(
	             maybe_handle.unsafe_unwrap())};
	         !handle)
		return lak::err_t<lak::u8string>{u8"Failed to create bank ptr"_str};
	else
		return lak::ok_t{lak::window(lak::move(handle))};
}
#endif

#ifdef LAK_ENABLE_COBALT
lak::result<lak::pair<lak::window, lak::cobalt_renderer_settings>,
            lak::u8string> lak::window::make(const lak::cobalt_settings &s)
{
	lak::u8string errs;

	RES_TRY_ASSIGN(
	  auto each_rsettings =,
	  lak::cobalt_renderer_settings::each_preferred().map_err(
	    [](auto &&)
	    { return u8"Failed to get preferred renderer settings"_str; }));

	for (auto &rsettings : each_rsettings)
	{
		auto add_settings = [&](lak::window &&wnd)
		  -> lak::pair<lak::window, lak::cobalt_renderer_settings>
		{ return {lak::move(wnd), lak::move(rsettings)}; };

		RES_TRY_ASSIGN_ERR(errs +=,
		                   lak::window::make(s, rsettings).map(add_settings));
		errs += u8"\n";
	}

	return lak::move_err(errs);
}

lak::result<lak::window, lak::u8string> lak::window::make(
  const lak::cobalt_settings &s, const lak::cobalt_renderer_settings &r)
{
	if (auto maybe_handle{lak::create_window(s, r)}; maybe_handle.is_err())
		return lak::err_t<lak::u8string>{
		  lak::move(maybe_handle.unsafe_unwrap_err())};
	else if (auto handle{
	           lak::unique_bank_ptr<lak::window_handle>::from_raw_bank_ptr(
	             maybe_handle.unsafe_unwrap())};
	         !handle)
		return lak::err_t<lak::u8string>{u8"Failed to create bank ptr"_str};
	else
		return lak::ok_t{lak::window(lak::move(handle))};
}
#endif

lak::window::~window()
{
	if (handle()) lak::destroy_window(_handle.release());
}
