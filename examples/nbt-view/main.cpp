#define APP_NAME "nbt-view"

#define LAK_BASIC_PROGRAM_IMGUI_IMPL

#define LAK_BASIC_PROGRAM_IMGUI_WINDOW_IMPL

#include <lak/basic_program.inl>

#include <lak/file/nbt.hpp>
#include <lak/optional.hpp>
#include <lak/system/file.hpp>

#include <filesystem>

using load_error  = lak::variant<lak::errno_error,
                                 lak::err::out_of_data,
                                 lak::nbt::err::invalid_type>;
using load_result = lak::result<lak::nbt::named_tag, load_error>;

load_result load_nbt(lak::fs::path path)
{
	RES_TRY_ASSIGN(
	  auto f =,
	  lak::read_file(path).map_err([](auto err) -> load_error { return err; }));
	RES_TRY_ASSIGN(
	  auto tag =,
	  lak::from_bytes<lak::nbt::named_tag, lak::endian::big>(f).map_err(
	    [](auto err) -> load_error { return err; }));
	return lak::move_ok(tag.first);
}

struct my_window : virtual public LAK_BASIC_PROGRAM(window_api)
{
	my_window() : LAK_BASIC_PROGRAM(window_api)() {}

	lak::optional<std::filesystem::path> openfile;
	lak::optional<std::filesystem::path> loadfile;
	lak::path_getter pget_open;
	lak::path_getter pget_save;
	lak::await<load_result> loader;
	lak::optional<lak::nbt::named_tag> nbt;

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
			if (ImGui::Button("Open file")) pget_open.open_file({}, "NBT{.dat}");

			ImGui::BeginDisabled(!nbt);
			if (ImGui::Button("Save file")) pget_save.save_file({}, "NBT{.dat}");
			ImGui::EndDisabled();

			ImGui::EndMenuBar();
		}
		if_let_some (auto pget, pget_open()) openfile = lak::move(pget);
		if_let_some (auto pget, pget_save())
		{
			if (nbt)
			{
				lak::binary_array_writer strm;
				if (strm.write_be(*nbt).IF_ERR("Failed to write").is_ok())
					if (!lak::save_file(pget, strm.data))
					{
						ERROR("Failed to save");
					}
			}
		}

		if (!loadfile && openfile)
			loadfile = lak::exchange(openfile, lak::nullopt);

		if (loadfile)
		{
			if_let_ok (auto res, loader(load_nbt, *loadfile))
			{
				if (res.is_ok())
					nbt = lak::move(res.unsafe_unwrap());
				else
				{
					ERROR("Failed to open ", *loadfile, ": ", res.unsafe_unwrap_err());
				}
				window().set_title(L"" APP_NAME " " + loadfile->generic_wstring());
				loadfile.reset();
			}
		}

		if (nbt) draw_nbt(*nbt);
	}

	template<typename T>
	void draw_nbt(const lak::nbt::pod_tag<T> &tag)
	{
		ImGui::Text("%s", lak::fmt<"{}">(tag).c_str());
	}

	template<typename T>
	void draw_nbt(const lak::nbt::array_tag<T> &tag)
	{
		ImGui::PushID((const void *)&tag);
		ImGui::Text("%s", lak::fmt<"{}">(tag).c_str());
		ImGui::PopID();
	}

	void draw_nbt(const lak::nbt::TAG_String &tag)
	{
		ImGui::Text("%s", lak::fmt<"{}">(tag).c_str());
	}

	void draw_nbt(const lak::nbt::TAG_Compound &tag)
	{
		ImGui::PushID((const void *)&tag);
		for (const auto &t : tag.value) draw_nbt(t);
		ImGui::Separator();
		ImGui::PopID();
	}

	void draw_nbt(const lak::nbt::TAG_List &tag)
	{
		ImGui::PushID((const void *)&tag);
		tag.value.visit(
		  [this]<typename T>(const lak::array<T> &val)
		  {
			  for (const auto &t : val) draw_nbt(t);
		  });
		ImGui::PopID();
	}

	void draw_nbt(const lak::nbt::tag_payload &tag)
	{
		ImGui::PushID((const void *)&tag);
		tag.value.visit([this](const auto &t) { draw_nbt(t); });
		ImGui::PopID();
	}

	void draw_nbt(const lak::nbt::named_tag &tag)
	{
		ImGui::PushID((const void *)&tag);
		LAK_TREE_NODE("%s", (const char *)tag.name.value.c_str())
		{
			draw_nbt(tag.payload);
		}
		ImGui::PopID();
	}

	void draw_nbt(const lak::nbt::TAG_End &) {}
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
