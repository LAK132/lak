#include "backend.inl"

#ifdef LAK_ENABLE_SOFTRENDER
#	include "backend_softrender.inl"
#endif
#ifdef LAK_ENABLE_OPENGL
#	include "backend_opengl.inl"
#endif
#ifdef LAK_ENABLE_COBALT
#	include "backend_cobalt.inl"
#endif

ImGui::ImplContext ImGui::ImplCreateContext(lak::graphics_mode mode)
{
	ImplContext result = new _ImplContext();
	result->mode       = mode;
	switch (mode)
	{
#ifdef LAK_ENABLE_SOFTRENDER
		case lak::graphics_mode::Software:
			result->sr_context = new _ImplSRContext();
			break;
#endif
#ifdef LAK_ENABLE_OPENGL
		case lak::graphics_mode::OpenGL:
			result->gl_context = new _ImplGLContext();
			break;
#endif
#ifdef LAK_ENABLE_COBALT
		case lak::graphics_mode::Cobalt:
			result->co_context = new _ImplCoContext();
			break;
#endif
		default: result->vd_context = nullptr; break;
	}
	result->imgui_context = ImGui::CreateContext();
	return result;
}

void ImGui::ImplDestroyContext(ImplContext context)
{
	if (context != nullptr)
	{
		if (context->imgui_context != nullptr)
			ImGui::DestroyContext(context->imgui_context);

		if (context->vd_context != nullptr)
		{
			switch (context->mode)
			{
#ifdef LAK_ENABLE_SOFTRENDER
				case lak::graphics_mode::Software: delete context->sr_context; break;
#endif
#ifdef LAK_ENABLE_OPENGL
				case lak::graphics_mode::OpenGL: delete context->gl_context; break;
#endif
#ifdef LAK_ENABLE_COBALT
				case lak::graphics_mode::Cobalt: delete context->co_context; break;
#endif
				default: FATAL("Invalid graphics mode"); break;
			}
		}
		delete context;
	}
}

inline void ImplUpdateDisplaySize(ImGui::ImplContext context,
                                  const lak::window_handle *handle)
{
	ImGuiIO &io = ImGui::GetIO();

	auto window_size = lak::window_drawable_size(handle);
	// auto window_size = lak::window_size(handle);
	io.DisplaySize.x = static_cast<float>(window_size.x);
	io.DisplaySize.y = static_cast<float>(window_size.y);

	switch (context->mode)
	{
#ifdef LAK_ENABLE_SOFTRENDER
		case lak::graphics_mode::Software:
			ImplUpdateDisplaySize(context->sr_context, handle, window_size);
			break;
#endif

#ifdef LAK_ENABLE_OPENGL
		case lak::graphics_mode::OpenGL:
		{
			auto drawable_size = lak::window_drawable_size(handle);
			io.DisplayFramebufferScale.x =
			  (window_size.x > 0) ? (drawable_size.x / (float)window_size.x) : 1.0f;
			io.DisplayFramebufferScale.y =
			  (window_size.y > 0) ? (drawable_size.y / (float)window_size.y) : 1.0f;
		}
		break;
#endif

#ifdef LAK_ENABLE_COBALT
		case lak::graphics_mode::Cobalt:
		{
			auto drawable_size = lak::window_drawable_size(handle);
			io.DisplayFramebufferScale.x =
			  (window_size.x > 0) ? (drawable_size.x / (float)window_size.x) : 1.0f;
			io.DisplayFramebufferScale.y =
			  (window_size.y > 0) ? (drawable_size.y / (float)window_size.y) : 1.0f;
		}
		break;
#endif

		default: FATAL("Invalid Context Mode"); break;
	}
}

