#include "main.hpp"

#include <lak/array.hpp>
#include <lak/format.hpp>

#include <lak/system/cobalt/log_target.hpp>
#include <lak/system/cobalt/renderers.hpp>
#include <lak/system/cobalt/result.hpp>

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

	if (!state.shader_program->LoadShaderStage(
	      ::cobalt::graphics::IShaderProgram::ShaderStage::Vertex,
	      ::cobalt::graphics::IShaderProgram::CodeFormat::HLSL,
	      reinterpret_cast<const uint8_t *>(vs.c_str()),
	      vs.size()))
	{
		ERROR("Loading vertex shader stage failed");
		return lak::err_t{u8"Loading vertex shader stage failed"_str};
	}
	if (!state.shader_program->LoadShaderStage(
	      ::cobalt::graphics::IShaderProgram::ShaderStage::Fragment,
	      ::cobalt::graphics::IShaderProgram::CodeFormat::HLSL,
	      reinterpret_cast<const uint8_t *>(fs.c_str()),
	      fs.size()))
	{
		ERROR("Loading fragment shader stage failed");
		return lak::err_t{u8"Loading fragment shader stage failed"_str};
	}

	if (!state.shader_program->CompileProgram())
	{
		ERROR("Failed to compile shader");
		return lak::err_t{u8"Failed to compile shader"_str};
	}

	state.program_node = rd->CreateProgramNode();

	if (!state.program_node->BindShaderProgram(state.shader_program.get()))
	{
		ERROR("Failed to bind shader program");
		return lak::err_t{u8"Failed to bind shader program"_str};
	}

	state.render_pass_node->AddChildNode(state.program_node.get());

	state.state_group_node = rd->CreateStateGroupNode();

	state.state_group_node->SetPolygonFillMode(
	  ::cobalt::graphics::IStateGroupNode::PolygonFillMode::Solid);
	state.state_group_node->SetDepthTestEnabled(true);
	state.state_group_node->SetDepthWriteEnabled(true);

	state.program_node->AddChildNode(state.state_group_node.get());

	size_t vertex_count = 3;
	lak::array<::cobalt::graphics::V3Float32> positions(
	  {{0.0f, 0.6f, 0.5f}, {-0.5f, -0.3f, 0.5f}, {0.5f, -0.3f, 0.5f}});
	lak::array<::cobalt::graphics::V3Float32> colors(
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

	if (!state.vertex_buffer->BindVertexAttribute(positions_attribute))
	{
		ERROR("Failed to bind vertex positions attribute");
		return lak::err_t{u8"Failed to bind vertex positions attribute"_str};
	}
	if (!state.vertex_buffer->BindVertexAttribute(colors_attribute))
	{
		ERROR("Failed to bind vertex colours attribute");
		return lak::err_t{u8"Failed to bind vertex colours attribute"_str};
	}

	if (!positions_attribute.SetInitialData(positions.data(), positions.size()))
	{
		ERROR("Failed to set initial positions data");
		return lak::err_t{u8"Failed to set initial positions data"_str};
	}
	if (!colors_attribute.SetInitialData(colors.data(), colors.size()))
	{
		ERROR("Failed to set initial colours data");
		return lak::err_t{u8"Failed to set initial colours data"_str};
	}

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

struct hello_cobalt_compute_state
{
	struct buffer_entry_out
	{
		::cobalt::graphics::V4UInt32 thread_id;
		::cobalt::graphics::V4UInt32 check_value;
	};

	::cobalt::graphics::IRenderPassNode::unique_ptr render_pass_node;
	::cobalt::graphics::IShaderProgram::unique_ptr shader_program;
	::cobalt::graphics::IProgramNode::unique_ptr program_node;
	::cobalt::graphics::IStateGroupNode::unique_ptr state_group_node;
	::cobalt::graphics::IDataArray::unique_ptr data_array_output;
	::cobalt::graphics::IDataArrayOutput::unique_ptr data_array_captured_output;

	hello_cobalt_compute_state()                              = default;
	hello_cobalt_compute_state(hello_cobalt_compute_state &&) = default;
	hello_cobalt_compute_state &operator=(hello_cobalt_compute_state &&) =
	  default;
	hello_cobalt_compute_state(const hello_cobalt_compute_state &) = delete;
	hello_cobalt_compute_state &operator=(const hello_cobalt_compute_state &) =
	  delete;

	~hello_cobalt_compute_state()
	{
		if (render_pass_node) render_pass_node->RemoveAllChildNodes();
		if (program_node) program_node->RemoveAllChildNodes();
		if (state_group_node) state_group_node->RemoveAllChildNodes();

		render_pass_node.reset();
		program_node.reset();
		state_group_node.reset();

		shader_program.reset();
		data_array_output.reset();
	}
};

lak::result<hello_cobalt_compute_state, lak::u8string> hello_cobalt_compute(
  lak::window &wnd)
{
	const auto &cgx = lak::cobalt_graphics_context(wnd.handle()).UNWRAP();
	auto *rd        = cgx.renderer.get();

	hello_cobalt_compute_state state;

	state.render_pass_node = rd->CreateRenderPassNode();

	auto cs = R"(struct buffer_entry_out
{
	uint4 threadId;
	uint4 checkValue;
};

RWStructuredBuffer<buffer_entry_out> buffer_data_out;

[numthreads(1, 1, 1)]
void main(uint3 threadId : SV_DispatchThreadID)
{
	uint counterValue = buffer_data_out.IncrementCounter();
	buffer_data_out[counterValue].threadId =
		uint4(threadId, threadId.x + threadId.y + threadId.z);
	buffer_data_out[counterValue].checkValue = uint4(1, 2, 3, 4);
})"_str;

	const uint32_t thread_count = 50U;

	state.shader_program = rd->CreateShaderProgram();

	if (!state.shader_program->LoadShaderStage(
	      ::cobalt::graphics::IShaderProgram::ShaderStage::Compute,
	      ::cobalt::graphics::IShaderProgram::CodeFormat::HLSL,
	      reinterpret_cast<const uint8_t *>(cs.c_str()),
	      cs.size()))
	{
		ERROR("Loading compute shader stage failed");
		return lak::err_t{u8"Loading compute shader stage failed"_str};
	}

	if (!state.shader_program->CompileProgram())
	{
		ERROR("Failed to compile shader");
		return lak::err_t{u8"Failed to compile shader"_str};
	}

	state.program_node = rd->CreateProgramNode();

	if (!state.program_node->BindShaderProgram(state.shader_program.get()))
	{
		ERROR("Failed to bind shader program");
		return lak::err_t{u8"Failed to bind shader program"_str};
	}

	state.render_pass_node->AddChildNode(state.program_node.get());

	state.state_group_node = rd->CreateStateGroupNode();

	state.state_group_node->SetComputeTask(
	  ::cobalt::graphics::V3UInt32(thread_count, 1, 1));

	state.program_node->AddChildNode(state.state_group_node.get());

	state.data_array_output = rd->CreateDataArray();

	state.data_array_output->SetBufferLayout(
	  sizeof(hello_cobalt_compute_state::buffer_entry_out), thread_count, true);

	if (!state.data_array_output->AllocateMemory())
	{
		ERROR("Data array could not be allocated");
		return lak::err_t{u8"Data array could not be allocated"_str};
	}

	state.data_array_captured_output = rd->CreateDataArrayOutput();

	state.data_array_output->AddOutputCaptureTarget(
	  state.data_array_captured_output.get());

	state.state_group_node->BindResourceArray(
	  state.shader_program->GetResourceArrayId("buffer_data_out"),
	  state.data_array_output.get());

	return lak::move_ok(state);
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
	lak::optional<hello_cobalt_compute_state> comp;
	lak::array<hello_cobalt_compute_state::buffer_entry_out> comp_data;

	const lak::cobalt::graphics_context *context;

	virtual void init() override final
	{
		ASSERT_EQUAL(window().graphics(), lak::graphics_mode::Cobalt);
		if_let_ok (auto t, hello_cobalt_triangle(window())) tri = lak::move(t);
		if_let_ok (auto c, hello_cobalt_compute(window())) comp = lak::move(c);
		context = &lak::cobalt_graphics_context(window().handle()).UNWRAP();

		renderer_name =
		  lak::fmt<"{} {}">(context->api_family, context->api_version);

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
		if (comp)
		{
			if (ImGui::Button("Compute"))
				lak::cobalt_append_render_pass(window().handle(),
				                               comp->render_pass_node.get());
			if (comp->data_array_captured_output->HasCapturedCounterValue() &&
			    comp->data_array_captured_output->HasCapturedOutput())
			{
				uint32_t counter_value = 0U;
				lak::cobalt::as_result(
				  comp->data_array_captured_output->ReadCounterValue(counter_value))
				  .UNWRAP();
				comp_data.resize(counter_value);
				lak::cobalt::as_result(
				  comp->data_array_captured_output->ReadBufferData(
				    comp_data.data(), comp_data.size() * sizeof(comp_data[0])))
				  .UNWRAP();
				DEBUG("Compute done");
				comp->data_array_captured_output->ClearCapturedOutput();
			}
			for (size_t i = 0; i < comp_data.size(); ++i)
			{
				LAK_TREE_NODE("%s", lak::fmt<"Entry {:d}">(i).c_str())
				{
					ImGui::Text(
					  lak::fmt<"Check {:d} {:d} {:d} {:d}">(comp_data[i].check_value.X(),
					                                        comp_data[i].check_value.Y(),
					                                        comp_data[i].check_value.Z(),
					                                        comp_data[i].check_value.W())
					    .c_str());
					ImGui::Text(
					  lak::fmt<"ID {:d} {:d} {:d} {:d}">(comp_data[i].thread_id.X(),
					                                     comp_data[i].thread_id.Y(),
					                                     comp_data[i].thread_id.Z(),
					                                     comp_data[i].thread_id.W())
					    .c_str());
				}
			}
		}
	}
};

