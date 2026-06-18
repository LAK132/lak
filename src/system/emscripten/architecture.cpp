#include "lak/system/architecture.hpp"

#include "lak/string_literals/view.hpp"

lak::result<lak::architecture> lak::host_os_architecture()
{
	return lak::ok_t{lak::architecture::compiled};
}
