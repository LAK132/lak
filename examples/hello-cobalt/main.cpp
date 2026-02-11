#include "main.hpp"

#include <lak/system/cobalt/log_target.hpp>
#include <lak/system/cobalt/renderers.hpp>

#include <lak/system/windowing/window.hpp>

#include <lak/string_literals/string.hpp>

#include <lak/format.hpp>

#include <Logging/Logging.pkg>

#include <lak/basic_program.inl>

struct hello_cobalt_triangle_state
{
	::cobalt::graphics::IRenderPassNode::unique_ptr render_pass_node;
	::cobalt::graphics::IShaderProgram::unique_ptr shader_program;
	::cobalt::graphics::IProgramNode::unique_ptr program_node;
	::cobalt::graphics::IStateGroupNode::unique_ptr state_group_node;
	::cobalt::graphics::IVertexBuffer::unique_ptr vertex_buffer;
	::cobalt::graphics::IRenderableNode::unique_ptr renderable_node;

	hello_cobalt_triangle_state()                               = default;
	hello_cobalt_triangle_state(hello_cobalt_triangle_state &&) = default;
	hello_cobalt_triangle_state &operator=(hello_cobalt_triangle_state &&) =
	  default;
	hello_cobalt_triangle_state(const hello_cobalt_triangle_state &) = delete;
	hello_cobalt_triangle_state &operator=(const hello_cobalt_triangle_state &) =
	  delete;

	~hello_cobalt_triangle_state()
	{
		if (render_pass_node) render_pass_node->RemoveAllChildNodes();
		if (program_node) program_node->RemoveAllChildNodes();
		if (state_group_node) state_group_node->RemoveAllChildNodes();

		render_pass_node.reset();
		program_node.reset();
		state_group_node.reset();

		shader_program.reset();
		renderable_node.reset();
		vertex_buffer.reset();
	}
};

lak::result<hello_cobalt_triangle_state, lak::u8string> hello_cobalt_triangle(
  lak::window &wnd)
{
	const auto &cgx = lak::cobalt_graphics_context(wnd.handle()).UNWRAP();
	auto *rd        = cgx.renderer.get();
	auto *fb        = cgx.frame_buffer.get();

	hello_cobalt_triangle_state state;

	state.render_pass_node = rd->CreateRenderPassNode();
	state.render_pass_node->BindFrameBuffer(fb);

	auto vs = R"(struct VSInput
{
    float3 position: position;
    float3 color: color;
};

struct VSOutput
{
    float4 position: SV_POSITION;
    float3 color: COLOR;
};

VSOutput main(VSInput IN)
{
    VSOutput OUT;

    OUT.position = float4(IN.position, 1.0f);
    OUT.color = IN.color;

    return OUT;
})"_str;

	auto fs = R"(struct VSOutput
{
    float4 position: SV_POSITION;
    float3 color: COLOR;
};

