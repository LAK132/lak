#include "lak/events.hpp"
#include "lak/os.hpp"

#include "lak/debug.hpp"
#include "lak/defer.hpp"
#include "lak/image.hpp"
#include "lak/trace.hpp"

#include "lak/opengl/shader.hpp"
#include "lak/opengl/state.hpp"
#include "lak/opengl/texture.hpp"

#define IMGUI_DEFINE_MATH_OPERATORS
#include "lak/imgui/backend.hpp"

#if defined(LAK_USE_WINAPI)
// #  error "NYI"
#	include "../windowing/win32/impl.hpp"
#elif defined(LAK_USE_XLIB)
#	error "NYI"
#elif defined(LAK_USE_XCB)
#	error "NYI"
#elif defined(LAK_USE_SDL)
#	include "../windowing/sdl/impl.hpp"
#	include <SDL.h>
#	ifdef LAK_OS_WINDOWS
#		include <SDL_syswm.h>
#	endif
#else
#	error "No implementation specified"
#endif

#include <examples/imgui_impl_softraster.h>
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

auto _call_checked =
  []<typename RET, typename... ARGS, typename... ARGS2>(
    lak::trace trace, RET(APIENTRYP func)(ARGS...), ARGS2... args)
{
	return lak::opengl::call_checked(func, lak::forward<ARGS2>(args)...)
	  .TRACE_UNWRAP(trace);
};

#define GL_DEFER_CALL(FUNC, ...)                                              \
	DEFER_CALL(_call_checked, lak::trace{}, FUNC, __VA_ARGS__)

#if !defined(LAK_SOFTWARE_RENDER_32BIT) &&                                    \
  !defined(LAK_SOFTWARE_RENDER_24BIT) &&                                      \
  !defined(LAK_SOFTWARE_RENDER_16BIT) && !defined(LAK_SOFTWARE_RENDER_8BIT)
#	define LAK_SOFTWARE_RENDER_16BIT
#endif

[[maybe_unused]] static const char *GetClipboardTextFn_DefaultImpl(void *);
[[maybe_unused]] static void SetClipboardTextFn_DefaultImpl(void *,
                                                            const char *text);

namespace ImGui
{
	typedef struct _ImplSRContext
	{
#if defined(LAK_USE_WINAPI)
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
#elif defined(LAK_USE_XLIB)
#	error "NYI"
#elif defined(LAK_USE_XCB)
#	error "NYI"
#elif defined(LAK_USE_SDL)
		SDL_Window *window;
		SDL_Surface *screen_surface;
#	if defined(LAK_SOFTWARE_RENDER_32BIT)
		static const Uint32 screen_format = SDL_PIXELFORMAT_ABGR8888;
#	elif defined(LAK_SOFTWARE_RENDER_24BIT)
		static const Uint32 screen_format = SDL_PIXELFORMAT_RGB24;
#	elif defined(LAK_SOFTWARE_RENDER_16BIT)
		static const Uint32 screen_format = SDL_PIXELFORMAT_RGB565;
#	elif defined(LAK_SOFTWARE_RENDER_8BIT)
		SDL_Palette *palette;
		static const Uint32 screen_format = SDL_PIXELFORMAT_INDEX8;
#	else
#		error "No software render colour bit depth specified"
#	endif
#else
#	error "No implementation specified"
#endif

		texture_alpha8_t atlas_texture;
#if defined(LAK_SOFTWARE_RENDER_32BIT)
		texture_color32_t screen_texture;
#elif defined(LAK_SOFTWARE_RENDER_24BIT)
		texture_color24_t screen_texture;
#elif defined(LAK_SOFTWARE_RENDER_16BIT)
		texture_color16_t screen_texture;
#elif defined(LAK_SOFTWARE_RENDER_8BIT)
		texture_value8_t screen_texture;
#else
#	error "No software render colour bit depth specified"
#endif
	} *ImplSRContext;

	typedef struct _ImplGLContext
	{
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
	} *ImplGLContext;

	typedef struct _ImplVkContext
	{
	} *ImplVkContext;

	typedef struct _ImplContext
	{
		ImGuiContext *imgui_context;
		lak::cursor mouse_cursors[ImGuiMouseCursor_COUNT];
		bool mouse_release[3];
		lak::graphics_mode mode;
		glm::mat4x4 transform = glm::mat4x4(1.0f);
		union
		{
			void *vd_context;
			ImplSRContext sr_context;
			ImplGLContext gl_context;
			ImplVkContext vk_context;
		};
	} *ImplContext;