void ImGui::ImplInit()
{
	ImGuiIO &io = ImGui::GetIO();

	io.BackendRendererName = "imgui_impl_lak";

#if defined(LAK_USE_WINAPI)
	io.BackendPlatformName = "imgui_impl_lak_win32";
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
	io.Fonts->Flags |= ImFontAtlasFlags_NoMouseCursors;
#elif defined(LAK_USE_XLIB)
#	error "NYI"
	io.BackendPlatformName = "imgui_impl_lak_xlib";
#elif defined(LAK_USE_XCB)
#	error "NYI"
	io.BackendPlatformName = "imgui_impl_lak_xcb";
#elif defined(LAK_USE_SDL)
	io.BackendPlatformName = "imgui_impl_lak_sdl2";
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
	io.Fonts->Flags |= ImFontAtlasFlags_NoMouseCursors;
#else
#	error "No implementation specified"
#endif

	ImGuiPlatformIO &pio            = ImGui::GetPlatformIO();
	pio.Platform_SetClipboardTextFn = ImplSetClipboard;
	pio.Platform_GetClipboardTextFn = ImplGetClipboard;
	pio.Platform_ClipboardUserData  = nullptr;
}

void ImGui::ImplInitContext(ImplContext context, const lak::window &window)
{
#if defined(LAK_USE_WINAPI)
	context->mouse_cursors[ImGuiMouseCursor_Arrow].platform_handle =
	  LoadCursorW(NULL, IDC_ARROW);
	context->mouse_cursors[ImGuiMouseCursor_TextInput].platform_handle =
	  LoadCursorW(NULL, IDC_IBEAM);
	context->mouse_cursors[ImGuiMouseCursor_ResizeAll].platform_handle =
	  LoadCursorW(NULL, IDC_SIZEALL);
	context->mouse_cursors[ImGuiMouseCursor_ResizeNS].platform_handle =
	  LoadCursorW(NULL, IDC_SIZENS);
	context->mouse_cursors[ImGuiMouseCursor_ResizeEW].platform_handle =
	  LoadCursorW(NULL, IDC_SIZEWE);
	context->mouse_cursors[ImGuiMouseCursor_ResizeNESW].platform_handle =
	  LoadCursorW(NULL, IDC_SIZENESW);
	context->mouse_cursors[ImGuiMouseCursor_ResizeNWSE].platform_handle =
	  LoadCursorW(NULL, IDC_SIZENWSE);
	context->mouse_cursors[ImGuiMouseCursor_Hand].platform_handle =
	  LoadCursorW(NULL, IDC_HAND);
	context->mouse_cursors[ImGuiMouseCursor_Wait].platform_handle =
	  LoadCursorW(NULL, IDC_WAIT);
	context->mouse_cursors[ImGuiMouseCursor_Progress].platform_handle =
	  LoadCursorW(NULL, IDC_APPSTARTING);
	context->mouse_cursors[ImGuiMouseCursor_NotAllowed].platform_handle =
	  LoadCursorW(NULL, IDC_NO);
#elif defined(LAK_USE_XLIB)
#	error "NYI"
#elif defined(LAK_USE_XCB)
#	error "NYI"
#elif defined(LAK_USE_SDL)
	context->mouse_cursors[ImGuiMouseCursor_Arrow].platform_handle =
	  SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
	context->mouse_cursors[ImGuiMouseCursor_TextInput].platform_handle =
	  SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
	context->mouse_cursors[ImGuiMouseCursor_ResizeAll].platform_handle =
	  SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEALL);
	context->mouse_cursors[ImGuiMouseCursor_ResizeNS].platform_handle =
	  SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENS);
	context->mouse_cursors[ImGuiMouseCursor_ResizeEW].platform_handle =
	  SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZEWE);
	context->mouse_cursors[ImGuiMouseCursor_ResizeNESW].platform_handle =
	  SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENESW);
	context->mouse_cursors[ImGuiMouseCursor_ResizeNWSE].platform_handle =
	  SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_SIZENWSE);
	context->mouse_cursors[ImGuiMouseCursor_Hand].platform_handle =
	  SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
	context->mouse_cursors[ImGuiMouseCursor_Wait].platform_handle =
	  SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAIT);
	context->mouse_cursors[ImGuiMouseCursor_Progress].platform_handle =
	  SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_WAITARROW);
	context->mouse_cursors[ImGuiMouseCursor_NotAllowed].platform_handle =
	  SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_NO);