float4 main(VSOutput IN) : SV_Target
{
    return float4(IN.color, 1.0f);
})"_str;

	state.shader_program = rd->CreateShaderProgram();

	state.shader_program->LoadShaderStage(
	  ::cobalt::graphics::IShaderProgram::ShaderStage::Vertex,
	  ::cobalt::graphics::IShaderProgram::CodeFormat::HLSL,
	  reinterpret_cast<const uint8_t *>(vs.c_str()),
	  vs.size());
	state.shader_program->LoadShaderStage(
	  ::cobalt::graphics::IShaderProgram::ShaderStage::Fragment,
	  ::cobalt::graphics::IShaderProgram::CodeFormat::HLSL,
	  reinterpret_cast<const uint8_t *>(fs.c_str()),
	  fs.size());

	if (!state.shader_program->CompileProgram())
	{
		ERROR("Failed to compile shader");
		return lak::err_t{u8"Failed to compile shader"_str};
	}

	state.program_node = rd->CreateProgramNode();

	state.program_node->BindShaderProgram(state.shader_program.get());

	state.render_pass_node->AddChildNode(state.program_node.get());

	state.state_group_node = rd->CreateStateGroupNode();

	state.state_group_node->SetPolygonFillMode(
	  ::cobalt::graphics::IStateGroupNode::PolygonFillMode::Solid);
	state.state_group_node->SetDepthTestEnabled(true);
	state.state_group_node->SetDepthWriteEnabled(true);

	state.program_node->AddChildNode(state.state_group_node.get());

	size_t vertex_count = 3;
	std::vector<::cobalt::graphics::V3Float32> positions(
	  {{0.0f, 0.6f, 0.5f}, {-0.5f, -0.3f, 0.5f}, {0.5f, -0.3f, 0.5f}});
	std::vector<::cobalt::graphics::V3Float32> colors(
	  {{1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f}});

	::cobalt::graphics::VertexAttribute<::cobalt::graphics::V3Float32>
	  positions_attribute(
	    vertex_count,
	    ::cobalt::graphics::IVertexAttribute::PerformanceHint::WriteNever |
	      ::cobalt::graphics::IVertexAttribute::PerformanceHint::ReadNever,
	    ::cobalt::graphics::IVertexAttribute::PerformanceHint::WriteNever |
	      ::cobalt::graphics::IVertexAttribute::PerformanceHint::ReadOften);
	::cobalt::graphics::VertexAttribute<::cobalt::graphics::V3Float32>
	  colors_attribute(
	    vertex_count,
	    ::cobalt::graphics::IVertexAttribute::PerformanceHint::WriteNever |
	      ::cobalt::graphics::IVertexAttribute::PerformanceHint::ReadNever,
	    ::cobalt::graphics::IVertexAttribute::PerformanceHint::WriteNever |
	      ::cobalt::graphics::IVertexAttribute::PerformanceHint::ReadOften);

	state.vertex_buffer = rd->CreateVertexBuffer();

	state.vertex_buffer->BindVertexAttribute(positions_attribute);
	state.vertex_buffer->BindVertexAttribute(colors_attribute);

	positions_attribute.SetInitialData(positions.data(), positions.size());
	colors_attribute.SetInitialData(colors.data(), colors.size());

	if (!state.vertex_buffer->AllocateMemory())
	{
		ERROR("Vertex buffer could not be allocated");
		return lak::err_t{u8"Vertex buffer could not be allocated"_str};
	}

	state.renderable_node = rd->CreateRenderableNode();

	state.renderable_node->SetPrimitiveMode(
	  ::cobalt::graphics::IRenderableNode::PrimitiveMode::Triangles);

	state.renderable_node->BindVertexAttribute(
	  positions_attribute,
	  state.shader_program->GetVertexAttributeId("position"));
	state.renderable_node->BindVertexAttribute(
	  colors_attribute, state.shader_program->GetVertexAttributeId("color"));

	state.state_group_node->AddChildNode(state.renderable_node.get());

	return lak::move_ok(state);
}

lak::result<lak::cobalt_settings, lak::u8string> get_renderer_settings(
  lak::cobalt::renderer_info_func get_info,
  ::cobalt::logging::ILogger::unique_ptr log)
{
	lak::cobalt_settings result;

	get_info(0, result.renderer_info);
	log->Info("Renderer {0} [{1}]",
	          result.renderer_info.GetDisplayName().Get(),
	          result.renderer_info.GetName().Get());

	result.device_enumerator =
	  result.renderer_info.CreateGraphicsDeviceEnumerator(log->CloneLogger());
	result.device_enumerator->EnumerateDevices();

	auto devices = result.device_enumerator->GetAllDevices();
	for (const auto &device : devices.Get())
	{
		log->Info("\tFound device {0}", device->GetDeviceName().Get());
	}

	result.device = result.device_enumerator->GetPreferredDevice();
	log->Info("\t\tPreferred device {0}", result.device->GetDeviceName().Get());

	return lak::move_ok(result);
}

struct hello_cobalt_window : virtual public LAK_BASIC_PROGRAM(window_api)
{
	hello_cobalt_window() : LAK_BASIC_PROGRAM(window_api)() {}
	virtual ~hello_cobalt_window()
	{
		if (context && context->renderer)
			context->renderer->RemoveAllRenderPasses();
	}

	lak::astring renderer_name;
	lak::optional<hello_cobalt_triangle_state> tri;

	const lak::cobalt::graphics_context *context;

