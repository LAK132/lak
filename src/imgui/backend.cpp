#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS
#define IMGUI_DISABLE_OBSOLETE_KEYIO

#define IMGUI_DEFINE_MATH_OPERATORS

#include "lak/system/os.hpp"
#include "lak/system/windowing/events.hpp"
#include "lak/system/windowing/window.hpp"

#include "lak/debug.hpp"
#include "lak/defer.hpp"
#include "lak/image.hpp"
#include "lak/trace.hpp"

#ifdef LAK_ENABLE_OPENGL
#	include "lak/system/opengl/shader.hpp"
#	include "lak/system/opengl/state.hpp"
#	include "lak/system/opengl/texture.hpp"
#endif

#include "lak/imgui/backend.hpp"

#if defined(LAK_USE_WINAPI)
#	include "../system/windowing/win32/impl.hpp"
#	include "lak/system/win32/windows.hpp"
#elif defined(LAK_USE_XLIB)
#	error "NYI"
#elif defined(LAK_USE_XCB)
#	error "NYI"
#elif defined(LAK_USE_SDL)
#	include "../system/windowing/sdl/impl.hpp"
#	include <SDL.h>
#	ifdef LAK_OS_WINDOWS
#		include <SDL_syswm.h>
#	endif
#else
#	error "No implementation specified"
#endif

#ifdef LAK_ENABLE_SOFTRENDER
#	include "lak/imgui/softrender.hpp"
#endif

#include <imgui_internal.h>
#include <misc/cpp/imgui_stdlib.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat3x4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <algorithm>
#include <cstdlib>
#include <cstring>

#ifdef LAK_ENABLE_OPENGL
auto _call_checked =
  []<typename RET, typename... ARGS, typename... ARGS2>(
    lak::trace trace, RET(APIENTRYP func)(ARGS...), ARGS2... args)
{
	return lak::opengl::call_checked(func, lak::forward<ARGS2>(args)...)
	  .TRACE_UNWRAP(trace);
};

#	define GL_DEFER_CALL(FUNC, ...)                                            \
		DEFER_CALL(_call_checked, lak::trace{}, FUNC, __VA_ARGS__)
#endif

#if !defined(LAK_SOFTWARE_RENDER_32BIT) &&                                    \
  !defined(LAK_SOFTWARE_RENDER_24BIT) &&                                      \
  !defined(LAK_SOFTWARE_RENDER_16BIT) && !defined(LAK_SOFTWARE_RENDER_8BIT)
#	if defined(LAK_OS_APPLE) && defined(LAK_ARCH_ARM64)
#		define LAK_SOFTWARE_RENDER_32BIT
#	else
#		define LAK_SOFTWARE_RENDER_16BIT
#	endif
#endif

namespace ImGui
{
	typedef struct _ImplSRContext
	{
#ifdef LAK_ENABLE_SOFTRENDER
#	if defined(LAK_USE_WINAPI)
		//     // HBITMAP bitmap_handle = NULL;
		// #  if defined(LAK_SOFTWARE_RENDER_32BIT)
		//     using screen_format_t = lak::colour::abgr8888;
		// #  elif defined(LAK_SOFTWARE_RENDER_24BIT)
		//     using screen_format_t = lak::colour::bgr888;
		// #  elif defined(LAK_SOFTWARE_RENDER_16BIT)
		//     using screen_format_t = lak::colour::bgr565;
		// #  else
		// #    error "No software render colour bit depth specified"
		// #  endif

		decltype(lak::software_context::platform_handle) *screen_surface = nullptr;
#	elif defined(LAK_USE_XLIB)
#		error "NYI"
#	elif defined(LAK_USE_XCB)
#		error "NYI"
#	elif defined(LAK_USE_SDL)
		SDL_Window *window;
		SDL_Surface *screen_surface;
#		if defined(LAK_SOFTWARE_RENDER_32BIT)
		static const Uint32 screen_format = SDL_PIXELFORMAT_ABGR8888;
#		elif defined(LAK_SOFTWARE_RENDER_24BIT)
		static const Uint32 screen_format = SDL_PIXELFORMAT_RGB24;
#		elif defined(LAK_SOFTWARE_RENDER_16BIT)
		static const Uint32 screen_format = SDL_PIXELFORMAT_RGB565;
#		elif defined(LAK_SOFTWARE_RENDER_8BIT)
		SDL_Palette *palette;
		static const Uint32 screen_format = SDL_PIXELFORMAT_INDEX8;
#		else
#			error "No software render colour bit depth specified"
#		endif
#	else
#		error "No implementation specified"
#	endif

		texture_alpha8_t atlas_texture;
#	if defined(LAK_SOFTWARE_RENDER_32BIT)
		texture_color32_t screen_texture;
#	elif defined(LAK_SOFTWARE_RENDER_24BIT)
		texture_color24_t screen_texture;
#	elif defined(LAK_SOFTWARE_RENDER_16BIT)
		texture_color16_t screen_texture;
#	elif defined(LAK_SOFTWARE_RENDER_8BIT)
		texture_value8_t screen_texture;
#	else
#		error "No software render colour bit depth specified"
#	endif
#endif
	} *ImplSRContext;