lak::error_code<int> LAK_BASIC_PROGRAM(program_preinit)(lak::span<char *>)
{
	return lak::ok_t{};
}

lak::weak_ptr<LAK_BASIC_PROGRAM(window_instance<hello_cobalt_window>)>
  ogl3_wnd, ogl4_wnd, d3d11_wnd, d3d12_wnd, vk_wnd;

void renderer_settings_info(const lak::cobalt_renderer_settings &settings)
{
	SCOPED_CHECKPOINT(lak::fmt<u8"Renderer {} [{}]">(
	  settings.renderer_info.GetDisplayName().Get(),
	  settings.renderer_info.GetName().Get()));

	auto devices = settings.device_enumerator->GetAllDevices();
	for (const auto &device : devices.Get())
	{
		DEBUG(lak::fmt<u8"Found device {}">(device->GetDeviceName().Get()));
	}

	DEBUG(lak::fmt<u8"Using device {}">(settings.device->GetDeviceName().Get()));
}

lak::error_code<int> LAK_BASIC_PROGRAM(program_init)()
{
	LAK_BASIC_PROGRAM(window_start_size) = lak::vec2l_t{500, 500};

#ifdef LAK_ENABLE_COBALT_OGL3
	if_let_ok (auto settings, lak::cobalt_renderer_settings::preferred_ogl3())
	{
		renderer_settings_info(settings);
		ogl3_wnd = LAK_BASIC_PROGRAM(create_window<hello_cobalt_window>)(
		             LAK_BASIC_PROGRAM(window_cobalt_settings), settings)
		             .UNWRAP();
	}
#endif

#ifdef LAK_ENABLE_COBALT_OGL4
	if_let_ok (auto settings, lak::cobalt_renderer_settings::preferred_ogl4())
	{
		renderer_settings_info(settings);
		ogl4_wnd = LAK_BASIC_PROGRAM(create_window<hello_cobalt_window>)(
		             LAK_BASIC_PROGRAM(window_cobalt_settings), settings)
		             .UNWRAP();
	}
#endif

#ifdef LAK_ENABLE_COBALT_D3D11
	if_let_ok (auto settings, lak::cobalt_renderer_settings::preferred_d3d11())
	{
		renderer_settings_info(settings);
		d3d11_wnd = LAK_BASIC_PROGRAM(create_window<hello_cobalt_window>)(
		              LAK_BASIC_PROGRAM(window_cobalt_settings), settings)
		              .UNWRAP();
	}
#endif

#ifdef LAK_ENABLE_COBALT_D3D12
	if_let_ok (auto settings, lak::cobalt_renderer_settings::preferred_d3d12())
	{
		renderer_settings_info(settings);
		d3d12_wnd = LAK_BASIC_PROGRAM(create_window<hello_cobalt_window>)(
		              LAK_BASIC_PROGRAM(window_cobalt_settings), settings)
		              .UNWRAP();
	}
#endif

#ifdef LAK_ENABLE_COBALT_VK
	if_let_ok (auto settings, lak::cobalt_renderer_settings::preferred_vk())
	{
		renderer_settings_info(settings);
		vk_wnd = LAK_BASIC_PROGRAM(create_window<hello_cobalt_window>)(
		           LAK_BASIC_PROGRAM(window_cobalt_settings), settings)
		           .UNWRAP();
	}
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
