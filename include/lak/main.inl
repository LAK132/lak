#include "lak/result.hpp"

lak::error_code<int> lak_main(lak::span<char *> argv);

int main(int argc, char **argv)
{
	if_let_err (int v, lak_main(lak::span<char *>(argv, size_t(argc))))
		return v;
	else
		return EXIT_SUCCESS;
}
