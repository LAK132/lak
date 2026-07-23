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
		case SDL_KEYUP:   return SDL_GetWindowFromID(event.key.windowID);

		case SDL_TEXTEDITING:
		case SDL_TEXTINPUT:   return SDL_GetWindowFromID(event.text.windowID);

		case SDL_MOUSEMOTION: return SDL_GetWindowFromID(event.motion.windowID);

		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:   return SDL_GetWindowFromID(event.button.windowID);

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

lak::mod_key get_mod_key_state()
{
	lak::mod_key mod         = lak::mod_key::none;
	const SDL_Keymod sdl_mod = SDL_GetModState();
	if ((sdl_mod & KMOD_LSHIFT) != 0) mod = mod | lak::mod_key::lshift;
	if ((sdl_mod & KMOD_RSHIFT) != 0) mod = mod | lak::mod_key::rshift;
	if ((sdl_mod & KMOD_LCTRL) != 0) mod = mod | lak::mod_key::lctrl;
	if ((sdl_mod & KMOD_RCTRL) != 0) mod = mod | lak::mod_key::rctrl;
	if ((sdl_mod & KMOD_LALT) != 0) mod = mod | lak::mod_key::lalt;
	if ((sdl_mod & KMOD_RALT) != 0) mod = mod | lak::mod_key::ralt;
	if ((sdl_mod & KMOD_LGUI) != 0) mod = mod | lak::mod_key::lsuper;
	if ((sdl_mod & KMOD_RGUI) != 0) mod = mod | lak::mod_key::rsuper;
	return mod;
}

