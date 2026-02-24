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

#ifdef LAK_ENABLE_OPENGL
#	include "lak/system/opengl/shader.hpp"
#	include "lak/system/opengl/state.hpp"
#	include "lak/system/opengl/texture.hpp"
#endif

#ifdef LAK_ENABLE_COBALT
#	include "lak/system/cobalt/math.hpp"
#endif

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
#endif
	} *ImplGLContext;

	typedef struct _ImplCoContext
	{
#ifdef LAK_ENABLE_COBALT
		struct renderable
		{
			struct renderable_state
			{
				::cobalt::graphics::IStateGroupNode::unique_ptr state_group_node;
				::cobalt::graphics::IRenderableNode::unique_ptr renderable_node;
			};
			::cobalt::graphics::IVertexBuffer::unique_ptr vertex_buffer;
			::cobalt::graphics::IIndexBuffer::unique_ptr index_buffer;
			lak::array<renderable_state> state;
		};

		const lak::window_handle *window_handle;
		::cobalt::graphics::IRenderer *renderer;
		::cobalt::graphics::IFrameBuffer *frame_buffer;

		::cobalt::graphics::IRenderPassNode::unique_ptr render_pass_node;
		::cobalt::graphics::IShaderProgram::unique_ptr shader_program;
		::cobalt::graphics::IProgramNode::unique_ptr program_node;
		::cobalt::graphics::ITextureSampler2D::unique_ptr sampler;
		lak::array<renderable> renderables;

		void clear_renderables()
		{
			if (program_node) program_node->RemoveAllChildNodes();
			for (auto &renderable : renderables)
			{
				for (auto &state : renderable.state)
				{
					if (state.state_group_node)
						state.state_group_node->RemoveAllChildNodes();
					state.state_group_node.reset();
					state.renderable_node.reset();
				}
			}
			renderables.clear();
		}

		::cobalt::graphics::StateValueId viewProj;
		::cobalt::graphics::StateValueId scissor_min;
		::cobalt::graphics::StateValueId scissor_max;
		::cobalt::graphics::VertexAttributeId vPosition;
		::cobalt::graphics::VertexAttributeId vUV;
		::cobalt::graphics::VertexAttributeId vColour;
		::cobalt::graphics::TextureId fTexture;
#endif
	} *ImplCoContext;

	typedef struct _ImplContext
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

#ifdef LAK_ENABLE_SOFTRENDER
void ImplInitSRContext(ImGui::ImplSRContext context, const lak::window &window)
{
	ImGuiIO &io = ImGui::GetIO();

	io.BackendRendererName = "imgui_impl_lak_softrender";

	io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;

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

	io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;
}
#endif

#ifdef LAK_ENABLE_COBALT
void ImplInitCoContext(ImGui::ImplCoContext context, const lak::window &window)
{
	context->window_handle = window.handle();
	const auto &cgx =
	  lak::cobalt_graphics_context(context->window_handle).UNWRAP();
	context->renderer     = cgx.renderer.get();
	context->frame_buffer = cgx.frame_buffer.get();

	context->render_pass_node = context->renderer->CreateRenderPassNode();
	context->render_pass_node->BindFrameBuffer(context->frame_buffer);

	auto vs_in  = R"(
struct VSInput
{
	float2 position : position;
	float2 texCoord : texCoord;
	float4 color : color;
};)"_str;
	auto vs_out = R"(
struct VSOutput
{
	float4 pos : SV_POSITION;
	float2 uv : TEXCOORD0;
	float4 colour : COLOR;
	float2 clip_pos : TEXCOORD1;
	float2 clip_range : TEXCOORD2;
};)"_str;

	auto vert_shader = vs_in + vs_out + R"(
uniform row_major float4x4 view_proj;
uniform float2 scissor_min;
uniform float2 scissor_max;

VSOutput main(VSInput IN)
{
	VSOutput OUT;

	OUT.uv = IN.texCoord;
	OUT.colour = IN.color;
	OUT.pos = mul(view_proj, float4(IN.position, 0.0f, 1.0f));

	float2 range = scissor_max - scissor_min;
	float2 diff = (OUT.pos.xy / OUT.pos.w) - scissor_min;
	OUT.clip_range = abs(range);
	OUT.clip_pos = diff * sign(range);

	return OUT;
})"_str;

	auto frag_shader = vs_out + R"(
uniform Texture2D tex;
uniform SamplerState tex_CombinedSampler;

