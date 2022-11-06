#include "lak/test.hpp"

#include "lak/optional.hpp"

BEGIN_TEST(optional)
{
	return *[]() -> lak::optional<int> { return lak::some_t{0}; }();
}
END_TEST()
