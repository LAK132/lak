#include "backend.inl"

#include "lak/system/cobalt/math.hpp"
#include "lak/system/cobalt/program.hpp"

namespace ImGui
{
	struct _ImplCoContext
	{
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
	};

	struct _ImplCoViewport
	{
		::cobalt::graphics::IFrameBuffer::unique_ptr fb;
		::cobalt::graphics::ITextureBuffer2D::unique_ptr db;
		lak::array<::cobalt::graphics::IRenderPassNode::unique_ptr> ps;
	};
}

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

	lak::cobalt::as_result(
	  context->shader_program->LoadShaderStage(
	    ::cobalt::graphics::IShaderProgram::ShaderStage::Vertex,
	    lak::cobalt::shader_source_hlsl(vert_shader)))
	  .UNWRAP();
	lak::cobalt::as_result(
	  context->shader_program->LoadShaderStage(
	    ::cobalt::graphics::IShaderProgram::ShaderStage::Fragment,
	    lak::cobalt::shader_source_hlsl(frag_shader)))
	  .UNWRAP();
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

	lak::cobalt::as_result(
	  context->program_node->BindShaderProgram(context->shader_program.get()))
	  .UNWRAP();

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
}

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

	if (pixels)
		lak::cobalt::as_result(
		  tex->SetInitialData(
		    pixels, size.x * size.y * pixel_stride, simgf, sdatf, 0))
		  .UNWRAP();

	lak::cobalt::as_result(tex->AllocateMemory()).UNWRAP();

	return (ImTextureID)(uintptr_t)&tex;
}

::cobalt::graphics::ITextureBuffer2D *ImGui::ImplGetCobaltTexture(
  ImTextureRef tex)
{
	return ((::cobalt::graphics::ITextureBuffer2D::unique_ptr *)(uintptr_t)
	          tex.GetTexID())
	  ->get();
}

