#define APP_NAME "colour-demo"

#define LAK_BASIC_PROGRAM_IMGUI_IMPL

#define LAK_BASIC_PROGRAM_IMPLOT_IMPL

#define LAK_BASIC_PROGRAM_IMPLOT3D_IMPL

#define LAK_BASIC_PROGRAM_IMGUI_WINDOW_IMPL

#include <lak/basic_program.inl>

#include <lak/optional.hpp>
#include <lak/string_literals/string.hpp>

#include <lak/col/cie.hpp>
#include <lak/col/srgb.hpp>

#include <lak/imgui/cie.hpp>
#include <lak/imgui/texture.hpp>

#include <lak/system/file.hpp>

#include <cie-cmf/CIE_cc_1931_2deg.csv.hpp>
#include <cie-cmf/CIE_cc_1964_10deg.csv.hpp>
#include <cie-cmf/CIE_xyz_1931_2deg.csv.hpp>
#include <cie-cmf/CIE_xyz_1964_10deg.csv.hpp>

#include <implot.h>
#include <implot3d.h>

#include <filesystem>

float colour_temp = 5500.f;

struct my_window : virtual public LAK_BASIC_PROGRAM(window_api)
{
	my_window() : LAK_BASIC_PROGRAM(window_api)() {}

	lak::col::cie::xyY_primaries primaries = lak::col::sRGB_primaries;

	virtual void init() override final { window().set_size({1250, 850}); }

	virtual ~my_window() {}

	virtual void handle_event(lak::event &event) override final
	{
		switch (event.type)
		{
			case lak::event_type::close_window: destroy(); break;
			default:                            break;
		}
	}

