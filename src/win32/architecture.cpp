#include "lak/architecture.hpp"

#include "lak/windows.hpp"

lak::result<lak::architecture> lak::host_os_architecture()
{
	SYSTEM_INFO info;
	::GetNativeSystemInfo(&info);

	switch (info.wProcessorArchitecture)
	{
		case PROCESSOR_ARCHITECTURE_AMD64:
			return lak::ok_t{lak::architecture::x86_64};
		case PROCESSOR_ARCHITECTURE_ARM: return lak::ok_t{lak::architecture::arm};
		case PROCESSOR_ARCHITECTURE_ARM64:
			return lak::ok_t{lak::architecture::aarch64};
		case PROCESSOR_ARCHITECTURE_IA64:
			return lak::ok_t{lak::architecture::ia64};
		case PROCESSOR_ARCHITECTURE_INTEL:
			return lak::ok_t{lak::architecture::x86};
		default: return lak::ok_t{lak::architecture::unknown};
	}
}