	char *ImplStaticClipboard = nullptr;
}

ImGui::ImplContext ImGui::ImplCreateContext(lak::graphics_mode mode)
{
	ImplContext result = new _ImplContext();
	result->mode       = mode;
	switch (mode)
	{
		case lak::graphics_mode::Software:
			result->sr_context = new _ImplSRContext();
			break;
		case lak::graphics_mode::OpenGL:
			result->gl_context = new _ImplGLContext();
			break;
		case lak::graphics_mode::Vulkan:
			result->vk_context = new _ImplVkContext();
			break;
		default:
			result->vd_context = nullptr;
			break;
	}
	result->imgui_context = CreateContext();
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
				case lak::graphics_mode::Software:
					delete context->sr_context;
					break;
				case lak::graphics_mode::OpenGL:
					delete context->gl_context;
					break;
				case lak::graphics_mode::Vulkan:
					delete context->vk_context;
					break;
				default:
					FATAL("Invalid graphics mode");
					break;
			}
		}
		delete context;
	}
}

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

#if defined(LAK_USE_WINAPI)
		// context->screen_surface.resize(lak::vec2s_t(window_size));
#elif defined(LAK_USE_XLIB)
#	error "NYI"
#elif defined(LAK_USE_XCB)
#	error "NYI"
#elif defined(LAK_USE_SDL)
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

#	ifdef LAK_SOFTWARE_RENDER_8BIT
		SDL_SetSurfacePalette(context->screen_surface, context->palette);
#	endif
#else
#	error "No implementation specified"
#endif
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
		case lak::graphics_mode::Software:
			ImplUpdateDisplaySize(context->sr_context, handle, window_size);
			break;

		case lak::graphics_mode::OpenGL:
		{
			auto drawable_size = lak::window_drawable_size(handle);
			io.DisplayFramebufferScale.x =
			  (window_size.x > 0) ? (drawable_size.x / (float)window_size.x) : 1.0f;
			io.DisplayFramebufferScale.y =
			  (window_size.y > 0) ? (drawable_size.y / (float)window_size.y) : 1.0f;
		}
		break;

		case lak::graphics_mode::Vulkan:
		{
		}
		break;

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

	io.KeyMap[ImGuiKey_Tab]        = VK_TAB;
	io.KeyMap[ImGuiKey_LeftArrow]  = VK_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = VK_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow]    = VK_UP;
	io.KeyMap[ImGuiKey_DownArrow]  = VK_DOWN;
	io.KeyMap[ImGuiKey_PageUp]     = VK_PRIOR;
	io.KeyMap[ImGuiKey_PageDown]   = VK_NEXT;
	io.KeyMap[ImGuiKey_Home]       = VK_HOME;
	io.KeyMap[ImGuiKey_End]        = VK_END;
	io.KeyMap[ImGuiKey_Insert]     = VK_INSERT;
	io.KeyMap[ImGuiKey_Delete]     = VK_DELETE;
	io.KeyMap[ImGuiKey_Backspace]  = VK_BACK;
	io.KeyMap[ImGuiKey_Space]      = VK_SPACE;
	io.KeyMap[ImGuiKey_Enter]      = VK_RETURN;
	io.KeyMap[ImGuiKey_Escape]     = VK_ESCAPE;
	io.KeyMap[ImGuiKey_A]          = 'A';
	io.KeyMap[ImGuiKey_C]          = 'C';
	io.KeyMap[ImGuiKey_V]          = 'V';
	io.KeyMap[ImGuiKey_X]          = 'X';
	io.KeyMap[ImGuiKey_Y]          = 'Y';
	io.KeyMap[ImGuiKey_Z]          = 'Z';
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

	io.KeyMap[ImGuiKey_Tab]        = SDL_SCANCODE_TAB;
	io.KeyMap[ImGuiKey_LeftArrow]  = SDL_SCANCODE_LEFT;
	io.KeyMap[ImGuiKey_RightArrow] = SDL_SCANCODE_RIGHT;
	io.KeyMap[ImGuiKey_UpArrow]    = SDL_SCANCODE_UP;
	io.KeyMap[ImGuiKey_DownArrow]  = SDL_SCANCODE_DOWN;
	io.KeyMap[ImGuiKey_PageUp]     = SDL_SCANCODE_PAGEUP;
	io.KeyMap[ImGuiKey_PageDown]   = SDL_SCANCODE_PAGEDOWN;
	io.KeyMap[ImGuiKey_Home]       = SDL_SCANCODE_HOME;
	io.KeyMap[ImGuiKey_End]        = SDL_SCANCODE_END;
	io.KeyMap[ImGuiKey_Insert]     = SDL_SCANCODE_INSERT;
	io.KeyMap[ImGuiKey_Delete]     = SDL_SCANCODE_DELETE;
	io.KeyMap[ImGuiKey_Backspace]  = SDL_SCANCODE_BACKSPACE;
	io.KeyMap[ImGuiKey_Space]      = SDL_SCANCODE_SPACE;
	io.KeyMap[ImGuiKey_Enter]      = SDL_SCANCODE_RETURN;
	io.KeyMap[ImGuiKey_Escape]     = SDL_SCANCODE_ESCAPE;
	io.KeyMap[ImGuiKey_A]          = SDL_SCANCODE_A;
	io.KeyMap[ImGuiKey_C]          = SDL_SCANCODE_C;
	io.KeyMap[ImGuiKey_V]          = SDL_SCANCODE_V;
	io.KeyMap[ImGuiKey_X]          = SDL_SCANCODE_X;
	io.KeyMap[ImGuiKey_Y]          = SDL_SCANCODE_Y;
	io.KeyMap[ImGuiKey_Z]          = SDL_SCANCODE_Z;
