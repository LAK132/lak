#include <lak/lisk/lisk.hpp>

#include <iostream>
#include <typeindex>
#include <typeinfo>

bool running = true;

lak::lisk::expression my_exit(lak::lisk::environment &, bool)
{
	running = false;
	return lak::lisk::expression(lak::lisk::atom(lak::lisk::atom::nil{}));
}

int main()
{
	std::set_terminate(lak::terminate_handler);

#ifdef LAK_OS_WINDOWS
	/* --- Enable SGR codes in the Windows terminal --- */

	do
	{
		HANDLE hOut = ::GetStdHandle(STD_OUTPUT_HANDLE);
		if (hOut == INVALID_HANDLE_VALUE)
		{
			WARNING(::GetLastError());
			break;
		}

		DWORD dwMode = 0;
		if (!::GetConsoleMode(hOut, &dwMode))
		{
			WARNING(::GetLastError());
			break;
		}

		dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
		if (!::SetConsoleMode(hOut, dwMode))
		{
			WARNING(::GetLastError());
			break;
		}
	} while (false);
#endif

	lak::lisk::environment default_env = lak::lisk::builtin::default_env();

	default_env.define_function("exit", LISK_FUNCTION_WRAPPER(my_exit));

	// REPL. Use "(exit)" to quit the program.
	while (running)
	{
		std::cout << "lisk> ";
		std::string string;
		std::getline(std::cin, string);
		if (!std::cin.good())
		{
			std::cout << "\nlisk$ Goodbye!\n";
			break;
		}
		const auto tokens = lak::lisk::tokenise(lak::to_u8string(string));
		const auto expr   = lak::lisk::parse(tokens);
		const auto eval   = lak::lisk::eval(expr, default_env, true);
		const auto result = to_string(eval);
		std::cout << "lisk$ " << lak::to_astring(result) << "\n";
	}
	std::cout << "\n";
}
