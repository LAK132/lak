#ifndef LAK_DSL_DSL_HPP
#define LAK_DSL_DSL_HPP

#include "lak/array.hpp"
#include "lak/compare.hpp"
#include "lak/concepts.hpp"
#include "lak/const_string.hpp"
#include "lak/result.hpp"
#include "lak/unicode.hpp"

#include <ostream>

namespace lak
{
	namespace dsl
	{
		/* --- parse_error --- */

		struct parse_error
		{
			inline lak::u8string to_string() const { return {}; }

			inline friend std::ostream &operator<<(std::ostream &strm,
			                                       const lak::dsl::parse_error &err)
			{
				return strm << lak::u8string_view(err.to_string());
			}
		};

		/* --- parse_result --- */

		template<typename T>
		struct parse_result
		{
			using value_type = T;
			lak::u8string_view consumed;
			lak::u8string_view remaining;
			T value;
		};

		/* --- result --- */

		template<typename T>
		using result =
		  lak::result<lak::dsl::parse_result<T>, lak::dsl::parse_error>;

		/* --- parser --- */

		template<typename T>
		concept parser = requires(const T t) {
			typename T::value_type;

			requires(T::is_pure_match
			           ? lak::is_same_v<typename T::value_type, lak::u8string_view>
			           : true);

			{
				t.parse(lak::u8string_view{})
			} -> lak::concepts::same_as<lak::dsl::result<typename T::value_type>>;
		};

		template<typename T>
		concept pure_match_parser = lak::dsl::parser<T> && T::is_pure_match;

		/* --- sequence --- */

		template<lak::dsl::parser auto... parsers>
		struct sequence_t
		{
			static constexpr bool is_pure_match =
			  ((lak::dsl::pure_match_parser<decltype(parsers)>)&&...);

			using value_type = lak::conditional_t<
			  is_pure_match,
			  lak::u8string_view,
			  lak::tuple<typename decltype(parsers)::value_type...>>;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			{
				return parse(str, lak::index_sequence_for<decltype(parsers)...>{});
			}

			template<size_t... I>
			requires !is_pure_match
			         lak::dsl::result<value_type> parse(
			           lak::u8string_view str, lak::index_sequence<I...>) const
			{
				// :TODO: an uninitialised tuple type could be an interesting
				// optimisation here, so that we aren't default constructing
				// everything.

				lak::dsl::result<value_type> result =
				  lak::dsl::result<value_type>::make_ok({
				    .consumed  = {},
				    .remaining = str,
				    .value     = {},
				  });

				((parsers.parse(result.unsafe_unwrap().remaining)
				    .if_err([&](const lak::dsl::parse_error &err)
				            { result = lak::err_t{err}; })
				    .if_ok(
				      [&]<typename T>(lak::dsl::parse_result<T> &&res)
				      {
					      result.unsafe_unwrap().remaining = res.remaining;
					      result.unsafe_unwrap().value.template get<I>() =
					        lak::forward<T>(res.value);
				      })
				    .is_ok()) &&
				 ...);

				if_let_ok (auto &res, result)
					res.consumed = str.first(str.size() - res.remaining.size());

				return result;
			}

			template<size_t... I>
			requires is_pure_match
			lak::dsl::result<value_type> parse(lak::u8string_view str,
			                                   lak::index_sequence<I...>) const
			{
				lak::dsl::result<value_type> result =
				  lak::dsl::result<value_type>::make_ok({
				    .consumed  = {},
				    .remaining = str,
				    .value     = {},
				  });

				((parsers.parse(result.unsafe_unwrap().remaining)
				    .if_err([&](const lak::dsl::parse_error &err)
				            { result = lak::err_t{err}; })
				    .if_ok([&]<typename T>(lak::dsl::parse_result<T> &&res)
				           { result.unsafe_unwrap().remaining = res.remaining; })
				    .is_ok()) &&
				 ...);

				if_let_ok (auto &res, result)
					result.value = result.consumed =
					  str.first(str.size() - result.remaining.size());

				return result;
			}
		};

