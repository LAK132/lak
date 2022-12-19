#include "lak/test.hpp"

#include "lak/compiler.hpp"
#include "lak/result.hpp"

#ifdef LAK_COMPILER_MSVC
#	pragma warning(push)
#	pragma warning(disable : 4702)
#endif

BEGIN_TEST(result)
{
	lak::result<int, int> res = lak::ok_t{10};

	do
	{
		if_let_ok (int &ok, res)
		{
			ASSERT_EQUAL(ok, 10);
			break;
		}
		else
		{
			ASSERT_UNREACHABLE();
		}

		ASSERT_UNREACHABLE();
	} while (false);

	do
	{
		if_let_err (int &err, res)
		{
			(void)err;
			ASSERT_UNREACHABLE();
		}
		else
			break;

		ASSERT_UNREACHABLE();
	} while (false);

	res = lak::err_t{-10};

	do
	{
		if_let_err (int &err, res)
		{
			ASSERT_EQUAL(err, -10);
			break;
		}
		else
		{
			ASSERT_UNREACHABLE();
		}

		ASSERT_UNREACHABLE();
	} while (false);

	do
	{
		if_let_ok (int &ok, res)
		{
			(void)ok;
			ASSERT_UNREACHABLE();
		}
		else
			break;

		ASSERT_UNREACHABLE();
	} while (false);

	return 0;
}
END_TEST()

#ifdef LAK_COMPILER_MSVC
#	pragma warning(pop)
#endif
