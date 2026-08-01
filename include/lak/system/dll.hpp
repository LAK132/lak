#ifndef LAK_SYSTEM_DLL_HPP
#define LAK_SYSTEM_DLL_HPP

#include "lak/span.hpp"
#include "lak/string_view.hpp"
#include "lak/system/file.hpp"

namespace lak
{
	uintptr_t load_library(const char *name,
	                       lak::span<const lak::fs::path> search_paths);

	void unload_library(uintptr_t library);

	uintptr_t load_function_ex(uintptr_t library, const char *name);

	template<typename FUNC>
	FUNC load_function(uintptr_t library, const char *name)
	{
		return reinterpret_cast<FUNC>(lak::load_function_ex(library, name));
	}
}

#endif
