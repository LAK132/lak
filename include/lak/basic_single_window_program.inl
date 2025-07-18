#include "lak/basic_program.inl"

#ifndef LAK_BASIC_SINGLE_WINDOW_PROGRAM_PREFIX
#	define LAK_BASIC_SINGLE_WINDOW_PROGRAM_PREFIX basic_single_window_
#endif

#define LAK_BASIC_SINGLE_WINDOW_PROGRAM(X)                                    \
	TOKEN_CONCAT(LAK_BASIC_SINGLE_WINDOW_PROGRAM_PREFIX, X)

// Implement this functions in your program.
lak::optional<int> LAK_BASIC_SINGLE_WINDOW_PROGRAM(program_init)();
lak::window *LAK_BASIC_SINGLE_WINDOW_PROGRAM(window);

lak::optional<int> LAK_BASIC_PROGRAM(program_init)()
{
	if_let_some (auto v, LAK_BASIC_SINGLE_WINDOW_PROGRAM(program_init)())
		return lak::some_t{v};

	LAK_BASIC_SINGLE_WINDOW_PROGRAM(window) =
	  &LAK_BASIC_PROGRAM(create_window)().UNWRAP();

	return lak::nullopt;
}

bool LAK_BASIC_PROGRAM(program_loop)(uint64_t)
{
	return !LAK_BASIC_PROGRAM(window_instances)().empty();
}
