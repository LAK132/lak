#include "lak/test.hpp"

#include <gphoto2/gphoto2-port.h>

BEGIN_TEST(libgphoto2)
{
	GPPort *gpport;

	gp_port_new(&gpport);

	DEBUG("Created port");

	gp_port_free(gpport);
	gpport = nullptr;

	DEBUG("Freed port");

	return 0;
}
END_TEST()