	typedef struct _ImplGLContext
	{
#ifdef LAK_ENABLE_OPENGL
		GLint attrib_tex;
		GLint attrib_view_proj;
		GLint attrib_pos;
		GLint attrib_UV;
		GLint attrib_col;
		GLuint elements;
		GLuint array_buffer;
		GLuint vertex_array;
		lak::opengl::program shader;
		lak::opengl::texture font;
#endif
	} *ImplGLContext;

	typedef struct _ImplContext
	{
		ImGuiContext *imgui_context;
		lak::cursor mouse_cursors[ImGuiMouseCursor_COUNT];
		lak::graphics_mode mode;
		glm::mat4x4 transform = glm::mat4x4(1.0f);
		union
		{
			void *vd_context;
			ImplSRContext sr_context;
			ImplGLContext gl_context;
		};
	} *ImplContext;
}

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
		default:
			result->vd_context = nullptr;
			break;
	}
	result->imgui_context = ImGui::CreateContext();
	return result;
}

void ImGui::ImplDestroyContext(ImplContext context)
{
	if (context != nullptr)
	{
		if (context->vd_context != nullptr)
		{
			switch (context->mode)
			{
#ifdef LAK_ENABLE_SOFTRENDER
				case lak::graphics_mode::Software:
					delete context->sr_context;
					break;
#endif
#ifdef LAK_ENABLE_OPENGL
				case lak::graphics_mode::OpenGL:
					delete context->gl_context;
					break;
#endif
				default:
					FATAL("Invalid graphics mode");
					break;
			}
		}
		delete context;
	}
}

#ifdef LAK_ENABLE_SOFTRENDER
inline void ImplUpdateDisplaySize(ImGui::ImplSRContext context,
                                  const lak::window_handle *,
                                  lak::vec2l_t window_size)
{
	ImGuiIO &io                  = ImGui::GetIO();
	io.DisplayFramebufferScale.x = 1.0f;
	io.DisplayFramebufferScale.y = 1.0f;
	if ((size_t)window_size.x != context->screen_texture.w ||
	    (size_t)window_size.y != context->screen_texture.h)
	{
		context->screen_texture.init(window_size.x, window_size.y);

#	if defined(LAK_USE_WINAPI)
		// context->screen_surface.resize(lak::vec2s_t(window_size));
#	elif defined(LAK_USE_XLIB)
#		error "NYI"
#	elif defined(LAK_USE_XCB)
#		error "NYI"
#	elif defined(LAK_USE_SDL)
		if (context->screen_surface != nullptr)
			SDL_FreeSurface(context->screen_surface);

		context->screen_surface = SDL_CreateRGBSurfaceWithFormatFrom(
		  context->screen_texture.pixels,
		  static_cast<int>(context->screen_texture.w),
		  static_cast<int>(context->screen_texture.h),
		  static_cast<int>(context->screen_texture.size * 8),
		  static_cast<int>(context->screen_texture.w *
		                   context->screen_texture.size),
		  context->screen_format);

#		ifdef LAK_SOFTWARE_RENDER_8BIT
		SDL_SetSurfacePalette(context->screen_surface, context->palette);
#		endif
#	else
#		error "No implementation specified"
#	endif
	}
}
#endif

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

		default:
			FATAL("Invalid Context Mode");
			break;
	}
}

