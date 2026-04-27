#include "lak/debug.hpp"
#include "lak/memmanip.hpp"

#include "impl.hpp"

#include <thread>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE) & __ImageBase)

lak::wstring win32_error_string(LPCWSTR lpszFunction);
void win32_error_popup(LPCWSTR lpszFunction);

lak::mod_key get_mod_key_state()
{
	lak::mod_key mod = lak::mod_key::none;

	auto pressed = [](int key) -> bool
	{ return (::GetKeyState(key) & 0x8000) != 0; };

	if (pressed(VK_LSHIFT)) mod = mod | lak::mod_key::lshift;
	if (pressed(VK_RSHIFT)) mod = mod | lak::mod_key::rshift;
	if (pressed(VK_LCONTROL)) mod = mod | lak::mod_key::lctrl;
	if (pressed(VK_RCONTROL)) mod = mod | lak::mod_key::rctrl;
	if (pressed(VK_LMENU)) mod = mod | lak::mod_key::lalt;
	if (pressed(VK_RMENU)) mod = mod | lak::mod_key::ralt;
	if (pressed(VK_LWIN)) mod = mod | lak::mod_key::lsuper;
	if (pressed(VK_RWIN)) mod = mod | lak::mod_key::rsuper;

	return mod;
}

uint32_t get_scancode(LPARAM lParam)
{
	WORD keyFlags   = HIWORD(lParam);
	WORD scanCode   = LOBYTE(keyFlags);
	BOOL isExtended = (keyFlags & KF_EXTENDED) == KF_EXTENDED;
	if (isExtended) scanCode = MAKEWORD(scanCode, 0xE0);
	return scanCode;
}

