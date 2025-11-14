#include "lak/system/profile.hpp"
#include "lak/system/os.hpp"

#include "lak/system/win32/windows.hpp"

#include "lak/debug.hpp"

uint64_t lak::performance_frequency()
{
	LARGE_INTEGER result;
	ASSERT(::QueryPerformanceFrequency(&result));
	return result.QuadPart;
}

uint64_t lak::performance_counter()
{
	LARGE_INTEGER result;
	ASSERT(::QueryPerformanceCounter(&result));
	return result.QuadPart;
}
