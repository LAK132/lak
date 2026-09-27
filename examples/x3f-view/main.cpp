#define APP_NAME "X3F viewer"

#define LAK_BASIC_PROGRAM_IMGUI_IMPL

#define LAK_BASIC_PROGRAM_IMGUI_WINDOW_IMPL

#include <lak/basic_program.inl>

#include <lak/string_literals/span.hpp>
#include <lak/string_literals/string.hpp>
#include <lak/string_utils.hpp>

#include <lak/file/x3f.hpp>
#include <lak/system/file.hpp>

#include <lak/imgui/texture.hpp>

#include <imgui_memory_editor.h>
#include <misc/cpp/imgui_stdlib.h>

lak::fs::path init_file;

struct x3f_data
{
	lak::fs::path source_file;
	lak::array<byte_t> source_data;
	lak::x3f::x3f raw_image;
	lak::ImUniqueTexture image;
	lak::vec2s_t image_size;
	lak::span<byte_t> right_data          = {};
	lak::optional<lak::vec2s_t> highlight = lak::nullopt;

	x3f_data(lak::fs::path srcf, lak::array<byte_t> &&src, lak::x3f::x3f &&raw)
	: source_file(lak::exchange(srcf, {})),
	  source_data(lak::exchange(src, {})),
	  raw_image(lak::exchange(raw, {}))
	{
		right_data = lak::span(source_data);
	}
	x3f_data()                            = default;
	x3f_data(x3f_data &&)                 = default;
	x3f_data &operator=(x3f_data &&)      = default;
	x3f_data(const x3f_data &)            = delete;
	x3f_data &operator=(const x3f_data &) = delete;

	void view_data(lak::span<byte_t> d,
	               lak::optional<lak::vec2s_t> h = lak::nullopt)
	{
		image.reset();
		right_data = d;
		highlight  = lak::move(h);
	}

	void view_image(lak::image<lak::vec4u16_t> &img)
	{
		image.emplace(img);
		image_size = img.size();
	}

