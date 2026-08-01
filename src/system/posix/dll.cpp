#include "lak/system/dll.hpp"

#include <dlfcn.h>

uintptr_t lak::load_library(const char *name,
                            lak::span<const lak::fs::path> search_paths)
{
	for (const auto &path : search_paths)
	{
		auto str = (path / name).string();
		auto ptr = dlopen(str.c_str(), RTLD_LAZY | RTLD_LOCAL);
		if (ptr) return reinterpret_cast<uintptr_t>(ptr);
	}
	return reinterpret_cast<uintptr_t>(dlopen(name, RTLD_LAZY | RTLD_LOCAL));
}

void lak::unload_library(uintptr_t library)
{
	dlclose(reinterpret_cast<void *>(library));
}

uintptr_t lak::load_function_ex(uintptr_t library, const char *name)
{
	return reinterpret_cast<uintptr_t>(
	  dlsym(reinterpret_cast<void *>(library), name));
}