		template<>
		struct sequence_t<>
		{
			static constexpr bool is_pure_match = false;
			using value_type                    = lak::tuple<>;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			{
				return lak::ok_t{lak::dsl::parse_result<value_type>{
				  .consumed  = {},
				  .remaining = str,
				  .value     = {},
				}};
			}
		};

		template<lak::dsl::parser auto... parsers>
		inline constexpr lak::dsl::sequence_t<parsers...> sequence;

		static_assert(lak::dsl::parser<lak::dsl::sequence_t<>>);
		static_assert(
		  lak::dsl::parser<lak::dsl::sequence_t<lak::dsl::sequence<>>>);

		/* --- is_sequence --- */

		template<typename T>
		struct is_sequence : lak::false_type
		{
		};
		template<lak::dsl::parser auto... parsers>
		struct is_sequence<lak::dsl::sequence_t<parsers...>> : lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_sequence_v = lak::dsl::is_sequence<T>::value;

		static_assert(lak::dsl::is_sequence_v<lak::dsl::sequence_t<>>);
		static_assert(
		  lak::dsl::is_sequence_v<lak::dsl::sequence_t<lak::dsl::sequence<>>>);

		/* --- operator+ --- */

		template<lak::dsl::parser L, lak::dsl::parser R>
		requires(!lak::dsl::is_sequence_v<L> && !lak::dsl::is_sequence_v<R>)
		inline constexpr auto operator+(L, R)
		{
			return lak::dsl::sequence<L{}, R{}>;
		}

		template<lak::dsl::parser auto... L, lak::dsl::parser R>
		requires(!lak::dsl::is_sequence_v<R>)
		inline constexpr auto operator+(lak::dsl::sequence_t<L...>, R)
		{
			return lak::dsl::sequence<L..., R{}>;
		}

		template<lak::dsl::parser L, lak::dsl::parser auto... R>
		requires(!lak::dsl::is_sequence_v<L>)
		inline constexpr auto operator+(L, lak::dsl::sequence_t<R...>)
		{
			return lak::dsl::sequence<L{}, R...>;
		}

		template<lak::dsl::parser auto... L, lak::dsl::parser auto... R>
		inline constexpr auto operator+(lak::dsl::sequence_t<L...>,
		                                lak::dsl::sequence_t<R...>)
		{
			return lak::dsl::sequence<L..., R...>;
		}

		/* --- repeat --- */

		template<lak::dsl::parser auto par,
		         size_t min = 0,
		         size_t max = lak::dynamic_extent>
		struct repeat_t
		{
			static constexpr bool is_pure_match = decltype(par)::is_pure_match;
			using value_type =
			  lak::conditional_t<is_pure_match,
			                     lak::u8string_view,
			                     lak::array<typename decltype(par)::value_type>>;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			requires !is_pure_match
			{
				lak::dsl::parse_result<value_type> result =
				  lak::dsl::parse_result<value_type>{
				    .consumed  = {},
				    .remaining = str,
				    .value     = {},
				  };

				size_t count = 0;
				while (count < max &&
				       par.parse(result.remaining)
				         .if_ok(
				           [&]<typename T>(lak::dsl::parse_result<T> &&res)
				           {
					           result.remaining = res.remaining;
					           result.value.push_back(lak::forward<T>(res.value));
				           })
				         .is_ok())
					++count;

				if (count < min) return lak::err_t{lak::dsl::parse_error{}};

				result.consumed = str.first(str.size() - result.remaining.size());

				return lak::ok_t{result};
			}

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			requires is_pure_match
			{
				lak::dsl::parse_result<value_type> result =
				  lak::dsl::parse_result<value_type>{
				    .consumed  = {},
				    .remaining = str,
				    .value     = {},
				  };

				size_t count = 0;
				while (count < max &&
				       par.parse(result.remaining)
				         .if_ok([&]<typename T>(lak::dsl::parse_result<T> &&res)
				                { result.remaining = res.remaining; })
				         .is_ok())
					++count;

				if (count < min) return lak::err_t{lak::dsl::parse_error{}};

				result.value = result.consumed =
				  str.first(str.size() - result.remaining.size());

				return lak::ok_t{result};
			}
		};

