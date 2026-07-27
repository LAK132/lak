#include "lak/lisk/lisk.hpp"

#include "lak/test.hpp"

struct my_type
{
	lak::lisk::uint_t value;
};

template<>
struct lak::lisk::from_expr_traits<my_type>
{
	using value_type = my_type;
	using error_type = lak::monostate;
	static lak::result<my_type> from_expr(const lak::lisk::expression &expr)
	{
		return lak::lisk::from_expr<lak::lisk::uint_t>(expr).map(
		  [](lak::lisk::uint_t v) -> my_type { return {v}; });
	}
};

const lak::lisk::string &type_name(const my_type &)
{
	const static lak::lisk::string name = "my_type";
	return name;
}

lak::lisk::expression function_taking_my_type(lak::lisk::environment &,
                                              bool,
                                              my_type my)
{
	return lak::lisk::atom{
	  lak::lisk::string{lak::fmt<u8"My type value: {}">(my.value)}};
}

lak::lisk::expression print_my_type_value(lak::lisk::environment &e,
                                          bool allow_tail,
                                          lak::shared_ptr<my_type> my)
{
	return my ? function_taking_my_type(e, allow_tail, *my)
	          : lak::lisk::expression::null{};
}

lak::lisk::expression create_my_type_ptr(lak::lisk::environment &, bool)
{
	return lak::lisk::atom(
	  lak::lisk::pointer(lak::shared_ptr<my_type>::make(10U)));
}

BEGIN_TEST(lisk)
{
	lak::lisk::environment default_env = lak::lisk::builtin::default_env();

	ASSERT_EQUAL(lak::lisk::eval_string("(10)", default_env)
	               .get_atom()
	               .EXPECT("expected an atom")
	               .get_number()
	               .EXPECT("expected a number")
	               .get_uint()
	               .EXPECT("expected a uint"),
	             10U);

	ASSERT_EQUAL(lak::lisk::eval_string("(+ 10 10)", default_env)
	               .get_atom()
	               .EXPECT("expected an atom")
	               .get_number()
	               .EXPECT("expected a number")
	               .get_uint()
	               .EXPECT("expected a uint"),
	             20U);

	ASSERT_EQUAL(lak::lisk::eval_string("(+ +10 +10)", default_env)
	               .get_atom()
	               .EXPECT("expected an atom")
	               .get_number()
	               .EXPECT("expected a number")
	               .get_sint()
	               .EXPECT("expected an sint"),
	             20);

	ASSERT_EQUAL(lak::lisk::eval_string("(+ (+ 10 10) (+ 10 10))", default_env)
	               .get_atom()
	               .EXPECT("expected an atom")
	               .get_number()
	               .EXPECT("expected a number")
	               .get_uint()
	               .EXPECT("expected a uint"),
	             40U);

	ASSERT_EQUAL(lak::lisk::eval_string(
	               "(begin "
	               "(define func (lambda (x n) "
	               "	(if (zero? n) x (tail (func (* x 2) (- n 1)))))) "
	               "(func 2 10)"
	               ")",
	               default_env)
	               .get_atom()
	               .EXPECT("expected an atom")
	               .get_number()
	               .EXPECT("expected a number")
	               .get_uint()
	               .EXPECT("expected a uint"),
	             2048U);

	default_env.define_function("print_my_type",
	                            LISK_FUNCTION_WRAPPER(print_my_type_value));
	default_env.define_function("create_my_type",
	                            LISK_FUNCTION_WRAPPER(create_my_type_ptr));

	ASSERT_EQUAL(
	  lak::lisk::eval_string("(print_my_type (create_my_type))", default_env)
	    .get_atom()
	    .EXPECT("expected an atom")
	    .get_string()
	    .EXPECT("expected a string"),
	  u8"My type value: 10"_view);

	lak::lisk::eval_string("(print_my_type 1337)", default_env)
	  .get_exception()
	  .EXPECT("expected an exception");

	return 0;
}
END_TEST()
