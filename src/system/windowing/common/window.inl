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

namespace
{
	namespace local
	{
		lak::result<lak::cobalt_renderer_settings> cobalt_try_renderer(
		  lak::cobalt::renderer_info_func info,
		  ::cobalt::logging::ILogger::unique_ptr log,
		  lak::cobalt_renderer_settings::feature_set_t &&required_features)
		{
			lak::cobalt_renderer_settings result;

			info(0, result.renderer_info);

			result.device_enumerator =
			  result.renderer_info.CreateGraphicsDeviceEnumerator(lak::move(log));
			RES_TRY(
			  lak::cobalt::as_result(result.device_enumerator->EnumerateDevices()));

			result.device = result.device_enumerator->GetPreferredDevice();

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
	  lak::cobalt::ogl3_get_renderer_info(),
	  lak::cobalt::log_manager.GetLogger("OpenGL3"),
	  lak::move(required_features));
}
#	endif

#	ifdef LAK_ENABLE_COBALT_OGL4
lak::result<lak::cobalt_renderer_settings> lak::cobalt_renderer_settings::
  preferred_ogl4(feature_set_t &&required_features)
{
	return local::cobalt_try_renderer(
	  lak::cobalt::ogl4_get_renderer_info(),
	  lak::cobalt::log_manager.GetLogger("OpenGL4"),
	  lak::move(required_features));
}
#	endif

#	ifdef LAK_ENABLE_COBALT_D3D11
lak::result<lak::cobalt_renderer_settings> lak::cobalt_renderer_settings::
  preferred_d3d11(feature_set_t &&required_features)
{
	return local::cobalt_try_renderer(
	  lak::cobalt::d3d11_get_renderer_info(),
	  lak::cobalt::log_manager.GetLogger("D3D11"),
	  lak::move(required_features));
}
#	endif

#	ifdef LAK_ENABLE_COBALT_D3D12
lak::result<lak::cobalt_renderer_settings> lak::cobalt_renderer_settings::
  preferred_d3d12(feature_set_t &&required_features)
{
	return local::cobalt_try_renderer(
	  lak::cobalt::d3d12_get_renderer_info(),
	  lak::cobalt::log_manager.GetLogger("D3D12"),
	  lak::move(required_features));
}
#	endif

#	ifdef LAK_ENABLE_COBALT_VK
lak::result<lak::cobalt_renderer_settings>
lak::cobalt_renderer_settings::preferred_vk(feature_set_t &&required_features)
{
	return local::cobalt_try_renderer(
	  lak::cobalt::vk_get_renderer_info(),
	  lak::cobalt::log_manager.GetLogger("Vulkan"),
	  lak::move(required_features));
}
#	endif

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