		template<lak::dsl::parser auto par>
		inline constexpr lak::dsl::repeat_t<par> repeat;

		template<lak::dsl::parser auto par, size_t count>
		inline constexpr lak::dsl::repeat_t<par, count> repeat_at_least;

		template<lak::dsl::parser auto par, size_t count>
		inline constexpr lak::dsl::repeat_t<par, count, count> repeat_exact;

		template<lak::dsl::parser auto par, size_t min, size_t max>
		inline constexpr lak::dsl::repeat_t<par, min, max> repeat_range;

		static_assert(lak::dsl::parser<lak::dsl::repeat_t<lak::dsl::sequence<>>>);
		static_assert(
		  lak::dsl::parser<lak::dsl::repeat_t<lak::dsl::sequence<>, 1>>);

		/* --- is_repeat --- */

		template<typename T>
		struct is_repeat : lak::false_type
		{
		};
		template<lak::dsl::parser auto parser, size_t count>
		struct is_repeat<lak::dsl::repeat_t<parser, count>> : lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_repeat_v = lak::dsl::is_repeat<T>::value;

		/* --- operator* --- */

		template<lak::dsl::parser R>
		requires(!lak::dsl::is_repeat_v<R>)
		inline constexpr auto operator*(R)
		{
			return lak::dsl::repeat<R{}>;
		}

		template<lak::dsl::parser auto R, size_t S>
		inline constexpr auto operator*(lak::dsl::repeat_t<R, S>)
		{
			return lak::dsl::repeat<R>;
		}

		/* --- operator+ --- */

		template<lak::dsl::parser R>
		requires(!lak::dsl::is_repeat_v<R>)
		inline constexpr auto operator+(R)
		{
			return lak::dsl::repeat_at_least<R{}, 1U>;
		}

		template<lak::dsl::parser auto R, size_t S>
		inline constexpr auto operator+(lak::dsl::repeat_t<R, S>)
		{
			return lak::dsl::repeat_at_least<R, 1U>;
		}

		/* --- disjunction --- */

		template<lak::dsl::parser auto... parsers>
		struct disjunction_t
		{
			static constexpr bool is_pure_match =
			  ((lak::dsl::pure_match_parser<decltype(parsers)>)&&...);
			using value_type = lak::conditional_t<
			  is_pure_match,
			  lak::u8string_view,
			  lak::create_from_pack_t<
			    lak::variant,
			    lak::make_unique_pack_t<typename decltype(parsers)::value_type...>>>;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			requires(!is_pure_match)
			{
				// :TODO: an uninitialised tuple type could be an interesting
				// optimisation here, so that we aren't default constructing
				// everything.

				lak::dsl::result<value_type> result =
				  lak::dsl::result<value_type>::make_ok({
				    .consumed  = {},
				    .remaining = str,
				    .value     = {},
				  });

				(((result = parsers.parse(str).map(
				     [&]<typename T>(lak::dsl::parse_result<T> &&res)
				       -> lak::dsl::parse_result<value_type>
				     {
					     return {
					       .consumed  = res.consumed,
					       .remaining = res.remaining,
					       .value     = lak::forward<T>(res.value),
					     };
				     }))
				    .is_ok()) ||
				 ...);

				return result;
			}

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			requires(is_pure_match)
			{
				lak::dsl::result<value_type> result =
				  lak::dsl::result<value_type>::make_ok({
				    .consumed  = {},
				    .remaining = str,
				    .value     = {},
				  });

				(((result = parsers.parse(str)).is_ok()) || ...);

				return result;
			}
		};

		template<>
		struct disjunction_t<>
		{
			static constexpr bool is_pure_match = false;
			using value_type                    = lak::variant<lak::monostate>;

			lak::dsl::result<value_type> parse(lak::u8string_view) const
			{
				return lak::dsl::result<value_type>::make_err({});
			}
		};

