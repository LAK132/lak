#include "lak/format.hpp"

#include "lak/imgui/cie.hpp"

#include <cie-cmf/CIE_cc_1931_2deg.csv.hpp>
#include <cie-cmf/CIE_cc_1964_10deg.csv.hpp>

#include <implot.h>

template<typename PRIMARIES>
static bool _chromaticity_plot(const char *name,
                               PRIMARIES *primaries,
                               ImVec2 size)
{
	bool result = false;

	static_assert(lak::is_same_v<lak::remove_const_t<PRIMARIES>,
	                             lak::col::cie::xyY_primaries> ||
	              lak::is_same_v<lak::remove_const_t<PRIMARIES>,
	                             lak::col::cie::xy_primaries>);

	ImGui::PushID(primaries);
	DEFER(ImGui::PopID());

	DEFER(ImGui::EndChild());
	if (ImGui::BeginChild("##primaries_plot",
	                      size,
	                      false,
	                      ImGuiWindowFlags_NoSavedSettings |
	                        ImGuiWindowFlags_NoScrollbar))
	{
		if (ImPlot::BeginPlot(name, ImVec2(-1, -1)))
		{
			DEFER(ImPlot::EndPlot());

			ImPlot::SetupAxes("x", "y");
			ImPlot::SetupAxesLimits(0.0, 0.9, 0.0, 0.9, ImPlotCond_Once);
			ImPlot::SetupLegend(ImPlotLocation_NorthEast);
			ImPlot::SetupFinish();

			ImPlotGetter cie_getter = [](int index, void *data) -> ImPlotPoint
			{
				auto &entry =
				  (*reinterpret_cast<lak::span<const cie_cmf::entry> *>(data))[index];
				return {entry.x, entry.y};
			};
			lak::span<const cie_cmf::entry> data_1931 =
			  lak::span(cie_cmf::cie_1931_cc_spectrum_loci_2deg_observer);
			lak::span<const cie_cmf::entry> data_1964 =
			  lak::span(cie_cmf::cie_1964_cc_spectrum_loci_10deg_observer);

			ImPlot::PlotLineG(
			  "1931 2deg", cie_getter, &data_1931, int(data_1931.size()));

			ImPlot::PlotLineG(
			  "1964 10deg", cie_getter, &data_1964, int(data_1964.size()));

			const auto mat = [&]()
			{
				if constexpr (lak::is_same_v<lak::remove_const_t<PRIMARIES>,
				                             lak::col::cie::xy_primaries>)
					return lak::col::cie::to_xyY(*primaries).linear_to_xyY();
				else
					return primaries->linear_to_xyY();
			}();

			const auto r_co = mat(lak::vec3f_t(1.f, 0.f, 0.f));
			const auto g_co = mat(lak::vec3f_t(0.f, 1.f, 0.f));
			const auto b_co = mat(lak::vec3f_t(0.f, 0.f, 1.f));
			const auto w_co = mat(lak::vec3f_t(1.f, 1.f, 1.f));

			const auto c_co = mat(lak::vec3f_t(0.f, 1.f, 1.f));
			const auto m_co = mat(lak::vec3f_t(1.f, 0.f, 1.f));
			const auto y_co = mat(lak::vec3f_t(1.f, 1.f, 0.f));

			double x_coords[] = {
			  r_co.x,
			  g_co.x,
			  b_co.x,
			  w_co.x,

			  c_co.x,
			  m_co.x,
			  y_co.x,
			};
			double y_coords[] = {
			  r_co.y,
			  g_co.y,
			  b_co.y,
			  w_co.y,

			  c_co.y,
			  m_co.y,
			  y_co.y,
			};

			ImPlot::PlotLine(lak::fmt<"{}##lines">(name).c_str(),
			                 x_coords,
			                 y_coords,
			                 3,
			                 ImPlotLineFlags_Loop);

			auto plot_point =
			  [&](const char *channel, const ImVec4 &col, double *x, double *y)
			{
				ImPlot::Annotation(
				  *x, *y, col, ImVec2(-10, -10), false, "%s", channel);
				ImPlot::SetNextMarkerStyle(
				  IMPLOT_AUTO, IMPLOT_AUTO, col, IMPLOT_AUTO, col);
				ImPlot::PlotScatter(
				  lak::fmt<"##{}scatter{}">(name, channel).c_str(), x, y, 1);
			};

			auto edit_point =
			  [&](
			    const char *channel, int id, const ImVec4 &col, double *x, double *y)
			{
				if constexpr (lak::is_const_v<PRIMARIES>)
				{
					LAK_UNUSED(id);
					plot_point(channel, col, x, y);
				}
				else
				{
					ImPlot::Annotation(
					  *x, *y, col, ImVec2(-10, -10), false, "%s", channel);
					result |= ImPlot::DragPoint(id, x, y, col);
				}
			};

			edit_point("R", 0, ImVec4(1, 0, 0, 1), x_coords + 0, y_coords + 0);
			edit_point("G", 1, ImVec4(0, 1, 0, 1), x_coords + 1, y_coords + 1);
			edit_point("B", 2, ImVec4(0, 0, 1, 1), x_coords + 2, y_coords + 2);
			edit_point("W", 3, ImVec4(1, 1, 1, 1), x_coords + 3, y_coords + 3);

			plot_point("C", ImVec4(0, 1, 1, 1), x_coords + 4, y_coords + 4);
			plot_point("M", ImVec4(1, 0, 1, 1), x_coords + 5, y_coords + 5);
			plot_point("Y", ImVec4(1, 1, 0, 1), x_coords + 6, y_coords + 6);

			if constexpr (!lak::is_const_v<PRIMARIES>)
				if (result)
				{
					primaries->r.x = float(x_coords[0]);
					primaries->g.x = float(x_coords[1]);
					primaries->b.x = float(x_coords[2]);
					primaries->w.x = float(x_coords[3]);

					primaries->r.y = float(y_coords[0]);
					primaries->g.y = float(y_coords[1]);
					primaries->b.y = float(y_coords[2]);
					primaries->w.y = float(y_coords[3]);

					if constexpr (lak::is_same_v<PRIMARIES,
					                             lak::col::cie::xyY_primaries>)
						primaries->regenerate_Y();
				}
		}
	}

	return result;
}

void lak::ChromaticityPlot(const char *name,
                           const lak::col::cie::xy_primaries *primaries,
                           ImVec2 size)
{
	_chromaticity_plot(name, primaries, size);
}

void lak::ChromaticityPlot(const char *name,
                           const lak::col::cie::xyY_primaries *primaries,
                           ImVec2 size)
{
	_chromaticity_plot(name, primaries, size);
}

bool lak::ChromaticityEdit(const char *name,
                           lak::col::cie::xy_primaries *primaries,
                           ImVec2 size)
{
	return _chromaticity_plot(name, primaries, size);
}

bool lak::ChromaticityEdit(const char *name,
                           lak::col::cie::xyY_primaries *primaries,
                           ImVec2 size)
{
	return _chromaticity_plot(name, primaries, size);
}
