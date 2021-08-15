#include "lak/bank_ptr.hpp"
#include "lak/debug.hpp"
#include "lak/memmanip.hpp"

#include "impl.hpp"

SDL_Window *sdl_window_from_event(const SDL_Event &event)
{
	switch (event.type)
	{
		case SDL_WINDOWEVENT: return SDL_GetWindowFromID(event.window.windowID);

		case SDL_KEYDOWN:
		case SDL_KEYUP: return SDL_GetWindowFromID(event.key.windowID);

		case SDL_TEXTEDITING:
		case SDL_TEXTINPUT: return SDL_GetWindowFromID(event.text.windowID);

		case SDL_MOUSEMOTION: return SDL_GetWindowFromID(event.motion.windowID);

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP: return SDL_GetWindowFromID(event.button.windowID);

		case SDL_MOUSEWHEEL: return SDL_GetWindowFromID(event.wheel.windowID);

		case SDL_DROPFILE:
		case SDL_DROPTEXT:
		case SDL_DROPBEGIN:
		case SDL_DROPCOMPLETE: return SDL_GetWindowFromID(event.drop.windowID);

		case SDL_USEREVENT: return SDL_GetWindowFromID(event.user.windowID);

		default: return nullptr;
	}
}

const lak::window_handle *window_from_event(const SDL_Event &event)
{
	SDL_Window *handle = sdl_window_from_event(event);

	return handle ? lak::bank<lak::window_handle>::find_if(
	                  [window = handle](const lak::window_handle &handle)
	                  { return handle.sdl_window == window; })
	              : nullptr;
}

