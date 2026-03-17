#ifndef LAK_IMGUI_BACKEND_INL
#define LAK_IMGUI_BACKEND_INL

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

#include "lak/string_literals/string.hpp"

#include "lak/imgui/backend.hpp"

#if defined(LAK_USE_WINAPI)
#	include "../system/windowing/win32/impl.hpp"
#elif defined(LAK_USE_XLIB)
#	include "../system/windowing/xlib/impl.hpp"
#elif defined(LAK_USE_XCB)
#	include "../system/windowing/xcb/impl.hpp"
#elif defined(LAK_USE_SDL)
#	include "../system/windowing/sdl/impl.hpp"
#else
#	error "No implementation specified"
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

namespace ImGui
{
	struct _ImplSRContext;
	using ImplSRContext = _ImplSRContext *;

	struct _ImplGLContext;
	using ImplGLContext = _ImplGLContext *;

	struct _ImplCoContext;
	using ImplCoContext = _ImplCoContext *;

	struct _ImplContext
	{
		ImGuiContext *imgui_context;
		lak::cursor mouse_cursors[ImGuiMouseCursor_COUNT];
		bool allow_set_cursor = false;
		lak::graphics_mode mode;
		glm::mat4x4 transform = glm::mat4x4(1.0f);
		union
		{
			void *vd_context;
			ImplSRContext sr_context;
			ImplGLContext gl_context;
			ImplCoContext co_context;
		};
	};

	struct _ImplSRViewport;
	using ImplSRViewport = _ImplSRViewport *;

	struct _ImplGLViewport;
	using ImplGLViewport = _ImplGLViewport *;

	struct _ImplCoViewport;
	using ImplCoViewport = _ImplCoViewport *;

	struct _ImplViewport
	{
		ImTextureRef output;
		ImGui::ImplTextureColourFormat colour;
		ImGui::ImplTextureChannelFormat channel;
		union
		{
			void *vd_viewport;
			ImplSRViewport sr_viewport;
			ImplGLViewport gl_viewport;
			ImplCoViewport co_viewport;
		};
	};
}

#endif