	virtual void loop(uint64_t counter_delta) override final
	{
		const auto content_size{ImGui::GetContentRegionAvail()};

		auto &style = ImGui::GetStyle();

		ImVec2 size{content_size.x / 3.f, content_size.y / 3.f};
		size.x -= style.FramePadding.x * 2.f;
		size.y -= style.FramePadding.y * 2.f;

		ImVec4 x_col{1, 0, 0, 1};
		ImVec4 y_col{0, 1, 0, 1};
		ImVec4 z_col{0, 0, 1, 1};

		constexpr int wavelength_min   = 300;
		constexpr int wavelength_max   = 900;
		constexpr int wavelength_steps = 10;

		ImPlotGetter cie_getter_x = [](int index, void *data) -> ImPlotPoint
		{
			auto &entry =
			  (*reinterpret_cast<lak::span<const cie_cmf::entry> *>(data))[index];
			return {double(entry.wavelength), entry.x};
		};
		ImPlotGetter cie_getter_y = [](int index, void *data) -> ImPlotPoint
		{
			auto &entry =
			  (*reinterpret_cast<lak::span<const cie_cmf::entry> *>(data))[index];
			return {double(entry.wavelength), entry.y};
		};
		ImPlotGetter cie_getter_z = [](int index, void *data) -> ImPlotPoint
		{
			auto &entry =
			  (*reinterpret_cast<lak::span<const cie_cmf::entry> *>(data))[index];
			return {double(entry.wavelength), entry.z};
		};
		ImPlotGetter cie_getter_temp_x = [](int index, void *data) -> ImPlotPoint
		{
			auto &entry =
			  (*reinterpret_cast<lak::span<const cie_cmf::entry> *>(data))[index];
			return {double(entry.wavelength),
			        entry.x * lak::earth_sun_blackbody_irradiance(
			                    double(entry.wavelength), colour_temp)};
		};
		ImPlotGetter cie_getter_temp_y = [](int index, void *data) -> ImPlotPoint
		{
			auto &entry =
			  (*reinterpret_cast<lak::span<const cie_cmf::entry> *>(data))[index];
			return {double(entry.wavelength),
			        entry.y * lak::earth_sun_blackbody_irradiance(
			                    double(entry.wavelength), colour_temp)};
		};
		ImPlotGetter cie_getter_temp_z = [](int index, void *data) -> ImPlotPoint
		{
			auto &entry =
			  (*reinterpret_cast<lak::span<const cie_cmf::entry> *>(data))[index];
			return {double(entry.wavelength),
			        entry.z * lak::earth_sun_blackbody_irradiance(
			                    double(entry.wavelength), colour_temp)};
		};
		ImPlotGetter irradiance_getter = [](int index, void *) -> ImPlotPoint
		{
			const double x = double((index * wavelength_steps) + wavelength_min);
			const double y = lak::earth_sun_blackbody_irradiance(x, colour_temp);
			return {x, y};
		};

		lak::ChromaticityPlot("sRGB", &primaries, size);

		ImGui::SameLine();

		double peak_irradiance =
		  lak::blackbody_peak_radiance(colour_temp) * lak::suns_steradian;

		if (DEFER(ImGui::EndChild()); ImGui::BeginChild(
		      "##temp",
		      size,
		      false,
		      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar))
		{
			if (ImGui::DragFloat("Temperature",
			                     &colour_temp,
			                     100.f,
			                     1000.f,
			                     25000.f,
			                     "%.0fK",
			                     ImGuiSliderFlags_AlwaysClamp))
				peak_irradiance =
				  lak::blackbody_peak_radiance(colour_temp) * lak::suns_steradian;

			if (ImPlot::BeginPlot("Temperature spectrum", ImVec2(-1, -1)))
			{
				DEFER(ImPlot::EndPlot());

				const auto peak_radiance = lak::blackbody_radiance(
				  std::min(std::max(lak::blackbody_peak_wavelength(colour_temp),
				                    double(wavelength_min)),
				           double(wavelength_max)),
				  colour_temp);

				ImPlot::SetupAxis(ImAxis_X1,
				                  "Wavelength (nm)",
				                  ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoMenus |
				                    ImPlotAxisFlags_NoSideSwitch);
				ImPlot::SetupAxisLimits(ImAxis_X1,
				                        double(wavelength_min),
				                        double(wavelength_max),
				                        ImPlotCond_Always);

				ImPlot::SetupAxis(ImAxis_Y1,
				                  "Sun's radiance (W/sr/m^2/nm)",
				                  ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoMenus |
				                    ImPlotAxisFlags_NoSideSwitch);
				ImPlot::SetupAxisLimits(
				  ImAxis_Y1, 0.0, peak_radiance, ImPlotCond_Always);

				ImPlot::SetupAxis(ImAxis_Y2,
				                  "Earth's irradiance (W/m^2/nm)",
				                  ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoMenus |
				                    ImPlotAxisFlags_NoSideSwitch |
				                    ImPlotAxisFlags_Opposite);
				ImPlot::SetupAxisLimits(ImAxis_Y2,
				                        0.0,
				                        peak_radiance * lak::suns_steradian,
				                        ImPlotCond_Always);

				ImPlot::SetupLegend(ImPlotLocation_SouthWest);
				ImPlot::SetupFinish();

				// ---

				ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2);

				ImPlot::PlotLineG("##blackbody",
				                  irradiance_getter,
				                  nullptr,
				                  (wavelength_max - wavelength_min) /
				                    wavelength_steps);
			}
		}

		// ---

		if (DEFER(ImGui::EndChild()); ImGui::BeginChild(
		      "##2deg observer colour match xyz",
		      size,
		      false,
		      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar))
		{
			if (ImPlot::BeginPlot("1931 2deg observer colour-match functions",
			                      ImVec2(-1, -1)))
			{
				DEFER(ImPlot::EndPlot());

				ImPlot::SetupAxes("Wavelength (nm)", "");
				ImPlot::SetupAxesLimits(double(wavelength_min),
				                        double(wavelength_max),
				                        0.0,
				                        3.0,
				                        ImPlotCond_Once);
				ImPlot::SetupLegend(ImPlotLocation_NorthEast);
				ImPlot::SetupFinish();

				lak::span<const cie_cmf::entry> data =
				  lak::span(cie_cmf::cie_1931_xyz_cmf_2deg_observer);

				ImPlot::SetNextLineStyle(x_col, IMPLOT_AUTO);
				ImPlot::PlotLineG("x", cie_getter_x, &data, int(data.size()));

				ImPlot::SetNextLineStyle(y_col, IMPLOT_AUTO);
				ImPlot::PlotLineG("y", cie_getter_y, &data, int(data.size()));

				ImPlot::SetNextLineStyle(z_col, IMPLOT_AUTO);
				ImPlot::PlotLineG("z", cie_getter_z, &data, int(data.size()));
			}
		}

