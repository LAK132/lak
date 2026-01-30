#include "lak/system/cobalt/renderers.hpp"

extern "C" __declspec(dllimport) bool GetOpenGL3RendererInfo(
  ::cobalt::graphics::IRendererInfo &);

lak::cobalt::renderer_info_func lak::cobalt::ogl3_get_renderer_info()
{
	return +[](unsigned int, ::cobalt::graphics::IRendererInfo &rendererInfo)
	{ return GetOpenGL3RendererInfo(rendererInfo); };
}
