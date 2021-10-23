#ifndef LAK_DSL_DSL_HPP
#define LAK_DSL_DSL_HPP

#include "lak/array.hpp"
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
		concept parser = requires(const T t)
		{
			typename T::value_type;

			{
				t.parse(lak::u8string_view{})
				} -> lak::concepts::same_as<lak::dsl::result<typename T::value_type>>;
		};

		/* --- sequence --- */

		template<lak::dsl::parser auto... parsers>
		struct sequence_t
		{
			using value_type = lak::tuple<typename decltype(parsers)::value_type...>;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			{
				return parse(str, lak::index_sequence_for<decltype(parsers)...>{});
			}

			template<size_t... I>
			lak::dsl::result<value_type> parse(lak::u8string_view str,
			                                   lak::index_sequence<I...>) const
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
		};

		template<>
		struct sequence_t<>
		{
			using value_type = lak::tuple<>;
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
		requires(!lak::dsl::is_sequence_v<L> && !lak::dsl::is_sequence_v<R>) //
		  inline constexpr auto
		  operator+(L, R)
		{
			return lak::dsl::sequence<L{}, R{}>;
		}

		template<lak::dsl::parser auto... L, lak::dsl::parser R>
		requires(!lak::dsl::is_sequence_v<R>) //
		  inline constexpr auto
		  operator+(lak::dsl::sequence_t<L...>, R)
		{
			return lak::dsl::sequence<L..., R{}>;
		}

		template<lak::dsl::parser L, lak::dsl::parser auto... R>
		requires(!lak::dsl::is_sequence_v<L>) //
		  inline constexpr auto
		  operator+(L, lak::dsl::sequence_t<R...>)
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

		template<lak::dsl::parser auto parser,
		         size_t min = 0,
		         size_t max = lak::dynamic_extent>
		struct repeat_t
		{
			using value_type = lak::array<typename decltype(parser)::value_type>;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			{
				lak::dsl::parse_result<value_type> result =
				  lak::dsl::parse_result<value_type>{
				    .consumed  = {},
				    .remaining = str,
				    .value     = {},
				  };

				size_t count = 0;
				while (count < max &&
				       parser.parse(result.remaining)
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
		};

		template<lak::dsl::parser auto parser>
		inline constexpr lak::dsl::repeat_t<parser> repeat;

		template<lak::dsl::parser auto parser, size_t count>
		inline constexpr lak::dsl::repeat_t<parser, count> repeat_at_least;

		template<lak::dsl::parser auto parser, size_t count>
		inline constexpr lak::dsl::repeat_t<parser, count, count> repeat_exact;

		template<lak::dsl::parser auto parser, size_t min, size_t max>
		inline constexpr lak::dsl::repeat_t<parser, min, max> repeat_range;

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
		requires(!lak::dsl::is_repeat_v<R>) //
		  inline constexpr auto
		  operator*(R)
		{
			return lak::dsl::repeat<R{}>;
		}

		template<lak::dsl::parser auto R, size_t S>
		inline constexpr auto operator*(lak::dsl::repeat_t<R, S>)
		{
			return lak::dsl::repeat<R>;
		}

		/* --- disjunction --- */

		template<lak::dsl::parser auto... parsers>
		struct disjunction_t
		{
			using value_type = lak::create_from_pack_t<
			  lak::variant,
			  lak::make_unique_pack_t<typename decltype(parsers)::value_type...>>;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
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
		};

		template<>
		struct disjunction_t<>
		{
			using value_type = lak::variant<lak::monostate>;

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
		requires(!lak::dsl::is_disjunction_v<L> &&
		         !lak::dsl::is_disjunction_v<R>) //
		  inline constexpr auto
		  operator|(L, R)
		{
			return lak::dsl::disjunction<L{}, R{}>;
		}

		template<lak::dsl::parser auto... L, lak::dsl::parser R>
		requires(!lak::dsl::is_disjunction_v<R>) //
		  inline constexpr auto
		  operator|(lak::dsl::disjunction_t<L...>, R)
		{
			return lak::dsl::disjunction<L..., R{}>;
		}

		template<lak::dsl::parser L, lak::dsl::parser auto... R>
		requires(!lak::dsl::is_disjunction_v<L>) //
		  inline constexpr auto
		  operator|(L, lak::dsl::disjunction_t<R...>)
		{
			return lak::dsl::disjunction<L{}, R...>;
		}

		template<lak::dsl::parser auto... L, lak::dsl::parser auto... R>
		inline constexpr auto operator|(lak::dsl::disjunction_t<L...>,
		                                lak::dsl::disjunction_t<R...>)
		{
			return lak::dsl::disjunction<L..., R...>;
		}

		/* --- literal --- */

		template<lak::u8const_string const_str, auto value>
		struct literal_t
		{
			using value_type = decltype(value);

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			{
				if (lak::compare(
				      lak::u8string_view(const_str.begin(), const_str.end()), str) ==
				    const_str.size())
					return lak::ok_t{lak::dsl::parse_result<value_type>{
					  .consumed  = str.first(const_str.size()),
					  .remaining = str.substr(const_str.size()),
					  .value     = value,
					}};
				else
					return lak::err_t{lak::dsl::parse_error{}};
			}
		};

		template<lak::u8const_string const_str, auto value>
		inline constexpr lak::dsl::literal_t<const_str, value> literal;

		static_assert(lak::dsl::parser<lak::dsl::literal_t<u8"a", 0>>);

		/* --- is_literal --- */

		template<typename T>
		struct is_literal : lak::false_type
		{
		};
		template<auto rule, auto value>
		struct is_literal<lak::dsl::literal_t<rule, value>> : lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_literal_v = lak::dsl::is_literal<T>::value;
	}
}

#endif