#else
#	error "No implementation specified"
#endif

	ImGuiIO &io = ImGui::GetIO();

	io.BackendPlatformUserData = context;

	switch (context->mode)
	{
#ifdef LAK_ENABLE_SOFTRENDER
		case lak::graphics_mode::Software:
			io.BackendRendererUserData = context->sr_context;
			ImplInitSRContext(context->sr_context, window);
			break;
#endif
#ifdef LAK_ENABLE_OPENGL
		case lak::graphics_mode::OpenGL:
			io.BackendRendererUserData = context->gl_context;
			ImplInitGLContext(context->gl_context, window);
			break;
#endif
#ifdef LAK_ENABLE_COBALT
		case lak::graphics_mode::Cobalt:
			ImplInitCoContext(context->co_context, window);
			break;
#endif
		default: ASSERTF(false, "Invalid Context Mode"); break;
	}

	ImplUpdateDisplaySize(context, window.handle());

#ifdef LAK_OS_WINDOWS
#	if defined(LAK_USE_WINAPI)
	ImGui::GetMainViewport()->PlatformHandleRaw =
	  window.handle()->_platform_handle;
#	elif defined(LAK_USE_XLIB)
#		error "NYI"
#	elif defined(LAK_USE_XCB)
#		error "NYI"
#	elif defined(LAK_USE_SDL)
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(window.handle()->sdl_window, &wmInfo);
	ImGui::GetMainViewport()->PlatformHandleRaw = wmInfo.info.win.window;
#	else
#		error "No implementation specified"
#	endif
#endif
}

void ImGui::ImplShutdownContext(ImplContext context)
{
	for (auto &cursor : context->mouse_cursors)
	{
#if defined(LAK_USE_WINAPI)
		cursor.platform_handle = NULL;
#elif defined(LAK_USE_XLIB)
#	error "NYI"
#elif defined(LAK_USE_XCB)
#	error "NYI"
#elif defined(LAK_USE_SDL)
		SDL_FreeCursor(cursor.platform_handle);
		cursor.platform_handle = nullptr;
#else
#	error "No implementation specified"
#endif
	}

	switch (context->mode)
	{
#ifdef LAK_ENABLE_SOFTRENDER
		case lak::graphics_mode::Software:
			ImplShutdownSRContext(context->sr_context);
			break;
#endif
#ifdef LAK_ENABLE_OPENGL
		case lak::graphics_mode::OpenGL:
			ImplShutdownGLContext(context->gl_context);
			break;
#endif
#ifdef LAK_ENABLE_COBALT
		case lak::graphics_mode::Cobalt:
			ImplShutdownCoContext(context->co_context);
			break;
#endif
		default: FATAL("Invalid Context Mode"); break;
	}

	context->imgui_context->IO.BackendPlatformUserData = nullptr;
	context->imgui_context->IO.BackendRendererUserData = nullptr;
}

void ImGui::ImplSetCurrentContext(ImplContext context)
{
	ImGui::SetCurrentContext(context->imgui_context);
}

void ImGui::ImplSetTransform(ImplContext context, const glm::mat4x4 &transform)
{
	context->transform = transform;
}

