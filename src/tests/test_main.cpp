#include "lak/string.hpp"

#include "lak/test.hpp"

#ifndef LAK_TEST_MAIN
#  define LAK_TEST_MAIN main
#endif

int LAK_TEST_MAIN(int argc, char **argv)
{
  if (argc < 2 || argv[1] == lak::astring("-help"))
  {
    std::cout << "options:\n"
                 "-help: print this text\n"
                 "-testall: run all registered tests\n"
                 "-tests \"test1;test2\": run the specified tests\n";
    return 0;
  }
  else if (argv[1] == lak::astring("-testall"))
  {
    return lak::run_tests();
  }
  else if (argv[1] == lak::astring("-tests"))
  {
    if (argc < 3) FATAL("Missing tests");
    return lak::run_tests(argv[2]);
  }
  else
  {
    FATAL(
      "unknown flag '", argv[1], "', use -help for a list of valid flags.");
    return EXIT_FAILURE;
  }
}