	void view()
	{
		if (ImGui::Button("View All")) view_data(lak::span(source_data));

		LAK_TREE_NODE("X3F")
		{
			LAK_TREE_NODE("Header")
			{
				lak::Text<u8"Magic: {}">(
				  lak::astring_view(lak::span(raw_image.header.section.fourcc)));
				lak::Text<u8"Version: {}">(raw_image.header.section.version);
				raw_image.header.versioned.visit(lak::overloaded{
				  [&](const lak::x3f::header_1_0 &h)
				  {
					  lak::Text<u8"Mark Bits: {:#X}">(h.mark_bits);
					  lak::Text<u8"Width: {}">(h.image_columns);
					  lak::Text<u8"Height: {}">(h.image_rows);
					  lak::Text<u8"Rotation: {}">(h.rotation);
				  },
				  [&](const lak::x3f::header_2_0 &h)
				  {
					  lak::Text<u8"Mark Bits: {:#X}">(h.mark_bits);
					  lak::Text<u8"Width: {}">(h.image_columns);
					  lak::Text<u8"Height: {}">(h.image_rows);
					  lak::Text<u8"Rotation: {}">(h.rotation);
					  lak::Text<u8"White Balance: {}">(
					    lak::astring_view(lak::span(h.white_balance)));
					  LAK_TREE_NODE("Extended data")
					  {
						  for (size_t i = 0U; i < h.extended_data.size(); ++i)
						  {
							  lak::Text<u8"{}: {:#X}">(h.extended_data_types[i],
							                           h.extended_data[i]);
						  }
					  }
				  },
				  [&](const lak::x3f::header_4_0 &h)
				  {
					  lak::Text<u8"Mark Bits: {:#X}">(h.mark_bits);
					  lak::Text<u8"Width: {}">(h.image_columns);
					  lak::Text<u8"Height: {}">(h.image_rows);
					  lak::Text<u8"Rotation: {}">(h.rotation);
				  },
				});
			}
			LAK_TREE_NODE("Directory")
			{
				lak::Text<u8"Magic: {}">(
				  lak::astring_view(lak::span(raw_image.directory.section.fourcc)));
				lak::Text<u8"Version: {}">(raw_image.directory.section.version);
				raw_image.directory.versioned.visit(lak::overloaded{
				  [&](const lak::x3f::directory_2_0 &d)
				  {
					  LAK_TREE_NODE2("Entries ({})", d.entries.size())
					  {
						  size_t entry_i = 0;
						  for (const auto &e : d.entries)
						  {
							  LAK_TREE_NODE2("Entry {}", entry_i)
							  {
								  lak::Text<u8"Offset: {} ({0:#X})">(e.offset);
								  lak::Text<u8"Size: {} ({0:#X})">(e.size);
								  lak::Text<u8"Type: {}">(
								    lak::astring_view(lak::span(e.type)));
								  if (ImGui::Button("View"))
									  view_data(lak::span(source_data),
									            {lak::in_place, e.offset, e.size});
							  }
							  ++entry_i;
						  }
					  }
				  }});
			}
			LAK_TREE_NODE2("Image Entries ({})", raw_image.image_entries.size())
			{
				size_t img_i = 0;
				for (auto &e : raw_image.image_entries)
				{
					LAK_TREE_NODE2("Entry {}", img_i)
					{
						lak::Text<u8"Magic: {}">(
						  lak::astring_view(lak::span(e.section.fourcc)));
						lak::Text<u8"Version: {}">(e.section.version);
						lak::Text<u8"Data Origin: {} ({0:#X})">(e.offset);
						lak::Text<u8"Data Size: {} ({0:#X})">(e.data.size());
						if (ImGui::Button("Goto"))
							view_data(lak::span(source_data), {lak::in_place, e.offset, 1U});
						ImGui::SameLine();
						if (ImGui::Button("View")) view_data(lak::span(e.data));
						e.versioned.visit(
						  lak::overloaded{[&](const lak::x3f::image_data_2_0 &i)
						                  {
							                  lak::Text<u8"Type: {}">(i.type);

							                  lak::Text<u8"Format: {}">(i.format);

							                  lak::Text<u8"Width: {} ({0:#X})">(i.columns);
							                  lak::Text<u8"Height: {} ({0:#X})">(i.rows);
							                  lak::Text<u8"Bytes: {} ({0:#X})">(i.row_bytes);
						                  }});
						if (!e.data.empty())
							if (ImGui::Button("View Image Data"))
								view_data(lak::span<byte_t>(lak::span(e.data)));
						if (e.image.contig_size() != 0U)
							if (ImGui::Button("View Image")) view_image(e.image);
					}
					++img_i;
				}
			}
			LAK_TREE_NODE2("CAMF Entries ({})", raw_image.camf_entries.size())
			{
				size_t img_i = 0;
				for (auto &e : raw_image.camf_entries)
				{
					LAK_TREE_NODE2("Entry {}", img_i)
					{
						lak::Text<u8"Magic: {}">(
						  lak::astring_view(lak::span(e.section.fourcc)));
						lak::Text<u8"Version: {}.{}">(e.section.version.major,
						                              e.section.version.minor);
						lak::Text<u8"Type: {}">(e.header.type);
						lak::Text<u8"Decompressed: {} ({0:#X})">(e.header.decompressed);
						lak::Text<u8"Seed: {} ({0:#X}), {} ({1:#X})">(e.header.seed[0],
						                                              e.header.seed[1]);
						lak::Text<u8"Width: {} ({0:#X})">(e.header.columns);
						lak::Text<u8"Height: {} ({0:#X})">(e.header.rows);
						lak::Text<u8"Data Size: {} ({0:#X})">(e.data.size());
						if (ImGui::Button("View")) view_data(lak::span(e.data));
						LAK_TREE_NODE("Entries")
						{
							size_t ent_i = 0U;
							for (auto &ee : e.entries)
							{
								LAK_TREE_NODE2("{}: {}", ent_i, ee.name)
								{
									if (ImGui::Button("View")) view_data(lak::span(ee.source));
									ee.data.visit(lak::overloaded{
									  [](const lak::monostate &)
									  { ImGui::Text("Invalid type"); },
									  [](const lak::astring &str)
									  {
										  ImGui::Text("String");
										  lak::Text<u8"\"{}\"">(str);
									  },
									  [&](lak::array<lak::x3f::camf_prop> &props)
									  {
										  ImGui::Text("Props");

										  size_t prop_i = 0U;
										  for (auto &p : props)
										  {
											  ImGui::PushID(&p);
											  DEFER(ImGui::PopID());
											  lak::Text<u8"{}: {}">(prop_i, p.name);
											  ImGui::SameLine();
											  if (ImGui::SmallButton("View"))
												  view_data(lak::span(p.data));
											  ++prop_i;
										  }
									  },
									  [](const lak::x3f::camf_matrix &mat)
									  {
										  ImGui::Text("Matrix");
										  mat.data.visit(lak::overloaded{
										    [](const lak::monostate &)
										    { ImGui::Text("Invalid type"); },
										    []<typename T>(
										      const lak::x3f::camf_matrix::matrix_type<T> &m)
										    {
											    lak::array<lak::u8string> strs;
											    strs.reserve(m.dimensions.size());

											    for (const auto &d : m.dimensions)
												    strs.push_back(lak::fmt<u8"{:d}">(d));
											    lak::Text<u8"Dim: {}">(lak::join_strings(
											      u8"x"_view, lak::span<const lak::u8string>(strs)));

											    LAK_TREE_NODE("Data")
											    {
												    strs.clear();
												    strs.reserve(m.data.size());

												    for (const auto &d : m.data)
													    strs.push_back(lak::fmt<u8"{}">(d));
												    lak::Text<u8"{{{}}}">(lak::join_strings(
												      u8", "_view,
												      lak::span<const lak::u8string>(strs)));
											    }
										    },
										  });
									  },
									});
								}
								++ent_i;
							}
						}
					}
					++img_i;
				}
			}
			LAK_TREE_NODE2("PROP Entries ({})", raw_image.prop_entries.size())
			{
				size_t img_i = 0;
				for (auto &e : raw_image.prop_entries)
				{
					LAK_TREE_NODE2("Entry {}", img_i)
					{
						lak::Text<u8"Magic: {}">(
						  lak::astring_view(lak::span(e.section.fourcc)));
						lak::Text<u8"Version: {}.{}">(e.section.version.major,
						                              e.section.version.minor);
						if (ImGui::Button("View")) view_data(lak::span(e.data));
						LAK_TREE_NODE2("Entries {}", e.entries.size())
						{
							for (const auto &ee : e.entries)
							{
								lak::Text<u8"{}: {}">(ee.name, ee.string_data());
							}
						}
					}
					++img_i;
				}
			}
		}
	}
};