void ImGui::ImplNewFrame(ImplContext context,
                         const lak::window &window,
                         const float delta_time,
                         const bool call_base_new_frame)
{
	ImGuiIO &io = ImGui::GetIO();

	ASSERT(delta_time > 0);
	io.DeltaTime = delta_time;

	// UpdateMousePosAndButtons()
	if (io.WantSetMousePos)
	{
		// ImGui enforces mouse position
		window.set_cursor_pos({(long)io.MousePos.x, (long)io.MousePos.y});
	}

	// UpdateMouseCursor()
	if (context->allow_set_cursor &&
	    (io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0)
	{
		ImGuiMouseCursor cursor = ImGui::GetMouseCursor();

		if (io.MouseDrawCursor || (cursor == ImGuiMouseCursor_None))
		{
#if defined(LAK_USE_WINAPI)
			SetCursor(NULL);
#elif defined(LAK_USE_XLIB)
#	error "NYI"
#elif defined(LAK_USE_XCB)
#	error "NYI"
#elif defined(LAK_USE_SDL)
			SDL_ShowCursor(SDL_FALSE);
#else
#	error "No implementation specified"
#endif
		}
		else
		{
#if defined(LAK_USE_WINAPI)
			SetCursor(context->mouse_cursors[cursor].platform_handle);
#elif defined(LAK_USE_XLIB)
#	error "NYI"
#elif defined(LAK_USE_XCB)
#	error "NYI"
#elif defined(LAK_USE_SDL)
			SDL_SetCursor(context->mouse_cursors[cursor].platform_handle);
			SDL_ShowCursor(SDL_TRUE);
#else
#	error "No implementation specified"
#endif
		}
	}

	if (call_base_new_frame) ImGui::NewFrame();
}

bool ImGui::ImplProcessEvent(ImplContext context, const lak::event &event)
{
	ImGuiIO &io = ImGui::GetIO();

	switch (event.type)
	{
		case lak::event_type::window_changed:
		{
			if (event.handle)
			{
				ImplUpdateDisplaySize(context, event.handle);
				return true;
			}
			else
			{
				WARNING("No window handle attached to event");
				return false;
			}
		}

		case lak::event_type::wheel:
		{
			io.AddMouseWheelEvent(-event.wheel().wheel.x, event.wheel().wheel.y);
			return true;
		}

		case lak::event_type::window_focus:
		{
			io.AddFocusEvent(true);
			return true;
		}

		case lak::event_type::window_no_focus:
		{
			io.AddFocusEvent(false);
			return true;
		}

		case lak::event_type::window_hover:
		{
			context->allow_set_cursor = true;
			return true;
		}
		break;

		case lak::event_type::window_leave:
		{
			context->allow_set_cursor = false;
			if (io.WantSetMousePos) return false;
			io.ClearInputMouse();
			io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
			io.ClearInputKeys();
			return true;
		}

		case lak::event_type::motion:
		{
			if (io.WantSetMousePos) return false;
			io.AddMousePosEvent((float)event.motion().position.x,
			                    (float)event.motion().position.y);
			return true;
		}

		case lak::event_type::button_down:
		{
			switch (event.button().button)
			{
				case lak::mouse_button::left:
					io.AddMouseButtonEvent(ImGuiMouseButton_Left, true);
					break;
				case lak::mouse_button::right:
					io.AddMouseButtonEvent(ImGuiMouseButton_Right, true);
					break;
				case lak::mouse_button::middle:
					io.AddMouseButtonEvent(ImGuiMouseButton_Middle, true);
					break;
				default: return false;
			}
#if defined(LAK_USE_WINAPI)
			SetCapture(event.handle->_platform_handle);
#elif defined(LAK_USE_XLIB)
#	error "NYI"
#elif defined(LAK_USE_XCB)
#	error "NYI"
#elif defined(LAK_USE_SDL)
			SDL_CaptureMouse(SDL_TRUE);
#else
#	error "No implementation specified"
#endif
			return true;
		}

		case lak::event_type::button_up:
		{
			switch (event.button().button)
			{
				case lak::mouse_button::left:
					io.AddMouseButtonEvent(ImGuiMouseButton_Left, false);
					break;
				case lak::mouse_button::right:
					io.AddMouseButtonEvent(ImGuiMouseButton_Right, false);
					break;
				case lak::mouse_button::middle:
					io.AddMouseButtonEvent(ImGuiMouseButton_Middle, false);
					break;
				default: return false;
			}
#if defined(LAK_USE_WINAPI)
			ReleaseCapture();
#elif defined(LAK_USE_XLIB)
#	error "NYI"
#elif defined(LAK_USE_XCB)
#	error "NYI"
#elif defined(LAK_USE_SDL)
			SDL_CaptureMouse(SDL_FALSE);
#else
#	error "No implementation specified"
#endif
			return true;
		}

		case lak::event_type::key_down: [[fallthrough]];
		case lak::event_type::key_up:
		{
			lak::key_code key  = event.key().key;
			lak::mod_key mod   = event.key().mod;
			const int scancode = event.key().native_scancode;
			const bool down    = event.type == lak::event_type::key_down;

			auto lak_to_imgui = [&event](lak::key_code key) -> ImGuiKey
			{
				switch (key)
				{
					case lak::key_code::tab:             return ImGuiKey_Tab;
					case lak::key_code::left:            return ImGuiKey_LeftArrow;
					case lak::key_code::right:           return ImGuiKey_RightArrow;
					case lak::key_code::up:              return ImGuiKey_UpArrow;
					case lak::key_code::down:            return ImGuiKey_DownArrow;
					case lak::key_code::page_up:         return ImGuiKey_PageUp;
					case lak::key_code::page_down:       return ImGuiKey_PageDown;
					case lak::key_code::home:            return ImGuiKey_Home;
					case lak::key_code::end:             return ImGuiKey_End;
					case lak::key_code::insert:          return ImGuiKey_Insert;
					case lak::key_code::del:             return ImGuiKey_Delete;
					case lak::key_code::backspace:       return ImGuiKey_Backspace;
					case lak::key_code::space:           return ImGuiKey_Space;
					case lak::key_code::enter:           return ImGuiKey_Enter;
					case lak::key_code::escape:          return ImGuiKey_Escape;
					case lak::key_code::lctrl:           return ImGuiKey_LeftCtrl;
					case lak::key_code::lshift:          return ImGuiKey_LeftShift;
					case lak::key_code::lalt:            return ImGuiKey_LeftAlt;
					case lak::key_code::lsuper:          return ImGuiKey_LeftSuper;
					case lak::key_code::rctrl:           return ImGuiKey_RightCtrl;
					case lak::key_code::rshift:          return ImGuiKey_RightShift;
					case lak::key_code::ralt:            return ImGuiKey_RightAlt;
					case lak::key_code::rsuper:          return ImGuiKey_RightSuper;
					case lak::key_code::menu:            return ImGuiKey_Menu;
					case lak::key_code::num_0:           return ImGuiKey_0;
					case lak::key_code::num_1:           return ImGuiKey_1;
					case lak::key_code::num_2:           return ImGuiKey_2;
					case lak::key_code::num_3:           return ImGuiKey_3;
					case lak::key_code::num_4:           return ImGuiKey_4;
					case lak::key_code::num_5:           return ImGuiKey_5;
					case lak::key_code::num_6:           return ImGuiKey_6;
					case lak::key_code::num_7:           return ImGuiKey_7;
					case lak::key_code::num_8:           return ImGuiKey_8;
					case lak::key_code::num_9:           return ImGuiKey_9;
					case lak::key_code::let_A:           return ImGuiKey_A;
					case lak::key_code::let_B:           return ImGuiKey_B;
					case lak::key_code::let_C:           return ImGuiKey_C;
					case lak::key_code::let_D:           return ImGuiKey_D;
					case lak::key_code::let_E:           return ImGuiKey_E;
					case lak::key_code::let_F:           return ImGuiKey_F;
					case lak::key_code::let_G:           return ImGuiKey_G;
					case lak::key_code::let_H:           return ImGuiKey_H;
					case lak::key_code::let_I:           return ImGuiKey_I;
					case lak::key_code::let_J:           return ImGuiKey_J;
					case lak::key_code::let_K:           return ImGuiKey_K;
					case lak::key_code::let_L:           return ImGuiKey_L;
					case lak::key_code::let_M:           return ImGuiKey_M;
					case lak::key_code::let_N:           return ImGuiKey_N;
					case lak::key_code::let_O:           return ImGuiKey_O;
					case lak::key_code::let_P:           return ImGuiKey_P;
					case lak::key_code::let_Q:           return ImGuiKey_Q;
					case lak::key_code::let_R:           return ImGuiKey_R;
					case lak::key_code::let_S:           return ImGuiKey_S;
					case lak::key_code::let_T:           return ImGuiKey_T;
					case lak::key_code::let_U:           return ImGuiKey_U;
					case lak::key_code::let_V:           return ImGuiKey_V;
					case lak::key_code::let_W:           return ImGuiKey_W;
					case lak::key_code::let_X:           return ImGuiKey_X;
					case lak::key_code::let_Y:           return ImGuiKey_Y;
					case lak::key_code::let_Z:           return ImGuiKey_Z;
					case lak::key_code::f1:              return ImGuiKey_F1;
					case lak::key_code::f2:              return ImGuiKey_F2;
					case lak::key_code::f3:              return ImGuiKey_F3;
					case lak::key_code::f4:              return ImGuiKey_F4;
					case lak::key_code::f5:              return ImGuiKey_F5;
					case lak::key_code::f6:              return ImGuiKey_F6;
					case lak::key_code::f7:              return ImGuiKey_F7;
					case lak::key_code::f8:              return ImGuiKey_F8;
					case lak::key_code::f9:              return ImGuiKey_F9;
					case lak::key_code::f10:             return ImGuiKey_F10;
					case lak::key_code::f11:             return ImGuiKey_F11;
					case lak::key_code::f12:             return ImGuiKey_F12;
					case lak::key_code::f13:             return ImGuiKey_F13;
					case lak::key_code::f14:             return ImGuiKey_F14;
					case lak::key_code::f15:             return ImGuiKey_F15;
					case lak::key_code::f16:             return ImGuiKey_F16;
					case lak::key_code::f17:             return ImGuiKey_F17;
					case lak::key_code::f18:             return ImGuiKey_F18;
					case lak::key_code::f19:             return ImGuiKey_F19;
					case lak::key_code::f20:             return ImGuiKey_F20;
					case lak::key_code::f21:             return ImGuiKey_F21;
					case lak::key_code::f22:             return ImGuiKey_F22;
					case lak::key_code::f23:             return ImGuiKey_F23;
					case lak::key_code::f24:             return ImGuiKey_F24;
					case lak::key_code::apostrophe:      return ImGuiKey_Apostrophe;
					case lak::key_code::comma:           return ImGuiKey_Comma;
					case lak::key_code::minus:           return ImGuiKey_Minus;
					case lak::key_code::period:          return ImGuiKey_Period;
					case lak::key_code::slash:           return ImGuiKey_Slash;
					case lak::key_code::semicolon:       return ImGuiKey_Semicolon;
					case lak::key_code::equal:           return ImGuiKey_Equal;
					case lak::key_code::open_bracket:    return ImGuiKey_LeftBracket;
					case lak::key_code::backslash:       return ImGuiKey_Backslash;
					case lak::key_code::close_bracket:   return ImGuiKey_RightBracket;
					case lak::key_code::backtick:        return ImGuiKey_GraveAccent;
					case lak::key_code::caps_lock:       return ImGuiKey_CapsLock;
					case lak::key_code::scroll_lock:     return ImGuiKey_ScrollLock;
					case lak::key_code::num_lock:        return ImGuiKey_NumLock;
					case lak::key_code::print_screen:    return ImGuiKey_PrintScreen;
					case lak::key_code::pause:           return ImGuiKey_Pause;
					case lak::key_code::keypad_0:        return ImGuiKey_Keypad0;
					case lak::key_code::keypad_1:        return ImGuiKey_Keypad1;
					case lak::key_code::keypad_2:        return ImGuiKey_Keypad2;
					case lak::key_code::keypad_3:        return ImGuiKey_Keypad3;
					case lak::key_code::keypad_4:        return ImGuiKey_Keypad4;
					case lak::key_code::keypad_5:        return ImGuiKey_Keypad5;
					case lak::key_code::keypad_6:        return ImGuiKey_Keypad6;
					case lak::key_code::keypad_7:        return ImGuiKey_Keypad7;
					case lak::key_code::keypad_8:        return ImGuiKey_Keypad8;
					case lak::key_code::keypad_9:        return ImGuiKey_Keypad9;
					case lak::key_code::keypad_decimal:  return ImGuiKey_KeypadDecimal;
					case lak::key_code::keypad_divide:   return ImGuiKey_KeypadDivide;
					case lak::key_code::keypad_multiply: return ImGuiKey_KeypadMultiply;
					case lak::key_code::keypad_subtract: return ImGuiKey_KeypadSubtract;
					case lak::key_code::keypad_add:      return ImGuiKey_KeypadAdd;
					case lak::key_code::keypad_enter:    return ImGuiKey_KeypadEnter;
					case lak::key_code::keypad_equal:    return ImGuiKey_KeypadEqual;
					case lak::key_code::back:            return ImGuiKey_AppBack;
					case lak::key_code::forward:         return ImGuiKey_AppForward;
					case lak::key_code::oem102:          return ImGuiKey_Oem102;
					default:                             break;
				}

				return ImGuiKey_None;
			};

			io.AddKeyEvent(ImGuiMod_Ctrl,
			               (mod & lak::mod_key::ctrl) != lak::mod_key::none);
			io.AddKeyEvent(ImGuiMod_Shift,
			               (mod & lak::mod_key::shift) != lak::mod_key::none);
			io.AddKeyEvent(ImGuiMod_Alt,
			               (mod & lak::mod_key::alt) != lak::mod_key::none);
			io.AddKeyEvent(ImGuiMod_Super,
			               (mod & lak::mod_key::super) != lak::mod_key::none);

			if (auto imkey = lak_to_imgui(key); imkey != ImGuiKey_None)
				io.AddKeyEvent(imkey, down);

			return true;
		}

		default: break;
	}

#if defined(LAK_USE_WINAPI)
	if (event._platform_event->msg.message == WM_CHAR)
	{
		io.AddInputCharacter((unsigned int)event._platform_event->msg.wParam);
		return true;
	}
#elif defined(LAK_USE_XLIB)
#	error "NYI"
#elif defined(LAK_USE_XCB)
#	error "NYI"
#elif defined(LAK_USE_SDL)
	if (event._platform_event->sdl_event.type == SDL_TEXTINPUT)
	{
		io.AddInputCharactersUTF8(event._platform_event->sdl_event.text.text);
		return true;
	}
#else
#	error "No implementation specified"
#endif

	return false;
}

ImTextureRef ImGui::ImplCreateTexture(ImGui::ImplContext context,
                                      const void *pixels,
                                      lak::vec2s_t size,
                                      ImGui::ImplTextureColourFormat colour,
                                      ImGui::ImplTextureChannelFormat channel)
{
	ASSERT(context);
	switch (context->mode)
	{
#ifdef LAK_ENABLE_SOFTRENDER
		case lak::graphics_mode::Software:
			return ImplSRCreateTexture(context, pixels, size, colour, channel);
			break;
#endif
#ifdef LAK_ENABLE_OPENGL
		case lak::graphics_mode::OpenGL:
			return ImplGLCreateTexture(context, pixels, size, colour, channel);
			break;
#endif
#ifdef LAK_ENABLE_COBALT
		case lak::graphics_mode::Cobalt:
			return ImplCoCreateTexture(context, pixels, size, colour, channel);
			break;
#endif
		default: FATAL("Invalid context mode"); break;
	}
}

ImTextureRef ImGui::ImplUpdateTexture(ImplContext context,
                                      ImTextureRef tex,
                                      const void *pixels,
                                      lak::vec2s_t size,
                                      ImplTextureColourFormat colour,
                                      ImplTextureChannelFormat channel,
                                      lak::span<const ImTextureRect> updates)
{
	ASSERT(context);
	switch (context->mode)
	{
#ifdef LAK_ENABLE_SOFTRENDER
		case lak::graphics_mode::Software:
			return ImplSRUpdateTexture(
			  context, tex.GetTexID(), pixels, size, colour, channel, updates);
			break;
#endif
#ifdef LAK_ENABLE_OPENGL
		case lak::graphics_mode::OpenGL:
			return ImplGLUpdateTexture(
			  context, tex.GetTexID(), pixels, size, colour, channel, updates);
			break;
#endif
#ifdef LAK_ENABLE_COBALT
		case lak::graphics_mode::Cobalt:
			return ImplCoUpdateTexture(
			  context, tex.GetTexID(), pixels, size, colour, channel, updates);
			break;
#endif
		default: FATAL("Invalid context mode"); break;
	}
}

void ImGui::ImplDestroyTexture(ImplContext context, ImTextureRef tex)
{
	ASSERT(context);
	switch (context->mode)
	{
#ifdef LAK_ENABLE_SOFTRENDER
		case lak::graphics_mode::Software:
			ImplSRDestroyTexture(context, tex.GetTexID());
			break;
#endif
#ifdef LAK_ENABLE_OPENGL
		case lak::graphics_mode::OpenGL:
			ImplGLDestroyTexture(context, tex.GetTexID());
			break;
#endif
#ifdef LAK_ENABLE_COBALT
		case lak::graphics_mode::Cobalt:
			ImplCoDestroyTexture(context, tex.GetTexID());
			break;
#endif
		default: FATAL("Invalid context mode"); break;
	}
}

lak::vec2s_t ImGui::ImplTextureSize(ImplContext context, ImTextureRef tex)
{
	ASSERT(context);
	switch (context->mode)
	{
#ifdef LAK_ENABLE_SOFTRENDER
		case lak::graphics_mode::Software:
			return ImplSRTextureSize(context, tex.GetTexID());
			break;
#endif
#ifdef LAK_ENABLE_OPENGL
		case lak::graphics_mode::OpenGL:
			return ImplGLTextureSize(context, tex.GetTexID());
			break;
#endif
#ifdef LAK_ENABLE_COBALT
		case lak::graphics_mode::Cobalt:
			return ImplCoTextureSize(context, tex.GetTexID());
			break;
#endif
		default: FATAL("Invalid context mode"); break;
	}
}

void ImGui::ImplUpdateTexture(ImplContext context, ImTextureData *texture)
{
	ImGui::ImplTextureColourFormat colour;
	ImGui::ImplTextureChannelFormat channel;

	switch (texture->Format)
	{
		case ImTextureFormat_RGBA32:
			colour  = ImGui::ImplTextureColourFormat::RGBA;
			channel = ImGui::ImplTextureChannelFormat::U8;
			break;
		case ImTextureFormat_Alpha8:
			colour  = ImGui::ImplTextureColourFormat::A;
			channel = ImGui::ImplTextureChannelFormat::U8;
			break;
		default: ASSERT_UNREACHABLE();
	}

	if (texture->Status == ImTextureStatus_WantCreate)
	{
		texture->SetTexID(ImGui::ImplCreateTexture(
		                    context,
		                    texture->GetPixels(),
		                    {size_t(texture->Width), size_t(texture->Height)},
		                    colour,
		                    channel)
		                    .GetTexID());

		texture->SetStatus(ImTextureStatus_OK);
	}
	else if (texture->Status == ImTextureStatus_WantUpdates)
	{
		texture->SetTexID(ImGui::ImplUpdateTexture(
		                    context,
		                    texture->GetTexRef(),
		                    texture->GetPixels(),
		                    {size_t(texture->Width), size_t(texture->Height)},
		                    colour,
		                    channel,
		                    ImGui::ToSpan(texture->Updates))
		                    .GetTexID());

		texture->SetStatus(ImTextureStatus_OK);
	}
	else if (texture->Status == ImTextureStatus_WantDestroy &&
	         texture->UnusedFrames > 0)
	{
		ImGui::ImplDestroyTexture(context, texture->GetTexRef());

		texture->SetStatus(ImTextureStatus_Destroyed);
	}
}

void ImGui::ImplRender(ImplContext context, const bool call_base_render)
{
	if (call_base_render) Render();
	ImplRenderData(context, ImGui::GetDrawData());
}

void ImGui::ImplRenderData(ImplContext context, ImDrawData *draw_data)
{
	ASSERT(context);
	ASSERT(draw_data);
	switch (context->mode)
	{
#ifdef LAK_ENABLE_SOFTRENDER
		case lak::graphics_mode::Software: ImplSRRender(context, draw_data); break;
#endif
#ifdef LAK_ENABLE_OPENGL
		case lak::graphics_mode::OpenGL: ImplGLRender(context, draw_data); break;
#endif
#ifdef LAK_ENABLE_COBALT
		case lak::graphics_mode::Cobalt: ImplCoRender(context, draw_data); break;
#endif
		default: FATAL("Invalid context mode"); break;
	}
}

void ImGui::ImplSetClipboard(ImGuiContext *, const char *text)
{
	lak::set_clipboard(text);
}

const char *ImGui::ImplGetClipboard(ImGuiContext *ctx)
{
	lak::u8string *user_data = reinterpret_cast<lak::u8string *>(
	  ctx->PlatformIO.Platform_ClipboardUserData);
	if (!user_data) user_data = new lak::u8string;
	lak::get_clipboard(user_data);
	return (const char *)user_data->c_str();
}