#else
#	error "No implementation specified"
#endif

	io.SetClipboardTextFn = ImplSetClipboard;
	io.GetClipboardTextFn = (const char *(*)(void *))ImplGetClipboard;
	io.ClipboardUserData  = (void *)&ImplStaticClipboard;
}

void ImplInitSRContext(ImGui::ImplSRContext context, const lak::window &window)
{
	ImGuiIO &io = ImGui::GetIO();

#if defined(LAK_USE_WINAPI)
	context->screen_surface =
	  &window.handle()->software_context().platform_handle;
#elif defined(LAK_USE_XLIB)
#	error "NYI"
#elif defined(LAK_USE_XCB)
#	error "NYI"
#elif defined(LAK_USE_SDL)
	context->window                = window.handle()->sdl_window;

#	ifdef LAK_SOFTWARE_RENDER_8BIT
	context->palette               = SDL_AllocPalette(256);
	SDL_Colour palette[256];
	for (size_t i = 0; i < 256; ++i)
	{
		palette[i].r = uint8_t(i);
		palette[i].g = uint8_t(i);
		palette[i].b = uint8_t(i);
		palette[i].a = uint8_t(255);
	}
	SDL_SetPaletteColors(context->palette, palette, 0, 256);
#	endif

#else
#	error "No implementation specified"
#endif

	uint8_t *pixels;
	int width, height;
	io.Fonts->GetTexDataAsAlpha8(&pixels, &width, &height);
	context->atlas_texture.init(width, height, (alpha8_t *)pixels);
	io.Fonts->TexID = &context->atlas_texture;

	ImplUpdateDisplaySize(context, window.handle(), window.size());

	ImGui_ImplSoftraster_Init(&context->screen_texture);
}

void ImplInitGLContext(ImGui::ImplGLContext context, const lak::window &)
{
	using namespace lak::opengl::literals;

	context->shader = lak::opengl::program::create(
	                    "#version 150\n"
	                    "uniform mat4 viewProj;\n"
	                    "in vec2 vPosition;\n"
	                    "in vec2 vUV;\n"
	                    "in vec4 vColor;\n"
	                    "out vec2 fUV;\n"
	                    "out vec4 fColor;\n"
	                    "void main()\n"
	                    "{\n"
	                    "   fUV = vUV;\n"
	                    "   fColor = vColor;\n"
	                    "   gl_Position = viewProj * vec4(vPosition.xy, 0, 1);\n"
	                    "}"_vertex_shader.UNWRAP(),
	                    "#version 150\n"
	                    "uniform sampler2D fTexture;\n"
	                    "in vec2 fUV;\n"
	                    "in vec4 fColor;\n"
	                    "out vec4 pColor;\n"
	                    "void main()\n"
	                    "{\n"
	                    "   pColor = fColor * texture(fTexture, fUV.st);\n"
	                    "}"_fragment_shader.UNWRAP())
	                    .UNWRAP();

	context->attrib_tex       = *context->shader.uniform_location("fTexture");
	context->attrib_view_proj = *context->shader.uniform_location("viewProj");
	context->attrib_pos       = *context->shader.attrib_location("vPosition");
	context->attrib_UV        = *context->shader.attrib_location("vUV");
	context->attrib_col       = *context->shader.attrib_location("vColor");

	lak::opengl::call_checked(glGenBuffers, 1, &context->array_buffer).UNWRAP();
	lak::opengl::call_checked(glGenBuffers, 1, &context->elements).UNWRAP();

	ImGuiIO &io = ImGui::GetIO();

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

	io.Fonts->TexID = (ImTextureID)(intptr_t)context->font.get();
}

