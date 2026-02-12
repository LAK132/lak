#include "lak/system/windowing/window.hpp"

#include "lak/string_literals/string.hpp"

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
lak::cobalt_renderer_settings lak::cobalt_renderer_settings::preferred(
  lak::cobalt::renderer_info_func info,
  ::cobalt::logging::ILogger::unique_ptr log)
{
	lak::cobalt_renderer_settings result;

	info(0, result.renderer_info);

	result.device_enumerator =
	  result.renderer_info.CreateGraphicsDeviceEnumerator(lak::move(log));
	result.device_enumerator->EnumerateDevices();

	result.device = result.device_enumerator->GetPreferredDevice();

	return result;
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
