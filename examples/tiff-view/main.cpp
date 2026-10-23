#define APP_NAME "tiff-view"

#define LAK_BASIC_PROGRAM_IMGUI_IMPL

#define LAK_BASIC_PROGRAM_IMGUI_WINDOW_IMPL

#include <lak/basic_program.inl>

#include <lak/imgui/widgets.hpp>

#include <lak/file/tiff.hpp>
#include <lak/optional.hpp>
#include <lak/string_literals/string.hpp>
#include <lak/string_literals/view.hpp>
#include <lak/system/file.hpp>

#include <filesystem>

lak::optional<std::filesystem::path> init_file;

using load_error  = lak::variant<std::error_code,
                                 lak::err::out_of_data,
                                 lak::err::value_out_of_range>;
using load_result = lak::result<lak::tiff::tiff, load_error>;

load_result load_tiff(lak::fs::path path)
{
	RES_TRY_ASSIGN(
	  auto f =,
	  lak::read_file(path).map_err([](auto err) -> load_error { return err; }));
	RES_TRY_ASSIGN(
	  auto tiff =,
	  lak::from_bytes<lak::tiff::tiff, lak::endian::little>(f).map_err(
	    [](auto err) -> load_error { return err; }));
	return lak::move_ok(tiff.first);
}

struct my_window : virtual public LAK_BASIC_PROGRAM(window_api)
{
	my_window() : LAK_BASIC_PROGRAM(window_api)() {}

	lak::optional<std::filesystem::path> openfile;
	lak::optional<std::filesystem::path> loadfile;
	lak::path_getter pget_open;
	lak::await<load_result> loader;
	lak::optional<lak::tiff::tiff> tiff;

	lak::array<byte_t> mem_edit_data;
	lak::optional<lak::span<byte_t>> mem_edit_highlight;
	MemoryEditor mem_edit;

	float _left_size  = -1.f;
	float _right_size = -1.f;

	virtual void init() override final
	{
		if (init_file.has_value())
			openfile = lak::exchange(init_file, lak::nullopt);
	}

	virtual ~my_window() {}

	virtual void handle_event(lak::event &event) override final
	{
		switch (event.type)
		{
			case lak::event_type::close_window: destroy(); break;
			case lak::event_type::dropfile:
				if (!openfile) openfile = event.dropfile().path;
				mem_edit_data.clear();
				break;
		}
	}

	void menu()
	{
		if (ImGui::Button("Open file"))
			pget_open.open_file({}, "TIFF{.tif;.tiff}");
	}

	void left_region()
	{
		if (tiff) draw_tiff(*tiff);
	}

	void right_region()
	{
		mem_edit.DrawContents(mem_edit_data.data(), mem_edit_data.size());
		if (mem_edit_highlight.has_value())
		{
			mem_edit.GotoAddrAndHighlight(
			  lak::distance(mem_edit_data.begin(), mem_edit_highlight->begin()),
			  lak::distance(mem_edit_data.begin(), mem_edit_highlight->end()));
			mem_edit_highlight.reset();
		}
	}

