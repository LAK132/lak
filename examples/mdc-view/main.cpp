#define APP_NAME "mdc-view"

#define LAK_BASIC_PROGRAM_IMGUI_IMPL

#define LAK_BASIC_PROGRAM_IMGUI_WINDOW_IMPL

#include <lak/basic_program.inl>

#include <lak/file/mdc.hpp>
#include <lak/imgui/texture.hpp>
#include <lak/imgui/widgets.hpp>
#include <lak/optional.hpp>
#include <lak/system/file.hpp>

#include <stb_image_write.h>

#include <filesystem>

using load_error  = lak::variant<std::error_code, lak::err::out_of_data>;
using load_result = lak::result<lak::mdc::mdc, load_error>;

load_result load_mdc(lak::fs::path path)
{
	RES_TRY_ASSIGN(
	  auto f =,
	  lak::read_file(path).map_err([](auto err) -> load_error { return err; }));
	RES_TRY_ASSIGN(auto raw =,
	               lak::from_bytes<lak::mdc::mdc>(f).map_err(
	                 [](auto err) -> load_error { return err; }));
	return lak::move_ok(raw.first);
}

lak::array<lak::image<lak::color4_t>, 4> image_change(const lak::image4_t &img)
{
	lak::array<lak::image<lak::color4_t>, 4> res;
	for (size_t i = 0U; i < 4U; ++i)
	{
		auto &_res = res[i];
		_res.resize({img.size().x - 1U, img.size().y - 1U});
		for (size_t y = 0U; y < _res.size().y; ++y)
		{
			for (size_t x = 0U; x < _res.size().x; ++x)
			{
				_res[{x, y}].r = uint8_t(
				  float(((((int16_t(img[{x, y}][i]) - int16_t(img[{x + 1U, y}][i])) +
				           (int16_t(img[{x, y + 1U}][i]) -
				            int16_t(img[{x + 1U, y + 1U}][i]))) /
				          2U) /
				         INT8_MAX) +
				        0.5f) *
				  255);
				_res[{x, y}].g = uint8_t(
				  float(((((int16_t(img[{x, y}][i]) - int16_t(img[{x, y + 1U}][i])) +
				           (int16_t(img[{x + 1U, y}][i]) -
				            int16_t(img[{x + 1U, y + 1U}][i]))) /
				          2U) /
				         INT8_MAX) +
				        0.5f) *
				  255);
				_res[{x, y}].b = 0;
				_res[{x, y}].a = 255;
			}
		}
	}
	return res;
}

struct my_window : virtual public LAK_BASIC_PROGRAM(window_api)
{
	my_window() : LAK_BASIC_PROGRAM(window_api)() {}

	lak::optional<std::filesystem::path> openfile;
	lak::optional<std::filesystem::path> loadfile;
	lak::path_getter pget_open;
	lak::path_getter pget_save;
	lak::await<load_result> loader;
	lak::optional<lak::mdc::mdc> mdc;

	int diff_offset[2] = {0, 0};

	int r_offset[2]  = {8, 0};
	int g1_offset[2] = {8, 0};
	int g2_offset[2] = {12, 4};
	int b_offset[2]  = {0, 0};

	float left_size  = -1.f;
	float right_size = -1.f;

	lak::fs::path binary_path;
	lak::array<byte_t> binary;
	bool binary_update = false, raw_update = false;

	lak::image3_t processedimg;

	lak::ImUniqueTexture rtex, g1tex, g1dtex, g2tex, g2dtex, btex, rgbtex,
	  gdifftex, rg1difftex, rg2difftex, processedtex;

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
			if (ImGui::Button("Open..."))
				pget_open.open_file({}, "Minolta RD-175 Raw Files{.MDC},.*");

			ImGui::BeginDisabled(processedimg.contig_size() == 0U);
			if (ImGui::Button("Save..."))
				pget_save.save_file({}, "Image Files{.PNG},.*");
			ImGui::EndDisabled();

