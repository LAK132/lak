#include "lak/debug.hpp"
#include "lak/memmanip.hpp"

#include "impl.hpp"

#include <thread>

EXTERN_C IMAGE_DOS_HEADER __ImageBase;
#define HINST_THISCOMPONENT ((HINSTANCE) & __ImageBase)

lak::wstring win32_error_string(LPCWSTR lpszFunction);
void win32_error_popup(LPCWSTR lpszFunction);

lak::mod_key key_to_mod(lak::key_code key)
{
	switch (key)
	{
		case lak::key_code::lshift:
			return lak::mod_key::lshift;
		case lak::key_code::rshift:
			return lak::mod_key::rshift;
		case lak::key_code::lctrl:
			return lak::mod_key::lctrl;
		case lak::key_code::rctrl:
			return lak::mod_key::rctrl;
		case lak::key_code::lalt:
			return lak::mod_key::lalt;
		case lak::key_code::ralt:
			return lak::mod_key::ralt;
		case lak::key_code::lsuper:
			return lak::mod_key::lsuper;
		case lak::key_code::rsuper:
			return lak::mod_key::rsuper;
		default:
			return lak::mod_key::none;
	}
}

lak::mod_key get_mod_key_state(lak::key_code key, bool pressed)
{
	lak::mod_key mod = lak::_platform_instance->mod_key_state;

	if (key == lak::key_code::lshift)
		mod = pressed ? mod | lak::mod_key::lshift : mod & -lak::mod_key::lshift;
	if (key == lak::key_code::rshift)
		mod = pressed ? mod | lak::mod_key::rshift : mod & -lak::mod_key::rshift;
	if (key == lak::key_code::lctrl)
		mod = pressed ? mod | lak::mod_key::lctrl : mod & -lak::mod_key::lctrl;
	if (key == lak::key_code::rctrl)
		mod = pressed ? mod | lak::mod_key::rctrl : mod & -lak::mod_key::rctrl;
	if (key == lak::key_code::lalt)
		mod = pressed ? mod | lak::mod_key::lalt : mod & -lak::mod_key::lalt;
	if (key == lak::key_code::ralt)
		mod = pressed ? mod | lak::mod_key::ralt : mod & -lak::mod_key::ralt;
	if (key == lak::key_code::lsuper)
		mod = pressed ? mod | lak::mod_key::lsuper : mod & -lak::mod_key::lsuper;
	if (key == lak::key_code::rsuper)
		mod = pressed ? mod | lak::mod_key::rsuper : mod & -lak::mod_key::rsuper;

	lak::_platform_instance->mod_key_state = mod;

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

lak::key_code get_key_code(WPARAM wParam, uint32_t scancode)
{
	WORD vkCode = LOWORD(wParam);
	switch (vkCode)
	{
		case VK_SHIFT:
		case VK_CONTROL:
		case VK_MENU:
		{
			vkCode = LOWORD(MapVirtualKeyW(scancode, MAPVK_VSC_TO_VK_EX));
		}
		break;
	}
	switch (vkCode)
	{
		case VK_LSHIFT:
			return lak::key_code::lshift;
		case VK_RSHIFT:
			return lak::key_code::rshift;
		case VK_LCONTROL:
			return lak::key_code::lctrl;
		case VK_RCONTROL:
			return lak::key_code::rctrl;
		case VK_LMENU:
			return lak::key_code::lalt;
		case VK_RMENU:
			return lak::key_code::ralt;
		case VK_LWIN:
			return lak::key_code::lsuper;
		case VK_RWIN:
			return lak::key_code::rsuper;
		// case VK_???:
		// 	return lak::key_code::menu;
		case VK_TAB:
			return lak::key_code::tab;
		case VK_LEFT:
			return lak::key_code::left;
		case VK_RIGHT:
			return lak::key_code::right;
		case VK_UP:
			return lak::key_code::up;
		case VK_DOWN:
			return lak::key_code::down;
		case VK_PRIOR:
			return lak::key_code::page_up;
		case VK_NEXT:
			return lak::key_code::page_down;
		case VK_HOME:
			return lak::key_code::home;
		case VK_END:
			return lak::key_code::end;
		case VK_INSERT:
			return lak::key_code::insert;
		case VK_DELETE:
			return lak::key_code::del;
		case VK_BACK:
			return lak::key_code::backspace;
		case VK_SPACE:
			return lak::key_code::space;
		case VK_RETURN:
			return lak::key_code::enter;
		case VK_ESCAPE:
			return lak::key_code::escape;
		default:
			return lak::key_code::none;
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
			lak::key_code key = get_key_code(msg.wParam, scancode);
			lak::mod_key mod  = get_mod_key_state(key, true);

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
			lak::key_code key = get_key_code(msg.wParam, scancode);
			lak::mod_key mod  = get_mod_key_state(key, true);

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

#ifdef LAK_ENABLE_SOFTRENDER
		if (handle.graphics_mode() == lak::graphics_mode::Software)
		{
			// we need to resize the pixel buffer
			// this is also touched in software create_window constructor
			handle.software_context().platform_handle.resize({(size_t)w, (size_t)h});
		}
#endif

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
						case 0x1:
							handle->_side = handle->left;
							break;
						case 0x2:
							handle->_side = handle->right;
							break;
						case 0x3:
							handle->_side = handle->top;
							break;
						case 0x4:
							handle->_side = handle->top | handle->left;
							break;
						case 0x5:
							handle->_side = handle->top | handle->right;
							break;
						case 0x6:
							handle->_side = handle->bottom;
							break;
						case 0x7:
							handle->_side = handle->bottom | handle->left;
							break;
						case 0x8:
							handle->_side = handle->bottom | handle->right;
							break;
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
		case WM_NCMOUSEHOVER:
			return handle_size_move(*handle);
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