		template<lak::dsl::parser auto... parsers>
		inline constexpr lak::dsl::disjunction_t<parsers...> disjunction;

		static_assert(lak::dsl::parser<lak::dsl::disjunction_t<>>);
		static_assert(
		  lak::dsl::parser<
		    lak::dsl::disjunction_t<lak::dsl::sequence<>, lak::dsl::sequence<>>>);

		/* --- is_disjunction --- */

		template<typename T>
		struct is_disjunction : lak::false_type
		{
		};
		template<lak::dsl::parser auto... parsers>
		struct is_disjunction<lak::dsl::disjunction_t<parsers...>> : lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_disjunction_v =
		  lak::dsl::is_disjunction<T>::value;

		static_assert(lak::dsl::is_disjunction_v<lak::dsl::disjunction_t<>>);
		static_assert(lak::dsl::is_disjunction_v<
		              lak::dsl::disjunction_t<lak::dsl::disjunction<>>>);

		/* --- operator| --- */

		template<lak::dsl::parser L, lak::dsl::parser R>
		requires(!lak::dsl::is_disjunction_v<L> && !lak::dsl::is_disjunction_v<R>)
		inline constexpr auto operator|(L, R)
		{
			return lak::dsl::disjunction<L{}, R{}>;
		}

		template<lak::dsl::parser auto... L, lak::dsl::parser R>
		requires(!lak::dsl::is_disjunction_v<R>)
		inline constexpr auto operator|(lak::dsl::disjunction_t<L...>, R)
		{
			return lak::dsl::disjunction<L..., R{}>;
		}

		template<lak::dsl::parser L, lak::dsl::parser auto... R>
		requires(!lak::dsl::is_disjunction_v<L>)
		inline constexpr auto operator|(L, lak::dsl::disjunction_t<R...>)
		{
			return lak::dsl::disjunction<L{}, R...>;
		}

		template<lak::dsl::parser auto... L, lak::dsl::parser auto... R>
		inline constexpr auto operator|(lak::dsl::disjunction_t<L...>,
		                                lak::dsl::disjunction_t<R...>)
		{
			return lak::dsl::disjunction<L..., R...>;
		}

		/* --- capture --- */

		template<lak::dsl::parser auto par>
		struct capture_t
		{
			static constexpr bool is_pure_match = false;
			using value_type                    = typename decltype(par)::value_type;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			{
				return par.parse(str);
			}
		};

		template<lak::dsl::parser auto par>
		inline constexpr lak::dsl::capture_t<par> capture;

		static_assert(lak::dsl::parser<lak::dsl::capture_t<lak::dsl::sequence<>>>);

		/* --- is_capture --- */

		template<typename T>
		struct is_capture : lak::false_type
		{
		};
		template<lak::dsl::parser auto par>
		struct is_capture<lak::dsl::capture_t<par>> : lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_capture_v = lak::dsl::is_capture<T>::value;

		static_assert(
		  lak::dsl::is_capture_v<lak::dsl::capture_t<lak::dsl::sequence<>>>);
		static_assert(
		  lak::dsl::is_capture_v<
		    lak::dsl::capture_t<lak::dsl::capture<lak::dsl::sequence<>>>>);

		/* --- capture_nth --- */

		template<size_t N,
		         lak::dsl::parser auto par,
		         lak::dsl::parser auto... parsers>
		struct capture_nth_t
		{
			static_assert(N <= sizeof...(parsers));

			static constexpr bool is_pure_match = false;
			using value_type                    = typename lak::
			  nth_type_t<N, decltype(par), decltype(parsers)...>::value_type;

			static force_inline lak::dsl::result<value_type> impl_parse(
			  lak::u8string_view str)
			requires(N > 0U)
			{
				auto result = par.parse(str);
				if_let_ok (const auto &res, result)
					return capture_nth_t<N - 1U, parsers...>::impl_parse(res.remaining);
				else
					return lak::err_t{result.unsafe_unwrap_err()};
			}