			ImGui::EndMenuBar();
		}
		if_let_some (auto pget, pget_open()) openfile = lak::move(pget);
		if_let_some (auto pget, pget_save())
		{
			stbi_write_png(
			  (const char *)pget.u8string().c_str(),
			  int(processedimg.size().x),
			  int(processedimg.size().y),
			  3,
			  processedimg.data(),
			  int(processedimg.contig_size_bytes() / processedimg.size().y));
		}

		if (!loadfile && openfile)
			loadfile = lak::exchange(openfile, lak::nullopt);

		if (loadfile)
		{
			if_let_ok (auto res, loader(load_mdc, *loadfile))
			{
				if (res.is_ok())
					mdc = lak::move(res.unsafe_unwrap());
				else
				{
					ERROR("Failed to open ", *loadfile, ": ", res.unsafe_unwrap_err());
				}
				window().set_title(L"" APP_NAME " " + loadfile->generic_wstring());
				loadfile.reset();
				raw_update = true;
			}
		}

		if (mdc)
		{
			if (raw_update) update_mdc(*mdc);
			draw_mdc(*mdc);
		}
	}

	void update_mdc(lak::mdc::mdc &raw)
	{
		{
			lak::image4_t rimg;
			rimg.resize({0x300U, 0x1EEU});
			for (size_t y = 0U; y < 0x1EEU; ++y)
			{
				const size_t _y2 = y * 0x180U;
				for (size_t x = 0U; x < 0x300U; ++x)
				{
					rimg[{x, y}] = lak::color4_t(raw.red[(x / 2U) + _y2], 0, 0, 255);
				}
			}
			rtex.emplace(rimg);
		}
		{
			lak::image4_t g1img;
			g1img.resize({0x300U, 0x1EEU});
			for (size_t y = 0U; y < 0x1EEU; ++y)
			{
				const size_t _y = y * 0x300U;
				for (size_t x = 0U; x < 0x300U; ++x)
				{
					g1img[{x, y}] = lak::color4_t(0, raw.green1[x + _y], 0, 255);
				}
			}
			g1tex.emplace(g1img);
			g1dtex.emplace(image_change(g1img)[1]);
		}
		{
			lak::image4_t g2img;
			g2img.resize({0x300U, 0x1EEU});
			for (size_t y = 0U; y < 0x1EEU; ++y)
			{
				const size_t _y = y * 0x300U;
				for (size_t x = 0U; x < 0x300U; ++x)
				{
					g2img[{x, y}] = lak::color4_t(0, raw.green2[x + _y], 0, 255);
				}
			}
			g2tex.emplace(g2img);
			g2dtex.emplace(image_change(g2img)[1]);
		}
		{
			lak::image4_t bimg;
			bimg.resize({0x300U, 0x1EEU});
			for (size_t y = 0U; y < 0x1EEU; ++y)
			{
				const size_t _y2 = y * 0x180U;
				for (size_t x = 0U; x < 0x300U; ++x)
				{
					bimg[{x, y}] = lak::color4_t(0, 0, raw.blue[(x / 2U) + _y2], 255);
				}
			}
			btex.emplace(bimg);
		}
		{
			lak::image4_t rgbimg;
			rgbimg.resize({0x300U, 0x1EEU});
			for (size_t y = 0U; y < 0x1EEU; ++y)
			{
				const size_t _y  = y * 0x300U;
				const size_t _y2 = y * 0x180U;
				for (size_t x = 0U; x < 0x300U; ++x)
				{
					rgbimg[{x, y}] = lak::color4_t(raw.red[(x / 2U) + _y2],
					                               raw.green1[x + _y],
					                               raw.blue[(x / 2U) + _y2],
					                               255);
				}
			}
			rgbtex.emplace(rgbimg);
		}
		{
			lak::image4_t gdiffimg;
			gdiffimg.resize({0x300U, 0x1EEU});
			for (size_t y = -std::min(0, diff_offset[1]);
			     y < size_t(0x1EEU - std::max(0, diff_offset[1]));
			     ++y)
			{
				const size_t _y  = y * 0x300U;
				const size_t _yd = (y + diff_offset[1]) * 0x300U;
				for (size_t x = -std::min(0, diff_offset[0]);
				     x < size_t(0x300U - std::max(0, diff_offset[0]));
				     ++x)
				{
					uint8_t v = std::max(raw.green1[x + _y],
					                     raw.green2[(x + diff_offset[0]) + _yd]) -
					            std::min(raw.green1[x + _y],
					                     raw.green2[(x + diff_offset[0]) + _yd]);
					gdiffimg[{x, y}] = lak::color4_t(v, 255 - v, v, 255);
				}
			}
			gdifftex.emplace(gdiffimg);
		}
		{
			lak::image4_t rg1diffimg;
			rg1diffimg.resize({0x300U, 0x1EEU});
			for (size_t y = -std::min(0, diff_offset[1]);
			     y < size_t(0x1EEU - std::max(0, diff_offset[1]));
			     ++y)
			{
				const size_t _yr = y * 0x180U;
				const size_t _yd = (y + diff_offset[1]) * 0x300U;
				for (size_t x = -std::min(0, diff_offset[0]);
				     x < size_t(0x300U - std::max(0, diff_offset[0]));
				     ++x)
				{
					uint8_t v = std::max(raw.red[(x / 2U) + _yr],
					                     raw.green1[(x + diff_offset[0]) + _yd]) -
					            std::min(raw.red[(x / 2U) + _yr],
					                     raw.green1[(x + diff_offset[0]) + _yd]);
					rg1diffimg[{x, y}] = lak::color4_t(v, 255 - v, v, 255);
				}
			}
			rg1difftex.emplace(rg1diffimg);
		}
		{
			lak::image4_t rg2diffimg;
			rg2diffimg.resize({0x300U, 0x1EEU});
			for (size_t y = -std::min(0, diff_offset[1]);
			     y < size_t(0x1EEU - std::max(0, diff_offset[1]));
			     ++y)
			{
				const size_t _yr = y * 0x180U;
				const size_t _yd = (y + diff_offset[1]) * 0x300U;
				for (size_t x = -std::min(0, diff_offset[0]);
				     x < size_t(0x300U - std::max(0, diff_offset[0]));
				     ++x)
				{
					uint8_t v = std::max(raw.red[(x / 2U) + _yr],
					                     raw.green2[(x + diff_offset[0]) + _yd]) -
					            std::min(raw.red[(x / 2U) + _yr],
					                     raw.green2[(x + diff_offset[0]) + _yd]);
					rg2diffimg[{x, y}] = lak::color4_t(v, 255 - v, v, 255);
				}
			}
			rg2difftex.emplace(rg2diffimg);
		}
		{
			raw.red_offset    = {int16_t(r_offset[0]), int16_t(r_offset[1])};
			raw.green1_offset = {int16_t(g1_offset[0]), int16_t(g1_offset[1])};
			raw.green2_offset = {int16_t(g2_offset[0]), int16_t(g2_offset[1])};
			raw.blue_offset   = {int16_t(b_offset[0]), int16_t(b_offset[1])};
			processedimg      = raw;
			lak::image4_t img2;
			img2.resize(processedimg.size());
			for (const auto i : lak::size_range_count(processedimg.contig_size()))
				img2[i] = {
				  processedimg[i].r, processedimg[i].g, processedimg[i].b, 255U};
			processedtex.emplace(img2);
		}
		raw_update = false;
	}

	void draw_image(const ImTextureRef &tex, float scale)
	{
		ImGui::BeginChild("Image View",
		                  ImVec2(0, 0),
		                  false,
		                  ImGuiWindowFlags_NoSavedSettings |
		                    ImGuiWindowFlags_AlwaysVerticalScrollbar |
		                    ImGuiWindowFlags_AlwaysHorizontalScrollbar);

		if (tex == ImTextureID_Invalid)
		{
			ImGui::Text("No image selected.");
		}
		else
		{
			const auto size = lak::TextureSize(tex);
			ImGui::Image(tex,
			             ImVec2(scale * static_cast<float>(size.x),
			                    scale * static_cast<float>(size.y)));
		}

		ImGui::EndChild();
	}

	void draw_mdc(lak::mdc::mdc &raw)
	{
		const auto content_size{ImGui::GetContentRegionAvail()};

		if (left_size <= 0.f || right_size <= 0.f)
		{
			left_size  = content_size.x / 2;
			right_size = content_size.x / 2;
		}

		lak::VertSplitter(left_size, right_size, content_size.x);

		ImGui::BeginChild(
		  "ImgLeft", {left_size, -1}, true, ImGuiWindowFlags_NoSavedSettings);
		LAK_TREE_NODE("Processed")
		{
			ImGui::Text("Sensor Alignment (x1/8th of a pixel)");
			ImGui::SliderInt2("Red", r_offset, -16, 16);
			if (ImGui::IsItemDeactivatedAfterEdit()) raw_update = true;
			ImGui::SliderInt2("Green 1", g1_offset, -16, 16);
			if (ImGui::IsItemDeactivatedAfterEdit()) raw_update = true;
			ImGui::SliderInt2("Green 2", g2_offset, -16, 16);
			if (ImGui::IsItemDeactivatedAfterEdit()) raw_update = true;
			ImGui::SliderInt2("Blue", b_offset, -16, 16);
			if (ImGui::IsItemDeactivatedAfterEdit()) raw_update = true;
			draw_image(processedtex, 1.0f);
		}
		LAK_TREE_NODE("RGB") { draw_image(rgbtex, 2.0f); }
		LAK_TREE_NODE("G1d") { draw_image(g1dtex, 2.0f); }
		LAK_TREE_NODE("G2d") { draw_image(g2dtex, 2.0f); }
		LAK_TREE_NODE("G diff")
		{
			ImGui::SliderInt2("diff offset", diff_offset, -2, 2);
			if (ImGui::IsItemDeactivatedAfterEdit()) raw_update = true;
			draw_image(gdifftex, 3.0f);
		}
		LAK_TREE_NODE("R G1 diff")
		{
			ImGui::SliderInt2("diff offset", diff_offset, -2, 2);
			if (ImGui::IsItemDeactivatedAfterEdit()) raw_update = true;
			draw_image(rg1difftex, 3.0f);
		}
		LAK_TREE_NODE("R G2 diff")
		{
			ImGui::SliderInt2("diff offset", diff_offset, -2, 2);
			if (ImGui::IsItemDeactivatedAfterEdit()) raw_update = true;
			draw_image(rg2difftex, 3.0f);
		}
		ImGui::EndChild();

		ImGui::SameLine();

		ImGui::BeginChild(
		  "ImgRight", {right_size, -1}, true, ImGuiWindowFlags_NoSavedSettings);
		LAK_TREE_NODE("Info")
		{
			lak::Text<u8"Shutter Speed: {:#X}">(raw.settings.shutter_speed);
			lak::Text<u8"Aperture: {:#X}">(raw.settings.aperture);
			// lak::Text<u8"Exposure Compensation: {:-+1.1}">(
			lak::Text<u8"Exposure Compensation: {:+1.1}">(
			  float(raw.settings.exposure_compensation) / 8.f);
			lak::Text<u8"Focal Length: {:#X}">(raw.settings.focal_length);
		}
		LAK_TREE_NODE("R") { draw_image(rtex, 2.0f); }
		LAK_TREE_NODE("G1") { draw_image(g1tex, 2.0f); }
		LAK_TREE_NODE("G2") { draw_image(g2tex, 2.0f); }
		LAK_TREE_NODE("B") { draw_image(btex, 2.0f); }
		ImGui::EndChild();
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
