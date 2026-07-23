#define APP_NAME "Ball Game"

#define LAK_BASIC_PROGRAM_IMGUI_IMPL
#include <lak/basic_program.inl>

#include <lak/stdint.hpp>

#include <lak/file/obj.hpp>
#include <lak/file/pnm.hpp>

#include <lak/system/opengl/mesh.hpp>
#include <lak/system/opengl/shader.hpp>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>

#include "space.hpp"

enum state_t
{
	LOADING,
	RUNNING,
	WIN,
	LOSS
};

struct camera
{
	lak::shared_ptr<reference_frame> frame;
	glm::mat4 projection;
	glm::mat4 view;

	inline glm::mat4 &update_projection(const lak::window &w)
	{
		return projection = glm::perspective(glm::pi<float>() / 2.0f,
		                                     (float)w.drawable_size().x /
		                                       (float)w.drawable_size().y,
		                                     0.01f,
		                                     100.0f);
	}

	inline glm::mat4 &update_view()
	{
		const auto trans   = frame->get_transform();
		const auto pos     = glm::vec3(trans * glm::vec4(0, 0, 0, 1));
		const auto forward = glm::vec3(trans * glm::vec4(0, -1, 0, 1));
		const auto up      = glm::vec3(trans * glm::vec4(0, 0, 1, 0));
		return view        = glm::lookAt(pos, forward, glm::normalize(up));
	}

	inline glm::mat4 update_projview(const lak::window &w)
	{
		return update_projection(w) * update_view();
	}
};

struct light
{
	lak::shared_ptr<reference_frame> frame;
	glm::vec4 colour = {0.5f, 0.5f, 0.5f, 1.0f};
};

struct model
{
	lak::shared_ptr<reference_frame> frame;
	lak::shared_ptr<lak::opengl::static_object_part> mesh;

	void draw()
	{
		auto model_transform = frame->get_transform();
		mesh->shader()->assert_set_uniform("model",
		                                   lak::as_bytes(&model_transform));
		mesh->draw();
	}
};

struct vertex
{
	glm::vec4 pos;
	glm::vec4 col;
	glm::vec3 norm;
	glm::vec2 tex_coord;

	static lak::array<lak::opengl::vertex_attribute> attributes()
	{
		return lak::array<lak::opengl::vertex_attribute>{
		  {
		    .size       = 4,
		    .type       = GL_FLOAT,
		    .normalised = GL_FALSE,
		    .stride     = sizeof(vertex),
		    .offset     = offsetof(vertex, pos),
		    .divisor    = 0,
		  },
		  {
		    .size       = 4,
		    .type       = GL_FLOAT,
		    .normalised = GL_FALSE,
		    .stride     = sizeof(vertex),
		    .offset     = offsetof(vertex, col),
		    .divisor    = 0,
		  },
		  {
		    .size       = 3,
		    .type       = GL_FLOAT,
		    .normalised = GL_FALSE,
		    .stride     = sizeof(vertex),
		    .offset     = offsetof(vertex, norm),
		    .divisor    = 0,
		  },
		  {
		    .size       = 2,
		    .type       = GL_FLOAT,
		    .normalised = GL_FALSE,
		    .stride     = sizeof(vertex),
		    .offset     = offsetof(vertex, tex_coord),
		    .divisor    = 0,
		  }};
	}

	static lak::array<lak::opengl::location, 4U> attribute_indices(
	  const lak::opengl::program &shader,
	  const GLchar *pos_name,
	  const GLchar *col_name,
	  const GLchar *norm_name,
	  const GLchar *tex_coord_name)
	{
		return lak::array<lak::opengl::location, 4U>{
		  shader.assert_attrib_location(pos_name),
		  shader.assert_attrib_location(col_name),
		  shader.assert_attrib_location(norm_name),
		  shader.assert_attrib_location(tex_coord_name),
		};
	}
};

