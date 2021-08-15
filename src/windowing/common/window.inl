#include "lak/window.hpp"

template struct lak::array<lak::window_handle, lak::dynamic_extent>;
template struct lak::railcar<lak::window_handle>;
template struct lak::bank<lak::window_handle>;
template size_t lak::bank<lak::window_handle>::internal_create<
  lak::window_handle>(lak::window_handle &&);
template size_t lak::bank<lak::window_handle>::internal_create<
  lak::const_window_handle_ref>(lak::const_window_handle_ref &&);
template struct lak::unique_bank_ptr<lak::window_handle>;
template struct lak::shared_bank_ptr<lak::window_handle>;

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

lak::result<lak::window> lak::window::make(const lak::software_settings &s)
{
	auto handle = lak::unique_bank_ptr<lak::window_handle>::from_raw_bank_ptr(
	  lak::create_window(s));
	if (!handle)
		return lak::err_t{};
	else
		return lak::ok_t{lak::window(lak::move(handle))};
}

lak::result<lak::window> lak::window::make(const lak::opengl_settings &s)
{
	auto handle = lak::unique_bank_ptr<lak::window_handle>::from_raw_bank_ptr(
	  lak::create_window(s));
	if (!handle)
		return lak::err_t{};
	else
		return lak::ok_t{lak::window(lak::move(handle))};
}

lak::result<lak::window> lak::window::make(const lak::vulkan_settings &s)
{
	auto handle = lak::unique_bank_ptr<lak::window_handle>::from_raw_bank_ptr(
	  lak::create_window(s));
	if (!handle)
		return lak::err_t{};
	else
		return lak::ok_t{lak::window(lak::move(handle))};
}

lak::window::~window()
{
	if (handle()) lak::destroy_window(_handle.release());
}