	virtual void init() override final
	{
		ASSERT_EQUAL(window().graphics(), lak::graphics_mode::Cobalt);
		if_let_ok (auto t, hello_cobalt_triangle(window())) tri = lak::move(t);
		context = &lak::cobalt_graphics_context(window().handle()).UNWRAP();
		switch (context->api_family)
		{
			case ::cobalt::graphics::IRendererInfo::ApiFamily::OpenGL:
				renderer_name = "OpenGL";
				break;
			case ::cobalt::graphics::IRendererInfo::ApiFamily::OpenGLES:
				renderer_name = "OpenGLES";
				break;
			case ::cobalt::graphics::IRendererInfo::ApiFamily::Direct3D:
				renderer_name = "Direct3D";
				break;
			case ::cobalt::graphics::IRendererInfo::ApiFamily::Metal:
				renderer_name = "Metal";
				break;
			case ::cobalt::graphics::IRendererInfo::ApiFamily::Vulkan:
				renderer_name = "Vulkan";
				break;
		}

		renderer_name = lak::fmt<"{} {:d}.{:d}">(
		  renderer_name, context->api_version.major, context->api_version.minor);

		window().set_title(
		  (L"" APP_NAME " (" + lak::to_wstring(renderer_name) + L")").c_str());
	}

	virtual void handle_event(lak::event &event) override final
	{
		switch (event.type)
		{
			case lak::event_type::close_window: destroy(); break;

			default: break;
		}
	}

	virtual void loop(uint64_t counter_delta) override final
	{
		bool demo_open = true;
		ImGui::ShowDemoWindow(&demo_open);
		ImGui::Text("%s", renderer_name.c_str());
		if (tri)
			lak::cobalt_append_render_pass(window().handle(),
			                               tri->render_pass_node.get())
			  .UNWRAP();
	}
};

lak::error_code<int> LAK_BASIC_PROGRAM(program_preinit)(lak::span<char *>)
{
	return lak::ok_t{};
}

lak::weak_ptr<LAK_BASIC_PROGRAM(window_instance<hello_cobalt_window>)>
  ogl3_wnd, ogl4_wnd, d3d11_wnd, d3d12_wnd, vk_wnd;

::cobalt::logging::LogManager log_manager;

lak::error_code<int> LAK_BASIC_PROGRAM(program_init)()
{
	LAK_BASIC_PROGRAM(window_start_size) = lak::vec2l_t{500, 500};

	auto log = log_manager.GetLogger("");
	{
		auto log_target = lak::cobalt::log_target::create();
		log_target->set_external(&lak::debugger);
		log_manager.AddLogTarget(lak::move(log_target));
	}

#ifdef LAK_ENABLE_COBALT_OGL3
	ogl3_wnd = LAK_BASIC_PROGRAM(create_window<hello_cobalt_window>)(
	             get_renderer_settings(lak::cobalt::ogl3_get_renderer_info(),
	                                   log->CloneLogger())
	               .UNWRAP())
	             .UNWRAP();
#endif

#ifdef LAK_ENABLE_COBALT_OGL4
	ogl4_wnd = LAK_BASIC_PROGRAM(create_window<hello_cobalt_window>)(
	             get_renderer_settings(lak::cobalt::ogl4_get_renderer_info(),
	                                   log->CloneLogger())
	               .UNWRAP())
	             .UNWRAP();
#endif

#ifdef LAK_ENABLE_COBALT_D3D11
	d3d11_wnd = LAK_BASIC_PROGRAM(create_window<hello_cobalt_window>)(
	              get_renderer_settings(lak::cobalt::d3d11_get_renderer_info(),
	                                    log->CloneLogger())
	                .UNWRAP())
	              .UNWRAP();
#endif

#ifdef LAK_ENABLE_COBALT_D3D12
	d3d12_wnd = LAK_BASIC_PROGRAM(create_window<hello_cobalt_window>)(
	              get_renderer_settings(lak::cobalt::d3d12_get_renderer_info(),
	                                    log->CloneLogger())
	                .UNWRAP())
	              .UNWRAP();
#endif

#ifdef LAK_ENABLE_COBALT_VK
	vk_wnd = LAK_BASIC_PROGRAM(create_window<hello_cobalt_window>)(
	           get_renderer_settings(lak::cobalt::vk_get_renderer_info(),
	                                 log->CloneLogger())
	             .UNWRAP())
	           .UNWRAP();
#endif

	return lak::ok_t{};
}

bool LAK_BASIC_PROGRAM(program_loop)(uint64_t counter_delta)
{
	return !LAK_BASIC_PROGRAM(window_instances)().empty();
}

int LAK_BASIC_PROGRAM(program_quit)() { return EXIT_SUCCESS; }

void LAK_BASIC_PROGRAM(program_handle_event)(lak::event &event)
{
	switch (event.type)
	{
		case lak::event_type::quit_program: break;

		default: break;
	}
}