float4 main(VSOutput IN) : SV_TARGET
{
	if ((IN.clip_pos.x < 0) | (IN.clip_pos.x >= IN.clip_range.x) |
	    (IN.clip_pos.y < 0) | (IN.clip_pos.y >= IN.clip_range.y))
		discard;
	float4 colour = tex.Sample(tex_CombinedSampler, IN.uv);
	colour *= IN.colour;
	return colour;
})"_str;

	context->shader_program = context->renderer->CreateShaderProgram();

	context->shader_program->LoadShaderStage(
	  ::cobalt::graphics::IShaderProgram::ShaderStage::Vertex,
	  ::cobalt::graphics::IShaderProgram::CodeFormat::HLSL,
	  reinterpret_cast<const uint8_t *>(vert_shader.c_str()),
	  vert_shader.size());
	context->shader_program->LoadShaderStage(
	  ::cobalt::graphics::IShaderProgram::ShaderStage::Fragment,
	  ::cobalt::graphics::IShaderProgram::CodeFormat::HLSL,
	  reinterpret_cast<const uint8_t *>(frag_shader.c_str()),
	  frag_shader.size());
	lak::cobalt::as_result(context->shader_program->CompileProgram()).UNWRAP();

	context->viewProj = context->shader_program->GetStateValueId("view_proj");
	context->scissor_min =
	  context->shader_program->GetStateValueId("scissor_min");
	context->scissor_max =
	  context->shader_program->GetStateValueId("scissor_max");
	context->vPosition =
	  context->shader_program->GetVertexAttributeId("position");
	context->vUV     = context->shader_program->GetVertexAttributeId("texCoord");
	context->vColour = context->shader_program->GetVertexAttributeId("color");
	context->fTexture = context->shader_program->GetTextureId("tex");

	ASSERT_NOT_EQUAL(context->viewProj, ::cobalt::graphics::StateValueId::Null);
	ASSERT_NOT_EQUAL(context->scissor_min,
	                 ::cobalt::graphics::StateValueId::Null);
	ASSERT_NOT_EQUAL(context->scissor_max,
	                 ::cobalt::graphics::StateValueId::Null);
	ASSERT_NOT_EQUAL(context->vPosition,
	                 ::cobalt::graphics::VertexAttributeId::Null);
	ASSERT_NOT_EQUAL(context->vUV, ::cobalt::graphics::VertexAttributeId::Null);
	ASSERT_NOT_EQUAL(context->vColour,
	                 ::cobalt::graphics::VertexAttributeId::Null);
	ASSERT_NOT_EQUAL(context->fTexture, ::cobalt::graphics::TextureId::Null);

	context->program_node = context->renderer->CreateProgramNode();

	context->program_node->BindShaderProgram(context->shader_program.get());

	context->render_pass_node->AddChildNode(context->program_node.get());

	context->sampler = context->renderer->CreateTextureSampler2D();

	context->sampler->SetTextureFilterMode(
	  ::cobalt::graphics::ITextureSampler::FilterMode::Linear,
	  ::cobalt::graphics::ITextureSampler::FilterMode::Nearest);

	ImGuiIO &io = ImGui::GetIO();

	io.BackendRendererName = "imgui_impl_lak_cobalt";
	io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
	io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;
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

	for (ImTextureData *tex : ImGui::GetPlatformIO().Textures)
		if (tex->RefCount == 1U) ImGui_ImplSoftrender_DestroyTexture(tex);
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

	for (ImTextureData *tex : ImGui::GetPlatformIO().Textures)
		if (tex->RefCount == 1U)
			delete (lak::opengl::texture *)(uintptr_t)tex->GetTexID();
}
#endif

