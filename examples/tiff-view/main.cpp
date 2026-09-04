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

	virtual void init() override final {}

	virtual ~my_window() {}

	virtual void handle_event(lak::event &event) override final
	{
		switch (event.type)
		{
			case lak::event_type::close_window: destroy(); break;
			case lak::event_type::dropfile:
				if (!openfile) openfile = event.dropfile().path;
				break;
		}
	}

	virtual void loop(uint64_t) override final
	{
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::Button("Open file"))
				pget_open.open_file({}, "TIFF{.tif;.tiff}");

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

		if (tiff) draw_tiff(*tiff);
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
			  switch (tag.id)
			  {
#define LAK_TIFF_STRINGS(NAME, ...)                                           \
	case lak::tiff::tag_name::NAME: [[fallthrough]];
				  LAK_FOREACH_STRING_TIFF_TAG(LAK_TIFF_STRINGS)
#undef LAK_TIFF_STRINGS
				  lak::Text<u8"{}[{}] = \"{}\"">(
				    TYPE_NAME(T),
				    data.size(),
				    lak::string_view(lak::span<const char>(lak::span(data))));
				  break;

				  default:
					  lak::Text<u8"{}[{}] = {{{}}}">(
					    TYPE_NAME(T),
					    data.size(),
					    lak::spaced_streamify(u8", "_str, data));
					  break;
			  }
		  });
	}

	void draw_tiff(const lak::tiff::strip &strip)
	{
		lak::Text<u8"Size: {}">(strip.data.size());
	}

	void draw_tiff(const lak::tiff::tile &tile)
	{
		lak::Text<u8"Size: {}">(tile.data.size());
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
			LAK_TREE_NODE("Exif")
			{
				ImGui::PushID(ifd.exif.get());
				DEFER(ImGui::PopID());
				draw_tiff(*ifd.exif);
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