		ImGui::SameLine();

		if (DEFER(ImGui::EndChild()); ImGui::BeginChild(
		      "##2deg observer colour match xyz temp",
		      size,
		      false,
		      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar))
		{
			if (ImPlot::BeginPlot("1931 2deg observer colour-match functions",
			                      ImVec2(-1, -1)))
			{
				DEFER(ImPlot::EndPlot());

				ImPlot::SetupAxis(ImAxis_X1,
				                  "Wavelength (nm)",
				                  ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoMenus |
				                    ImPlotAxisFlags_NoSideSwitch);
				ImPlot::SetupAxisLimits(ImAxis_X1,
				                        double(wavelength_min),
				                        double(wavelength_max),
				                        ImPlotCond_Always);

				ImPlot::SetupAxis(ImAxis_Y1,
				                  "Earth's irradiance (W/m^2/nm)",
				                  ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoMenus |
				                    ImPlotAxisFlags_NoSideSwitch |
				                    ImPlotAxisFlags_Opposite);
				ImPlot::SetupAxisLimits(
				  ImAxis_Y1, 0.0, peak_irradiance, ImPlotCond_Always);

				ImPlot::SetupAxis(ImAxis_Y2,
				                  "",
				                  ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoMenus |
				                    ImPlotAxisFlags_NoSideSwitch);
				ImPlot::SetupAxisLimits(
				  ImAxis_Y2, 0.0, 2.0 * peak_irradiance, ImPlotCond_Always);

				ImPlot::SetupLegend(ImPlotLocation_NorthEast);
				ImPlot::SetupFinish();

				lak::span<const cie_cmf::entry> data =
				  lak::span(cie_cmf::cie_1931_xyz_cmf_2deg_observer);

				ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1);

				ImPlot::PlotLineG("##blackbody",
				                  irradiance_getter,
				                  nullptr,
				                  (wavelength_max - wavelength_min) /
				                    wavelength_steps);

				ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2);

				ImPlot::SetNextLineStyle(x_col, IMPLOT_AUTO);
				ImPlot::PlotLineG("x", cie_getter_temp_x, &data, int(data.size()));

				ImPlot::SetNextLineStyle(y_col, IMPLOT_AUTO);
				ImPlot::PlotLineG("y", cie_getter_temp_y, &data, int(data.size()));