	virtual void loop(uint64_t) override final
	{
		if (ImGui::BeginMenuBar())
		{
			menu();
			ImGui::EndMenuBar();
		}

		if_let_some (auto pget, pget_open()) openfile = lak::move(pget);

		if (!loadfile && openfile)
			loadfile = lak::exchange(openfile, lak::nullopt);

		if (loadfile)
		{
			if_let_ok (auto res, loader(load_tiff, *loadfile))
			{
				if (res.is_ok())
					tiff = lak::move(res.unsafe_unwrap());
				else
				{
					ERROR("Failed to open ", *loadfile, ": ", res.unsafe_unwrap_err());
				}
				window().set_title(L"" APP_NAME " " + loadfile->generic_wstring());
				loadfile.reset();
			}
		}

		const auto content_size{ImGui::GetContentRegionAvail()};

		if (_left_size <= 0.f || _right_size <= 0.f)
		{
			_left_size  = content_size.x / 2;
			_right_size = content_size.x / 2;
		}

		lak::VertSplitter(
		  _left_size, _right_size, content_size.x, 8.f, 8.f, -1.f, false);

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

	void draw_tiff(const lak::tiff::image_file_header &ifh)
	{
		lak::Text<u8"Version: {}">(ifh.version);
	}

	void draw_tiff(const lak::tiff::ifd_tag &tag)
	{
		tag.data.visit(
		  [&]<typename T>(lak::span<T> data)
		  {
			  if (ImGui::Button("View data"))
			  {
				  mem_edit_data.clear();
				  auto bytes = lak::span<const byte_t>(data);
				  mem_edit_data.push_back(bytes.begin(), bytes.end());
			  }

			  LAK_TREE_NODE2("Data ({}[{}])", TYPE_NAME(T), data.size())
			  switch (tag.id)
			  {
#define LAK_TIFF_STRINGS(NAME, ...)                                           \
	case lak::tiff::tag_name::NAME: [[fallthrough]];
				  LAK_FOREACH_STRING_TIFF_TAG(LAK_TIFF_STRINGS)
#undef LAK_TIFF_STRINGS
				  lak::Text<u8"{}">(
				    lak::string_view(lak::span<const char>(lak::span(data))));
				  break;

				  default:
					  if (data.size() <= 10U)
						  lak::Text<u8"{{{}}}">(lak::spaced_streamify(u8", "_str, data));
					  else
						  lak::Text<u8"{{{}, ...}}">(lak::spaced_streamify(
						    u8", "_str, lak::span(data).first(100U)));
					  break;
			  }
		  });
	}

	void draw_tiff(const lak::tiff::strip &strip)
	{
		lak::Text<u8"Size: {}">(strip.data.size());
		if (ImGui::Button("View data"))
		{
			mem_edit_data = strip.data;
		}
	}

	void draw_tiff(const lak::tiff::tile &tile)
	{
		lak::Text<u8"Size: {}">(tile.data.size());
		if (ImGui::Button("View data"))
		{
			mem_edit_data = tile.data;
		}
	}

	void draw_tiff(const lak::tiff::image_file_directory &ifd)
	{
		if (!ifd.strips.empty()) LAK_TREE_NODE("Strips")
			{
				for (size_t i = 0U; const auto &strip : ifd.strips)
				{
					ImGui::PushID(&strip);
					DEFER(ImGui::PopID());
					LAK_TREE_NODE("Strip %zu", i) { draw_tiff(strip); }
					++i;
				}
			}

		// LAK_TREE_NODE("Tiles")
		// {
		// 	for (size_t i = 0U; const auto &tile : ifd.tiles)
		// 	{
		// 		ImGui::PushID(&tile);
		// 		DEFER(ImGui::PopID());
		// 		LAK_TREE_NODE("Tile %zu", i)
		// 		{
		// 			//
		// 		}
		// 		++i;
		// 	}
		// }

		if (!ifd.subifds.empty()) LAK_TREE_NODE("Sub Image File Descriptions")
			{
				for (size_t i = 0U; const auto &sub : ifd.subifds)
				{
					ImGui::PushID(&sub);
					DEFER(ImGui::PopID());
					LAK_TREE_NODE("Sub IFD %zu", i) { draw_tiff(sub); }
					++i;
				}
			}

		if (!ifd.tags.empty()) LAK_TREE_NODE("Tags")
			{
				for (const auto &tag : ifd.tags)
				{
					ImGui::PushID(&tag);
					DEFER(ImGui::PopID());
					LAK_TREE_NODE2("{}", tag.id) { draw_tiff(tag); }
				}
			}

		if (ifd.exif)
		{
			LAK_TREE_NODE("Exif IFD")
			{
				ImGui::PushID(ifd.exif.get());
				DEFER(ImGui::PopID());
				draw_tiff(*ifd.exif);
			}
		}

		if (ifd.interop)
		{
			LAK_TREE_NODE("Interop IFD")
			{
				ImGui::PushID(ifd.interop.get());
				DEFER(ImGui::PopID());
				draw_tiff(*ifd.interop);
			}
		}

		if (ifd.kodak)
		{
			LAK_TREE_NODE("Kodak IFD")
			{
				ImGui::PushID(ifd.kodak.get());
				DEFER(ImGui::PopID());
				draw_tiff(*ifd.kodak);
			}
		}
	}

	void draw_tiff(const lak::tiff::tiff &root)
	{
		ImGui::PushID(&root);
		DEFER(ImGui::PopID());

		LAK_TREE_NODE("Image File Header") { draw_tiff(root.ifh); }

		if (!root.ifd.empty()) LAK_TREE_NODE("Image File Descriptions")
			{
				for (size_t i = 0U; const auto &ifd : root.ifd)
				{
					ImGui::PushID(&ifd);
					DEFER(ImGui::PopID());
					LAK_TREE_NODE("IFD %zu", i) { draw_tiff(ifd); }
					++i;
				}
			}
	}
};

lak::error_code<int> LAK_BASIC_PROGRAM(program_preinit)(lak::span<char *> args)
{
	if (args.size() >= 2) init_file = std::filesystem::path(args[1]);

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

	DEBUG_EXPR(my_window_ptr.get()->window().graphics());

	return lak::ok_t{};
}

void LAK_BASIC_PROGRAM(program_handle_event)(lak::event &event) {}

bool LAK_BASIC_PROGRAM(program_loop)(uint64_t counter_delta)
{
	return !LAK_BASIC_PROGRAM(window_instances)().empty();
}

int LAK_BASIC_PROGRAM(program_quit)()
{
	my_window_ptr.reset();

	return EXIT_SUCCESS;
}