lak::shared_ptr<lak::opengl::static_object_part> make_mesh(
  lak::span<const vertex> vertices,
  GLenum draw_mode,
  lak::opengl::shared_program shader,
  lak::shared_ptr<lak::opengl::texture> albedo)
{
	auto buffer = lak::opengl::shared_vertex_buffer::make(
	  lak::opengl::vertex_buffer::create());
	buffer->bind()
	  .set_data(vertices, vertices.size(), draw_mode)
	  .set_vertex_attributes(vertex::attributes());
	return lak::shared_ptr<lak::opengl::static_object_part>::make(
	  lak::opengl::static_object_part::create(
	    buffer,
	    shader,
	    vertex::attribute_indices(
	      *shader, "vPosition", "vColor", "vNormal", "vTexCoord"),
	    {{albedo, shader->assert_uniform_location("albedo")}}));
}

lak::array<vertex> load_model(const lak::obj::obj &obj)
{
	lak::array<vertex> result;
	for (const auto &face : obj.faces)
	{
		face.visit_fan(
		  obj.vertex_coords,
		  obj.texture_coords,
		  obj.vertex_normals,
		  obj.face_coords,
		  [&](const lak::obj::vertex_coord &v,
		      const lak::obj::texture_coord *vt,
		      const lak::obj::vertex_normal *vn)
		  {
			  result.push_back(vertex{
			    .pos  = glm::vec4{float(v.x), float(v.z), float(v.y), float(v.w)},
			    .col  = glm::vec4{1.0, 1.0, 1.0, 1.0},
			    .norm = vn ? glm::vec3{float(vn->x), float(vn->z), float(vn->y)}
			               : glm::vec3{0.0},
			    // lak::image is top-left origin, opengl is bottom-left
			    .tex_coord =
			      vt ? glm::vec2{float(vt->u), -float(vt->v)} : glm::vec2{0.0},
			  });
		  });
	}
	return result;
}

lak::array<vertex> load_model(lak::u8string_view file)
{
	auto obj = lak::dsl::obj.parse(file).EXPECT("failed to read obj").value;
	return load_model(obj);
}

lak::image3_t load_texture3(lak::span<const byte_t> file)
{
	lak::binary_reader strm{file};
	auto pnm = strm.read<lak::pnm::pnm>().EXPECT("failed to read pnm");
	return static_cast<lak::image3_t>(pnm);
}

lak::opengl::texture load_opengl_texture(const lak::image3_t &img)
{
	lak::opengl::texture tex(GL_TEXTURE_2D);
	tex.bind()
	  .apply(GL_TEXTURE_WRAP_S, GL_REPEAT)
	  .apply(GL_TEXTURE_WRAP_T, GL_REPEAT)
	  .apply(GL_TEXTURE_MIN_FILTER, GL_LINEAR)
	  .apply(GL_TEXTURE_MAG_FILTER, GL_NEAREST)
	  .build(0,
	         GL_RGB,
	         (lak::vec2<GLsizei>)img.size(),
	         0,
	         GL_RGB,
	         GL_UNSIGNED_BYTE,
	         img.data());
	return tex;
}

struct scene
{
	lak::shared_ptr<lak::opengl::program> shader;
	lak::shared_ptr<reference_frame> world;
	lak::shared_ptr<reference_frame> player;
	lak::shared_ptr<reference_frame> cameraBoom;
	::camera camera;
	lak::array<light> lights;
	lak::array<model> blocks;
	lak::array<model> coins;
	lak::array<model> coins_reset;
	model ball;
};

std::atomic_bool assets_loaded = false;

lak::image3_t ball_texture;
lak::array<vertex> ball_vertices;
lak::image3_t cube_texture;
lak::array<vertex> cube_vertices;
lak::image3_t coin_texture;
lak::array<vertex> coin_vertices;
lak::image3_t map_texture;

lak::optional<std::thread> asset_loader;

namespace bga
{
	constexpr uint8_t ball_ppm[] = {
#embed "assets/ball.ppm"
	};
	constexpr uint8_t coin_ppm[] = {
#embed "assets/coin.ppm"
	};
	constexpr uint8_t cube_ppm[] = {
#embed "assets/cube.ppm"
	};
	constexpr uint8_t map_ppm[] = {
#embed "assets/map.ppm"
	};

	constexpr char8_t ball_obj[] = {
#embed "assets/ball.obj" suffix(, )
	  0};
	constexpr char8_t coin_obj[] = {
#embed "assets/coin.obj" suffix(, )
	  0};
	constexpr char8_t cube_obj[] = {
#embed "assets/cube.obj" suffix(, )
	  0};