ImTextureRef ImplCoUpdateTexture(ImGui::ImplContext,
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

void ImplCoDestroyTexture(ImGui::ImplContext, ImTextureID tex)
{
	delete (::cobalt::graphics::ITextureBuffer2D::unique_ptr *)(uintptr_t)tex;
}

lak::vec2s_t ImplCoTextureSize(ImGui::ImplContext, ImTextureID tex)
{
	::cobalt::graphics::V2UInt32 dims =
	  (*(::cobalt::graphics::ITextureBuffer2D::unique_ptr *)(uintptr_t)tex)
	    ->MipmapLevelDimensions(0);
	return {size_t(dims.X()), size_t(dims.Y())};
}

void ImplCoCreateViewport(ImGui::ImplContext context,
                          ImGui::ImplViewport viewport)
{
	viewport->co_viewport = new ImGui::_ImplCoViewport();

	viewport->co_viewport->fb =
	  context->co_context->renderer->CreateFrameBuffer();

	auto &tx = *(new ::cobalt::graphics::ITextureBuffer2D::unique_ptr);
	tx       = context->co_context->renderer->CreateTextureBuffer2D();
	tx->SetUsageFlags(
	  ::cobalt::graphics::ITextureBuffer::UsageFlags::ShaderInput |
	  ::cobalt::graphics::ITextureBuffer::UsageFlags::FrameBufferOutput);
	::cobalt::graphics::ITextureBuffer::ImageFormat imgf =
	  ::cobalt::graphics::ITextureBuffer::ImageFormat::RGBA;
	::cobalt::graphics::ITextureBuffer::DataFormat datf =
	  ::cobalt::graphics::ITextureBuffer::DataFormat::UNorm8;
	switch (viewport->colour)
	{
		case ImGui::ImplTextureColourFormat::RGBA:
			imgf = ::cobalt::graphics::ITextureBuffer::ImageFormat::RGBA;
			break;
		case ImGui::ImplTextureColourFormat::BGRA:
			imgf = ::cobalt::graphics::ITextureBuffer::ImageFormat::BGRA;
			break;
		case ImGui::ImplTextureColourFormat::RGB:
			imgf = ::cobalt::graphics::ITextureBuffer::ImageFormat::RGB;
			break;
		case ImGui::ImplTextureColourFormat::R:
			imgf = ::cobalt::graphics::ITextureBuffer::ImageFormat::R;
			break;
		default: ASSERT_UNREACHABLE();
	}
	switch (viewport->channel)
	{
		case ImGui::ImplTextureChannelFormat::U8:
			datf = ::cobalt::graphics::ITextureBuffer::DataFormat::UNorm8;
			break;
		case ImGui::ImplTextureChannelFormat::U16:
			datf = ::cobalt::graphics::ITextureBuffer::DataFormat::UNorm16;
			break;
		case ImGui::ImplTextureChannelFormat::F32:
			datf = ::cobalt::graphics::ITextureBuffer::DataFormat::Float32;
			break;
		default: ASSERT_UNREACHABLE();
	}
	tx->SetTextureFormat(imgf, datf);
	viewport->output = (ImTextureID)(uintptr_t)&tx;

	lak::cobalt::as_result(
	  viewport->co_viewport->fb->BindTexture(
	    tx.get(), ::cobalt::graphics::IFrameBuffer::AttachmentType::Color))
	  .UNWRAP();

	viewport->co_viewport->db =
	  context->co_context->renderer->CreateTextureBuffer2D();
	viewport->co_viewport->db->SetUsageFlags(
	  ::cobalt::graphics::ITextureBuffer::UsageFlags::FrameBufferOutput);
	viewport->co_viewport->db->SetTextureFormat(
	  ::cobalt::graphics::ITextureBuffer::ImageFormat::Depth,
	  ::cobalt::graphics::ITextureBuffer::DataFormat::DepthFloat32);

	lak::cobalt::as_result(
	  viewport->co_viewport->fb->BindTexture(
	    viewport->co_viewport->db.get(),
	    ::cobalt::graphics::IFrameBuffer::AttachmentType::Depth))
	  .UNWRAP();
}

void ImplCoDestroyViewport(ImGui::ImplContext context,
                           ImGui::ImplViewport viewport)
{
	if (!viewport) return;

	ImplCoDestroyTexture(context, viewport->output.GetTexID());

	if (!viewport->co_viewport) return;

	viewport->co_viewport->fb.reset();

	delete viewport->co_viewport;
}

ImGui::ImplCoViewportDetails ImplCoBeginViewport(ImGui::ImplContext context,
                                                 ImGui::ImplViewport viewport,
                                                 lak::vec2s_t size)
{
	auto id = viewport->output.GetTexID();

	if (id == ImTextureID_Invalid)
	{
		viewport->output = ImplCoCreateTexture(
		  context, nullptr, size, viewport->colour, viewport->channel);
		id = viewport->output.GetTexID();
	}
	else
	{
		auto *tex =
		  ((::cobalt::graphics::ITextureBuffer2D::unique_ptr *)id)->get();
		if (tex->MipmapLevelCount() < 1U ||
		    tex->MipmapLevelDimensions(0) !=
		      ::cobalt::graphics::V2UInt32(uint32_t(size.x), uint32_t(size.y)))
		{
			viewport->co_viewport->db->SetTextureDimensions(
			  {uint32_t(size.x), uint32_t(size.y)});
			lak::cobalt::as_result(viewport->co_viewport->db->AllocateMemory())
			  .UNWRAP();
			tex->SetTextureDimensions({uint32_t(size.x), uint32_t(size.y)});
			lak::cobalt::as_result(tex->AllocateMemory()).UNWRAP();
		}
	}

	viewport->co_viewport->fb->DefineViewportRegion(
	  {0, 0}, {uint32_t(size.x), uint32_t(size.y)});
	viewport->co_viewport->fb->DefineScissorRegion(
	  {0, 0}, {uint32_t(size.x), uint32_t(size.y)});

	return {
	  .renderer    = context->co_context->renderer,
	  .framebuffer = viewport->co_viewport->fb.get(),
	  .passes      = &viewport->co_viewport->ps,
	};
}

void ImplCoEndViewport(ImGui::ImplContext context,
                       ImGui::ImplViewport viewport)
{
	auto *rd = context->co_context->renderer;
	auto &ps = viewport->co_viewport->ps;
	rd->SetRenderPasses(ps.data(), ps.size());
	rd->StartNewFrame();
	rd->WaitForDrawComplete();
	rd->WaitForOutputCaptureComplete(); // :TODO: needed?
	rd->RemoveAllRenderPasses();
}

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
		// size_t idx_buffer_offset   = 0;

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

			if (cmd.UserCallback == ImDrawCallback_ResetRenderState)
			{
			}
			else if (cmd.UserCallback)
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
					  ImGui::ImplGetCobaltTexture(cmd.GetTexID()),
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
