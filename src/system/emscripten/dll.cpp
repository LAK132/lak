#include "lak/system/dll.hpp"

uintptr_t lak::load_library(const char *name,
                            lak::span<const lak::fs::path> search_paths)
{
	LAK_UNUSED(name);
	LAK_UNUSED(search_paths);
	ASSERT_NYI();
}

void lak::unload_library(uintptr_t library)
{
	LAK_UNUSED(library);
	ASSERT_NYI();
}

uintptr_t lak::load_function_ex(uintptr_t library, const char *name)
{
	LAK_UNUSED(library);
	LAK_UNUSED(name);
	ASSERT_NYI();
}