#ifdef LAK_ENABLE_COBALT
void ImplShutdownCoContext(ImGui::ImplCoContext context)
{
	context->renderer      = nullptr;
	context->frame_buffer  = nullptr;
	context->window_handle = nullptr;
	context->render_pass_node.reset();
	context->shader_program.reset();
	context->program_node.reset();
	context->sampler.reset();
	context->clear_renderables();

	for (ImTextureData *tex : ImGui::GetPlatformIO().Textures)
		if (tex->RefCount == 1U)
			delete (::cobalt::graphics::ITextureBuffer2D::unique_ptr *)(uintptr_t)
			  tex->GetTexID();
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

#ifdef LAK_ENABLE_SOFTRENDER
ImTextureID ImplSRCreateTexture(ImGui::ImplContext context,
                                const void *pixels,
                                lak::vec2s_t size,
                                ImGui::ImplTextureColourFormat colour,
                                ImGui::ImplTextureChannelFormat channel)
{
	ASSERT(channel == ImGui::ImplTextureChannelFormat::U8);

	auto compressor = lak::overloaded{
	  [](lak::span<const uint16_t> val) -> lak::span<uint8_t>
	  {
		  auto result =
		    lak::span<uint8_t>((uint8_t *)malloc(val.size()), val.size());
		  for (size_t i = 0U; i < result.size(); ++i) result[i] = val[i] >> 8U;
		  return result;
	  },
	  [](lak::span<const float> val) -> lak::span<uint8_t>
	  {
		  auto result =
		    lak::span<uint8_t>((uint8_t *)malloc(val.size()), val.size());
		  for (size_t i = 0U; i < result.size(); ++i)
			  result[i] = uint8_t(std::min<uint64_t>(uint64_t(val[i] * 256), 255));
		  return result;
	  }};

	texture_base_t *tex;
	switch (colour)
	{
		case ImGui::ImplTextureColourFormat::RGBA:
		{
			auto t = new texture_color32_t;
			switch (channel)
			{
				case ImGui::ImplTextureChannelFormat::U8:
					t->copy(size.x, size.y, (const color32_t *)pixels);
					break;
				case ImGui::ImplTextureChannelFormat::U16:
					t->init(size.x,
					        size.y,
					        (color32_t *)compressor(
					          lak::span<const uint16_t>(lak::span<const void>(
					            pixels, size.x * size.y * sizeof(uint16_t) * 4U)))
					          .data());
					break;
				case ImGui::ImplTextureChannelFormat::F32:
					t->init(size.x,
					        size.y,
					        (color32_t *)compressor(
					          lak::span<const float>(lak::span<const void>(
					            pixels, size.x * size.y * sizeof(float) * 4U)))
					          .data());
					break;
			}
			tex = t;
		}
		break;
		case ImGui::ImplTextureColourFormat::RGB:
		{
			auto t = new texture_color24_t;
			switch (channel)
			{
				case ImGui::ImplTextureChannelFormat::U8:
					t->copy(size.x, size.y, (const color24_t *)pixels);
					break;
				case ImGui::ImplTextureChannelFormat::U16:
					t->init(size.x,
					        size.y,
					        (color24_t *)compressor(
					          lak::span<const uint16_t>(lak::span<const void>(
					            pixels, size.x * size.y * sizeof(uint16_t) * 3U)))
					          .data());
					break;
				case ImGui::ImplTextureChannelFormat::F32:
					t->init(size.x,
					        size.y,
					        (color24_t *)compressor(
					          lak::span<const float>(lak::span<const void>(
					            pixels, size.x * size.y * sizeof(float) * 3U)))
					          .data());
					break;
			}
			tex = t;
		}
		break;
		case ImGui::ImplTextureColourFormat::R:
		{
			auto t = new texture_value8_t;
			switch (channel)
			{
				case ImGui::ImplTextureChannelFormat::U8:
					t->copy(size.x, size.y, (const value8_t *)pixels);
					break;
				case ImGui::ImplTextureChannelFormat::U16:
					t->init(size.x,
					        size.y,
					        (value8_t *)compressor(
					          lak::span<const uint16_t>(lak::span<const void>(
					            pixels, size.x * size.y * sizeof(uint16_t) * 1U)))
					          .data());
					break;
				case ImGui::ImplTextureChannelFormat::F32:
					t->init(size.x,
					        size.y,
					        (value8_t *)compressor(
					          lak::span<const float>(lak::span<const void>(
					            pixels, size.x * size.y * sizeof(float) * 1U)))
					          .data());
					break;
			}
			tex = t;
		}
		break;
		case ImGui::ImplTextureColourFormat::A:
		{
			auto t = new texture_alpha8_t;
			switch (channel)
			{
				case ImGui::ImplTextureChannelFormat::U8:
					t->copy(size.x, size.y, (const alpha8_t *)pixels);
					break;
				case ImGui::ImplTextureChannelFormat::U16:
					t->init(size.x,
					        size.y,
					        (alpha8_t *)compressor(
					          lak::span<const uint16_t>(lak::span<const void>(
					            pixels, size.x * size.y * sizeof(uint16_t) * 1U)))
					          .data());
					break;
				case ImGui::ImplTextureChannelFormat::F32:
					t->init(size.x,
					        size.y,
					        (alpha8_t *)compressor(
					          lak::span<const float>(lak::span<const void>(
					            pixels, size.x * size.y * sizeof(float) * 1U)))
					          .data());
					break;
			}
			tex = t;
		}
		break;
		default: ASSERT_UNREACHABLE();
	}

	return (ImTextureID)tex;
}
#endif

#ifdef LAK_ENABLE_OPENGL
ImTextureID ImplGLCreateTexture(ImGui::ImplContext context,
                                const void *pixels,
                                lak::vec2s_t size,
                                ImGui::ImplTextureColourFormat colour,
                                ImGui::ImplTextureChannelFormat channel)
{
	auto tex = new lak::opengl::texture;

	ASSERT(!!tex);

	GLenum gl_colour;
	switch (colour)
	{
		case ImGui::ImplTextureColourFormat::RGBA: gl_colour = GL_RGBA; break;
		case ImGui::ImplTextureColourFormat::BGRA: gl_colour = GL_BGRA; break;
		case ImGui::ImplTextureColourFormat::RGB:  gl_colour = GL_RGB; break;
		case ImGui::ImplTextureColourFormat::R:    gl_colour = GL_RED; break;
		default:                                   ASSERT_UNREACHABLE();
	}

	GLenum gl_channel;
	switch (channel)
	{
		case ImGui::ImplTextureChannelFormat::U8:
			gl_channel = GL_UNSIGNED_BYTE;
			break;
		case ImGui::ImplTextureChannelFormat::U16:
			gl_channel = GL_UNSIGNED_SHORT;
			break;
		case ImGui::ImplTextureChannelFormat::F32: gl_channel = GL_FLOAT; break;
		default:                                   ASSERT_UNREACHABLE();
	}

	tex->init(GL_TEXTURE_2D)
	  .bind()
	  .apply(GL_TEXTURE_MIN_FILTER, GL_LINEAR)
	  .apply(GL_TEXTURE_MAG_FILTER, GL_NEAREST)
	  .store_mode(GL_UNPACK_ROW_LENGTH, 0)
	  .store_mode(GL_UNPACK_ALIGNMENT, 1)
	  .build(0, GL_RGBA, lak::vec2i_t(size), 0, gl_colour, gl_channel, pixels);

	return (ImTextureID)(uintptr_t)tex;
}
#endif

#ifdef LAK_ENABLE_COBALT
ImTextureID ImplCoCreateTexture(ImGui::ImplContext context,
                                const void *pixels,
                                lak::vec2s_t size,
                                ImGui::ImplTextureColourFormat colour,
                                ImGui::ImplTextureChannelFormat channel)
{
	auto &tex = *(new ::cobalt::graphics::ITextureBuffer2D::unique_ptr);

	tex = context->co_context->renderer->CreateTextureBuffer2D();

	size_t pixel_stride;
	::cobalt::graphics::ITextureBuffer::ImageFormat imgf;
	::cobalt::graphics::ITextureBuffer::SourceImageFormat simgf;
	switch (colour)
	{
		case ImGui::ImplTextureColourFormat::RGBA:
			imgf  = ::cobalt::graphics::ITextureBuffer::ImageFormat::RGBA;
			simgf = ::cobalt::graphics::ITextureBuffer::SourceImageFormat::RGBA;
			pixel_stride = 4U;
			break;
		case ImGui::ImplTextureColourFormat::BGRA:
			imgf  = ::cobalt::graphics::ITextureBuffer::ImageFormat::BGRA;
			simgf = ::cobalt::graphics::ITextureBuffer::SourceImageFormat::BGRA;
			pixel_stride = 4U;
			break;
		case ImGui::ImplTextureColourFormat::RGB:
			imgf  = ::cobalt::graphics::ITextureBuffer::ImageFormat::RGB;
			simgf = ::cobalt::graphics::ITextureBuffer::SourceImageFormat::RGB;
			pixel_stride = 3U;
			break;
		case ImGui::ImplTextureColourFormat::R:
			imgf         = ::cobalt::graphics::ITextureBuffer::ImageFormat::R;
			simgf        = ::cobalt::graphics::ITextureBuffer::SourceImageFormat::R;
			pixel_stride = 1U;
			break;
		default: ASSERT_UNREACHABLE();
	}

	::cobalt::graphics::ITextureBuffer::DataFormat datf;
	::cobalt::graphics::ITextureBuffer::SourceDataFormat sdatf;
	switch (channel)
	{
		case ImGui::ImplTextureChannelFormat::U8:
			datf  = ::cobalt::graphics::ITextureBuffer::DataFormat::UNorm8;
			sdatf = ::cobalt::graphics::ITextureBuffer::SourceDataFormat::UNorm8;
			pixel_stride *= 1U;
			break;
		case ImGui::ImplTextureChannelFormat::U16:
			datf  = ::cobalt::graphics::ITextureBuffer::DataFormat::UNorm16;
			sdatf = ::cobalt::graphics::ITextureBuffer::SourceDataFormat::UNorm16;
			pixel_stride *= 2U;
			break;
		case ImGui::ImplTextureChannelFormat::F32:
			datf  = ::cobalt::graphics::ITextureBuffer::DataFormat::Float32;
			sdatf = ::cobalt::graphics::ITextureBuffer::SourceDataFormat::Float32;
			pixel_stride *= 4U;
			break;
		default: ASSERT_UNREACHABLE();
	}

	tex->SetTextureFormat(imgf, datf);

	tex->SetTextureDimensions({uint32_t(size.x), uint32_t(size.y)});

	lak::cobalt::as_result(
	  tex->SetInitialData(
	    pixels, size.x * size.y * pixel_stride, simgf, sdatf, 0))
	  .UNWRAP();

	lak::cobalt::as_result(tex->AllocateMemory()).UNWRAP();

	return (ImTextureID)(uintptr_t)&tex;
}
#endif

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

#ifdef LAK_ENABLE_SOFTRENDER
ImTextureRef ImplSRUpdateTexture(ImGui::ImplContext context,
                                 ImTextureID tex,
                                 const void *pixels,
                                 lak::vec2s_t size,
                                 ImGui::ImplTextureColourFormat colour,
                                 ImGui::ImplTextureChannelFormat channel,
                                 lak::span<const ImTextureRect> updates)
{
	// :TODO: do this correctly

	return ImplSRCreateTexture(context, pixels, size, colour, channel);
}
#endif

#ifdef LAK_ENABLE_OPENGL
ImTextureRef ImplGLUpdateTexture(ImGui::ImplContext context,
                                 ImTextureID tex,
                                 const void *pixels,
                                 lak::vec2s_t size,
                                 ImGui::ImplTextureColourFormat colour,
                                 ImGui::ImplTextureChannelFormat channel,
                                 lak::span<const ImTextureRect> updates)
{
	auto t = (lak::opengl::texture *)(uintptr_t)tex;

	size_t pixel_stride;
	GLenum gl_colour;
	switch (colour)
	{
		case ImGui::ImplTextureColourFormat::RGBA:
			gl_colour    = GL_RGBA;
			pixel_stride = 4U;
			break;
		case ImGui::ImplTextureColourFormat::BGRA:
			gl_colour    = GL_BGRA;
			pixel_stride = 4U;
			break;
		case ImGui::ImplTextureColourFormat::RGB:
			gl_colour    = GL_RGB;
			pixel_stride = 3U;
			break;
		case ImGui::ImplTextureColourFormat::R:
			gl_colour    = GL_RED;
			pixel_stride = 1U;
			break;
		default: ASSERT_UNREACHABLE();
	}

	GLenum gl_channel;
	switch (channel)
	{
		case ImGui::ImplTextureChannelFormat::U8:
			gl_channel = GL_UNSIGNED_BYTE;
			pixel_stride *= 1U;
			break;
		case ImGui::ImplTextureChannelFormat::U16:
			gl_channel = GL_UNSIGNED_SHORT;
			pixel_stride *= 2U;
			break;
		case ImGui::ImplTextureChannelFormat::F32:
			gl_channel = GL_FLOAT;
			pixel_stride *= 4U;
			break;
		default: ASSERT_UNREACHABLE();
	}

	auto byte_pixels = lak::span<const byte_t>(
	  lak::span<const void>(pixels, size.x * size.y * pixel_stride));

	t->bind().store_mode(GL_UNPACK_ROW_LENGTH, (GLint)size.x);
	for (const ImTextureRect &r : updates)
		t->rebuild(
		  0,
		  lak::vec2i_t{r.x, r.y},
		  lak::vec2i_t{r.w, r.h},
		  gl_colour,
		  gl_channel,
		  byte_pixels.subspan((r.x * pixel_stride) + (r.y * size.x * pixel_stride))
		    .data());
	t->store_mode(GL_UNPACK_ROW_LENGTH, 0);

	return tex;
}
#endif

#ifdef LAK_ENABLE_COBALT
ImTextureRef ImplCoUpdateTexture(ImGui::ImplContext context,
                                 ImTextureID tex,
                                 const void *pixels,
                                 lak::vec2s_t size,
                                 ImGui::ImplTextureColourFormat colour,
                                 ImGui::ImplTextureChannelFormat channel,
                                 lak::span<const ImTextureRect> updates)
{
	auto &t =
	  *(::cobalt::graphics::ITextureBuffer2D::unique_ptr *)(uintptr_t)tex;

	size_t pixel_stride;
	::cobalt::graphics::ITextureBuffer::SourceImageFormat imgf;
	switch (colour)
	{
		case ImGui::ImplTextureColourFormat::RGBA:
			imgf = ::cobalt::graphics::ITextureBuffer::SourceImageFormat::RGBA;
			pixel_stride = 4U;
			break;
		case ImGui::ImplTextureColourFormat::BGRA:
			imgf = ::cobalt::graphics::ITextureBuffer::SourceImageFormat::BGRA;
			pixel_stride = 4U;
			break;
		case ImGui::ImplTextureColourFormat::RGB:
			imgf = ::cobalt::graphics::ITextureBuffer::SourceImageFormat::RGB;
			pixel_stride = 3U;
			break;
		case ImGui::ImplTextureColourFormat::R:
			imgf         = ::cobalt::graphics::ITextureBuffer::SourceImageFormat::R;
			pixel_stride = 1U;
			break;
		default: ASSERT_UNREACHABLE();
	}

	::cobalt::graphics::ITextureBuffer::SourceDataFormat datf;
	switch (channel)
	{
		case ImGui::ImplTextureChannelFormat::U8:
			datf = ::cobalt::graphics::ITextureBuffer::SourceDataFormat::UNorm8;
			pixel_stride *= 1U;
			break;
		case ImGui::ImplTextureChannelFormat::U16:
			datf = ::cobalt::graphics::ITextureBuffer::SourceDataFormat::UNorm16;
			pixel_stride *= 2U;
			break;
		case ImGui::ImplTextureChannelFormat::F32:
			datf = ::cobalt::graphics::ITextureBuffer::SourceDataFormat::Float32;
			pixel_stride *= 4U;
			break;
		default: ASSERT_UNREACHABLE();
	}

	auto byte_pixels = lak::span<const byte_t>(
	  lak::span<const void>(pixels, size.x * size.y * pixel_stride));

	lak::array<byte_t> repack_buffer;
	for (const ImTextureRect &r : updates)
	{
		repack_buffer.resize(r.w * r.h * pixel_stride);
		for (size_t y = 0U; y < r.h; ++y)
			lak::memcpy(
			  lak::span(repack_buffer)
			    .subspan(r.w * y * pixel_stride, r.w * pixel_stride),
			  byte_pixels.subspan((r.x + ((r.y + y) * size.x)) * pixel_stride,
			                      r.w * pixel_stride));

		lak::cobalt::as_result(
		  t->QueueDataUpdate((const void *)repack_buffer.data(),
		                     repack_buffer.size(),
		                     imgf,
		                     datf,
		                     0,
		                     {r.x, r.y},
		                     {r.w, r.h}))
		  .UNWRAP();
	}

	return tex;
}
#endif

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

#ifdef LAK_ENABLE_SOFTRENDER
void ImplSRDestroyTexture(ImGui::ImplContext context, ImTextureID tex)
{
	auto _t = (texture_base_t *)(uintptr_t)tex;
	switch (_t->type)
	{
		case texture_type_t::ALPHA8:  delete (texture_alpha8_t *)_t; break;
		case texture_type_t::VALUE8:  delete (texture_value8_t *)_t; break;
		case texture_type_t::COLOR16: delete (texture_color16_t *)_t; break;
		case texture_type_t::COLOR24: delete (texture_color24_t *)_t; break;
		case texture_type_t::COLOR32: delete (texture_color32_t *)_t; break;
		default:                      ASSERT_UNREACHABLE();
	}
}
#endif

#ifdef LAK_ENABLE_OPENGL
void ImplGLDestroyTexture(ImGui::ImplContext context, ImTextureID tex)
{
	delete (lak::opengl::texture *)(uintptr_t)tex;
}
#endif

#ifdef LAK_ENABLE_COBALT
void ImplCoDestroyTexture(ImGui::ImplContext context, ImTextureID tex)
{
	delete (::cobalt::graphics::ITextureBuffer2D::unique_ptr *)(uintptr_t)tex;
}
#endif

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

#ifdef LAK_ENABLE_SOFTRENDER
lak::vec2s_t ImplSRTextureSize(ImGui::ImplContext context, ImTextureID tex)
{
	auto _t = (texture_base_t *)(uintptr_t)tex;
	return {_t->w, _t->h};
}
#endif

#ifdef LAK_ENABLE_OPENGL
lak::vec2s_t ImplGLTextureSize(ImGui::ImplContext context, ImTextureID tex)
{
	return lak::vec2s_t(((lak::opengl::texture *)(uintptr_t)tex)->size());
}
#endif

#ifdef LAK_ENABLE_COBALT
lak::vec2s_t ImplCoTextureSize(ImGui::ImplContext context, ImTextureID tex)
{
	::cobalt::graphics::V2UInt32 dims =
	  (*(::cobalt::graphics::ITextureBuffer2D::unique_ptr *)(uintptr_t)tex)
	    ->MipmapLevelDimensions(0);
	return {size_t(dims.X()), size_t(dims.Y())};
}
#endif

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

	if (draw_data->Textures != nullptr)
		for (ImTextureData *tex : *draw_data->Textures)
			if (tex->Status != ImTextureStatus_OK)
				ImGui::ImplUpdateTexture(context, tex);

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
	                          (GLvoid *)offsetof(ImDrawVert, pos))
	  .UNWRAP();

	lak::opengl::call_checked(glEnableVertexAttribArray, gl_context->attrib_UV)
	  .UNWRAP();
	lak::opengl::call_checked(glVertexAttribPointer,
	                          gl_context->attrib_UV,
	                          2,
	                          GL_FLOAT,
	                          GL_FALSE,
	                          sizeof(ImDrawVert),
	                          (GLvoid *)offsetof(ImDrawVert, uv))
	  .UNWRAP();

	lak::opengl::call_checked(glEnableVertexAttribArray, gl_context->attrib_col)
	  .UNWRAP();
	lak::opengl::call_checked(glVertexAttribPointer,
	                          gl_context->attrib_col,
	                          4,
	                          GL_UNSIGNED_BYTE,
	                          GL_TRUE,
	                          sizeof(ImDrawVert),
	                          (GLvoid *)offsetof(ImDrawVert, col))
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

					((const lak::opengl::texture *)(uintptr_t)pcmd.GetTexID())->bind();

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