struct my_window : public virtual LAK_BASIC_PROGRAM(window_api)
{
	lak::fs::path working;
	lak::path_getter pgetter;
	lak::optional<lak::fs::path> open_file;
	MemoryEditor mem_edit;

	lak::optional<x3f_data> data;

	float _left_size   = -1.f;
	float _right_size  = -1.f;
	float _image_scale = 1.f;

	my_window() : LAK_BASIC_PROGRAM(window_api)()
	{
		//
	}

	virtual void init() override final
	{
		working = lak::fs::current_path();
		if (!init_file.empty()) open_file = lak::exchange(init_file, {});
	}

	virtual ~my_window() { data.reset(); }

	virtual void handle_event(lak::event &event) override final
	{
		switch (event.type)
		{
			case lak::event_type::close_window: destroy(); break;
			case lak::event_type::dropfile:
			{
				open_file = event.dropfile().path;
				data.reset();
			}
			break;
		}
	}

	void menu()
	{
		if (ImGui::Button("Open X3F")) pgetter.open_file(working, "X3F{.X3F}");
	}

	void left_region() { data->view(); }

	void right_region()
	{
		if (data->image)
		{
			ImGui::SliderFloat("Size", &_image_scale, 0.1f, 10.f);
			ImGui::Image(data->image.get().GetTexID(),
			             {float(data->image_size.x) * _image_scale,
			              float(data->image_size.y) * _image_scale});
		}
		else
		{
			mem_edit.DrawContents(data->right_data.data(), data->right_data.size());
			if (data->highlight)
			{
				mem_edit.GotoAddrAndHighlight(data->highlight->x, data->highlight->y);
				data->highlight.reset();
			}
		}
	}