void ImplInitVkContext(ImGui::ImplVkContext, const lak::window &) {}

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

	context->mouse_release[0] = false;
	context->mouse_release[1] = false;
	context->mouse_release[2] = false;

	switch (context->mode)
	{
		case lak::graphics_mode::Software:
			ImplInitSRContext(context->sr_context, window);
			break;
		case lak::graphics_mode::OpenGL:
			ImplInitGLContext(context->gl_context, window);
			break;
		case lak::graphics_mode::Vulkan:
			ImplInitVkContext(context->vk_context, window);
			break;
		default:
			ASSERTF(false, "Invalid Context Mode");
			break;
	}

	ImplUpdateDisplaySize(context, window.handle());

#ifdef LAK_OS_WINDOWS
#	if defined(LAK_USE_WINAPI)
	ImGui::GetIO().ImeWindowHandle = window.platform_handle();
#	elif defined(LAK_USE_XLIB)
#		error "NYI"
#	elif defined(LAK_USE_XCB)
#		error "NYI"
#	elif defined(LAK_USE_SDL)
	SDL_SysWMinfo wmInfo;
	SDL_VERSION(&wmInfo.version);
	SDL_GetWindowWMInfo(window.handle()->sdl_window, &wmInfo);
	ImGui::GetIO().ImeWindowHandle = wmInfo.info.win.window;
#	else
#		error "No implementation specified"
#	endif
#endif
}

void ImplShutdownSRContext(ImGui::ImplSRContext context)
{
	ImGui_ImplSoftraster_Shutdown();

#if defined(LAK_USE_WINAPI)
	context->screen_surface = nullptr;
#elif defined(LAK_USE_XLIB)
#	error "NYI"
#elif defined(LAK_USE_XCB)
#	error "NYI"
#elif defined(LAK_USE_SDL)
	context->window = nullptr;

	SDL_FreeSurface(context->screen_surface);
	context->screen_surface = nullptr;

#	ifdef LAK_SOFTWARE_RENDER_8BIT
	SDL_FreePalette(context->palette);
	context->palette = nullptr;
#	endif
#else
#	error "No implementation specified"
#endif

	context->screen_texture.init(0, 0);
	context->atlas_texture.init(0, 0);
}

void ImplShutdownGLContext(ImGui::ImplGLContext context)
{
	if (context->array_buffer) glDeleteBuffers(1, &context->array_buffer);
	context->array_buffer = 0;

	if (context->elements) glDeleteBuffers(1, &context->elements);
	context->elements = 0;

	context->shader.clear();

	context->font.clear();
	ImGui::GetIO().Fonts->TexID = (ImTextureID)(intptr_t)0;
}

void ImplShutdownVkContext(ImGui::ImplVkContext) {}

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
		case lak::graphics_mode::Software:
			ImplShutdownSRContext(context->sr_context);
			break;
		case lak::graphics_mode::OpenGL:
			ImplShutdownGLContext(context->gl_context);
			break;
		case lak::graphics_mode::Vulkan:
			ImplShutdownVkContext(context->vk_context);
			break;
		default:
			ASSERTF(false, "Invalid Context Mode");
			break;
	}

	if (context->imgui_context != nullptr)
	{
		DestroyContext(context->imgui_context);
		context->imgui_context = nullptr;
	}

	std::free(context);
	context = nullptr;
}

