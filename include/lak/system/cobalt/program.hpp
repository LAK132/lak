#ifndef LAK_SYSTEM_COBALT_PROGRAM_HPP
#define LAK_SYSTEM_COBALT_PROGRAM_HPP

#include "lak/string_view.hpp"

#include <RendererInterface/RendererInterface.pkg>

namespace lak
{
	namespace cobalt
	{
		inline auto shader_source_hlsl(lak::astring_view source,
		                               lak::astring_view entry = {})
		{
			return ::cobalt::graphics::ShaderSourceInfoHLSL(
			  source.data(), source.size(), entry.data(), entry.size());
		}
	}
}

#endif