				ImPlot::SetNextLineStyle(z_col, IMPLOT_AUTO);
				ImPlot::PlotLineG("z", cie_getter_temp_z, &data, int(data.size()));
			}
		}

		ImGui::SameLine();

		if (DEFER(ImGui::EndChild()); ImGui::BeginChild(
		      "##2deg observer colour match cc",
		      size,
		      false,
		      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar))
		{
			if (ImPlot::BeginPlot("1931 2deg observer chromaticity coordinates",
			                      ImVec2(-1, -1)))
			{
				DEFER(ImPlot::EndPlot());

				ImPlot::SetupAxes("Wavelength (nm)", "");
				ImPlot::SetupAxesLimits(double(wavelength_min),
				                        double(wavelength_max),
				                        0.0,
				                        1.0,
				                        ImPlotCond_Once);
				ImPlot::SetupLegend(ImPlotLocation_NorthEast);
				ImPlot::SetupFinish();

				lak::span<const cie_cmf::entry> data =
				  lak::span(cie_cmf::cie_1931_cc_spectrum_loci_2deg_observer);

				ImPlot::SetNextLineStyle(x_col, IMPLOT_AUTO);
				ImPlot::PlotLineG("x", cie_getter_x, &data, int(data.size()));

				ImPlot::SetNextLineStyle(y_col, IMPLOT_AUTO);
				ImPlot::PlotLineG("y", cie_getter_y, &data, int(data.size()));

				ImPlot::SetNextLineStyle(z_col, IMPLOT_AUTO);
				ImPlot::PlotLineG("z", cie_getter_z, &data, int(data.size()));
			}
		}

		// ---

		if (DEFER(ImGui::EndChild()); ImGui::BeginChild(
		      "##10deg observer colour match xyz",
		      size,
		      false,
		      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar))
		{
			if (ImPlot::BeginPlot("1964 10deg observer colour-match functions",
			                      ImVec2(-1, -1)))
			{
				DEFER(ImPlot::EndPlot());

				ImPlot::SetupAxes("Wavelength (nm)", "");
				ImPlot::SetupAxesLimits(double(wavelength_min),
				                        double(wavelength_max),
				                        0.0,
				                        3.0,
				                        ImPlotCond_Once);
				ImPlot::SetupLegend(ImPlotLocation_NorthEast);
				ImPlot::SetupFinish();

				lak::span<const cie_cmf::entry> data =
				  lak::span(cie_cmf::cie_1964_xyz_cmf_10deg_observer);

				ImPlot::SetNextLineStyle(x_col, IMPLOT_AUTO);
				ImPlot::PlotLineG("x", cie_getter_x, &data, int(data.size()));

				ImPlot::SetNextLineStyle(y_col, IMPLOT_AUTO);
				ImPlot::PlotLineG("y", cie_getter_y, &data, int(data.size()));

				ImPlot::SetNextLineStyle(z_col, IMPLOT_AUTO);
				ImPlot::PlotLineG("z", cie_getter_z, &data, int(data.size()));
			}
		}

		ImGui::SameLine();

		if (DEFER(ImGui::EndChild()); ImGui::BeginChild(
		      "##10deg observer colour match xyz temp",
		      size,
		      false,
		      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar))
		{
			if (ImPlot::BeginPlot("1964 10deg observer colour-match functions",
			                      ImVec2(-1, -1)))
			{
				DEFER(ImPlot::EndPlot());

				ImPlot::SetupAxis(ImAxis_X1,
				                  "Wavelength (nm)",
				                  ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoMenus |
				                    ImPlotAxisFlags_NoSideSwitch);
				ImPlot::SetupAxisLimits(ImAxis_X1,
				                        double(wavelength_min),
				                        double(wavelength_max),
				                        ImPlotCond_Always);

				ImPlot::SetupAxis(ImAxis_Y1,
				                  "Earth's irradiance (W/m^2/nm)",
				                  ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoMenus |
				                    ImPlotAxisFlags_NoSideSwitch |
				                    ImPlotAxisFlags_Opposite);
				ImPlot::SetupAxisLimits(
				  ImAxis_Y1, 0.0, peak_irradiance, ImPlotCond_Always);

				ImPlot::SetupAxis(ImAxis_Y2,
				                  "",
				                  ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoMenus |
				                    ImPlotAxisFlags_NoSideSwitch);
				ImPlot::SetupAxisLimits(
				  ImAxis_Y2, 0.0, 2.0 * peak_irradiance, ImPlotCond_Always);

				ImPlot::SetupLegend(ImPlotLocation_NorthEast);
				ImPlot::SetupFinish();

				lak::span<const cie_cmf::entry> data =
				  lak::span(cie_cmf::cie_1964_xyz_cmf_10deg_observer);

				ImPlot::SetAxes(ImAxis_X1, ImAxis_Y1);

				ImPlot::PlotLineG("##blackbody",
				                  irradiance_getter,
				                  nullptr,
				                  (wavelength_max - wavelength_min) /
				                    wavelength_steps);

				ImPlot::SetAxes(ImAxis_X1, ImAxis_Y2);

				ImPlot::SetNextLineStyle(x_col, IMPLOT_AUTO);
				ImPlot::PlotLineG("x", cie_getter_temp_x, &data, int(data.size()));

				ImPlot::SetNextLineStyle(y_col, IMPLOT_AUTO);
				ImPlot::PlotLineG("y", cie_getter_temp_y, &data, int(data.size()));

				ImPlot::SetNextLineStyle(z_col, IMPLOT_AUTO);
				ImPlot::PlotLineG("z", cie_getter_temp_z, &data, int(data.size()));
			}
		}

		ImGui::SameLine();

		if (DEFER(ImGui::EndChild()); ImGui::BeginChild(
		      "##10deg observer colour match cc",
		      size,
		      false,
		      ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar))
		{
			if (ImPlot::BeginPlot("1964 10deg observer chromaticity coordinates",
			                      ImVec2(-1, -1)))
			{
				DEFER(ImPlot::EndPlot());

				ImPlot::SetupAxes("Wavelength (nm)", "");
				ImPlot::SetupAxesLimits(double(wavelength_min),
				                        double(wavelength_max),
				                        0.0,
				                        1.0,
				                        ImPlotCond_Once);
				ImPlot::SetupLegend(ImPlotLocation_NorthEast);
				ImPlot::SetupFinish();

				lak::span<const cie_cmf::entry> data =
				  lak::span(cie_cmf::cie_1964_cc_spectrum_loci_10deg_observer);

				ImPlot::SetNextLineStyle(x_col, IMPLOT_AUTO);
				ImPlot::PlotLineG("x", cie_getter_x, &data, int(data.size()));

				ImPlot::SetNextLineStyle(y_col, IMPLOT_AUTO);
				ImPlot::PlotLineG("y", cie_getter_y, &data, int(data.size()));

				ImPlot::SetNextLineStyle(z_col, IMPLOT_AUTO);
				ImPlot::PlotLineG("z", cie_getter_z, &data, int(data.size()));
			}
		}
	}
};

