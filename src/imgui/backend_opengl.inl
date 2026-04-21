#include "backend.inl"

#include "lak/system/opengl/shader.hpp"
#include "lak/system/opengl/state.hpp"
#include "lak/system/opengl/texture.hpp"

auto _call_checked =
  []<typename RET, typename... ARGS, typename... ARGS2>(
    lak::trace trace, RET(APIENTRYP func)(ARGS...), ARGS2... args)
{
	return lak::opengl::call_checked(func, lak::forward<ARGS2>(args)...)
	  .TRACE_UNWRAP(trace);
};

#define GL_DEFER_CALL(FUNC, ...)                                              \
	DEFER_CALL(_call_checked, lak::trace{}, FUNC, __VA_ARGS__)

namespace ImGui
{
	struct _ImplGLContext
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
	};

	struct _ImplGLViewport
	{
		GLuint fb;
		GLuint rb;
		GLuint old_dfb;
		GLuint old_rfb;
		GLuint old_rb;
		lak::array<GLint, 4U> old_vp;
		lak::array<GLint, 4U> old_sc;
	};
}

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

ImTextureID ImplGLCreateTexture(ImGui::ImplContext,
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

ImTextureRef ImplGLUpdateTexture(ImGui::ImplContext,
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

void ImplGLDestroyTexture(ImGui::ImplContext, ImTextureID tex)
{
	delete (lak::opengl::texture *)(uintptr_t)tex;
}

lak::vec2s_t ImplGLTextureSize(ImGui::ImplContext, ImTextureID tex)
{
	return lak::vec2s_t(((lak::opengl::texture *)(uintptr_t)tex)->size());
}

void ImplGLCreateViewport(ImGui::ImplContext, ImGui::ImplViewport viewport)
{
	viewport->gl_viewport = new ImGui::_ImplGLViewport();

	lak::opengl::call_checked(glGenFramebuffers, 1, &viewport->gl_viewport->fb)
	  .UNWRAP();
	lak::opengl::call_checked(glGenRenderbuffers, 1, &viewport->gl_viewport->rb)
	  .UNWRAP();
}

void ImplGLDestroyViewport(ImGui::ImplContext context,
                           ImGui::ImplViewport viewport)
{
	if (!viewport) return;

	ImplGLDestroyTexture(context, viewport->output.GetTexID());

	if (!viewport->gl_viewport) return;

	lak::opengl::call_checked(
	  glDeleteFramebuffers, 1, &viewport->gl_viewport->fb)
	  .UNWRAP();
	lak::opengl::call_checked(
	  glDeleteRenderbuffers, 1, &viewport->gl_viewport->rb)
	  .UNWRAP();

	delete viewport->gl_viewport;
}

ImGui::ImplGLViewportDetails ImplGLBeginViewport(ImGui::ImplContext context,
                                                 ImGui::ImplViewport viewport,
                                                 lak::vec2s_t size)
{
	auto id = viewport->output.GetTexID();

	viewport->gl_viewport->old_dfb =
	  lak::opengl::get_int<1U>(GL_DRAW_FRAMEBUFFER_BINDING).UNWRAP();
	viewport->gl_viewport->old_rfb =
	  lak::opengl::get_int<1U>(GL_READ_FRAMEBUFFER_BINDING).UNWRAP();
	viewport->gl_viewport->old_rb =
	  lak::opengl::get_int<1U>(GL_RENDERBUFFER_BINDING).UNWRAP();
	viewport->gl_viewport->old_vp =
	  lak::opengl::get_int<4U>(GL_VIEWPORT).UNWRAP();
	viewport->gl_viewport->old_sc =
	  lak::opengl::get_int<4U>(GL_SCISSOR_BOX).UNWRAP();

	if (id == ImTextureID_Invalid)
	{
		viewport->output = ImplGLCreateTexture(
		  context, nullptr, size, viewport->colour, viewport->channel);
		id = viewport->output.GetTexID();
	}
	else
	{
		auto *tex = (lak::opengl::texture *)(uintptr_t)id;

		GLenum gl_colour;
		switch (viewport->colour)
		{
			case ImGui::ImplTextureColourFormat::RGBA: gl_colour = GL_RGBA; break;
			case ImGui::ImplTextureColourFormat::BGRA: gl_colour = GL_BGRA; break;
			case ImGui::ImplTextureColourFormat::RGB:  gl_colour = GL_RGB; break;
			case ImGui::ImplTextureColourFormat::R:    gl_colour = GL_RED; break;
			default:                                   ASSERT_UNREACHABLE();
		}

		GLenum gl_channel;
		switch (viewport->channel)
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

		tex->bind().build(
		  0, GL_RGBA, lak::vec2i_t(size), 0, gl_colour, gl_channel, nullptr);
	}

	auto *tex = (lak::opengl::texture *)(uintptr_t)id;

	lak::opengl::call_checked(
	  glBindFramebuffer, GL_FRAMEBUFFER, viewport->gl_viewport->fb)
	  .UNWRAP();

	lak::opengl::call_checked(
	  glBindRenderbuffer, GL_RENDERBUFFER, viewport->gl_viewport->rb)
	  .UNWRAP();
	lak::opengl::call_checked(glRenderbufferStorage,
	                          GL_RENDERBUFFER,
	                          GL_DEPTH24_STENCIL8,
	                          GLsizei(size.x),
	                          GLsizei(size.y))
	  .UNWRAP();

	lak::opengl::call_checked(glFramebufferTexture2D,
	                          GL_FRAMEBUFFER,
	                          GL_COLOR_ATTACHMENT0,
	                          GL_TEXTURE_2D,
	                          tex->get(),
	                          0)
	  .UNWRAP();
	lak::opengl::call_checked(glFramebufferRenderbuffer,
	                          GL_FRAMEBUFFER,
	                          GL_DEPTH_STENCIL_ATTACHMENT,
	                          GL_RENDERBUFFER,
	                          viewport->gl_viewport->rb)
	  .UNWRAP();

	ASSERT_EQUAL(glCheckFramebufferStatus(GL_FRAMEBUFFER),
	             GLuint(GL_FRAMEBUFFER_COMPLETE));

	glViewport(0, 0, GLsizei(size.x), GLsizei(size.y));
	glScissor(0, 0, GLsizei(size.x), GLsizei(size.y));

	return {};
}

void ImplGLEndViewport(ImGui::ImplContext, ImGui::ImplViewport viewport)
{
	GLuint dfb = viewport->gl_viewport->old_dfb;
	GLuint rfb = viewport->gl_viewport->old_rfb;
	GLuint rb  = viewport->gl_viewport->old_rb;
	auto &vp   = viewport->gl_viewport->old_vp;
	auto &sc   = viewport->gl_viewport->old_sc;
	lak::opengl::call_checked(glBindFramebuffer, GL_DRAW_FRAMEBUFFER, dfb)
	  .UNWRAP();
	lak::opengl::call_checked(glBindFramebuffer, GL_READ_FRAMEBUFFER, rfb)
	  .UNWRAP();
	lak::opengl::call_checked(glBindRenderbuffer, GL_RENDERBUFFER, rb).UNWRAP();
	glViewport(vp[0], vp[1], vp[2], vp[3]);
	glScissor(sc[0], sc[1], sc[2], sc[3]);
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

	// As these are deferred, they are evaluated in reverse order to
	// how they appear here.
	DEFER(gl_context->vertex_array = 0);
	GL_DEFER_CALL(glDeleteVertexArrays, 1, &gl_context->vertex_array);
#ifdef GL_CLIP_ORIGIN
#	ifndef LAK_OS_APPLE
	auto old_clip_origin = lak::opengl::get_enum(GL_CLIP_ORIGIN).UNWRAP();
#	endif
#endif

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
		                          (GLboolean)GL_FALSE,
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
	                          (GLboolean)GL_FALSE,
	                          (GLsizei)sizeof(ImDrawVert),
	                          (GLvoid *)offsetof(ImDrawVert, pos))
	  .UNWRAP();

	lak::opengl::call_checked(glEnableVertexAttribArray, gl_context->attrib_UV)
	  .UNWRAP();
	lak::opengl::call_checked(glVertexAttribPointer,
	                          gl_context->attrib_UV,
	                          2,
	                          GL_FLOAT,
	                          (GLboolean)GL_FALSE,
	                          (GLsizei)sizeof(ImDrawVert),
	                          (GLvoid *)offsetof(ImDrawVert, uv))
	  .UNWRAP();

	lak::opengl::call_checked(glEnableVertexAttribArray, gl_context->attrib_col)
	  .UNWRAP();
	lak::opengl::call_checked(glVertexAttribPointer,
	                          gl_context->attrib_col,
	                          4,
	                          GL_UNSIGNED_BYTE,
	                          (GLboolean)GL_TRUE,
	                          (GLsizei)sizeof(ImDrawVert),
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
			if (pcmd.UserCallback == ImDrawCallback_ResetRenderState)
			{
				set_state();
			}
			else if (pcmd.UserCallback)
			{
				pcmd.UserCallback(cmdList, &pcmd);
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
#ifdef GL_CLIP_ORIGIN
#	ifndef LAK_OS_APPLE
					if (old_clip_origin == GL_UPPER_LEFT)
						// Support for GL 4.5's glClipControl(GL_UPPER_LEFT)
						lak::opengl::call_checked(glScissor,
						                          (GLint)clip.x,
						                          (GLint)clip.y,
						                          (GLsizei)(clip.z - clip.x),
						                          (GLsizei)(clip.w - clip.y))
						  .UNWRAP();
					else
#	endif
#endif
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
