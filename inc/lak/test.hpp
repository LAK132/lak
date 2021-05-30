#ifndef LAK_TEST_HPP
#define LAK_TEST_HPP

#include "lak/debug.hpp"
#include "lak/string.hpp"

#include <unordered_map>

namespace lak
{
  int run_tests(const lak::astring &tests = "");

  [[nodiscard]] bool register_test(const lak::astring &test_name,
                                   int (*test_function)());

#define BEGIN_TEST(NAME)                                                      \
  const static bool UNIQUIFY(TEST_##NAME) = lak::register_test(#NAME, []()->int
#define END_TEST() );
}

#endif