lak::key_code get_key_code(uint32_t scancode)
{
	switch (scancode)
	{
		case SDL_SCANCODE_LSHIFT: return lak::key_code::lshift;
		case SDL_SCANCODE_RSHIFT: return lak::key_code::rshift;
		case SDL_SCANCODE_LCTRL:  return lak::key_code::lctrl;
		case SDL_SCANCODE_RCTRL:  return lak::key_code::rctrl;
		case SDL_SCANCODE_LALT:   return lak::key_code::lalt;
		case SDL_SCANCODE_RALT:   return lak::key_code::ralt;
		case SDL_SCANCODE_LGUI:   return lak::key_code::lsuper;
		case SDL_SCANCODE_RGUI:   return lak::key_code::rsuper;

		case SDL_SCANCODE_UP:    return lak::key_code::up;
		case SDL_SCANCODE_DOWN:  return lak::key_code::down;
		case SDL_SCANCODE_LEFT:  return lak::key_code::left;
		case SDL_SCANCODE_RIGHT: return lak::key_code::right;

		case SDL_SCANCODE_DELETE:    return lak::key_code::del;
		case SDL_SCANCODE_BACKSPACE: return lak::key_code::backspace;
		case SDL_SCANCODE_TAB:       return lak::key_code::tab;
		case SDL_SCANCODE_HOME:      return lak::key_code::home;
		case SDL_SCANCODE_END:       return lak::key_code::end;
		case SDL_SCANCODE_INSERT:    return lak::key_code::insert;
		case SDL_SCANCODE_PAGEUP:    return lak::key_code::page_up;
		case SDL_SCANCODE_PAGEDOWN:  return lak::key_code::page_down;
		case SDL_SCANCODE_RETURN:    return lak::key_code::enter;
		case SDL_SCANCODE_ESCAPE:    return lak::key_code::escape;

		case SDL_SCANCODE_CAPSLOCK:     return lak::key_code::caps_lock;
		case SDL_SCANCODE_SCROLLLOCK:   return lak::key_code::scroll_lock;
		case SDL_SCANCODE_NUMLOCKCLEAR: return lak::key_code::num_lock;

		case SDL_SCANCODE_SPACE:          return lak::key_code::space;
		case SDL_SCANCODE_APOSTROPHE:     return lak::key_code::apostrophe;
		case SDL_SCANCODE_COMMA:          return lak::key_code::comma;
		case SDL_SCANCODE_MINUS:          return lak::key_code::minus;
		// case SDL_SCANCODE_PLUS:       return lak::key_code::plus;
		case SDL_SCANCODE_EQUALS:         return lak::key_code::equal;
		case SDL_SCANCODE_PERIOD:         return lak::key_code::period;
		case SDL_SCANCODE_SEMICOLON:      return lak::key_code::semicolon;
		case SDL_SCANCODE_SLASH:          return lak::key_code::slash;
		case SDL_SCANCODE_BACKSLASH:      return lak::key_code::backslash;
		case SDL_SCANCODE_NONUSBACKSLASH: return lak::key_code::oem102;
		case SDL_SCANCODE_GRAVE:          return lak::key_code::backtick;
		case SDL_SCANCODE_LEFTBRACKET:    return lak::key_code::open_bracket;
		case SDL_SCANCODE_RIGHTBRACKET:   return lak::key_code::close_bracket;
		case SDL_SCANCODE_0:              return lak::key_code::num_0;
		case SDL_SCANCODE_1:              return lak::key_code::num_1;
		case SDL_SCANCODE_2:              return lak::key_code::num_2;
		case SDL_SCANCODE_3:              return lak::key_code::num_3;
		case SDL_SCANCODE_4:              return lak::key_code::num_4;
		case SDL_SCANCODE_5:              return lak::key_code::num_5;
		case SDL_SCANCODE_6:              return lak::key_code::num_6;
		case SDL_SCANCODE_7:              return lak::key_code::num_7;
		case SDL_SCANCODE_8:              return lak::key_code::num_8;
		case SDL_SCANCODE_9:              return lak::key_code::num_9;
		case SDL_SCANCODE_A:              return lak::key_code::let_A;
		case SDL_SCANCODE_B:              return lak::key_code::let_B;
		case SDL_SCANCODE_C:              return lak::key_code::let_C;
		case SDL_SCANCODE_D:              return lak::key_code::let_D;
		case SDL_SCANCODE_E:              return lak::key_code::let_E;
		case SDL_SCANCODE_F:              return lak::key_code::let_F;
		case SDL_SCANCODE_G:              return lak::key_code::let_G;
		case SDL_SCANCODE_H:              return lak::key_code::let_H;
		case SDL_SCANCODE_I:              return lak::key_code::let_I;
		case SDL_SCANCODE_J:              return lak::key_code::let_J;
		case SDL_SCANCODE_K:              return lak::key_code::let_K;
		case SDL_SCANCODE_L:              return lak::key_code::let_L;
		case SDL_SCANCODE_M:              return lak::key_code::let_M;
		case SDL_SCANCODE_N:              return lak::key_code::let_N;
		case SDL_SCANCODE_O:              return lak::key_code::let_O;
		case SDL_SCANCODE_P:              return lak::key_code::let_P;
		case SDL_SCANCODE_Q:              return lak::key_code::let_Q;
		case SDL_SCANCODE_R:              return lak::key_code::let_R;
		case SDL_SCANCODE_S:              return lak::key_code::let_S;
		case SDL_SCANCODE_T:              return lak::key_code::let_T;
		case SDL_SCANCODE_U:              return lak::key_code::let_U;
		case SDL_SCANCODE_V:              return lak::key_code::let_V;
		case SDL_SCANCODE_W:              return lak::key_code::let_W;
		case SDL_SCANCODE_X:              return lak::key_code::let_X;
		case SDL_SCANCODE_Y:              return lak::key_code::let_Y;
		case SDL_SCANCODE_Z:              return lak::key_code::let_Z;

		case SDL_SCANCODE_KP_DECIMAL:  return lak::key_code::keypad_decimal;
		case SDL_SCANCODE_KP_DIVIDE:   return lak::key_code::keypad_divide;
		case SDL_SCANCODE_KP_MULTIPLY: return lak::key_code::keypad_multiply;
		case SDL_SCANCODE_KP_MINUS:    return lak::key_code::keypad_subtract;
		case SDL_SCANCODE_KP_PLUS:     return lak::key_code::keypad_add;
		case SDL_SCANCODE_KP_EQUALS:   return lak::key_code::keypad_equal;
		case SDL_SCANCODE_KP_0:        return lak::key_code::keypad_0;
		case SDL_SCANCODE_KP_1:        return lak::key_code::keypad_1;
		case SDL_SCANCODE_KP_2:        return lak::key_code::keypad_2;
		case SDL_SCANCODE_KP_3:        return lak::key_code::keypad_3;
		case SDL_SCANCODE_KP_4:        return lak::key_code::keypad_4;
		case SDL_SCANCODE_KP_5:        return lak::key_code::keypad_5;
		case SDL_SCANCODE_KP_6:        return lak::key_code::keypad_6;
		case SDL_SCANCODE_KP_7:        return lak::key_code::keypad_7;
		case SDL_SCANCODE_KP_8:        return lak::key_code::keypad_8;
		case SDL_SCANCODE_KP_9:        return lak::key_code::keypad_9;

		case SDL_SCANCODE_PRINTSCREEN: return lak::key_code::print_screen;
		case SDL_SCANCODE_PAUSE:       return lak::key_code::pause;
		case SDL_SCANCODE_MENU:        return lak::key_code::menu;
		case SDL_SCANCODE_AC_BACK:     return lak::key_code::back;
		case SDL_SCANCODE_AC_FORWARD:  return lak::key_code::forward;
		case SDL_SCANCODE_F1:          return lak::key_code::f1;
		case SDL_SCANCODE_F2:          return lak::key_code::f2;
		case SDL_SCANCODE_F3:          return lak::key_code::f3;
		case SDL_SCANCODE_F4:          return lak::key_code::f4;
		case SDL_SCANCODE_F5:          return lak::key_code::f5;
		case SDL_SCANCODE_F6:          return lak::key_code::f6;
		case SDL_SCANCODE_F7:          return lak::key_code::f7;
		case SDL_SCANCODE_F8:          return lak::key_code::f8;
		case SDL_SCANCODE_F9:          return lak::key_code::f9;
		case SDL_SCANCODE_F10:         return lak::key_code::f10;
		case SDL_SCANCODE_F11:         return lak::key_code::f11;
		case SDL_SCANCODE_F12:         return lak::key_code::f12;
		case SDL_SCANCODE_F13:         return lak::key_code::f13;
		case SDL_SCANCODE_F14:         return lak::key_code::f14;
		case SDL_SCANCODE_F15:         return lak::key_code::f15;
		case SDL_SCANCODE_F16:         return lak::key_code::f16;
		case SDL_SCANCODE_F17:         return lak::key_code::f17;
		case SDL_SCANCODE_F18:         return lak::key_code::f18;
		case SDL_SCANCODE_F19:         return lak::key_code::f19;
		case SDL_SCANCODE_F20:         return lak::key_code::f20;
		case SDL_SCANCODE_F21:         return lak::key_code::f21;
		case SDL_SCANCODE_F22:         return lak::key_code::f22;
		case SDL_SCANCODE_F23:         return lak::key_code::f23;
		case SDL_SCANCODE_F24:         return lak::key_code::f24;

		default: return lak::key_code::none;
	}
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
					/* --- close_window --- */

				case SDL_WINDOWEVENT_CLOSE:
				{
					*event = lak::event(lak::event_type::close_window,
					                    window,
					                    lak::move(platform_event),
					                    lak::window_event{});
				}
				break;

					/* --- hover changed --- */

				case SDL_WINDOWEVENT_ENTER:
				{
					*event = lak::event(lak::event_type::window_hover,
					                    window,
					                    lak::move(platform_event),
					                    lak::window_event{});
				}
				break;
				case SDL_WINDOWEVENT_LEAVE:
				{
					*event = lak::event(lak::event_type::window_leave,
					                    window,
					                    lak::move(platform_event),
					                    lak::window_event{});
				}
				break;

					/* --- focus changed --- */

				case SDL_WINDOWEVENT_FOCUS_GAINED:
				{
					*event = lak::event(lak::event_type::window_focus,
					                    window,
					                    lak::move(platform_event),
					                    lak::window_event{});
				}
				break;
				case SDL_WINDOWEVENT_FOCUS_LOST:
				{
					*event = lak::event(lak::event_type::window_no_focus,
					                    window,
					                    lak::move(platform_event),
					                    lak::window_event{});
				}
				break;

					/* --- window_changed --- */

				case SDL_WINDOWEVENT_RESIZED:
				case SDL_WINDOWEVENT_SIZE_CHANGED:
				{
					if (window) lak::window_handle_resize(window);
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

			/* --- text --- */

		case SDL_TEXTINPUT:
		{
			auto str = lak::as_u8string(lak::astring_view::from_c_str(
			  sdl_event.text.text, SDL_TEXTINPUTEVENT_TEXT_SIZE));
			lak::array<char8_t> text;
			text.push_back(str.begin(), str.end());
			*event = lak::event(lak::event_type::text,
			                    window,
			                    lak::move(platform_event),
			                    lak::text_event{lak::move(text)});
		}
		break;

			/* --- key_down --- */

		case SDL_KEYDOWN:
		{
			uint32_t scancode = sdl_event.key.keysym.scancode;
			lak::key_code key = get_key_code(scancode);
			lak::mod_key mod  = get_mod_key_state();

			*event = lak::event(lak::event_type::key_down,
			                    window,
			                    lak::move(platform_event),
			                    lak::key_event{key, mod, scancode});
		}
		break;

			/* --- key_up --- */

		case SDL_KEYUP:
		{
			uint32_t scancode = sdl_event.key.keysym.scancode;
			lak::key_code key = get_key_code(scancode);
			lak::mod_key mod  = get_mod_key_state();

			*event = lak::event(lak::event_type::key_up,
			                    window,
			                    lak::move(platform_event),
			                    lak::key_event{key, mod, scancode});
		}
		break;

			/* --- button_down --- */

		case SDL_MOUSEBUTTONDOWN:
		{
			lak::mouse_button btn = lak::mouse_button::none;
			switch (sdl_event.button.button)
			{
				case SDL_BUTTON_LEFT:   btn = lak::mouse_button::left; break;
				case SDL_BUTTON_RIGHT:  btn = lak::mouse_button::right; break;
				case SDL_BUTTON_MIDDLE: btn = lak::mouse_button::middle; break;
				case SDL_BUTTON_X1:     btn = lak::mouse_button::x1; break;
				case SDL_BUTTON_X2:     btn = lak::mouse_button::x2; break;
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
				case SDL_BUTTON_LEFT:   btn = lak::mouse_button::left; break;
				case SDL_BUTTON_RIGHT:  btn = lak::mouse_button::right; break;
				case SDL_BUTTON_MIDDLE: btn = lak::mouse_button::middle; break;
				case SDL_BUTTON_X1:     btn = lak::mouse_button::x1; break;
				case SDL_BUTTON_X2:     btn = lak::mouse_button::x2; break;
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

		case SDL_DROPFILE:
		{
			*event = lak::event(
			  lak::event_type::dropfile,
			  window,
			  lak::move(platform_event),
			  lak::dropfile_event{
			    .position = lak::vec2l_t{} /* :TODO: get the cursor position */,
			    .path =
			      lak::u8string(reinterpret_cast<char8_t *>(sdl_event.drop.file))});
			SDL_free(sdl_event.drop.file);
		}
		break;

		case SDL_DROPTEXT:
		{
			SDL_free(sdl_event.drop.file);
		}
			[[fallthrough]];
		case SDL_DROPBEGIN:    [[fallthrough]];
		case SDL_DROPCOMPLETE: [[fallthrough]];
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