	virtual void loop(uint64_t counter_delta) override final
	{
		if (ImGui::BeginMenuBar())
		{
			menu();
			ImGui::EndMenuBar();
		}
		if_let_some (auto pget, pgetter())
		{
			open_file = lak::move(pget);
			data.reset();
		}
		if (!data && open_file)
		{
			auto file = lak::read_file(*open_file).UNWRAP();
			auto path = *lak::exchange(open_file, lak::nullopt);
			lak::binary_reader strm{lak::span(file)};
			auto raw = strm.read_le<lak::x3f::x3f>().UNWRAP();
			data.emplace<x3f_data>({path, lak::move(file), lak::move(raw)});
			window().set_title(lak::fmt<L"X3F viewer - {}">(path.generic_wstring()));
		}

		if (!data) return;

		const auto content_size{ImGui::GetContentRegionAvail()};

		if (_left_size <= 0.f || _right_size <= 0.f)
		{
			_left_size  = content_size.x / 2;
			_right_size = content_size.x / 2;
		}

		lak::VertSplitter(_left_size, _right_size, content_size.x);

		if (ImGui::BeginChild("left",
		                      ImVec2(_left_size, -1),
		                      ImGuiChildFlags_Borders,
		                      ImGuiWindowFlags_NoSavedSettings))
		{
			left_region();
		}
		ImGui::EndChild();

		ImGui::SameLine();

		if (ImGui::BeginChild("right",
		                      ImVec2(_right_size, -1),
		                      ImGuiChildFlags_Borders,
		                      ImGuiWindowFlags_NoSavedSettings))
		{
			right_region();
		}
		ImGui::EndChild();
	}
};

lak::error_code<int> LAK_BASIC_PROGRAM(program_preinit)(lak::span<char *> args)
{
	if (args.size() >= 2)
	{
		init_file = args[1];
	}

	return lak::ok_t{};
}

lak::weak_ptr<LAK_BASIC_PROGRAM(window_instance<my_window>)> my_window_ptr;

lak::error_code<int> LAK_BASIC_PROGRAM(program_init)()
{
	auto map_str_err = [](lak::u8string err) -> int
	{
		ERROR(err);
		return EXIT_FAILURE;
	};

	RES_TRY_ASSIGN(
	  my_window_ptr =,
	  LAK_BASIC_PROGRAM(create_window<my_window>)().map_err(map_str_err));

	return lak::ok_t{};
}

void LAK_BASIC_PROGRAM(program_handle_event)(lak::event &event)
{
	switch (event.type)
	{
		case lak::event_type::quit_program:
			if (auto wnd = my_window_ptr.get(); wnd) wnd->destroy();
			my_window_ptr.reset();
			break;
	}
}

bool LAK_BASIC_PROGRAM(program_loop)(uint64_t counter_delta)
{
	return !LAK_BASIC_PROGRAM(window_instances)().empty();
}

int LAK_BASIC_PROGRAM(program_quit)()
{
	my_window_ptr.reset();
	return EXIT_SUCCESS;
}