			static force_inline lak::dsl::result<value_type> impl_parse(
			  lak::u8string_view str)
			requires(N == 0U)
			{
				auto result = par.parse(str);
				((parsers.parse(result.unsafe_unwrap().remaining)
				    .if_err([&](const lak::dsl::parse_error &err)
				            { result = lak::err_t{err}; })
				    .if_ok([&]<typename T>(lak::dsl::parse_result<T> &&res)
				           { result.unsafe_unwrap().remaining = res.remaining; })
				    .is_ok()) &&
				 ...);
				return result;
			}

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			{
				auto result = impl_parse(str);

				if_let_ok (auto &res, result)
					res.consumed = str.first(str.size() - res.remaining.size());

				return result;
			}
		};

		template<size_t N, lak::dsl::parser auto... parsers>
		inline constexpr lak::dsl::capture_nth_t<N, parsers...> capture_nth;

		static_assert(
		  lak::dsl::parser<lak::dsl::capture_nth_t<0U, lak::dsl::sequence<>>>);

		/* --- is_capture_nth --- */

		template<typename T>
		struct is_capture_nth : lak::false_type
		{
		};
		template<size_t N, lak::dsl::parser auto... parsers>
		struct is_capture_nth<lak::dsl::capture_nth_t<N, parsers...>>
		: lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_capture_nth_v =
		  lak::dsl::is_capture_nth<T>::value;

		static_assert(lak::dsl::is_capture_nth_v<
		              lak::dsl::capture_nth_t<0U, lak::dsl::sequence<>>>);
		static_assert(
		  lak::dsl::is_capture_nth_v<
		    lak::dsl::capture_nth_t<0U, lak::dsl::capture<lak::dsl::sequence<>>>>);

		/* --- str_literal --- */

		template<lak::u8const_string const_str>
		struct str_literal_t
		{
			static constexpr bool is_pure_match = true;
			using value_type                    = lak::u8string_view;

			lak::dsl::result<lak::u8string_view> parse(lak::u8string_view str) const
			{
				if (lak::compare(
				      lak::u8string_view(const_str.begin(), const_str.end()), str) ==
				    const_str.size())
					return lak::ok_t{lak::dsl::parse_result<value_type>{
					  .consumed  = str.first(const_str.size()),
					  .remaining = str.substr(const_str.size()),
					  .value     = str.first(const_str.size()),
					}};
				else
					return lak::err_t{lak::dsl::parse_error{}};
			}
		};

		template<lak::u8const_string const_str>
		inline constexpr lak::dsl::str_literal_t<const_str> str_literal;

		static_assert(lak::dsl::parser<lak::dsl::str_literal_t<u8"a">>);

		template<lak::u8const_string... const_strs>
		inline constexpr lak::dsl::disjunction_t<
		  lak::dsl::str_literal<const_strs>...>
		  one_of_strs;

		/* --- is_str_literal --- */

		template<typename T>
		struct is_str_literal : lak::false_type
		{
		};
		template<auto rule>
		struct is_str_literal<lak::dsl::str_literal_t<rule>> : lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_str_literal_v =
		  lak::dsl::is_str_literal<T>::value;

		/* --- char_literal --- */

		template<char32_t chr>
		struct char_literal_t
		{
			static constexpr bool is_pure_match = true;
			using value_type                    = lak::u8string_view;

			lak::dsl::result<lak::u8string_view> parse(lak::u8string_view str) const
			{
				const uint8_t clen = lak::character_length(str);
				if (clen < 1 || clen > 4) return lak::err_t{lak::dsl::parse_error{}};
				const char32_t c = lak::codepoint(str);
				if (c != chr) return lak::err_t{lak::dsl::parse_error{}};
				return lak::ok_t{lak::dsl::parse_result<value_type>{
				  .consumed  = str.first(clen),
				  .remaining = str.substr(clen),
				  .value     = str.first(clen),
				}};
			}
		};

		template<char32_t chr>
		inline constexpr lak::dsl::char_literal_t<chr> char_literal;

