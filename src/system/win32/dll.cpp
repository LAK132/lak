#include "lak/system/dll.hpp"

#include "lak/system/win32/windows.hpp"

uintptr_t lak::load_library(const char *name,
                            lak::span<const lak::fs::path> search_paths)
{
	for (const auto &path : search_paths)
	{
		auto str = (path / name).string();
		auto ptr = ::LoadLibraryA(str.c_str());
		if (ptr) return reinterpret_cast<uintptr_t>(ptr);
	}
	return reinterpret_cast<uintptr_t>(::LoadLibraryA(name));
}

void lak::unload_library(uintptr_t library)
{
	::FreeLibrary(reinterpret_cast<HMODULE>(library));
}

uintptr_t lak::load_function_ex(uintptr_t library, const char *name)
{
	return reinterpret_cast<uintptr_t>(
	  ::GetProcAddress(reinterpret_cast<HMODULE>(library), name));
}
