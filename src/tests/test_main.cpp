#include "lak/string.hpp"
#include "lak/string_literals.hpp"

#include "lak/test.hpp"

#ifndef LAK_TEST_MAIN
#	define LAK_TEST_MAIN main
#endif

int LAK_TEST_MAIN(int argc, char **argv)
{
	lak::astring_view arg1{
	  lak::astring_view::from_c_str(argc < 2 ? "" : argv[1])};
	if (argc < 2 || arg1 == "-h"_view || arg1 == "--help"_view)
	{
		std::cout << "options:\n"
		             "-help: print this text\n"
		             "-testall: run all registered tests\n"
		             "-test[s] \"test1;test2\": run the specified tests\n";
		return argc < 2 ? EXIT_FAILURE : EXIT_SUCCESS;
	}
	else if (arg1 == "--testall"_view)
	{
		return lak::run_tests();
	}
	else if (arg1 == "--tests"_view || arg1 == "--test"_view)
	{
		if (argc < 3) FATAL("Missing tests");
		return lak::run_tests(
		  lak::as_u8string(lak::astring_view::from_c_str(argv[2])));
	}
	else
	{
		FATAL("unknown flag '", arg1, "', use --help for a list of valid flags.");
	}
}
