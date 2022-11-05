#include "lak/test.hpp"

#include "libfive/eval/evaluator.hpp"
#include "libfive/render/brep/mesh.hpp"
#include "libfive/render/brep/settings.hpp"
#include "libfive/tree/tree.hpp"

BEGIN_TEST(libfive)
{
	namespace Kernel = libfive;

	auto x{Kernel::Tree::X()};
	auto y{Kernel::Tree::Y()};
	auto z{Kernel::Tree::Z()};

	auto out{(x * x) + (y * y) + (z * z) - 1};

	auto bounds{Kernel::Region<3>({-2, -2, -2}, {2, 2, 2})};

	auto mesh{Kernel::Mesh::render(out, bounds, libfive::BRepSettings{})};

	return 0;
}
END_TEST()