	constexpr char vshader[] = {
#embed "assets/vert.glsl" suffix(, )
	  0};
	constexpr char fshader[] = {
#embed "assets/frag.glsl" suffix(, )
	  0};
}

void load_assets()
{
	lak::fs::path assets_dir = "assets";

	ball_texture = load_texture3(
	  lak::span<const byte_t>(lak::span<const uint8_t>(bga::ball_ppm)));
	ball_vertices = load_model(lak::u8string_view::from_c_str(bga::ball_obj));

	cube_texture = load_texture3(
	  lak::span<const byte_t>(lak::span<const uint8_t>(bga::cube_ppm)));
	cube_vertices = load_model(lak::u8string_view::from_c_str(bga::cube_obj));

	coin_texture = load_texture3(
	  lak::span<const byte_t>(lak::span<const uint8_t>(bga::coin_ppm)));
	coin_vertices = load_model(lak::u8string_view::from_c_str(bga::coin_obj));

	map_texture = load_texture3(
	  lak::span<const byte_t>(lak::span<const uint8_t>(bga::map_ppm)));

	assets_loaded = true;
}

struct game_window : virtual public LAK_BASIC_PROGRAM(window_api)
{
	game_window() : LAK_BASIC_PROGRAM(window_api)() {}

	state_t state = LOADING;

	scene sc;

	bool init_game_state()
	{
		if (assets_loaded)
		{
			if (asset_loader)
			{
				asset_loader->join();
				asset_loader.reset();
			}

			{
				auto vshader =
				  lak::opengl::shader::create(bga::vshader, GL_VERTEX_SHADER).UNWRAP();
				auto fshader =
				  lak::opengl::shader::create(bga::fshader, GL_FRAGMENT_SHADER)
				    .UNWRAP();
				sc.shader =
				  lak::opengl::program::create_shared(vshader, fshader).UNWRAP();
				sc.shader->use().UNWRAP();
			}

			sc.world      = lak::shared_ptr<reference_frame>::make();
			sc.player     = sc.world->add_child();
			sc.cameraBoom = sc.player->add_child();

			sc.camera = camera{.frame = sc.cameraBoom->add_child()};

			sc.cameraBoom->rotation.value.x      = 0.58f;
			sc.camera.frame->translation.value.y = 2.2f;
			sc.camera.frame->translation.value.z = 0.7f;

			{
				auto albedo = lak::shared_ptr<lak::opengl::texture>::make(
				  load_opengl_texture(ball_texture));

				sc.ball = model{
				  .frame = sc.player->add_child(),
				  .mesh  = make_mesh(ball_vertices, GL_TRIANGLES, sc.shader, albedo),
				};
			}

			{
				auto albedo = lak::shared_ptr<lak::opengl::texture>::make(
				  load_opengl_texture(cube_texture));

				auto obj_part =
				  make_mesh(cube_vertices, GL_TRIANGLES, sc.shader, albedo);

				sc.blocks.clear();
				sc.blocks.reserve(map_texture.size().x * map_texture.size().y);
				for (size_t x = 0; x < map_texture.size().x; x++)
				{
					for (size_t y = 0; y < map_texture.size().y; y++)
					{
						if (map_texture[{x, y}].r > 0)
						{
							auto &block = sc.blocks.push_back(model{
							  .frame = lak::shared_ptr<reference_frame>::make(),
							  .mesh  = obj_part,
							});

							block.frame->translation.value = {x * 2.0f, y * -2.0f, -2.0f};
						}
					}
				}
			}

			{
				auto albedo = lak::shared_ptr<lak::opengl::texture>::make(
				  load_opengl_texture(coin_texture));

				auto obj_part =
				  make_mesh(coin_vertices, GL_TRIANGLES, sc.shader, albedo);

				sc.coins.clear();
				sc.coins.reserve(map_texture.size().x * map_texture.size().y);
				for (size_t x = 0; x < map_texture.size().x; x++)
				{
					for (size_t y = 0; y < map_texture.size().y; y++)
					{
						if (map_texture[{x, y}].g > 0)
						{
							auto &coin = sc.coins.push_back(model{
							  .frame = lak::shared_ptr<reference_frame>::make(),
							  .mesh  = obj_part,
							});

							coin.frame->translation.value   = {x * 2.0f, y * -2.0f, 0.0f};
							coin.frame->rotation.velocity.z = 1.0f;
						}
					}
				}
				sc.coins_reset = sc.coins;
			}

			{
				sc.lights.clear();
				for (size_t x = 0; x < map_texture.size().x; x++)
				{
					for (size_t y = 0; y < map_texture.size().y; y++)
					{
						if (map_texture[{x, y}].b > 0)
						{
							auto &li = sc.lights.push_back(light{
							  .frame  = lak::shared_ptr<reference_frame>::make(),
							  .colour = {0.5f, 0.5f, 0.5f, 1.0f},
							});

							li.frame->translation.value = {x * 2.0f, y * -2.0f, 2.0f};
						}
					}
				}

				size_t lightCount = std::min<size_t>(6U, sc.lights.size());
				sc.shader->assert_set_uniform("lightCount",
				                              lak::as_bytes(&lightCount));

				for (size_t i = 0; i < lightCount; i++)
				{
					auto lightname = "lights["_str + (char)('0' + i) + "]"_str;
					sc.shader->assert_set_uniform(
					  (lightname + ".position").c_str(),
					  lak::as_bytes(&sc.lights[i].frame->translation.value));
					sc.shader->assert_set_uniform((lightname + ".color").c_str(),
					                              lak::as_bytes(&sc.lights[i].colour));
				}

				auto temp_vec = glm::vec4(0.1f, 0.1f, 0.1f, 1.0f);
				sc.shader->assert_set_uniform("ambient", lak::as_bytes(&temp_vec));
				temp_vec = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				sc.shader->assert_set_uniform("diffuse", lak::as_bytes(&temp_vec));
				temp_vec = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
				sc.shader->assert_set_uniform("specular", lak::as_bytes(&temp_vec));
				float shininess = 100.0f;
				sc.shader->assert_set_uniform("shininess", lak::as_bytes(&shininess));
				auto temp_mat = glm::mat4(1.0f);
				sc.shader->assert_set_uniform("model", lak::as_bytes(&temp_mat));
			}

			return true;
		}

		if (!asset_loader)
		{
			asset_loader = std::thread(load_assets);
		}

		return false;
	}

