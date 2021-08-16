#include "lak/test.hpp"

#include "lak/dsl/dsl.hpp"

BEGIN_TEST(dsl)
{
	lak::dsl::sequence_t<lak::dsl::disjunction<>,
	                     lak::dsl::disjunction<>,
	                     lak::dsl::disjunction<>>
	  sequence =
	    lak::dsl::sequence<lak::dsl::disjunction<>> +
	    lak::dsl::sequence<lak::dsl::disjunction<>> + lak::dsl::disjunction<>;

	lak::dsl::disjunction_t<
	  lak::dsl::sequence<>,
	  lak::dsl::sequence<lak::dsl::sequence<>>,
	  lak::dsl::sequence<lak::dsl::sequence<lak::dsl::sequence<>>>>
	  disjunction =
	    lak::dsl::disjunction<lak::dsl::sequence<>> |
	    lak::dsl::disjunction<lak::dsl::sequence<lak::dsl::sequence<>>> |
	    lak::dsl::sequence<lak::dsl::sequence<lak::dsl::sequence<>>>;

	(void)sequence;
	(void)disjunction;

	{
		DEBUG("a+b")
		lak::dsl::sequence_t<lak::dsl::literal<u8"a", u8'a'>,
		                     lak::dsl::literal<u8"b", u8'b'>>
		  string_sequence =
		    lak::dsl::literal<u8"a", u8'a'> + lak::dsl::literal<u8"b", u8'b'>;

		lak::tuple<char8_t, char8_t> result =
		  string_sequence.parse(u8"ab").UNWRAP().value;

		ASSERT_EQUAL(result.template get<0>(), u8'a');
		ASSERT_EQUAL(result.template get<1>(), u8'b');
	}

	{
		DEBUG("a|b")
		lak::variant<char8_t> result =
		  (lak::dsl::literal<u8"a", u8'a'> | lak::dsl::literal<u8"b", u8'b'>)
		    .parse(u8"ab")
		    .UNWRAP()
		    .value;

		ASSERT_EQUAL(*result.template get<0>(), u8'a');
	}

	{
		DEBUG("(a|b)+(a|b)")
		((lak::dsl::literal<u8"a", u8'a'> | lak::dsl::literal<u8"b", u8'b'>)+(
		   lak::dsl::literal<u8"a", u8'a'> | lak::dsl::literal<u8"b", u8'b'>))
		  .parse(u8"acab")
		  .UNWRAP_ERR();
	}

	{
		DEBUG("{2}(a|b)")
		auto result = (lak::dsl::repeat_exact<(lak::dsl::literal<u8"a", u8'a'> |
		                                       lak::dsl::literal<u8"b", u8'b'>),
		                                      2>)
		                .parse(u8"abab")
		                .UNWRAP();

		ASSERT_EQUAL(result.value.size(), 2U);
		ASSERT_EQUAL(*result.value[0].template get<0>(), u8'a');
		ASSERT_EQUAL(*result.value[1].template get<0>(), u8'b');
	}

	{
		DEBUG("*(a|b)")
		auto result =
		  (*(lak::dsl::literal<u8"a", u8'a'> | lak::dsl::literal<u8"b", u8'b'>))
		    .parse(u8"abab")
		    .UNWRAP();

		ASSERT_EQUAL(result.value.size(), 4U);
		ASSERT_EQUAL(*result.value[0].template get<0>(), u8'a');
		ASSERT_EQUAL(*result.value[1].template get<0>(), u8'b');
		ASSERT_EQUAL(*result.value[2].template get<0>(), u8'a');
		ASSERT_EQUAL(*result.value[3].template get<0>(), u8'b');
	}

	return 0;
}
END_TEST()
