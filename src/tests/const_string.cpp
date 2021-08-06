#include "lak/test.hpp"

#include "lak/const_string.hpp"
#include "lak/string_view.hpp"

template<lak::u8const_string STR>
lak::u8string_view template_test_function()
{
  return lak::string_view(STR.begin(), STR.size());
}

BEGIN_TEST(const_view)
{
  std::cout << template_test_function<u8"Hello, World!">();
  return 0;
}
END_TEST()