	virtual void init() override final
	{
		instance_base().clear_colour = {0.0f, 0.3125f, 0.3125f, 1.0f};
	}

	virtual ~game_window() {}

	virtual void handle_event(lak::event &event) override final
	{
		switch (event.type)
		{
			case lak::event_type::window_closed: break;

			case lak::event_type::close_window: destroy(); break;

			case lak::event_type::key_down:
				switch (event.key().key)
				{
					case lak::key_code::right:
						sc.player->rotation.velocity.z = -2;
						break;
					case lak::key_code::left: //
						sc.player->rotation.velocity.z = 2;
						break;
					case lak::key_code::down:
						sc.ball.frame->rotation.acceleration.x = -3;
						break;
					case lak::key_code::up:
						sc.ball.frame->rotation.acceleration.x = 3;
						break;

					default: break;
				}
				break;

			case lak::event_type::key_up:
				switch (event.key().key)
				{
					case lak::key_code::right:
					case lak::key_code::left: //
						sc.player->rotation.velocity.z = 0;
						break;
					case lak::key_code::down:
					case lak::key_code::up:
						sc.ball.frame->rotation.acceleration.x = 0;
						break;

					default: break;
				}
				break;

			default: break;
		}
	}

	virtual void loop(uint64_t counter_delta) override final
	{
		const float frame_time =
		  (float)counter_delta / lak::performance_frequency();

		ImGuiIO &io = ImGui::GetIO();

		bool mainOpen = true;

		if (state == state_t::RUNNING)
			ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
		else
			ImGui::SetNextWindowPos(
			  ImVec2(io.DisplaySize.x * 0.5f, io.DisplaySize.y * 0.5f),
			  ImGuiCond_Always,
			  ImVec2(0.5f, 0.5f));
		ImGui::Begin(APP_NAME,
		             &mainOpen,
		             ImGuiWindowFlags_AlwaysAutoResize |
		               ImGuiWindowFlags_NoScrollbar |
		               ImGuiWindowFlags_NoSavedSettings |
		               ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove);

		switch (state)
		{
			case state_t::LOADING:
			{
				ImGui::Text("Loading...");
				if (init_game_state()) state = state_t::RUNNING;
				ImGui::End();
				return;
			}
			break;

			case state_t::RUNNING:
			{
				sc.player->translation.velocity.x =
				  std::sin(sc.player->rotation.value.z) *
				  sc.ball.frame->rotation.velocity.x;

				sc.player->translation.velocity.y =
				  -std::cos(sc.player->rotation.value.z) *
				  sc.ball.frame->rotation.velocity.x;

				sc.world->update(frame_time);
				sc.player->update(frame_time);
				sc.cameraBoom->update(frame_time);
				sc.camera.frame->update(frame_time);

				for (auto &light : sc.lights) light.frame->update(frame_time);
				for (auto &block : sc.blocks) block.frame->update(frame_time);
				for (auto &coin : sc.coins) coin.frame->update(frame_time);

				sc.ball.frame->update(frame_time);

				auto player_world_pos = sc.player->total_translation();
				for (auto it = sc.coins.begin(); it != sc.coins.end();)
				{
					glm::vec3 dist = it->frame->total_translation() - player_world_pos;
					float dst      = std::sqrt((dist.x * dist.x) + (dist.y * dist.y));

					if (dst < 1.0f)
						it = sc.coins.erase(it);
					else
						++it;
				}
				if (sc.coins.empty()) state = WIN;

				bool onTrack = false;
				for (auto &block : sc.blocks)
				{
					glm::vec3 dist = block.frame->total_translation() - player_world_pos;
					onTrack |= std::abs(dist.x) <= 1.0f && std::abs(dist.y) <= 1.0f;
				}
				if (!onTrack) state = LOSS;

				ImGui::Text("Score");
				ImGui::Text("%zu/%zu",
				            sc.coins_reset.size() - sc.coins.size(),
				            sc.coins_reset.size());
			}
			break;

			case state_t::WIN:
			{
				ImGui::Text("YOUR'RE WINNER !");
				if (ImGui::Button("restart"))
				{
					sc.player->translation.value     = glm::vec3(0);
					sc.player->translation.velocity  = glm::vec3(0);
					sc.player->rotation.value        = glm::vec3(0);
					sc.player->rotation.velocity     = glm::vec3(0);
					sc.ball.frame->rotation.value    = glm::vec3(0);
					sc.ball.frame->rotation.velocity = glm::vec3(0);
					sc.coins                         = sc.coins_reset;
					state                            = RUNNING;
				}
			}
			break;

			case state_t::LOSS:
			{
				ImGui::Text("you fell off :(");
				if (ImGui::Button("try again"))
				{
					sc.player->translation.value     = glm::vec3(0);
					sc.player->translation.velocity  = glm::vec3(0);
					sc.player->rotation.value        = glm::vec3(0);
					sc.player->rotation.velocity     = glm::vec3(0);
					sc.ball.frame->rotation.value    = glm::vec3(0);
					sc.ball.frame->rotation.velocity = glm::vec3(0);
					sc.coins                         = sc.coins_reset;
					state                            = RUNNING;
				}
			}
			break;
		}

		{
			auto projview    = sc.camera.update_projview(window());
			auto invprojview = glm::transpose(glm::inverse(projview));
			sc.shader->assert_set_uniform("projview", lak::as_bytes(&projview));
			sc.shader->assert_set_uniform("invprojview",
			                              lak::as_bytes(&invprojview));
		}

		lak::opengl::enable_if(GL_BLEND, true).UNWRAP();
		lak::opengl::call_checked(
		  glBlendEquationSeparate, GL_FUNC_ADD, GL_FUNC_ADD)
		  .UNWRAP();
		lak::opengl::call_checked(glBlendFuncSeparate,
		                          GL_SRC_ALPHA,
		                          GL_ONE_MINUS_SRC_ALPHA,
		                          GL_SRC_ALPHA,
		                          GL_ONE_MINUS_SRC_ALPHA)
		  .UNWRAP();

		lak::opengl::enable_if(GL_DEPTH_TEST, true).UNWRAP();
		lak::opengl::call_checked(glDepthFunc, GL_LESS).UNWRAP();
		lak::opengl::call_checked(glDepthRange, GLdouble(0.0), GLdouble(1.0))
		  .UNWRAP();

		lak::opengl::enable_if(GL_CULL_FACE, false).UNWRAP();

		lak::opengl::enable_if(GL_SCISSOR_TEST, false).UNWRAP();

		lak::opengl::call_checked(glViewport,
		                          static_cast<GLint>(0),
		                          static_cast<GLint>(0),
		                          static_cast<GLsizei>(window().drawable_size().x),
		                          static_cast<GLsizei>(window().drawable_size().y))
		  .UNWRAP();

		sc.ball.draw();
		for (auto &it : sc.blocks) it.draw();
		for (auto &it : sc.coins) it.draw();

		ImGui::End();
	}
};

