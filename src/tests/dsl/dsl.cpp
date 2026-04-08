#include "lak/test.hpp"

#include "lak/dsl/dsl.hpp"
#include "lak/dsl/utility.hpp"

#include "lak/string_literals/view.hpp"

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

	static_assert(lak::dsl::concepts::pure_match_parser<lak::dsl::disjunction_t<
	                lak::dsl::sequence<lak::dsl::str_literal<u8"a">>,
	                lak::dsl::sequence<
	                  lak::dsl::str_literal<u8"b">,
	                  lak::dsl::str_literal<u8"c">,
	                  lak::dsl::disjunction<
	                    lak::dsl::sequence<lak::dsl::str_literal<u8"d">>,
	                    lak::dsl::sequence<lak::dsl::str_literal<u8"e">,
	                                       lak::dsl::str_literal<u8"f">>>>>>);

	static_assert(lak::dsl::concepts::pure_match_parser<lak::dsl::conjunction_t<
	                lak::dsl::sequence<lak::dsl::str_literal<u8"a">>,
	                lak::dsl::sequence<
	                  lak::dsl::str_literal<u8"b">,
	                  lak::dsl::str_literal<u8"c">,
	                  lak::dsl::conjunction<
	                    lak::dsl::sequence<lak::dsl::str_literal<u8"d">>,
	                    lak::dsl::sequence<lak::dsl::str_literal<u8"e">,
	                                       lak::dsl::str_literal<u8"f">>>>>>);

	// static_assert(lak::dsl::concepts::pure_match_parser<decltype(U"a"_dsl_char)>);
	// static_assert(lak::dsl::concepts::pure_match_parser<decltype(u8"asdf"_dsl_str)>);

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
		ASSERT_EQUAL(result.value[0], u8'a');
		ASSERT_EQUAL(result.value[1], u8'b');
	}

	{
		DEBUG("*(a|b)");

		auto result = (*(lak::dsl::replace_str_literal<u8"a", u8'a'> |
		                 lak::dsl::replace_str_literal<u8"b", u8'b'>))
		                .parse(u8"abab")
		                .UNWRAP();

		ASSERT_EQUAL(result.value.size(), 4U);
		ASSERT_EQUAL(result.value[0], u8'a');
		ASSERT_EQUAL(result.value[1], u8'b');
		ASSERT_EQUAL(result.value[2], u8'a');
		ASSERT_EQUAL(result.value[3], u8'b');
	}

	{
		DEBUG("*(!a&!b)");

		auto result =
		  // (*((!U"a"_dsl_char) & (!U"b"_dsl_char))).parse(u8"cccab").UNWRAP();
		  (*((!lak::dsl::char_literal<U'a'>)&(!lak::dsl::char_literal<U'b'>)))
		    .parse(u8"cccab")
		    .UNWRAP();

		ASSERT_EQUAL(result.value, u8"ccc"_view);
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
		DEBUG("{cba}");

		ASSERT_EQUAL(
		  lak::u8string((lak::dsl::unordered<lak::dsl::str_literal<u8"c">,
		                                     lak::dsl::str_literal<u8"b">,
		                                     lak::dsl::str_literal<u8"a">>)
		                  .parse(u8"abc")
		                  .UNWRAP()
		                  .value),
		  u8"abc"_str);

		ASSERT((lak::dsl::unordered<lak::dsl::str_literal<u8"c">,
		                            lak::dsl::str_literal<u8"b">,
		                            lak::dsl::str_literal<u8"a">>)
		         .parse(u8"accb")
		         .is_err());
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

		ASSERT(result.value == foolean::talse);
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

		(+lak::dsl::whitespace)
		  .parse(
		    u8"a\u0009\u000A\u000B\u000C\u000D"
		    "\u0020\u0085\u00A0\u1680\u2000"
		    "\u2001\u2002\u2003\u2004\u2005"
		    "\u2006\u2007\u2008\u2009\u200A"
		    "\u2028\u2029\u202F\u205F\u3000")
		  .UNWRAP_ERR();

		auto nbsp = (!lak::dsl::char_literal<U'\n'>)&lak::dsl::whitespace;

		nbsp.parse(u8"a"_view).UNWRAP_ERR();

		ASSERT_EQUAL((*nbsp).parse(u8" asd\n"_view).UNWRAP().value, u8" "_view);
		ASSERT_EQUAL((*nbsp).parse(u8" \n"_view).UNWRAP().value, u8" "_view);
	}

	{
		DEBUG("until");

		auto str = u8"'asd'f'g''hhh"_view;

		constexpr auto parser1 = lak::dsl::str_literal<u8"'"> +
		                         lak::dsl::until<lak::dsl::str_literal<u8"'">> +
		                         lak::dsl::str_literal<u8"'">;
		ASSERT_EQUAL(parser1.parse(str).UNWRAP().value, u8"'asd'"_view);

		constexpr auto parser2 = lak::dsl::str_literal<u8"'"> +
		                         lak::dsl::until_str<u8"'"> +
		                         lak::dsl::str_literal<u8"'">;
		ASSERT_EQUAL(parser2.parse(str).UNWRAP().value, u8"'asd'"_view);

		constexpr auto parser3 =
		  lak::dsl::capture_nth<1U,
		                        lak::dsl::str_literal<u8"'">,
		                        lak::dsl::until_str<u8"'">,
		                        lak::dsl::str_literal<u8"'">>;
		ASSERT_EQUAL(parser3.parse(str).UNWRAP().value, u8"asd"_view);

		constexpr auto parser4 =
		  lak::dsl::capture_2nd<lak::dsl::str_literal<u8"'">,
		                        lak::dsl::until_str<u8"'">,
		                        lak::dsl::str_literal<u8"'">>;
		ASSERT_EQUAL(parser4.parse(str).UNWRAP().value, u8"asd"_view);

		constexpr auto parser5 =
		  lak::dsl::capture_simple_bounded_str<u8"'", u8"'">;
		ASSERT_EQUAL(parser5.parse(str).UNWRAP().value, u8"asd"_view);
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

		str = u8"+0.1e-2";
		auto [int_part, frac_part, exp_part] = lak::dsl::dec_float<
			lak::dsl::char_literal<U'.'>, lak::dsl::char_literal<U'e'>
		>.parse(str).UNWRAP().value;
		ASSERT_EQUAL(lak::u8string_view(int_part), u8"+0"_view);
		ASSERT(!frac_part.empty());
		ASSERT_EQUAL(lak::u8string_view(frac_part), u8"1"_view);
		ASSERT(!exp_part.empty());
		ASSERT_EQUAL(lak::u8string_view(exp_part), u8"-2"_view);

		auto f = lak::dsl::parsed_dec_float<double,
							lak::dsl::char_literal<U'.'>, lak::dsl::char_literal<U'e'>>
							.parse(u8"+0.0e+0").UNWRAP().value;
		ASSERT_EQUAL(f, 0.0);

		f = lak::dsl::parsed_dec_float<double,
					lak::dsl::char_literal<U'.'>, lak::dsl::char_literal<U'e'>>
					.parse(u8"-1.0e+0").UNWRAP().value;
		ASSERT_EQUAL(f, -1.0);
	}

	{
		auto str = u8"a"_view;

		auto match1 = lak::dsl::match_sequence<
		  lak::dsl::match<lak::dsl::bottom, lak::dsl::dummy_impure<int>>,
		  lak::dsl::match<lak::dsl::bottom, lak::dsl::top>>;
		static_assert(!lak::dsl::concepts::pure_match_parser<
		              lak::remove_cvref_t<decltype(match1)>>);
		ASSERT(
		  !match1.parse(str).UNWRAP_ERR().info.holds<lak::dsl::err::bottom>());

		auto match2 = match1 | lak::dsl::match<lak::dsl::top, lak::dsl::bottom>;
		static_assert(!lak::dsl::concepts::pure_match_parser<
		              lak::remove_cvref_t<decltype(match2)>>);
		ASSERT(match2.parse(str).UNWRAP_ERR().info.holds<lak::dsl::err::bottom>());

		auto match3 =
		  lak::dsl::match_sequence<lak::dsl::match<lak::dsl::top, lak::dsl::top>>;
		static_assert(lak::dsl::concepts::pure_match_parser<
		              lak::remove_cvref_t<decltype(match3)>>);
		match3.parse(str).UNWRAP();
	}

	return 0;
}
END_TEST()
