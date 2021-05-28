#include "lak/test.hpp"
#include "lak/span.hpp"

std::unordered_map<lak::astring, int (*)()> &registered_tests()
{
  static std::unordered_map<lak::astring, int (*)()> tests;
  return tests;
}

int lak::run_tests(const lak::astring &tests)
{
  auto run_test = [](const lak::astring &test, int (*func)()) -> int {
    lak::scoped_indenter indent("testing " + test);
    const int result = func();
    if (result == 0)
      lak::debugger.std_out(""_u8, LAK_GREEN "PASSED" LAK_SGR_RESET "\n");
    else
      lak::debugger.std_err(""_u8, LAK_RED "FAILED" LAK_SGR_RESET "\n");
    return result;
  };

  auto test = lak::string_view(tests);

  const auto &reg_tests = registered_tests();

  while_or(!test.empty())
  {
    auto [do_test, remaining_tests] = lak::split_before<const char>(test, ';');

    test = remaining_tests.subspan(remaining_tests.empty() ? 0 : 1);

    if (auto func = reg_tests.find(do_test.to_string());
        func != reg_tests.end())
    {
      if (int result = run_test(func->first, func->second); result != 0)
        return result;
    }
    else
    {
      FATAL("Test '", do_test.to_string(), "' not found");
      return EXIT_FAILURE;
    }
  }
  else
  {
    for (const auto &[do_test, func] : reg_tests)
      if (int result = run_test(do_test, func); result != 0) return result;
  }

  return 0;
}

bool lak::register_test(const lak::astring &test_name, int (*test_function)())
{
  ASSERT(registered_tests().try_emplace(test_name, test_function).second);
  return true;
}
