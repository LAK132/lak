#ifndef LAK_IMGUI_CIE_HPP
#define LAK_IMGUI_CIE_HPP

#include "lak/col/cie_1931.hpp"

#include <imgui.h>

// requires a valid ImPlot context

namespace lak
{
	// view only
	void ChromaticityPlot(const char *name,
	                      const lak::col::cie::xy_primaries *primaries,
	                      ImVec2 size = ImVec2(600, 600));
	void ChromaticityPlot(const char *name,
	                      const lak::col::cie::xyY_primaries *primaries,
	                      ImVec2 size = ImVec2(600, 600));

	// view and edit. returns true on modification of the primaries object
	bool ChromaticityEdit(const char *name,
	                      lak::col::cie::xy_primaries *primaries,
	                      ImVec2 size = ImVec2(600, 600));
	bool ChromaticityEdit(const char *name,
	                      lak::col::cie::xyY_primaries *primaries,
	                      ImVec2 size = ImVec2(600, 600));
}

#endif
