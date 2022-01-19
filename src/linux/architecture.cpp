#include "lak/architecture.hpp"

#include "lak/string_literals.hpp"

#include <sys/utsname.h>

lak::result<lak::architecture> lak::host_os_architecture()
{
	struct utsname name;
	if (uname(&name) < 0) return lak::err_t{};

	const auto machine{lak::astring_view::from_c_str(name.machine)};

	if (machine == "i686"_view || machine == "i586"_view ||
	    machine == "i486"_view || machine == "i386"_view ||
	    machine == "i86pc"_view || machine == "x86"_view)
		return lak::ok_t{lak::architecture::x86};
	else if (machine == "x86_64"_view || machine == "amd64"_view ||
	         machine == "genuineintel"_view || machine == "i686-64"_view)
		return lak::ok_t{lak::architecture::x86_64};
	else if (machine == "ia64"_view)
		return lak::ok_t{lak::architecture::ia64};
	else if (machine == "armv6l"_view || machine == "armv7l"_view)
		return lak::ok_t{lak::architecture::arm};
	else if (machine == "aarch64"_view)
		return lak::ok_t{lak::architecture::aarch64};

	return lak::ok_t{lak::architecture::unknown};
}