void ImGui::ImplInit()
{
	ImGuiIO &io = ImGui::GetIO();

	io.BackendRendererName = "imgui_impl_lak";

#if defined(LAK_USE_WINAPI)
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
	io.BackendPlatformName = "imgui_impl_lak_win32";
#elif defined(LAK_USE_XLIB)
#	error "NYI"
	io.BackendPlatformName = "imgui_impl_lak_xlib";
#elif defined(LAK_USE_XCB)
#	error "NYI"
	io.BackendPlatformName = "imgui_impl_lak_xcb";
#elif defined(LAK_USE_SDL)
	io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
	io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
	io.BackendPlatformName = "imgui_impl_lak_sdl2";
#else
#	error "No implementation specified"
#endif

	ImGuiPlatformIO &pio            = ImGui::GetPlatformIO();
	pio.Platform_SetClipboardTextFn = ImplSetClipboard;
	pio.Platform_GetClipboardTextFn = ImplGetClipboard;
	pio.Platform_ClipboardUserData  = nullptr;
}

#ifdef LAK_ENABLE_SOFTRENDER
void ImplInitSRContext(ImGui::ImplSRContext context, const lak::window &window)
{
	ImGuiIO &io = ImGui::GetIO();

	io.BackendRendererName = "imgui_impl_lak_softrender";

#	if defined(LAK_USE_WINAPI)
	context->screen_surface =
	  &window.handle()->software_context().platform_handle;
#	elif defined(LAK_USE_XLIB)
#		error "NYI"
#	elif defined(LAK_USE_XCB)
#		error "NYI"
#	elif defined(LAK_USE_SDL)
	context->window = window.handle()->sdl_window;

#		ifdef LAK_SOFTWARE_RENDER_8BIT
	context->palette = SDL_AllocPalette(256);
	SDL_Colour palette[256];
	for (size_t i = 0; i < 256; ++i)
	{
		palette[i].r = uint8_t(i);
		palette[i].g = uint8_t(i);
		palette[i].b = uint8_t(i);
		palette[i].a = uint8_t(255);
	}
	SDL_SetPaletteColors(context->palette, palette, 0, 256);
#		endif

#	else
#		error "No implementation specified"
#	endif

	uint8_t *pixels;
	int width, height;
	io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);
	context->atlas_texture.init(width, height, (alpha8_t *)pixels);
	io.Fonts->TexID = (ImTextureID)(uintptr_t)&context->atlas_texture;

	ImplUpdateDisplaySize(context, window.handle(), window.size());

	ImGui_ImplSoftrender_Init(&context->screen_texture);
}
#endif

