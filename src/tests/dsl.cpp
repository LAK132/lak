#include "lak/test.hpp"

#include "lak/dsl/dsl.hpp"
#include "lak/dsl/utility.hpp"

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

	static_assert(lak::dsl::pure_match_parser<lak::dsl::disjunction_t<
	                lak::dsl::sequence<lak::dsl::str_literal<u8"a">>,
	                lak::dsl::sequence<
	                  lak::dsl::str_literal<u8"b">,
	                  lak::dsl::str_literal<u8"c">,
	                  lak::dsl::disjunction<
	                    lak::dsl::sequence<lak::dsl::str_literal<u8"d">>,
	                    lak::dsl::sequence<lak::dsl::str_literal<u8"e">,
	                                       lak::dsl::str_literal<u8"f">>>>>>);

	{
		DEBUG("a+b");

		lak::dsl::sequence_t<lak::dsl::replace_str_literal<u8"a", u8'a'>,
		                     lak::dsl::replace_str_literal<u8"b", u8'b'>>
		  string_sequence = lak::dsl::replace_str_literal<u8"a", u8'a'> +
		                    lak::dsl::replace_str_literal<u8"b", u8'b'>;

		lak::tuple<char8_t, char8_t> result =
		  string_sequence.parse(u8"ab").UNWRAP().value;

		ASSERT_EQUAL(result.template get<0>(), u8'a');
		ASSERT_EQUAL(result.template get<1>(), u8'b');
	}

	{
		DEBUG("a|b");

		lak::variant<char8_t> result =
		  (lak::dsl::replace_str_literal<u8"a", u8'a'> |
		   lak::dsl::replace_str_literal<u8"b", u8'b'>)
		    .parse(u8"ab")
		    .UNWRAP()
		    .value;

		ASSERT_EQUAL(*result.template get<0>(), u8'a');
	}

	{
		DEBUG("(a|b)+(a|b)");

		((lak::dsl::replace_str_literal<u8"a", u8'a'> |
		  lak::dsl::replace_str_literal<
		    u8"b",
		    u8'b'>)+(lak::dsl::replace_str_literal<u8"a", u8'a'> |
		             lak::dsl::replace_str_literal<u8"b", u8'b'>))
		  .parse(u8"acab")
		  .UNWRAP_ERR();
	}

	{
		DEBUG("{2}(a|b)");

		auto result =
		  (lak::dsl::repeat_exact<(lak::dsl::replace_str_literal<u8"a", u8'a'> |
		                           lak::dsl::replace_str_literal<u8"b", u8'b'>),
		                          2>)
		    .parse(u8"abab")
		    .UNWRAP();

		ASSERT_EQUAL(result.value.size(), 2U);
		ASSERT_EQUAL(*result.value[0].template get<0>(), u8'a');
		ASSERT_EQUAL(*result.value[1].template get<0>(), u8'b');
	}

	{
		DEBUG("*(a|b)");

		auto result = (*(lak::dsl::replace_str_literal<u8"a", u8'a'> |
		                 lak::dsl::replace_str_literal<u8"b", u8'b'>))
		                .parse(u8"abab")
		                .UNWRAP();

		ASSERT_EQUAL(result.value.size(), 4U);
		ASSERT_EQUAL(*result.value[0].template get<0>(), u8'a');
		ASSERT_EQUAL(*result.value[1].template get<0>(), u8'b');
		ASSERT_EQUAL(*result.value[2].template get<0>(), u8'a');
		ASSERT_EQUAL(*result.value[3].template get<0>(), u8'b');
	}

	{
		DEBUG("a(b)c");

		ASSERT_EQUAL(
		  lak::u8string((lak::dsl::capture_nth<1U,
		                                       lak::dsl::str_literal<u8"a">,
		                                       lak::dsl::str_literal<u8"b">,
		                                       lak::dsl::str_literal<u8"c">>)
		                  .parse(u8"abc")
		                  .UNWRAP()
		                  .value),
		  u8"b"_str);
	}

	{
		DEBUG("frue|talse");

		enum struct foolean
		{
			frue  = 420,
			talse = 69,
		};

		auto result = (lak::dsl::replace_str_literal<u8"frue", foolean::frue> |
		               lak::dsl::replace_str_literal<u8"talse", foolean::talse>)
		                .parse(u8"talse")
		                .UNWRAP();

		ASSERT(*result.value.template get<0>() == foolean::talse);
	}

	{
		DEBUG("whitespace")

		(+lak::dsl::whitespace)
		  .parse(
		    u8"\u0009\u000A\u000B\u000C\u000D"
		    "\u0020\u0085\u00A0\u1680\u2000"
		    "\u2001\u2002\u2003\u2004\u2005"
		    "\u2006\u2007\u2008\u2009\u200A"
		    "\u2028\u2029\u202F\u205F\u3000")
		  .UNWRAP();
	}

	{
		DEBUG("numbers");

		lak::u8string str = u8"01010101";
		ASSERT_EQUAL(lak::u8string(lak::dsl::bin_number.parse(str).UNWRAP().value),
		             str);

		str = u8"0123456701234567";
		ASSERT_EQUAL(lak::u8string(lak::dsl::oct_number.parse(str).UNWRAP().value),
		             str);

		str = u8"01234567890123456789";
		ASSERT_EQUAL(lak::u8string(lak::dsl::dec_number.parse(str).UNWRAP().value),
		             str);

		str = u8"0123456789abcdefABCDEF0123456789abcdefABCDEF";
		ASSERT_EQUAL(lak::u8string(lak::dsl::hex_number.parse(str).UNWRAP().value),
		             str);
	}

	return 0;
}
END_TEST()