void translate_event(const SDL_Event &sdl_event,
                     lak::event *event,
                     const lak::window_handle *window = nullptr)
{
	event->platform();
	lak::platform_event_ptr platform_event = lak::move(event->_platform_event);
	lak::memcpy(&platform_event->sdl_event, &sdl_event);

	switch (sdl_event.type)
	{
			/* --- quit_program --- */

		// Quit the application entirely.
		case SDL_QUIT:
		{
			*event = lak::event(
			  lak::event_type::quit_program, window, lak::move(platform_event));
		}
		break;

		// Quit the application entirely.
		case SDL_APP_TERMINATING:
		{
			*event = lak::event(
			  lak::event_type::quit_program, window, lak::move(platform_event));
		}
		break;

		case SDL_WINDOWEVENT:
			switch (sdl_event.window.event)
			{
					/* --- window_closed --- */

				// Window *has* been destroyed.
				case SDL_WINDOWEVENT_CLOSE:
				{
					*event = lak::event(lak::event_type::window_closed,
					                    window,
					                    lak::move(platform_event),
					                    lak::window_event{});
				}
				break;

					/* --- window_changed --- */

				case SDL_WINDOWEVENT_RESIZED:
				case SDL_WINDOWEVENT_SIZE_CHANGED:
				{
					*event = lak::event(
					  lak::event_type::window_changed,
					  window,
					  lak::move(platform_event),
					  lak::window_event{
					    {}, {sdl_event.window.data1, sdl_event.window.data2}});
				}
				break;

				case SDL_WINDOWEVENT_MOVED:
				{
					*event = lak::event(
					  lak::event_type::window_changed,
					  window,
					  lak::move(platform_event),
					  lak::window_event{{sdl_event.window.data1, sdl_event.window.data2},
					                    {}});
				}
				break;

					/* --- window_exposed --- */

				case SDL_WINDOWEVENT_EXPOSED:
				{
					*event = lak::event(lak::event_type::window_exposed,
					                    window,
					                    lak::move(platform_event),
					                    lak::window_event{});
				}
				break;
			}
			break;

			/* --- key_down --- */

		case SDL_KEYDOWN:
		{
			uint32_t scancode        = sdl_event.key.keysym.scancode;
			lak::mod_key mod         = lak::mod_key::none;
			const SDL_Keymod sdl_mod = SDL_GetModState();
			if ((sdl_mod & KMOD_SHIFT) != 0) mod = mod | lak::mod_key::shift;
			if ((sdl_mod & KMOD_CTRL) != 0) mod = mod | lak::mod_key::ctrl;
			if ((sdl_mod & KMOD_ALT) != 0) mod = mod | lak::mod_key::alt;
			if ((sdl_mod & KMOD_GUI) != 0) mod = mod | lak::mod_key::super;

			*event = lak::event(lak::event_type::key_down,
			                    window,
			                    lak::move(platform_event),
			                    lak::key_event{scancode, mod});
		}
		break;

			/* --- key_up --- */

		case SDL_KEYUP:
		{
			uint32_t scancode        = sdl_event.key.keysym.scancode;
			lak::mod_key mod         = lak::mod_key::none;
			const SDL_Keymod sdl_mod = SDL_GetModState();
			if ((sdl_mod & KMOD_SHIFT) != 0) mod = mod | lak::mod_key::shift;
			if ((sdl_mod & KMOD_CTRL) != 0) mod = mod | lak::mod_key::ctrl;
			if ((sdl_mod & KMOD_ALT) != 0) mod = mod | lak::mod_key::alt;
			if ((sdl_mod & KMOD_GUI) != 0) mod = mod | lak::mod_key::super;

			*event = lak::event(lak::event_type::key_up,
			                    window,
			                    lak::move(platform_event),
			                    lak::key_event{scancode, mod});
		}
		break;

			/* --- button_down --- */

		case SDL_MOUSEBUTTONDOWN:
		{
			lak::mouse_button btn = lak::mouse_button::none;
			switch (sdl_event.button.button)
			{
				case SDL_BUTTON_LEFT: btn = lak::mouse_button::left; break;
				case SDL_BUTTON_RIGHT: btn = lak::mouse_button::right; break;
				case SDL_BUTTON_MIDDLE: btn = lak::mouse_button::middle; break;
				case SDL_BUTTON_X1: btn = lak::mouse_button::x1; break;
				case SDL_BUTTON_X2: btn = lak::mouse_button::x2; break;
			}
			*event = lak::event(lak::event_type::button_down,
			                    window,
			                    lak::move(platform_event),
			                    lak::button_event{btn});
		}
		break;

			/* --- button_up --- */

		case SDL_MOUSEBUTTONUP:
		{
			lak::mouse_button btn = lak::mouse_button::none;
			switch (sdl_event.button.button)
			{
				case SDL_BUTTON_LEFT: btn = lak::mouse_button::left; break;
				case SDL_BUTTON_RIGHT: btn = lak::mouse_button::right; break;
				case SDL_BUTTON_MIDDLE: btn = lak::mouse_button::middle; break;
				case SDL_BUTTON_X1: btn = lak::mouse_button::x1; break;
				case SDL_BUTTON_X2: btn = lak::mouse_button::x2; break;
			}
			*event = lak::event(lak::event_type::button_up,
			                    window,
			                    lak::move(platform_event),
			                    lak::button_event{btn});
		}
		break;

			/* --- motion --- */

		case SDL_MOUSEMOTION:
		{
			*event = lak::event(
			  lak::event_type::motion,
			  window,
			  lak::move(platform_event),
			  lak::motion_event{{sdl_event.motion.x, sdl_event.motion.y}});
		}
		break;

			/* --- wheel --- */

		case SDL_MOUSEWHEEL:
		{
			*event =
			  lak::event(lak::event_type::wheel,
			             window,
			             lak::move(platform_event),
			             lak::wheel_event{{static_cast<float>(sdl_event.wheel.x),
			                               static_cast<float>(sdl_event.wheel.y)}});
		}
		break;

		default:
		{
			*event = lak::event(
			  lak::event_type::platform_event, window, lak::move(platform_event));
		}
		break;
	}
}

bool handle_next_event(lak::event *event, SDL_eventaction action)
{
	SDL_PumpEvents();
	if (SDL_Event e;
	    SDL_PeepEvents(&e, 1, action, SDL_FIRSTEVENT, SDL_LASTEVENT) > 0)
	{
		translate_event(e, event, window_from_event(e));
		lak::memcpy(&e, &event->platform().sdl_event);
		return true;
	}
	return false;
}

bool lak::next_event(lak::event *event)
{
	return handle_next_event(event, SDL_GETEVENT);
}

bool lak::peek_event(lak::event *event)
{
	return handle_next_event(event, SDL_PEEKEVENT);
}

#include "../common/events.inl"
