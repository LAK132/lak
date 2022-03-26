#include "lak/test.hpp"

#include "lak/compiler.hpp"
#include "lak/macro_utils.hpp"
#include "lak/string_literals.hpp"

#ifdef LAK_COMPILER_MSVC
#	pragma warning(push)
#	pragma warning(disable : 4702)
#endif

BEGIN_TEST(macro_utils)
{
	/* --- do_with --- */

	{
		do
		{
			do_with (const int i = 10)
			{
				ASSERT_EQUAL(i, 10);
				break;
			}

			ASSERT_UNREACHABLE();
		} while (false);
	}

	/* --- for_or --- */

	{
		do
		{
			for_or (size_t i = 0U, i != 0U, ++i)
			{
				ASSERT_UNREACHABLE();
			}
			else
			{
				break;
			}

			ASSERT_UNREACHABLE();
		} while (false);

		for_or (size_t i = 0U, i != 10U, ++i)
		{
			break;
		}
		else
		{
			ASSERT_UNREACHABLE();
		}
	}

	/* --- range_for_or --- */

	{
		do
		{
			range_for_or (const char &c, ""_view)
			{
				(void)c;
				ASSERT_UNREACHABLE();
			}
			else
			{
				break;
			}

			ASSERT_UNREACHABLE();
		} while (false);

		range_for_or (const char &c, "asdf"_view)
		{
			(void)c;
			break;
		}
		else
		{
			ASSERT_UNREACHABLE();
		}
	}

	/* --- while_or --- */

	{
		size_t i = 0U;

		do
		{
			while_or (i != 0U)
			{
				ASSERT_UNREACHABLE();
			}
			else
			{
				break;
			}

			ASSERT_UNREACHABLE();
		} while (false);

		while_or (i == 0U)
		{
			break;
		}
		else
		{
			ASSERT_UNREACHABLE();
		}
	}

	/* --- if_ref --- */

	{
		const char *str = nullptr;

		do
		{
			if_ref (const char &c, str)
			{
				(void)c;
				ASSERT_UNREACHABLE();
			}
			else
			{
				break;
			}

			ASSERT_UNREACHABLE();
		} while (false);

		str = "hello";

		do
		{
			if_ref (const char &c, str)
			{
				(void)c;
				break;
			}
			else
			{
				ASSERT_UNREACHABLE();
			}

			ASSERT_UNREACHABLE();
		} while (false);
	}

	/* --- if_in --- */

	{
		auto arr = {1, 2, 3, 4};

		do
		{
			if_in (10, arr, i)
			{
				(void)i;
				ASSERT_UNREACHABLE();
			}
			else
			{
				break;
			}

			ASSERT_UNREACHABLE();
		} while (false);

		do
		{
			if_in (2, arr, i)
			{
				ASSERT_EQUAL(i, 1U);
				break;
			}
			else
			{
				ASSERT_UNREACHABLE();
			}

			ASSERT_UNREACHABLE();
		} while (false);
	}

	/* --- if_not_in --- */

	{
		auto arr = {1, 2, 3, 4};

		do
		{
			if_not_in (2, arr)
			{
				ASSERT_UNREACHABLE();
			}
			else
			{
				break;
			}

			ASSERT_UNREACHABLE();
		} while (false);

		do
		{
			if_not_in (10, arr)
			{
				break;
			}
			else
			{
				ASSERT_UNREACHABLE();
			}

			ASSERT_UNREACHABLE();
		} while (false);
	}

	return 0;
}
END_TEST()

#ifdef LAK_COMPILER_MSVC
#	pragma warning(pop)
#endif