lak::key_code get_key_code(WPARAM wParam, LPARAM lParam)
{
	WORD vkCode = LOWORD(wParam);

	WORD keyFlags   = HIWORD(lParam);
	WORD scanCode   = LOBYTE(keyFlags);
	BOOL isExtended = (keyFlags & KF_EXTENDED) == KF_EXTENDED;
	if (isExtended) scanCode = MAKEWORD(scanCode, 0xE0);

	switch (vkCode)
	{
		case VK_SHIFT:
		case VK_CONTROL:
		case VK_MENU:
		{
			vkCode = LOWORD(MapVirtualKeyW(scanCode, MAPVK_VSC_TO_VK_EX));
		}
		break;
	}
	switch (vkCode)
	{
		case VK_LSHIFT:   return lak::key_code::lshift;
		case VK_RSHIFT:   return lak::key_code::rshift;
		case VK_LCONTROL: return lak::key_code::lctrl;
		case VK_RCONTROL: return lak::key_code::rctrl;
		case VK_LMENU:    return lak::key_code::lalt;
		case VK_RMENU:    return lak::key_code::ralt;
		case VK_LWIN:     return lak::key_code::lsuper;
		case VK_RWIN:     return lak::key_code::rsuper;

		case VK_LEFT:  return lak::key_code::left;
		case VK_RIGHT: return lak::key_code::right;
		case VK_UP:    return lak::key_code::up;
		case VK_DOWN:  return lak::key_code::down;

		case VK_DELETE: return lak::key_code::del;
		case VK_BACK:   return lak::key_code::backspace;
		case VK_TAB:    return lak::key_code::tab;
		case VK_HOME:   return lak::key_code::home;
		case VK_END:    return lak::key_code::end;
		case VK_INSERT: return lak::key_code::insert;
		case VK_PRIOR:  return lak::key_code::page_up;
		case VK_NEXT:   return lak::key_code::page_down;
		case VK_RETURN:
			return isExtended ? lak::key_code::keypad_enter : lak::key_code::enter;
		case VK_ESCAPE: return lak::key_code::escape;

		case VK_SPACE:      return lak::key_code::space;
		// case VK_:           return lak::key_code::apostrophe;    // scancode
		case VK_OEM_COMMA:  return lak::key_code::comma;
		case VK_OEM_MINUS:  return lak::key_code::minus;
		case VK_OEM_PLUS:   return lak::key_code::plus;
		// case VK_:           return lak::key_code::equal;         // scancode
		case VK_OEM_PERIOD: return lak::key_code::period;
		// case VK_:           return lak::key_code::semicolon;     // scancode
		// case VK_:           return lak::key_code::slash;         // scancode
		// case VK_:           return lak::key_code::backslash;     // scancode
		// case VK_:           return lak::key_code::oem102;        // scancode
		// case VK_:           return lak::key_code::backtick;      // scancode
		// case VK_:           return lak::key_code::open_bracket;  // scancode
		// case VK_:           return lak::key_code::close_bracket; // scancode
		case '0':           return lak::key_code::num_0;
		case '1':           return lak::key_code::num_1;
		case '2':           return lak::key_code::num_2;
		case '3':           return lak::key_code::num_3;
		case '4':           return lak::key_code::num_4;
		case '5':           return lak::key_code::num_5;
		case '6':           return lak::key_code::num_6;
		case '7':           return lak::key_code::num_7;
		case '8':           return lak::key_code::num_8;
		case '9':           return lak::key_code::num_9;
		case 'A':           return lak::key_code::let_A;
		case 'B':           return lak::key_code::let_B;
		case 'C':           return lak::key_code::let_C;
		case 'D':           return lak::key_code::let_D;
		case 'E':           return lak::key_code::let_E;
		case 'F':           return lak::key_code::let_F;
		case 'G':           return lak::key_code::let_G;
		case 'H':           return lak::key_code::let_H;
		case 'I':           return lak::key_code::let_I;
		case 'J':           return lak::key_code::let_J;
		case 'K':           return lak::key_code::let_K;
		case 'L':           return lak::key_code::let_L;
		case 'M':           return lak::key_code::let_M;
		case 'N':           return lak::key_code::let_N;
		case 'O':           return lak::key_code::let_O;
		case 'P':           return lak::key_code::let_P;
		case 'Q':           return lak::key_code::let_Q;
		case 'R':           return lak::key_code::let_R;
		case 'S':           return lak::key_code::let_S;
		case 'T':           return lak::key_code::let_T;
		case 'U':           return lak::key_code::let_U;
		case 'V':           return lak::key_code::let_V;
		case 'W':           return lak::key_code::let_W;
		case 'X':           return lak::key_code::let_X;
		case 'Y':           return lak::key_code::let_Y;
		case 'Z':           return lak::key_code::let_Z;

		case VK_DECIMAL:  return lak::key_code::keypad_decimal;
		case VK_DIVIDE:   return lak::key_code::keypad_divide;
		case VK_MULTIPLY: return lak::key_code::keypad_multiply;
		case VK_SUBTRACT: return lak::key_code::keypad_subtract;
		case VK_ADD:      return lak::key_code::keypad_add;
		// case VK_:         return lak::key_code::keypad_equal;
		case VK_NUMPAD0:  return lak::key_code::keypad_0;
		case VK_NUMPAD1:  return lak::key_code::keypad_1;
		case VK_NUMPAD2:  return lak::key_code::keypad_2;
		case VK_NUMPAD3:  return lak::key_code::keypad_3;
		case VK_NUMPAD4:  return lak::key_code::keypad_4;
		case VK_NUMPAD5:  return lak::key_code::keypad_5;
		case VK_NUMPAD6:  return lak::key_code::keypad_6;
		case VK_NUMPAD7:  return lak::key_code::keypad_7;
		case VK_NUMPAD8:  return lak::key_code::keypad_8;
		case VK_NUMPAD9:  return lak::key_code::keypad_9;

		case VK_SNAPSHOT:        return lak::key_code::print_screen;
		case VK_PAUSE:           return lak::key_code::pause;
		case VK_APPS:            return lak::key_code::menu;
		case VK_BROWSER_BACK:    return lak::key_code::back;
		case VK_BROWSER_FORWARD: return lak::key_code::forward;
		case VK_F1:              return lak::key_code::f1;
		case VK_F2:              return lak::key_code::f2;
		case VK_F3:              return lak::key_code::f3;
		case VK_F4:              return lak::key_code::f4;
		case VK_F5:              return lak::key_code::f5;
		case VK_F6:              return lak::key_code::f6;
		case VK_F7:              return lak::key_code::f7;
		case VK_F8:              return lak::key_code::f8;
		case VK_F9:              return lak::key_code::f9;
		case VK_F10:             return lak::key_code::f10;
		case VK_F11:             return lak::key_code::f11;
		case VK_F12:             return lak::key_code::f12;
		case VK_F13:             return lak::key_code::f13;
		case VK_F14:             return lak::key_code::f14;
		case VK_F15:             return lak::key_code::f15;
		case VK_F16:             return lak::key_code::f16;
		case VK_F17:             return lak::key_code::f17;
		case VK_F18:             return lak::key_code::f18;
		case VK_F19:             return lak::key_code::f19;
		case VK_F20:             return lak::key_code::f20;
		case VK_F21:             return lak::key_code::f21;
		case VK_F22:             return lak::key_code::f22;
		case VK_F23:             return lak::key_code::f23;
		case VK_F24:             return lak::key_code::f24;

		default:
			switch (scanCode)
			{
				// https://handmade.network/forums/t/2011-keyboard_inputs_-_scancodes,_raw_input,_text_input,_key_names
				case 0x3A:   return lak::key_code::caps_lock;
				case 0x46:   return lak::key_code::scroll_lock;
				// :TODO: i don't have a num lock to verify...
				case 0xE045: return lak::key_code::num_lock;

				case 0x28: return lak::key_code::apostrophe;
				case 0x33: return lak::key_code::comma;
				case 0x0C: return lak::key_code::minus;
				case 0x0D: return lak::key_code::equal;
				case 0x34: return lak::key_code::period;
				case 0x27: return lak::key_code::semicolon;
				case 0x35: return lak::key_code::slash;
				case 0x2B: return lak::key_code::backslash;
				case 0x56: return lak::key_code::oem102;
				case 0x29: return lak::key_code::backtick;
				case 0x1A: return lak::key_code::open_bracket;
				case 0x1B: return lak::key_code::close_bracket;

				default: return lak::key_code::none;
			}
	}
}