lak::graphics_mode forced_graphics_mode = lak::graphics_mode::None;
#ifdef LAK_ENABLE_COBALT
lak::optional<lak::cobalt_renderer_settings> forced_cobalt_mode;
#endif

lak::error_code<int> LAK_BASIC_PROGRAM(program_preinit)(lak::span<char *> args)
{
	if (args.size() >= 2U)
	{
#ifdef LAK_ENABLE_SOFTRNEDER
		if (args[1] == "--software"_str)
		{
			forced_graphics_mode = lak::graphics_mode::Software;
		}
		else
#endif
#ifdef LAK_ENABLE_OPENGL
		  if (args[1] == "--opengl"_str)
		{
			forced_graphics_mode = lak::graphics_mode::OpenGL;
		}
		else
#endif
#ifdef LAK_ENABLE_COBALT
		  if (args[1] == "--cobalt"_str)
		{
			forced_graphics_mode = lak::graphics_mode::Cobalt;
		}
		else
#	ifdef LAK_ENABLE_COBALT_OGL3
		  if (args[1] == "--cobalt-opengl3"_str)
		{
			forced_graphics_mode = lak::graphics_mode::Cobalt;
			RES_TRY_ASSIGN(forced_cobalt_mode =,
			               lak::cobalt_renderer_settings::preferred_ogl3().map_err(
			                 [](lak::monostate) -> int
			                 {
				                 ERROR("Failed to get OpenGL3 renderer");
				                 return EXIT_FAILURE;
			                 }));
		}
		else
#	endif
#	ifdef LAK_ENABLE_COBALT_OGL4
		  if (args[1] == "--cobalt-opengl4"_str)
		{
			forced_graphics_mode = lak::graphics_mode::Cobalt;
			RES_TRY_ASSIGN(forced_cobalt_mode =,
			               lak::cobalt_renderer_settings::preferred_ogl4().map_err(
			                 [](lak::monostate) -> int
			                 {
				                 ERROR("Failed to get OpenGL4 renderer");
				                 return EXIT_FAILURE;
			                 }));
		}
		else
#	endif
#	ifdef LAK_ENABLE_COBALT_VK
		  if (args[1] == "--cobalt-vulkan"_str)
		{
			forced_graphics_mode = lak::graphics_mode::Cobalt;
			RES_TRY_ASSIGN(forced_cobalt_mode =,
			               lak::cobalt_renderer_settings::preferred_vk().map_err(
			                 [](lak::monostate) -> int
			                 {
				                 ERROR("Failed to get Vulkan renderer");
				                 return EXIT_FAILURE;
			                 }));
		}
		else
#	endif
#	ifdef LAK_ENABLE_COBALT_D3D11
		  if (args[1] == "--cobalt-d3d11"_str)
		{
			forced_graphics_mode = lak::graphics_mode::Cobalt;
			RES_TRY_ASSIGN(forced_cobalt_mode =,
			               lak::cobalt_renderer_settings::preferred_d3d11().map_err(
			                 [](lak::monostate) -> int
			                 {
				                 ERROR("Failed to get Direct3D11 renderer");
				                 return EXIT_FAILURE;
			                 }));
		}
		else
#	endif
#	ifdef LAK_ENABLE_COBALT_D3D12
		  if (args[1] == "--cobalt-d3d12"_str)
		{
			forced_graphics_mode = lak::graphics_mode::Cobalt;
			RES_TRY_ASSIGN(forced_cobalt_mode =,
			               lak::cobalt_renderer_settings::preferred_d3d12().map_err(
			                 [](lak::monostate) -> int
			                 {
				                 ERROR("Failed to get Direct3D12 renderer");
				                 return EXIT_FAILURE;
			                 }));
		}
		else
#	endif
#	ifdef LAK_ENABLE_COBALT_MTL
		  if (args[1] == "--cobalt-metal"_str)
		{
			forced_graphics_mode = lak::graphics_mode::Cobalt;
			RES_TRY_ASSIGN(forced_cobalt_mode =,
			               lak::cobalt_renderer_settings::preferred_metal().map_err(
			                 [](lak::monostate) -> int
			                 {
				                 ERROR("Failed to get Metal renderer");
				                 return EXIT_FAILURE;
			                 }));
		}
		else
#	endif
#endif
		{
			ERROR(args[1], " not supported");
			return lak::err_t{EXIT_FAILURE};
		}
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

	switch (forced_graphics_mode)
	{
		case lak::graphics_mode::None:
		{
			RES_TRY_ASSIGN(
			  my_window_ptr =,
			  LAK_BASIC_PROGRAM(create_window<my_window>)().map_err(map_str_err));
		}
		break;
#ifdef LAK_ENABLE_SOFTRENDER
		case lak::graphics_mode::Software:
		{
			RES_TRY_ASSIGN(my_window_ptr =,
			               LAK_BASIC_PROGRAM(create_window<my_window>)(
			                 LAK_BASIC_PROGRAM(window_software_settings))
			                 .map_err(map_str_err));
		}
		break;
#endif
#ifdef LAK_ENABLE_OPENGL
		case lak::graphics_mode::OpenGL:
		{
			RES_TRY_ASSIGN(my_window_ptr =,
			               LAK_BASIC_PROGRAM(create_window<my_window>)(
			                 LAK_BASIC_PROGRAM(window_opengl_settings))
			                 .map_err(map_str_err));
		}
		break;
#endif
#ifdef LAK_ENABLE_COBALT
		case lak::graphics_mode::Cobalt:
		{
			if_let_some (const auto &settings, forced_cobalt_mode)
			{
				RES_TRY_ASSIGN(my_window_ptr =,
				               LAK_BASIC_PROGRAM(create_window<my_window>)(
				                 LAK_BASIC_PROGRAM(window_cobalt_settings), settings)
				                 .map_err(map_str_err));
			}
			else
			{
				RES_TRY_ASSIGN(my_window_ptr =,
				               LAK_BASIC_PROGRAM(create_window<my_window>)(
				                 LAK_BASIC_PROGRAM(window_cobalt_settings))
				                 .map_err(map_str_err));
			}
		}
		break;
#endif
		default:
			ERROR(
			  lak::fmt<u8"Graphics mode {} not available">(forced_graphics_mode));
			return lak::err_t{EXIT_FAILURE};
	}

	DEBUG_EXPR(my_window_ptr.get()->window().graphics());

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
	[[maybe_unused]] auto frame_time_ms =
	  ((float)counter_delta * 1000U) / lak::performance_frequency();

	return !LAK_BASIC_PROGRAM(window_instances)().empty();
}

int LAK_BASIC_PROGRAM(program_quit)()
{
	my_window_ptr.reset();

	return EXIT_SUCCESS;
}