#ifdef LAK_ENABLE_OPENGL
void ImplInitGLContext(ImGui::ImplGLContext context, const lak::window &)
{
	using namespace lak::opengl::literals;

	context->shader = lak::opengl::program::create(
	                    R"(#version 150
uniform mat4 viewProj;
in vec2 vPosition;
in vec2 vUV;
in vec4 vColour;
out vec2 fUV;
out vec4 fColour;
void main()
{
	fUV = vUV;
	fColour = vColour;
	gl_Position = viewProj * vec4(vPosition.xy, 0, 1);
})"_vertex_shader.UNWRAP(),
	                    R"(#version 150
uniform sampler2D fTexture;
in vec2 fUV;
in vec4 fColour;
out vec4 pColour;
void main()
{
	pColour = fColour * texture(fTexture, fUV.st);
})"_fragment_shader.UNWRAP())
	                    .UNWRAP();

	context->attrib_tex       = *context->shader.uniform_location("fTexture");
	context->attrib_view_proj = *context->shader.uniform_location("viewProj");
	context->attrib_pos       = *context->shader.attrib_location("vPosition");
	context->attrib_UV        = *context->shader.attrib_location("vUV");
	context->attrib_col       = *context->shader.attrib_location("vColour");

	lak::opengl::call_checked(glGenBuffers, 1, &context->array_buffer).UNWRAP();
	lak::opengl::call_checked(glGenBuffers, 1, &context->elements).UNWRAP();

	ImGuiIO &io = ImGui::GetIO();

	io.BackendRendererName = "imgui_impl_lak_opengl";

	// Create fonts texture
	uint8_t *pixels;
	lak::vec2i_t size;
	io.Fonts->GetTexDataAsRGBA32(&pixels, &size.x, &size.y);

	auto old_texture = lak::opengl::get_uint(GL_TEXTURE_BINDING_2D).UNWRAP();
	DEFER(glBindTexture(GL_TEXTURE_2D, old_texture));

	context->font.init(GL_TEXTURE_2D)
	  .bind()
	  .apply(GL_TEXTURE_MIN_FILTER, GL_LINEAR)
	  .apply(GL_TEXTURE_MAG_FILTER, GL_NEAREST)
	  .store_mode(GL_UNPACK_ROW_LENGTH, 0)
	  .build(0, GL_RGBA, size, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

	io.Fonts->TexID = (ImTextureID)(uintptr_t)context->font.get();
}
#endif

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
#else
#	error "No implementation specified"
#endif

	switch (context->mode)
	{
#ifdef LAK_ENABLE_SOFTRENDER
		case lak::graphics_mode::Software:
			ImplInitSRContext(context->sr_context, window);
			break;
#endif
#ifdef LAK_ENABLE_OPENGL
		case lak::graphics_mode::OpenGL:
			ImplInitGLContext(context->gl_context, window);
			break;
#endif
		default:
			ASSERTF(false, "Invalid Context Mode");
			break;
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

#ifdef LAK_ENABLE_SOFTRENDER
void ImplShutdownSRContext(ImGui::ImplSRContext context)
{
	ImGui_ImplSoftrender_Shutdown();

#	if defined(LAK_USE_WINAPI)
	context->screen_surface = nullptr;
#	elif defined(LAK_USE_XLIB)
#		error "NYI"
#	elif defined(LAK_USE_XCB)
#		error "NYI"
#	elif defined(LAK_USE_SDL)
	context->window = nullptr;

	SDL_FreeSurface(context->screen_surface);
	context->screen_surface = nullptr;

#		ifdef LAK_SOFTWARE_RENDER_8BIT
	SDL_FreePalette(context->palette);
	context->palette = nullptr;
#		endif
#	else
#		error "No implementation specified"
#	endif

	context->screen_texture.init(0, 0);
	context->atlas_texture.init(0, 0);
}
#endif

#ifdef LAK_ENABLE_OPENGL
void ImplShutdownGLContext(ImGui::ImplGLContext context)
{
	if (context->array_buffer) glDeleteBuffers(1, &context->array_buffer);
	context->array_buffer = 0;

	if (context->elements) glDeleteBuffers(1, &context->elements);
	context->elements = 0;

	context->shader.clear().discard();

	context->font.clear();
	ImGui::GetIO().Fonts->TexID = (ImTextureID)(intptr_t)0;
}
#endif

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
		default:
			ASSERTF(false, "Invalid Context Mode");
			break;
	}

	if (context->imgui_context != nullptr)
	{
		ImGui::DestroyContext(context->imgui_context);
		context->imgui_context = nullptr;
	}

	std::free(context);
	context = nullptr;
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

	ASSERTF(io.Fonts->IsBuilt(), "Font atlas not built");

	ASSERT(delta_time > 0);
	io.DeltaTime = delta_time;

	// UpdateMousePosAndButtons()
	if (io.WantSetMousePos)
	{
		// ImGui enforces mouse position
		window.set_cursor_pos({(long)io.MousePos.x, (long)io.MousePos.y});
	}

	// UpdateMouseCursor()
	if ((io.ConfigFlags & ImGuiConfigFlags_NoMouseCursorChange) == 0)
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

		case lak::event_type::window_leave:
		{
			if (io.WantSetMousePos) return false;
			io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
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
				default:
					return false;
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
				default:
					return false;
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

		case lak::event_type::key_down:
			[[fallthrough]];
		case lak::event_type::key_up:
		{
			lak::key_code key  = event.key().key;
			const int scancode = event.key().native_scancode;
			const bool down    = event.type == lak::event_type::key_down;

			switch (key)
			{
				case lak::key_code::lshift:
					io.AddKeyEvent(ImGuiKey_LeftShift, down);
					break;
				case lak::key_code::rshift:
					io.AddKeyEvent(ImGuiKey_RightShift, down);
					break;
				case lak::key_code::lctrl:
					io.AddKeyEvent(ImGuiKey_LeftCtrl, down);
					break;
				case lak::key_code::rctrl:
					io.AddKeyEvent(ImGuiKey_RightCtrl, down);
					break;
				case lak::key_code::lalt:
					io.AddKeyEvent(ImGuiKey_LeftAlt, down);
					break;
				case lak::key_code::ralt:
					io.AddKeyEvent(ImGuiKey_RightAlt, down);
					break;
				case lak::key_code::lsuper:
					io.AddKeyEvent(ImGuiKey_LeftSuper, down);
					break;
				case lak::key_code::rsuper:
					io.AddKeyEvent(ImGuiKey_RightSuper, down);
					break;
				case lak::key_code::menu:
					io.AddKeyEvent(ImGuiKey_Menu, down);
					break;
				case lak::key_code::tab:
					io.AddKeyEvent(ImGuiKey_Tab, down);
					break;
				case lak::key_code::left:
					io.AddKeyEvent(ImGuiKey_LeftArrow, down);
					break;
				case lak::key_code::right:
					io.AddKeyEvent(ImGuiKey_RightArrow, down);
					break;
				case lak::key_code::up:
					io.AddKeyEvent(ImGuiKey_UpArrow, down);
					break;
				case lak::key_code::down:
					io.AddKeyEvent(ImGuiKey_DownArrow, down);
					break;
				case lak::key_code::page_up:
					io.AddKeyEvent(ImGuiKey_PageUp, down);
					break;
				case lak::key_code::page_down:
					io.AddKeyEvent(ImGuiKey_PageDown, down);
					break;
				case lak::key_code::home:
					io.AddKeyEvent(ImGuiKey_Home, down);
					break;
				case lak::key_code::end:
					io.AddKeyEvent(ImGuiKey_End, down);
					break;
				case lak::key_code::insert:
					io.AddKeyEvent(ImGuiKey_Insert, down);
					break;
				case lak::key_code::del:
					io.AddKeyEvent(ImGuiKey_Delete, down);
					break;
				case lak::key_code::backspace:
					io.AddKeyEvent(ImGuiKey_Backspace, down);
					break;
				case lak::key_code::space:
					io.AddKeyEvent(ImGuiKey_Space, down);
					break;
				case lak::key_code::enter:
					io.AddKeyEvent(ImGuiKey_Enter, down);
					break;
				case lak::key_code::escape:
					io.AddKeyEvent(ImGuiKey_Escape, down);
					break;
				case lak::key_code::none:
				default:
					break;
			}

#if defined(LAK_USE_WINAPI)
			switch (scancode)
			{
				case 'A':
					io.AddKeyEvent(ImGuiKey_A, down);
					break;
				case 'C':
					io.AddKeyEvent(ImGuiKey_C, down);
					break;
				case 'V':
					io.AddKeyEvent(ImGuiKey_V, down);
					break;
				case 'X':
					io.AddKeyEvent(ImGuiKey_X, down);
					break;
				case 'Y':
					io.AddKeyEvent(ImGuiKey_Y, down);
					break;
				case 'Z':
					io.AddKeyEvent(ImGuiKey_Z, down);
					break;
				default:
					break;
			}
#elif defined(LAK_USE_XLIB)
#	error "NYI"
			io.BackendPlatformName = "imgui_impl_lak_xlib";
#elif defined(LAK_USE_XCB)
#	error "NYI"
			io.BackendPlatformName = "imgui_impl_lak_xcb";
#elif defined(LAK_USE_SDL)
			switch (scancode)
			{
				case SDL_SCANCODE_A:
					io.AddKeyEvent(ImGuiKey_A, down);
					break;
				case SDL_SCANCODE_C:
					io.AddKeyEvent(ImGuiKey_C, down);
					break;
				case SDL_SCANCODE_V:
					io.AddKeyEvent(ImGuiKey_V, down);
					break;
				case SDL_SCANCODE_X:
					io.AddKeyEvent(ImGuiKey_X, down);
					break;
				case SDL_SCANCODE_Y:
					io.AddKeyEvent(ImGuiKey_Y, down);
					break;
				case SDL_SCANCODE_Z:
					io.AddKeyEvent(ImGuiKey_Z, down);
					break;
			}
#else
#	error "No implementation specified"
#endif
			return true;
		}

		default:
			break;
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

void ImGui::ImplRender(ImplContext context, const bool call_base_render)
{
	if (call_base_render) Render();
	ImplRenderData(context, ImGui::GetDrawData());
}

#ifdef LAK_ENABLE_SOFTRENDER
void ImplSRRender(ImGui::ImplContext context, ImDrawData *draw_data)
{
	ASSERT(context != nullptr);
	ASSERT(context->sr_context != nullptr);
	auto *sr_context = context->sr_context;

	ImGui_ImplSoftrender_RenderDrawData(draw_data);

#	if defined(LAK_USE_WINAPI)
#		if defined(LAK_SOFTWARE_RENDER_32BIT)
	using texture_colour_t = color32_t; // lak::colour::rgba8888;
#		elif defined(LAK_SOFTWARE_RENDER_24BIT)
	using texture_colour_t = color24_t; // lak::colour::rgb888;
#		elif defined(LAK_SOFTWARE_RENDER_16BIT)
	using texture_colour_t = color16_t; // lak::colour::rgb565;
#		elif defined(LAK_SOFTWARE_RENDER_8BIT)
	using texture_colour_t = alpha8_t; // lak::colour::v8;
#		else
#			error "No software render colour bit depth specified"
#		endif
	auto screen_texture_pixels = lak::span<void>(
	  sr_context->screen_texture.pixels,
	  sr_context->screen_texture.w * sr_context->screen_texture.h *
	    sr_context->screen_texture.size);
	{
		lak::blit(
		  lak::image_subview(*sr_context->screen_surface),
		  lak::image_subview(lak::image_view(
		    lak::span<texture_colour_t>(screen_texture_pixels),
		    {sr_context->screen_texture.w, sr_context->screen_texture.h})));
	}
#	elif defined(LAK_USE_XLIB)
#		error "NYI"
#	elif defined(LAK_USE_XCB)
#		error "NYI"
#	elif defined(LAK_USE_SDL)
	ASSERT(sr_context->window != nullptr);

	SDL_Surface *window = SDL_GetWindowSurface(sr_context->window);

	if (window != nullptr)
	{
		SDL_Rect clip;
		SDL_GetClipRect(window, &clip);
		SDL_FillRect(
		  window, &clip, SDL_MapRGBA(window->format, 0x00, 0x00, 0x00, 0xFF));
		if (SDL_BlitSurface(sr_context->screen_surface, nullptr, window, nullptr))
			ERROR(SDL_GetError());
	}
#	else
#		error "No implementation specified"
#	endif
}
#endif

#ifdef LAK_ENABLE_OPENGL
void ImplGLRender(ImGui::ImplContext context, ImDrawData *draw_data)
{
	ASSERT(draw_data != nullptr);
	ASSERT(context->gl_context != nullptr);
	auto *gl_context = context->gl_context;

	ImGuiIO &io = ImGui::GetIO();

	lak::vec4f_t viewport;
	viewport.x = draw_data->DisplayPos.x;
	viewport.y = draw_data->DisplayPos.y;
	viewport.z = draw_data->DisplaySize.x * io.DisplayFramebufferScale.x;
	viewport.w = draw_data->DisplaySize.y * io.DisplayFramebufferScale.y;
	if (viewport.z <= 0 || viewport.w <= 0) return;

	draw_data->ScaleClipRects(io.DisplayFramebufferScale);

	// As these are deferred, they are evaluated in reverse order to how they
	// appear here.
	DEFER(gl_context->vertex_array = 0);
	GL_DEFER_CALL(glDeleteVertexArrays, 1, &gl_context->vertex_array);
#	ifdef GL_CLIP_ORIGIN
#		ifndef LAK_OS_APPLE
	auto old_clip_origin = lak::opengl::get_enum(GL_CLIP_ORIGIN).UNWRAP();
#		endif
#	endif

	lak::opengl::call_checked(glGenVertexArrays, 1, &gl_context->vertex_array)
	  .UNWRAP();

	const bool using_scissor_test = true;

	const auto viewport_matrix = [&]()
	{
		const float &W = draw_data->DisplaySize.x;
		const float &H = draw_data->DisplaySize.y;
		// clang-format off
		const glm::mat4x4 orthoProj = {
			2.0f / W,  0.0f,      0.0f,  0.0f,
			0.0f,      2.0f / -H, 0.0f,  0.0f,
			0.0f,      0.0f,      1.0f,  0.0f,
			-1.0,      1.0,       0.0f,  1.0f
		};
		// clang-format on
		return orthoProj;
	}();

	auto set_state = [&]()
	{
		gl_context->shader.use().UNWRAP();
		gl_context->font.bind();
		lak::opengl::call_checked(glActiveTexture, GL_TEXTURE0).UNWRAP();
		lak::opengl::call_checked(glBindVertexArray, gl_context->vertex_array)
		  .UNWRAP();
		lak::opengl::call_checked(
		  glBlendEquationSeparate, GL_FUNC_ADD, GL_FUNC_ADD)
		  .UNWRAP();
		lak::opengl::call_checked(glBlendFuncSeparate,
		                          GL_SRC_ALPHA,
		                          GL_ONE_MINUS_SRC_ALPHA,
		                          GL_SRC_ALPHA,
		                          GL_ONE_MINUS_SRC_ALPHA)
		  .UNWRAP();
		lak::opengl::enable_if(GL_BLEND, true).UNWRAP();
		lak::opengl::enable_if(GL_CULL_FACE, false).UNWRAP();
		lak::opengl::enable_if(GL_DEPTH_TEST, false).UNWRAP();
		lak::opengl::enable_if(GL_SCISSOR_TEST, using_scissor_test).UNWRAP();
		lak::opengl::call_checked(glViewport,
		                          static_cast<GLint>(viewport.x),
		                          static_cast<GLint>(viewport.y),
		                          static_cast<GLsizei>(viewport.z),
		                          static_cast<GLsizei>(viewport.w))
		  .UNWRAP();

		lak::opengl::call_checked(glUniformMatrix4fv,
		                          gl_context->attrib_view_proj,
		                          1,
		                          GL_FALSE,
		                          &viewport_matrix[0][0])
		  .UNWRAP();
		lak::opengl::call_checked(glUniform1i, gl_context->attrib_tex, 0).UNWRAP();
		// #ifdef GL_SAMPLER_BINDING
		// glBindSampler(0, 0);
		// #endif
	};

	set_state();

	lak::opengl::call_checked(
	  glBindBuffer, GL_ARRAY_BUFFER, gl_context->array_buffer)
	  .UNWRAP();
	lak::opengl::call_checked(
	  glBindBuffer, GL_ELEMENT_ARRAY_BUFFER, gl_context->elements)
	  .UNWRAP();

	lak::opengl::call_checked(glEnableVertexAttribArray, gl_context->attrib_pos)
	  .UNWRAP();
	lak::opengl::call_checked(glVertexAttribPointer,
	                          gl_context->attrib_pos,
	                          2,
	                          GL_FLOAT,
	                          GL_FALSE,
	                          sizeof(ImDrawVert),
	                          (GLvoid *)IM_OFFSETOF(ImDrawVert, pos))
	  .UNWRAP();

	lak::opengl::call_checked(glEnableVertexAttribArray, gl_context->attrib_UV)
	  .UNWRAP();
	lak::opengl::call_checked(glVertexAttribPointer,
	                          gl_context->attrib_UV,
	                          2,
	                          GL_FLOAT,
	                          GL_FALSE,
	                          sizeof(ImDrawVert),
	                          (GLvoid *)IM_OFFSETOF(ImDrawVert, uv))
	  .UNWRAP();

	lak::opengl::call_checked(glEnableVertexAttribArray, gl_context->attrib_col)
	  .UNWRAP();
	lak::opengl::call_checked(glVertexAttribPointer,
	                          gl_context->attrib_col,
	                          4,
	                          GL_UNSIGNED_BYTE,
	                          GL_TRUE,
	                          sizeof(ImDrawVert),
	                          (GLvoid *)IM_OFFSETOF(ImDrawVert, col))
	  .UNWRAP();

	for (int n = 0; n < draw_data->CmdListsCount; ++n)
	{
		const ImDrawList *cmdList        = draw_data->CmdLists[n];
		const ImDrawIdx *idxBufferOffset = 0;

		lak::opengl::call_checked(glBufferData,
		                          GL_ARRAY_BUFFER,
		                          (GLsizeiptr)cmdList->VtxBuffer.Size *
		                            sizeof(ImDrawVert),
		                          (const GLvoid *)cmdList->VtxBuffer.Data,
		                          GL_STREAM_DRAW)
		  .UNWRAP();

		lak::opengl::call_checked(glBufferData,
		                          GL_ELEMENT_ARRAY_BUFFER,
		                          (GLsizeiptr)cmdList->IdxBuffer.Size *
		                            sizeof(ImDrawIdx),
		                          (const GLvoid *)cmdList->IdxBuffer.Data,
		                          GL_STREAM_DRAW)
		  .UNWRAP();

		for (int cmdI = 0; cmdI < cmdList->CmdBuffer.Size; ++cmdI)
		{
			const ImDrawCmd &pcmd = cmdList->CmdBuffer[cmdI];
			if (pcmd.UserCallback)
			{
				pcmd.UserCallback(cmdList, &pcmd);
				set_state(); // reset state in case user changed anything
			}
			else if (!using_scissor_test)
			{
				lak::opengl::call_checked(
				  glBindTexture, GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd.GetTexID())
				  .UNWRAP();
				lak::opengl::call_checked(glDrawElements,
				                          GL_TRIANGLES,
				                          (GLsizei)pcmd.ElemCount,
				                          sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT
				                                                 : GL_UNSIGNED_INT,
				                          idxBufferOffset)
				  .UNWRAP();
			}
			else
			{
				lak::vec4f_t clip;
				clip.x = pcmd.ClipRect.x - viewport.x;
				clip.y = pcmd.ClipRect.y - viewport.y;
				clip.z = pcmd.ClipRect.z - viewport.x;
				clip.w = pcmd.ClipRect.w - viewport.y;

				if (clip.x < viewport.z && clip.y < viewport.w && clip.z >= 0.0f &&
				    clip.w >= 0.0f)
				{
#	ifdef GL_CLIP_ORIGIN
#		ifndef LAK_OS_APPLE
					if (old_clip_origin == GL_UPPER_LEFT)
						// Support for GL 4.5's glClipControl(GL_UPPER_LEFT)
						lak::opengl::call_checked(glScissor,
						                          (GLint)clip.x,
						                          (GLint)clip.y,
						                          (GLsizei)(clip.z - clip.x),
						                          (GLsizei)(clip.w - clip.y))
						  .UNWRAP();
					else
#		endif
#	endif
						lak::opengl::call_checked(glScissor,
						                          (GLint)clip.x,
						                          (GLint)(viewport.w - clip.w),
						                          (GLsizei)(clip.z - clip.x),
						                          (GLsizei)(clip.w - clip.y))
						  .UNWRAP();

					lak::opengl::call_checked(
					  glBindTexture, GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd.GetTexID())
					  .UNWRAP();
					lak::opengl::call_checked(glDrawElements,
					                          GL_TRIANGLES,
					                          (GLsizei)pcmd.ElemCount,
					                          sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT
					                                                 : GL_UNSIGNED_INT,
					                          idxBufferOffset)
					  .UNWRAP();
				}
			}
			idxBufferOffset += pcmd.ElemCount;
		}
	}
}
#endif

void ImGui::ImplRenderData(ImplContext context, ImDrawData *draw_data)
{
	ASSERT(context);
	ASSERT(draw_data);
	switch (context->mode)
	{
#ifdef LAK_ENABLE_SOFTRENDER
		case lak::graphics_mode::Software:
			ImplSRRender(context, draw_data);
			break;
#endif
#ifdef LAK_ENABLE_OPENGL
		case lak::graphics_mode::OpenGL:
			ImplGLRender(context, draw_data);
			break;
#endif
		default:
			FATAL("Invalid context mode");
			break;
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

ImTextureID ImGui::ImplGetFontTexture(ImplContext context)
{
	switch (context->mode)
	{
#ifdef LAK_ENABLE_SOFTRENDER
		case lak::graphics_mode::Software:
			return (ImTextureID)&context->sr_context->atlas_texture;
#endif
#ifdef LAK_ENABLE_OPENGL
		case lak::graphics_mode::OpenGL:
			return (ImTextureID)(uintptr_t)context->gl_context->font.get();
#endif
		default:
			FATAL("Invalid context mode");
			break;
	}
}