void translate_event(const MSG &msg,
                     lak::event *event,
                     const lak::window_handle *window = nullptr)
{
	event->platform();
	lak::platform_event_ptr platform_event = lak::move(event->_platform_event);
	lak::memcpy(&platform_event->msg, &msg);

	switch (msg.message)
	{
			/* --- quit_program --- */

		// Quit the application entirely.
		case WM_QUIT:
		{
			CHECKPOINT();
			*event = lak::event(
			  lak::event_type::quit_program, window, lak::move(platform_event));
		}
		break;

			/* --- close_window --- */

		// User is *asking* to close this window.
		case WM_CLOSE:
		{
			*event = lak::event(lak::event_type::close_window,
			                    window,
			                    lak::move(platform_event),
			                    lak::window_event{});
		}
		break;

			/* --- window_closed --- */

		// Window *has* been destroyed.
		case WM_DESTROY:
		{
			*event = lak::event(lak::event_type::window_closed,
			                    window,
			                    lak::move(platform_event),
			                    lak::window_event{});
		}
		break;

			/* --- hover changed --- */

		case WM_MOUSEHOVER:
		{
			*event = lak::event(lak::event_type::window_hover,
			                    window,
			                    lak::move(platform_event),
			                    lak::window_event{});
		}
		break;
		case WM_MOUSELEAVE:
		{
			*event = lak::event(lak::event_type::window_leave,
			                    window,
			                    lak::move(platform_event),
			                    lak::window_event{});
		}
		break;

			/* --- focus changed --- */

		case WM_SETFOCUS:
		{
			*event = lak::event(lak::event_type::window_focus,
			                    window,
			                    lak::move(platform_event),
			                    lak::window_event{});
		}
		break;
		case WM_KILLFOCUS:
		{
			*event = lak::event(lak::event_type::window_no_focus,
			                    window,
			                    lak::move(platform_event),
			                    lak::window_event{});
		}
		break;

			/* --- window_changed --- */

		case WM_WINDOWPOSCHANGED:
		{
			// :TODO: get position/size data
			*event = lak::event(lak::event_type::window_changed,
			                    window,
			                    lak::move(platform_event),
			                    lak::window_event{});
		}
		break;

			/* --- window_exposed --- */

		case WM_PAINT:
		{
			*event = lak::event(lak::event_type::window_exposed,
			                    window,
			                    lak::move(platform_event),
			                    lak::window_event{});
		}
		break;

			/* --- key_down --- */

		case WM_SYSKEYDOWN:
		case WM_KEYDOWN:
		{
			uint32_t scancode = get_scancode(msg.lParam);
			lak::key_code key = get_key_code(msg.wParam, msg.lParam);
			lak::mod_key mod  = get_mod_key_state();

			*event = lak::event(lak::event_type::key_down,
			                    window,
			                    lak::move(platform_event),
			                    lak::key_event{key, mod, scancode});
		}
		break;

			/* --- key_up --- */

		case WM_SYSKEYUP:
		case WM_KEYUP:
		{
			uint32_t scancode = get_scancode(msg.lParam);
			lak::key_code key = get_key_code(msg.wParam, msg.lParam);
			lak::mod_key mod  = get_mod_key_state();

			*event = lak::event(lak::event_type::key_up,
			                    window,
			                    lak::move(platform_event),
			                    lak::key_event{key, mod, scancode});
		}
		break;

			/* --- character --- */

		case WM_UNICHAR:
		{
			// :TODO: is this a text input event? should SDL2 have something like
			// this?
			if (msg.wParam != UNICODE_NOCHAR)
				*event = lak::event(lak::event_type::character,
				                    window,
				                    lak::move(platform_event),
				                    lak::character_event{(char32_t)msg.wParam});
			else
				*event = lak::event(
				  lak::event_type::platform_event, window, lak::move(platform_event));
		}
		break;

			/* --- button_down --- */

		case WM_LBUTTONDOWN:
		{
			*event = lak::event(lak::event_type::button_down,
			                    window,
			                    lak::move(platform_event),
			                    lak::button_event{lak::mouse_button::left});
		}
		break;

		case WM_MBUTTONDOWN:
		{
			*event = lak::event(lak::event_type::button_down,
			                    window,
			                    lak::move(platform_event),
			                    lak::button_event{lak::mouse_button::middle});
		}
		break;

		case WM_RBUTTONDOWN:
		{
			*event = lak::event(lak::event_type::button_down,
			                    window,
			                    lak::move(platform_event),
			                    lak::button_event{lak::mouse_button::right});
		}
		break;

		case WM_XBUTTONDOWN:
		{
			*event = lak::event(
			  lak::event_type::button_down,
			  window,
			  lak::move(platform_event),
			  lak::button_event{(GET_XBUTTON_WPARAM(msg.wParam) == XBUTTON1
			                       ? lak::mouse_button::x1
			                       : lak::mouse_button::none) |
			                    (GET_XBUTTON_WPARAM(msg.wParam) == XBUTTON2
			                       ? lak::mouse_button::x2
			                       : lak::mouse_button::none)});
		}
		break;

			/* --- button_up --- */

		case WM_LBUTTONUP:
		{
			*event = lak::event(lak::event_type::button_up,
			                    window,
			                    lak::move(platform_event),
			                    lak::button_event{lak::mouse_button::left});
		}
		break;

		case WM_MBUTTONUP:
		{
			*event = lak::event(lak::event_type::button_up,
			                    window,
			                    lak::move(platform_event),
			                    lak::button_event{lak::mouse_button::middle});
		}
		break;

		case WM_RBUTTONUP:
		{
			*event = lak::event(lak::event_type::button_up,
			                    window,
			                    lak::move(platform_event),
			                    lak::button_event{lak::mouse_button::right});
		}
		break;

		case WM_XBUTTONUP:
		{
			*event = lak::event(
			  lak::event_type::button_up,
			  window,
			  lak::move(platform_event),
			  lak::button_event{(GET_XBUTTON_WPARAM(msg.wParam) == XBUTTON1
			                       ? lak::mouse_button::x1
			                       : lak::mouse_button::none) |
			                    (GET_XBUTTON_WPARAM(msg.wParam) == XBUTTON2
			                       ? lak::mouse_button::x2
			                       : lak::mouse_button::none)});
		}
		break;

			/* --- motion --- */

		case WM_MOUSEMOVE:
		{
			*event = lak::event(lak::event_type::motion,
			                    window,
			                    lak::move(platform_event),
			                    lak::motion_event{{GET_X_LPARAM(msg.lParam),
			                                       GET_Y_LPARAM(msg.lParam)}});
		}
		break;

			/* --- wheel --- */

		case WM_MOUSEWHEEL:
		{
			*event = lak::event(
			  lak::event_type::wheel,
			  window,
			  lak::move(platform_event),
			  lak::wheel_event{
			    {0.0f,
			     float(GET_WHEEL_DELTA_WPARAM(msg.wParam)) / float(WHEEL_DELTA)}});
		}
		break;

			/* --- platform_event --- */

		// Window has been created.
		case WM_CREATE:
		{
			*event = lak::event(
			  lak::event_type::platform_event, window, lak::move(platform_event));
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

bool handle_size_move(lak::window_handle &handle)
{
	if (handle._moving)
	{
		POINT cursor;
		ASSERTF(::GetCursorPos(&cursor) != 0,
		        lak::to_astring(win32_error_string(L"GetCursorPos")));
		const RECT &rect = handle._window_start;
		ASSERTF(::MoveWindow(handle._platform_handle,
		                     rect.left + cursor.x - handle._cursor_start.x,
		                     rect.top + cursor.y - handle._cursor_start.y,
		                     rect.right - rect.left,
		                     rect.bottom - rect.top,
		                     TRUE) != 0,
		        lak::to_astring(win32_error_string(L"MoveWindow")));
		return true;
	}
	if (handle._resizing)
	{
		POINT cursor;
		ASSERTF(::GetCursorPos(&cursor) != 0,
		        lak::to_astring(win32_error_string(L"GetCursorPos")));
		const RECT &rect = handle._window_start;
		RECT diff        = {0, 0, 0, 0};
		if (handle._side & handle.left)
		{
			diff.left  = cursor.x - handle._cursor_start.x;
			diff.right = -diff.left;
		}
		if (handle._side & handle.top)
		{
			diff.top    = cursor.y - handle._cursor_start.y;
			diff.bottom = -diff.top;
		}
		if (handle._side & handle.right)
		{
			diff.right = cursor.x - handle._cursor_start.x;
		}
		if (handle._side & handle.bottom)
		{
			diff.bottom = cursor.y - handle._cursor_start.y;
		}
		int x = rect.left + diff.left;
		int y = rect.top + diff.top;
		int w = std::max(0L, (rect.right - rect.left) + diff.right);
		int h = std::max(0L, (rect.bottom - rect.top) + diff.bottom);
		ASSERTF(::MoveWindow(handle._platform_handle, x, y, w, h, TRUE) != 0,
		        lak::to_astring(win32_error_string(L"MoveWindow")));

		lak::window_handle_resize(&handle);

		return true;
	}

	return false;
}

// Returns true if the event has handled by this function.
bool handle_size_move_event(lak::window_handle *handle, const MSG &msg)
{
	if (!handle) return false;

	switch (msg.message)
	{
		case WM_SYSCOMMAND:
		{
			switch (msg.wParam & 0xFFF0)
			{
				case SC_SIZE:
				{
					switch (msg.wParam & 0xF)
					{
						case 0x1: handle->_side = handle->left; break;
						case 0x2: handle->_side = handle->right; break;
						case 0x3: handle->_side = handle->top; break;
						case 0x4: handle->_side = handle->top | handle->left; break;
						case 0x5: handle->_side = handle->top | handle->right; break;
						case 0x6: handle->_side = handle->bottom; break;
						case 0x7: handle->_side = handle->bottom | handle->left; break;
						case 0x8: handle->_side = handle->bottom | handle->right; break;
						default:
							handle->_side = 0;
							FATAL("Invalid side");
							break;
					}

					// screen coords.
					ASSERTF(::GetWindowRect(msg.hwnd, &handle->_window_start) != 0,
					        lak::to_astring(win32_error_string(L"GetWindowRect")));
					handle->_cursor_start.x = GET_X_LPARAM(msg.lParam);
					handle->_cursor_start.y = GET_Y_LPARAM(msg.lParam);
					handle->_resizing       = true;
					::SetCapture(handle->_platform_handle);
				}
				break;

				case SC_MOVE:
				{
					// screen coords.
					ASSERTF(::GetWindowRect(msg.hwnd, &handle->_window_start) != 0,
					        lak::to_astring(win32_error_string(L"GetWindowRect")));
					handle->_cursor_start.x = GET_X_LPARAM(msg.lParam);
					handle->_cursor_start.y = GET_Y_LPARAM(msg.lParam);
					handle->_moving         = true;
					::SetCapture(handle->_platform_handle);
				}
				break;
			}
			return true;
		}

		case WM_LBUTTONUP:
		case WM_NCLBUTTONUP:
		{
			if (handle->_moving || handle->_resizing)
			{
				handle->_moving   = false;
				handle->_resizing = false;
				ASSERTF(::ReleaseCapture() != 0,
				        lak::to_astring(win32_error_string(L"ReleaseCapture")));
				return true;
			}
		}
		break;

		case WM_MOUSEMOVE:
		case WM_MOUSELEAVE:
		case WM_MOUSEHOVER:
		case WM_NCMOUSEMOVE:
		case WM_NCMOUSELEAVE:
		case WM_NCMOUSEHOVER: return handle_size_move(*handle);
	}

	return false;
}

lak::window_handle *window_from_event(const MSG &event)
{
	return event.hwnd ? lak::bank<lak::window_handle>::find_if(
	                      [hwnd = event.hwnd](const lak::window_handle &handle)
	                      { return handle._platform_handle == hwnd; })
	                  : nullptr;
}

// If this ends up being for a window that is about to get destroyed,
// this message should be zeroed out.
thread_local MSG previous_event = {};

bool peek_next_event(MSG *msg,
                     UINT filter_min,
                     UINT filter_max,
                     UINT remove,
                     bool *buffered_message = nullptr)
{
	if (auto *hacked_msg = lak::_platform_instance->platform_events.front();
	    hacked_msg)
	{
		if (buffered_message) *buffered_message = true;
		lak::memcpy(msg, hacked_msg);
		if (remove == UINT(PM_REMOVE))
			lak::_platform_instance->platform_events.pop_front();
		else
			ASSERT_EQUAL(remove, UINT(PM_NOREMOVE));
		return true;
	}
	else
	{
		if (buffered_message) *buffered_message = false;
		return ::PeekMessageW(msg, NULL, filter_min, filter_max, remove);
	}
}

bool handle_next_event(lak::event *event, const bool pop_messages)
{
	// Delaying dispatch until the next time through here should let us handle
	// WM_PAINT correctly after the call to next_event.
	if (pop_messages && previous_event.message)
	{
		::DispatchMessageW(&previous_event);
		lak::bzero(&previous_event);
	}

	// Some messages are hacked into our own message queue via the WndProc in
	// platform.cpp, those messages are marked as buffered.

	const UINT filter_min = 0;
	const UINT filter_max = 0;

	// We automatically handle size/move events here, don't return them to the
	// caller.
	MSG msg                    = {};
	bool buffered_message      = false;
	lak::window_handle *handle = nullptr;
	if (pop_messages)
	{
		do
		{
			if (!peek_next_event(
			      &msg, filter_min, filter_max, PM_REMOVE, &buffered_message))
			{
				return false;
			}
			handle = window_from_event(msg);
			// handle_size_move_event calls handle_size_move
		} while (handle_size_move_event(handle, msg));
	}
	else
	{
		if (!peek_next_event(
		      &msg, filter_min, filter_max, PM_NOREMOVE, &buffered_message))
		{
			return false;
		}
		handle = window_from_event(msg);
		while (handle_size_move_event(handle, msg))
		{
			// remove the previously peeked message as it was used in
			// handle_size_move_event.
			ASSERT(peek_next_event(
			  &msg, filter_min, filter_max, PM_REMOVE, &buffered_message));
			if (!peek_next_event(
			      &msg, filter_min, filter_max, PM_NOREMOVE, &buffered_message))
			{
				return false;
			}
			handle = window_from_event(msg);
		}
	}

	::TranslateMessage(&msg);

	if (pop_messages)
	{
		// Do no re-dispatch messages that were buffered from WndProc.
		if (buffered_message)
			lak::bzero(&previous_event);
		else
			lak::memcpy(&previous_event, &msg);
	}

	translate_event(msg, event, handle);

	return true;
}

bool lak::next_event(lak::event *event)
{
	return handle_next_event(event, true);
}

bool lak::peek_event(lak::event *event)
{
	return handle_next_event(event, false);
}

#include "../common/events.inl"