		static_assert(lak::dsl::parser<lak::dsl::char_literal_t<U'a'>>);

		template<char32_t... chars>
		using one_of_chars_t =
		  lak::dsl::disjunction_t<lak::dsl::char_literal<chars>...>;
		template<char32_t... chars>
		inline constexpr lak::dsl::one_of_chars_t<chars...> one_of_chars;
		template<lak::u32const_string chars>
		inline constexpr lak::u32apply_const_string_t<lak::dsl::one_of_chars_t,
		                                              chars>
		  one_of_chars_str;

		/* --- is_char_literal --- */

		template<typename T>
		struct is_char_literal : lak::false_type
		{
		};
		template<auto rule>
		struct is_char_literal<lak::dsl::char_literal_t<rule>> : lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_char_literal_v =
		  lak::dsl::is_char_literal<T>::value;

		/* --- char_range --- */

		template<char32_t begin, char32_t end>
		struct char_range_t
		{
			static constexpr bool is_pure_match = true;
			using value_type                    = lak::u8string_view;

			lak::dsl::result<lak::u8string_view> parse(lak::u8string_view str) const
			{
				const uint8_t clen = lak::character_length(str);
				if (clen < 1 || clen > 4) return lak::err_t{lak::dsl::parse_error{}};
				const char32_t c = lak::codepoint(str);
				if (c < begin || c > end) return lak::err_t{lak::dsl::parse_error{}};
				return lak::ok_t{lak::dsl::parse_result<value_type>{
				  .consumed  = str.first(clen),
				  .remaining = str.substr(clen),
				  .value     = str.first(clen),
				}};
			}
		};

		template<char32_t begin, char32_t end>
		inline constexpr lak::dsl::char_range_t<begin, end> char_range;

		static_assert(lak::dsl::parser<lak::dsl::char_range_t<U'a', U'b'>>);

		/* --- is_char_range --- */

		template<typename T>
		struct is_char_range : lak::false_type
		{
		};
		template<char32_t begin, char32_t end>
		struct is_char_range<lak::dsl::char_range_t<begin, end>> : lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_char_range_v = lak::dsl::is_char_range<T>::value;

		/* --- replace --- */

		template<lak::dsl::parser auto par, auto value>
		struct replace_t
		{
			static constexpr bool is_pure_match = false;
			using value_type                    = decltype(value);

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			{
				return par.parse(str).map(
				  []<typename T>(const lak::dsl::parse_result<T> &res)
				  {
					  return lak::dsl::parse_result<value_type>{
					    .consumed  = res.consumed,
					    .remaining = res.remaining,
					    .value     = value,
					  };
				  });
			}
		};

		template<lak::dsl::parser auto par, auto value>
		inline constexpr lak::dsl::replace_t<par, value> replace;

		template<lak::u8const_string const_str, auto value>
		inline constexpr lak::dsl::replace_t<lak::dsl::str_literal<const_str>,
		                                     value>
		  replace_str_literal;

		static_assert(
		  lak::dsl::parser<lak::dsl::replace_t<lak::dsl::str_literal<u8"a">, 0>>);

		template<char32_t chr, auto value>
		inline constexpr lak::dsl::replace_t<lak::dsl::char_literal<chr>, value>
		  replace_char_literal;

		static_assert(
		  lak::dsl::parser<lak::dsl::replace_t<lak::dsl::char_literal<U'a'>, 0>>);
	}

	/* --- is_replace --- */

	// :TODO: ubuntu g++11
	// error: class template argument deduction failed:
	// struct is_replace<lak::dsl::replace_t<rule, value>> : lak::true_type
	//                                             ^~~~~

	// template<typename T>
	// struct is_replace : lak::false_type
	// {
	// };
	// template<auto rule, auto value>
	// struct is_replace<lak::dsl::replace_t<rule, value>> : lak::true_type
	// {
	// };
	// template<typename T>
	// inline constexpr bool is_replace_v = lak::dsl::is_replace<T>::value;

}

#endif