#ifdef LAK_ENABLE_COBALT
void ImplCoRender(ImGui::ImplContext ctx, ImDrawData *draw_data)
{
	ASSERT(draw_data != nullptr);
	ASSERT(ctx->co_context != nullptr);

	auto *context = ctx->co_context;

	ImGuiIO &io = ImGui::GetIO();

	lak::vec4f_t viewport;
	viewport.x = draw_data->DisplayPos.x;
	viewport.y = draw_data->DisplayPos.y;
	viewport.z = draw_data->DisplaySize.x * io.DisplayFramebufferScale.x;
	viewport.w = draw_data->DisplaySize.y * io.DisplayFramebufferScale.y;
	if (viewport.z <= 0 || viewport.w <= 0) return;

	draw_data->ScaleClipRects(io.DisplayFramebufferScale);

	if (draw_data->Textures != nullptr)
		for (ImTextureData *tex : *draw_data->Textures)
			if (tex->Status != ImTextureStatus_OK)
				ImGui::ImplUpdateTexture(ctx, tex);

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

	context->clear_renderables();
	context->renderables.resize(draw_data->CmdListsCount);

	for (int n = 0; n < draw_data->CmdListsCount; ++n)
	{
		const ImDrawList *cmd_list = draw_data->CmdLists[n];
		size_t idx_buffer_offset   = 0;

		auto &renderable = context->renderables[n];

		renderable.vertex_buffer = context->renderer->CreateVertexBuffer();
		renderable.index_buffer  = context->renderer->CreateIndexBuffer();

		::cobalt::graphics::VertexAttribute<::cobalt::graphics::V2Float32>
		  vPosition(
		    cmd_list->VtxBuffer.Size,
		    ::cobalt::graphics::IVertexAttribute::PerformanceHint::WriteNever |
		      ::cobalt::graphics::IVertexAttribute::PerformanceHint::ReadNever,
		    ::cobalt::graphics::IVertexAttribute::PerformanceHint::WriteNever |
		      ::cobalt::graphics::IVertexAttribute::PerformanceHint::ReadOften);
		::cobalt::graphics::VertexAttribute<::cobalt::graphics::V2Float32> vUV(
		  cmd_list->VtxBuffer.Size,
		  ::cobalt::graphics::IVertexAttribute::PerformanceHint::WriteNever |
		    ::cobalt::graphics::IVertexAttribute::PerformanceHint::ReadNever,
		  ::cobalt::graphics::IVertexAttribute::PerformanceHint::WriteNever |
		    ::cobalt::graphics::IVertexAttribute::PerformanceHint::ReadOften);
		::cobalt::graphics::VertexAttribute<::cobalt::graphics::V4UNorm8> vColour(
		  cmd_list->VtxBuffer.Size,
		  ::cobalt::graphics::IVertexAttribute::PerformanceHint::WriteNever |
		    ::cobalt::graphics::IVertexAttribute::PerformanceHint::ReadNever,
		  ::cobalt::graphics::IVertexAttribute::PerformanceHint::WriteNever |
		    ::cobalt::graphics::IVertexAttribute::PerformanceHint::ReadOften);

		lak::cobalt::as_result(
		  renderable.vertex_buffer->BindVertexAttributeManualLayout(
		    vPosition, offsetof(ImDrawVert, pos), sizeof(ImDrawVert)))
		  .UNWRAP();
		lak::cobalt::as_result(
		  renderable.vertex_buffer->BindVertexAttributeManualLayout(
		    vUV, offsetof(ImDrawVert, uv), sizeof(ImDrawVert)))
		  .UNWRAP();
		lak::cobalt::as_result(
		  renderable.vertex_buffer->BindVertexAttributeManualLayout(
		    vColour, offsetof(ImDrawVert, col), sizeof(ImDrawVert)))
		  .UNWRAP();

		static_assert(sizeof(ImDrawIdx) == sizeof(uint16_t) ||
		              sizeof(ImDrawIdx) == sizeof(uint32_t));
		lak::conditional_t<
		  sizeof(ImDrawIdx) == sizeof(uint16_t),
		  ::cobalt::graphics::IndexAttribute<::cobalt::graphics::V1UInt16>,
		  ::cobalt::graphics::IndexAttribute<::cobalt::graphics::V1UInt32>>
		  vIndex(
		    cmd_list->IdxBuffer.Size,
		    ::cobalt::graphics::IIndexAttribute::PerformanceHint::WriteNever |
		      ::cobalt::graphics::IIndexAttribute::PerformanceHint::ReadNever,
		    ::cobalt::graphics::IIndexAttribute::PerformanceHint::WriteNever |
		      ::cobalt::graphics::IIndexAttribute::PerformanceHint::ReadOften);

		lak::cobalt::as_result(
		  renderable.index_buffer->BindIndexAttributeManualLayout(
		    vIndex, 0, sizeof(ImDrawIdx)))
		  .UNWRAP();

		lak::cobalt::as_result(
		  renderable.vertex_buffer->SetRawInitialData(
		    reinterpret_cast<const uint8_t *>(cmd_list->VtxBuffer.Data),
		    cmd_list->VtxBuffer.Size * sizeof(ImDrawVert)))
		  .UNWRAP();
		lak::cobalt::as_result(
		  renderable.index_buffer->SetRawInitialData(
		    reinterpret_cast<const uint8_t *>(cmd_list->IdxBuffer.Data),
		    cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx)))
		  .UNWRAP();

		lak::cobalt::as_result(renderable.vertex_buffer->AllocateMemory())
		  .UNWRAP();
		lak::cobalt::as_result(renderable.index_buffer->AllocateMemory()).UNWRAP();

		renderable.state.clear();
		renderable.state.reserve(cmd_list->CmdBuffer.Size);

		for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; ++cmd_i)
		{
			const ImDrawCmd &cmd = cmd_list->CmdBuffer[cmd_i];

			if (cmd.UserCallback)
			{
				cmd.UserCallback(cmd_list, &cmd);
			}
			else
			{
				lak::vec4f_t clip;
				clip.x = cmd.ClipRect.x - viewport.x;
				clip.y = cmd.ClipRect.y - viewport.y;
				clip.z = cmd.ClipRect.z - viewport.x;
				clip.w = cmd.ClipRect.w - viewport.y;

				if ((clip.x < viewport.z) & (clip.y < viewport.w) & (clip.z >= 0.0f) &
				    (clip.w >= 0.0f))
				{
					auto &state = renderable.state.emplace_back();

					state.state_group_node = context->renderer->CreateStateGroupNode();

					state.state_group_node->SetPolygonFillMode(
					  ::cobalt::graphics::IStateGroupNode::PolygonFillMode::Solid);
					state.state_group_node->SetBlendEnabled(true);
					state.state_group_node->SetPolygonCullMode(
					  ::cobalt::graphics::IStateGroupNode::PolygonCullMode::None);
					state.state_group_node->SetDepthTestEnabled(false);
					state.state_group_node->SetDepthWriteEnabled(false);

					state.state_group_node->SetBlendMode(
					  ::cobalt::graphics::IStateGroupNode::BlendOperation::Add,
					  ::cobalt::graphics::IStateGroupNode::BlendFactor::SourceAlpha,
					  ::cobalt::graphics::IStateGroupNode::BlendFactor::
					    OneMinusSourceAlpha,
					  ::cobalt::graphics::IStateGroupNode::BlendOperation::Add,
					  ::cobalt::graphics::IStateGroupNode::BlendFactor::SourceAlpha,
					  ::cobalt::graphics::IStateGroupNode::BlendFactor::
					    OneMinusSourceAlpha);

					state.state_group_node->BindTextureWithCombinedSampler(
					  context->fTexture,
					  ((::cobalt::graphics::ITextureBuffer2D::unique_ptr *)(uintptr_t)
					     cmd.GetTexID())
					    ->get(),
					  context->sampler.get());

					state.state_group_node->SetStateValue(
					  context->viewProj, lak::cobalt::from_glm(viewport_matrix));

					state.state_group_node->SetStateValue(
					  context->scissor_min,
					  lak::cobalt::from_glm(glm::vec2(
					    viewport_matrix * glm::vec4{clip.x, clip.y, 0.f, 1.f})));

					state.state_group_node->SetStateValue(
					  context->scissor_max,
					  lak::cobalt::from_glm(glm::vec2(
					    viewport_matrix * glm::vec4{clip.z, clip.w, 0.f, 1.f})));

					state.renderable_node = context->renderer->CreateRenderableNode();

					lak::cobalt::as_result(state.renderable_node->BindVertexAttribute(
					                         vPosition, context->vPosition))
					  .UNWRAP();
					lak::cobalt::as_result(
					  state.renderable_node->BindVertexAttribute(vUV, context->vUV))
					  .UNWRAP();
					lak::cobalt::as_result(state.renderable_node->BindVertexAttribute(
					                         vColour, context->vColour))
					  .UNWRAP();

					lak::cobalt::as_result(
					  state.renderable_node->BindIndexAttribute(vIndex))
					  .UNWRAP();

					lak::cobalt::as_result(
					  state.renderable_node->SetPrimitiveMode(
					    ::cobalt::graphics::IRenderableNode::PrimitiveMode::Triangles))
					  .UNWRAP();

					lak::cobalt::as_result(
					  state.renderable_node->SetVertexCount(size_t(cmd.ElemCount),
					                                        size_t(cmd.VtxOffset),
					                                        size_t(cmd.IdxOffset)))
					  .UNWRAP();

					state.state_group_node->AddChildNode(state.renderable_node.get());

					context->program_node->AddChildNode(state.state_group_node.get());
				}
			}
		}
	}

	lak::cobalt_append_render_pass(context->window_handle,
	                               context->render_pass_node.get())
	  .UNWRAP();
}
#endif

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
