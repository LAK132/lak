#include "lak/platform.hpp"
#include "lak/debug.hpp"

bool lak::platform_init(lak::platform_instance *handle)
{
	handle->handle = xcb_connection(NULL, NULL);
	return static_cast<bool>(handle->handle);
}

bool lak::platform_quit(lak::platform_instance *handle)
{
	xcb_disconnect(handle->handle);
	return true;
}