void ImGui::ImplSetCurrentContext(ImplContext context)
{
	SetCurrentContext(context->imgui_context);
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
			if (event.wheel().wheel.y > 0)
				io.MouseWheel += 1;
			else if (event.wheel().wheel.y < 0)
				io.MouseWheel -= 1;

			if (event.wheel().wheel.x > 0)
				io.MouseWheelH -= 1;
			else if (event.wheel().wheel.x < 0)
				io.MouseWheelH += 1;

			return true;
		}

		case lak::event_type::motion:
		{
			if (io.WantSetMousePos) return false;
			io.MousePos.x = (float)event.motion().position.x;
			io.MousePos.y = (float)event.motion().position.y;
			return true;
		}

		case lak::event_type::button_down:
		{
			switch (event.button().button)
			{
				case lak::mouse_button::left:
				{
					io.MouseDown[0]           = true;
					context->mouse_release[0] = false;
				}
				break;
				case lak::mouse_button::right:
				{
					io.MouseDown[1]           = true;
					context->mouse_release[1] = false;
				}
				break;
				case lak::mouse_button::middle:
				{
					io.MouseDown[2]           = true;
					context->mouse_release[2] = false;
				}
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
				{
					context->mouse_release[0] = true;
				}
				break;
				case lak::mouse_button::right:
				{
					context->mouse_release[1] = true;
				}
				break;
				case lak::mouse_button::middle:
				{
					context->mouse_release[2] = true;
				}
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
			const int key = event.key().scancode;
			ASSERT(key >= 0 && key <= IM_ARRAYSIZE(io.KeysDown));
			io.KeysDown[key] = event.type == lak::event_type::key_down;

			io.KeyShift =
			  (event.key().mod & lak::mod_key::shift) != lak::mod_key::none;
			io.KeyCtrl =
			  (event.key().mod & lak::mod_key::ctrl) != lak::mod_key::none;
			io.KeyAlt = (event.key().mod & lak::mod_key::alt) != lak::mod_key::none;
			io.KeySuper =
			  (event.key().mod & lak::mod_key::super) != lak::mod_key::none;
			return true;
		}

		default:
			break;
	}

