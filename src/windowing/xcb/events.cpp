#include "lak/events.hpp"
#include "lak/debug.hpp"

#include "impl.hpp"

bool lak::next_event(const lak::platform_instance &handle, lak::event *event)
{
	xcb_generic_event_t *e;
	if (!(e = xcb_poll_for_event(handle.handle, 0))) return false;
	lak::memcpy(&event->platform_event, e);
	free(e);

	switch (event->platform_event.type)
	{
			/* --- quit_program --- */

			/* --- window_close --- */

			/* --- window_destroyed --- */

			/* --- window_created --- */

			/* --- window_resized --- */

			/* --- window_moved --- */

			/* --- window_exposed --- */

			/* --- key_down --- */

			/* --- key_up --- */

			/* --- mouse_button_down --- */

			/* --- mouse_button_up --- */

			/* --- mouse_move --- */

			/* --- mouse_wheel --- */

		default:
			event->type = event_type::platform_event;
	}

	return true;
}

bool lak::peek_thread_event(const lak::platform_instance &instance,
                            lak::event *event)
{
	return false;
}

#include "../common/events.inl"