lak::graphics_mode forced_graphics_mode = lak::graphics_mode::None;

lak::error_code<int> LAK_BASIC_PROGRAM(program_preinit)(lak::span<char *> args)
{
	if (args.size() >= 2U)
	{
		// if (args[1] == "--software"_str)
		// {
		// 	forced_graphics_mode = lak::graphics_mode::Software;
		// }
		// else
		if (args[1] == "--opengl"_str)
		{
			forced_graphics_mode = lak::graphics_mode::OpenGL;
		}
		// else if (args[1] == "--cobalt"_str)
		// {
		// 	forced_graphics_mode = lak::graphics_mode::Cobalt;
		// }
	}

	return lak::ok_t{};
}

lak::weak_ptr<LAK_BASIC_PROGRAM(window_instance<game_window>)> game_window_ptr;

lak::error_code<int> LAK_BASIC_PROGRAM(program_init)()
{
	basic_window_target_framerate                = 60;
	basic_window_opengl_settings.major           = 3;
	basic_window_opengl_settings.minor           = 2;
	basic_window_opengl_settings.double_buffered = true;

	auto map_str_err = [](lak::u8string err) -> int
	{
		ERROR(err);
		return EXIT_FAILURE;
	};

	switch (forced_graphics_mode)
	{
		case lak::graphics_mode::None:
		{
			RES_TRY_ASSIGN(
			  game_window_ptr =,
			  LAK_BASIC_PROGRAM(create_window<game_window>)().map_err(map_str_err));
		}
		break;
		// #ifdef LAK_ENABLE_SOFTRENDER
		// 		case lak::graphics_mode::Software:
		// 		{
		// 			RES_TRY_ASSIGN(game_window_ptr =,
		// 			               LAK_BASIC_PROGRAM(create_window<game_window>)(
		// 			                 LAK_BASIC_PROGRAM(window_software_settings))
		// 			                 .map_err(map_str_err));
		// 		}
		// 		break;
		// #endif
#ifdef LAK_ENABLE_OPENGL
		case lak::graphics_mode::OpenGL:
		{
			RES_TRY_ASSIGN(game_window_ptr =,
			               LAK_BASIC_PROGRAM(create_window<game_window>)(
			                 LAK_BASIC_PROGRAM(window_opengl_settings))
			                 .map_err(map_str_err));
		}
		break;
#endif
			// #ifdef LAK_ENABLE_COBALT
			// 		case lak::graphics_mode::Cobalt:
			// 		{
			// 			RES_TRY_ASSIGN(game_window_ptr =,
			// 			               LAK_BASIC_PROGRAM(create_window<game_window>)(
			// 			                 LAK_BASIC_PROGRAM(window_cobalt_settings))
			// 			                 .map_err(map_str_err));
			// 		}
			// 		break;
			// #endif
		default:
			ERROR(
			  lak::fmt<u8"Graphics mode {} not available">(forced_graphics_mode));
			return lak::err_t{EXIT_FAILURE};
	}

	DEBUG_EXPR(game_window_ptr.get()->window().graphics());

	return lak::ok_t{};
}

void LAK_BASIC_PROGRAM(program_handle_event)(lak::event &event)
{
	switch (event.type)
	{
		case lak::event_type::quit_program:
			for (auto &inst : LAK_BASIC_PROGRAM(window_instances)()) inst->destroy();
			break;
		default: break;
	}
}

bool LAK_BASIC_PROGRAM(program_loop)(uint64_t counter_delta)
{
	LAK_UNUSED(counter_delta);
	return !LAK_BASIC_PROGRAM(window_instances)().empty();
}

int LAK_BASIC_PROGRAM(program_quit)()
{
	game_window_ptr.reset();
	return EXIT_SUCCESS;
}
