#include "lak/test.hpp"

#include "lak/system/architecture.hpp"

BEGIN_TEST(architecture)
{
	DEBUG_EXPR(lak::host_cpu_architecture());
	auto cpuid = lak::cpuid(0U);
	DEBUG_EXPR(cpuid[0U], cpuid[1U], cpuid[2U], cpuid[3U])
	return EXIT_SUCCESS;
}
END_TEST()