#if defined(LAK_USE_WINAPI)
	if (event.platform_event.message == WM_CHAR)
	{
		io.AddInputCharacter((unsigned int)event.platform_event.wParam);
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

void ImplSRRender(ImGui::ImplContext context, ImDrawData *draw_data)
{
	ASSERT(context != nullptr);
	ASSERT(context->sr_context != nullptr);
	auto *sr_context = context->sr_context;

	ImGui_ImplSoftraster_RenderDrawData(draw_data);

#if defined(LAK_USE_WINAPI)
#	if defined(LAK_SOFTWARE_RENDER_32BIT)
	using texture_colour_t = color32_t; // lak::colour::rgba8888;
#	elif defined(LAK_SOFTWARE_RENDER_24BIT)
	using texture_colour_t = color24_t; // lak::colour::rgb888;
#	elif defined(LAK_SOFTWARE_RENDER_16BIT)
	using texture_colour_t = color16_t; // lak::colour::rgb565;
#	elif defined(LAK_SOFTWARE_RENDER_8BIT)
	using texture_colour_t         = alpha8_t; // lak::colour::v8;
#	else
#		error "No software render colour bit depth specified"
#	endif
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
#elif defined(LAK_USE_XLIB)
#	error "NYI"
#elif defined(LAK_USE_XCB)
#	error "NYI"
#elif defined(LAK_USE_SDL)
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
#else
#	error "No implementation specified"
#endif
}

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
#ifdef GL_CLIP_ORIGIN
	auto old_clip_origin = lak::opengl::get_enum(GL_CLIP_ORIGIN).UNWRAP();
#endif

	lak::opengl::call_checked(glGenVertexArrays, 1, &gl_context->vertex_array)
	  .UNWRAP();

	const bool using_scissor_test = true;

	const auto viewport_matrix = [&]()
	{
		const float &W = draw_data->DisplaySize.x;
		const float &H = draw_data->DisplaySize.y;
		const float O1 = ((draw_data->DisplayPos.x * 2) + W) / -W;
		const float O2 = ((draw_data->DisplayPos.y * 2) + H) / H;
		// // clang-format off
		// const float orthoProj[] = {
		//   2.0f / W, 0.0f,       0.0f,   0.0f,
		//   0.0f,     2.0f / -H,  0.0f,   0.0f,
		//   0.0f,     0.0f,       -1.0f,  0.0f,
		//   O1,       O2,         0.0f,   1.0f
		// };
		// // clang-format on
		// clang-format off
      [[maybe_unused]] const glm::mat4x4 orthoProj = {
        2.0f / W, 0.0f,       0.0f,   0.0f,
        0.0f,     2.0f / -H,  0.0f,   0.0f,
        0.0f,     0.0f,       -1.0f,  0.0f,
        O1,       O2,         0.0f,   1.0f
      };
		// clang-format on
		// const glm::mat4x4 transform = context->transform * orthoProj;
		// const glm::mat4x4 transform = glm::mat4x4(1.0f);
		const glm::mat4x4 transform = glm::scale(
		  glm::translate(glm::mat4x4(1.0f), glm::vec3(-1.0, 1.0, 0.0)),
		  glm::vec3(
		    2.0 / draw_data->DisplaySize.x, 2.0 / -draw_data->DisplaySize.y, 1.0));
		// const glm::mat4x4 transform = orthoProj * glm::mat4x4(1.0f);
		return transform;
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
				  glBindTexture, GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd.TextureId)
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
#ifdef GL_CLIP_ORIGIN
					if (old_clip_origin == GL_UPPER_LEFT)
						// Support for GL 4.5's glClipControl(GL_UPPER_LEFT)
						lak::opengl::call_checked(glScissor,
						                          (GLint)clip.x,
						                          (GLint)clip.y,
						                          (GLsizei)clip.z,
						                          (GLsizei)clip.w)
						  .UNWRAP();
					else
#endif
						lak::opengl::call_checked(glScissor,
						                          (GLint)clip.x,
						                          (GLint)(viewport.w - clip.w),
						                          (GLsizei)(clip.z - clip.x),
						                          (GLsizei)(clip.w - clip.y))
						  .UNWRAP();

					lak::opengl::call_checked(
					  glBindTexture, GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd.TextureId)
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

void ImplVkRender(ImGui::ImplContext, ImDrawData *) {}

void ImGui::ImplRenderData(ImplContext context, ImDrawData *draw_data)
{
	ASSERT(context);
	ASSERT(draw_data);
	switch (context->mode)
	{
		case lak::graphics_mode::Software:
			ImplSRRender(context, draw_data);
			break;
		case lak::graphics_mode::OpenGL:
			ImplGLRender(context, draw_data);
			break;
		case lak::graphics_mode::Vulkan:
			ImplVkRender(context, draw_data);
			break;
		default:
			FATAL("Invalid context mode");
			break;
	}

	ImGuiIO &io = ImGui::GetIO();
	for (size_t i = 0; i < 3; ++i)
		if (context->mouse_release[i]) io.MouseDown[i] = false;
}

void ImGui::ImplSetClipboard(void *, const char *text)
{
#if defined(LAK_USE_WINAPI)
	SetClipboardTextFn_DefaultImpl(v, text);
#elif defined(LAK_USE_XLIB)
#	error "NYI"
#elif defined(LAK_USE_XCB)
#	error "NYI"
#elif defined(LAK_USE_SDL)
	SDL_SetClipboardText(text);
#else
#	error "No implementation specified"
#endif
}

const char *ImGui::ImplGetClipboard(char **clipboard)
{
#if defined(LAK_USE_WINAPI)
	return GetClipboardTextFn_DefaultImpl(clipboard);
#elif defined(LAK_USE_XLIB)
#	error "NYI"
#elif defined(LAK_USE_XCB)
#	error "NYI"
#elif defined(LAK_USE_SDL)
	if (*clipboard) SDL_free(*clipboard);
	*clipboard = SDL_GetClipboardText();
#else
#	error "No implementation specified"
#endif
	return *clipboard;
}

ImTextureID ImGui::ImplGetFontTexture(ImplContext context)
{
	switch (context->mode)
	{
		case lak::graphics_mode::Software:
			return (ImTextureID)&context->sr_context->atlas_texture;
		case lak::graphics_mode::OpenGL:
			return (ImTextureID)(uintptr_t)context->gl_context->font.get();
		case lak::graphics_mode::Vulkan:
		default:
			FATAL("Invalid context mode");
			break;
	}
}
