#ifndef LAK_DSL_DSL_HPP
#define LAK_DSL_DSL_HPP

#include "lak/dsl/concepts.hpp"
#include "lak/dsl/result.hpp"

#include "lak/array.hpp"
#include "lak/char_utils.hpp"
#include "lak/compare.hpp"
#include "lak/const_string.hpp"
#include "lak/result.hpp"
#include "lak/unicode.hpp"
#include "lak/utility.hpp"

#include <ostream>

namespace lak
{
	namespace dsl
	{
		/* --- bottom --- */

		struct bottom_t
		{
			static constexpr bool is_pure_match = true;
			using value_type                    = lak::u8string_view;
			lak::dsl::result<value_type> parse(lak::u8string_view) const
			{
				return lak::err_t{lak::dsl::err::parse{.message = u8"bottom"}};
			}
		};

		inline constexpr lak::dsl::bottom_t bottom;

		/* --- dummy_impure --- */

		template<typename T>
		struct dummy_impure_t
		{
			static constexpr bool is_pure_match = false;
			using value_type                    = T;
			lak::dsl::result<value_type> parse(lak::u8string_view) const
			{
				return lak::err_t{lak::dsl::err::parse{.message = u8"dummy"}};
			}
		};

		template<typename T>
		inline constexpr lak::dsl::dummy_impure_t<T> dummy_impure;

		static_assert(!decltype(lak::dsl::dummy_impure<int>)::is_pure_match);
		static_assert(lak::is_same_v<
		              int,
		              typename decltype(lak::dsl::dummy_impure<int>)::value_type>);

		/* --- top --- */

		struct top_t
		{
			static constexpr bool is_pure_match = true;
			using value_type                    = lak::u8string_view;
			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			{
				return lak::ok_t{lak::dsl::parse_result<value_type>{
				  .consumed  = str.first(0),
				  .remaining = str,
				  .value     = {},
				}};
			}
		};

		inline constexpr lak::dsl::top_t top;

		/* --- eof --- */

		struct eof_t
		{
			static constexpr bool is_pure_match = true;
			using value_type                    = lak::u8string_view;
			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			{
				if (str.empty())
					return lak::ok_t{lak::dsl::parse_result<value_type>{
					  .consumed  = str.first(0),
					  .remaining = str,
					  .value     = str.first(0),
					}};
				else
					return lak::err_t{
					  lak::dsl::err::parse{.message = u8"expected end of file"}};
			}
		};

		inline constexpr lak::dsl::eof_t eof;

		static_assert(lak::dsl::concepts::parser<lak::dsl::eof_t>);

		/* --- is_eof --- */

		template<typename T>
		struct is_eof : lak::false_type
		{
		};
		template<>
		struct is_eof<lak::dsl::eof_t> : lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_eof_v = lak::dsl::is_eof<T>::value;

		static_assert(lak::dsl::is_eof_v<lak::dsl::eof_t>);

		/* --- sequence --- */

		template<lak::dsl::concepts::parser auto... parsers>
		struct sequence_t
		{
			static constexpr bool is_pure_match =
			  ((lak::dsl::concepts::pure_match_parser<
			     lak::remove_cvref_t<decltype(parsers)>>) &&
			   ...);

			using value_type = lak::conditional_t<
			  is_pure_match,
			  lak::u8string_view,
			  lak::tuple<typename decltype(parsers)::value_type...>>;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			{
				return parse(str, lak::index_sequence_for<decltype(parsers)...>{});
			}

			template<size_t... I>
			requires(!is_pure_match)
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
				    .if_err([&](const lak::dsl::err::parse &err)
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
			requires(is_pure_match)
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
				    .if_err([&](const lak::dsl::err::parse &err)
				            { result = lak::err_t{err}; })
				    .if_ok([&]<typename T>(lak::dsl::parse_result<T> &&res)
				           { result.unsafe_unwrap().remaining = res.remaining; })
				    .is_ok()) &&
				 ...);

				if_let_ok (auto &res, result)
					res.value = res.consumed =
					  str.first(str.size() - res.remaining.size());

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

		template<lak::dsl::concepts::parser auto... parsers>
		inline constexpr lak::dsl::sequence_t<parsers...> sequence;

		static_assert(lak::dsl::concepts::parser<lak::dsl::sequence_t<>>);
		static_assert(
		  lak::dsl::concepts::parser<lak::dsl::sequence_t<lak::dsl::sequence<>>>);

		/* --- is_sequence --- */

		template<typename T>
		struct is_sequence : lak::false_type
		{
		};
		template<lak::dsl::concepts::parser auto... parsers>
		struct is_sequence<lak::dsl::sequence_t<parsers...>> : lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_sequence_v = lak::dsl::is_sequence<T>::value;

		static_assert(lak::dsl::is_sequence_v<lak::dsl::sequence_t<>>);
		static_assert(
		  lak::dsl::is_sequence_v<lak::dsl::sequence_t<lak::dsl::sequence<>>>);

		/* --- operator+ --- */

		template<lak::dsl::concepts::parser L, lak::dsl::concepts::parser R>
		requires(!lak::dsl::is_sequence_v<L> && !lak::dsl::is_sequence_v<R>)
		inline constexpr auto operator+(L, R)
		{
			return lak::dsl::sequence<L{}, R{}>;
		}

		template<lak::dsl::concepts::parser auto... L,
		         lak::dsl::concepts::parser R>
		requires(!lak::dsl::is_sequence_v<R>)
		inline constexpr auto operator+(lak::dsl::sequence_t<L...>, R)
		{
			return lak::dsl::sequence<L..., R{}>;
		}

		template<lak::dsl::concepts::parser L,
		         lak::dsl::concepts::parser auto... R>
		requires(!lak::dsl::is_sequence_v<L>)
		inline constexpr auto operator+(L, lak::dsl::sequence_t<R...>)
		{
			return lak::dsl::sequence<L{}, R...>;
		}

		template<lak::dsl::concepts::parser auto... L,
		         lak::dsl::concepts::parser auto... R>
		inline constexpr auto operator+(lak::dsl::sequence_t<L...>,
		                                lak::dsl::sequence_t<R...>)
		{
			return lak::dsl::sequence<L..., R...>;
		}

		/* --- optional --- */

		template<lak::dsl::concepts::parser auto par>
		struct optional_t
		{
			static constexpr bool is_pure_match = decltype(par)::is_pure_match;

			using _par_value_type = typename decltype(par)::value_type;

			using value_type = lak::conditional_t<is_pure_match,
			                                      lak::u8string_view,
			                                      lak::optional<_par_value_type>>;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			requires(!is_pure_match)
			{
				return lak::ok_t{par.parse(str)
				                   .map(
				                     [](lak::dsl::parse_result<_par_value_type> &&res)
				                       -> lak::dsl::parse_result<value_type>
				                     {
					                     return {
					                       .consumed  = res.consumed,
					                       .remaining = res.remaining,
					                       .value     = value_type(
                                   lak::forward<_par_value_type>(res.value)),
					                     };
				                     })
				                   .unwrap_or(lak::dsl::parse_result<value_type>{
				                     .consumed  = str.first(0),
				                     .remaining = str,
				                     .value     = {},
				                   })};
			};

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			requires(is_pure_match)
			{
				return lak::ok_t{
				  par.parse(str).unwrap_or(lak::dsl::parse_result<value_type>{
				    .consumed  = str.first(0),
				    .remaining = str,
				    .value     = {},
				  })};
			}
		};

		template<lak::dsl::concepts::parser auto par>
		inline constexpr lak::dsl::optional_t<par> optional;

		static_assert(
		  lak::dsl::concepts::parser<lak::dsl::optional_t<lak::dsl::sequence<>>>);

		/* --- is_optional --- */

		template<typename T>
		struct is_optional : lak::false_type
		{
		};
		template<lak::dsl::concepts::parser auto parser>
		struct is_optional<lak::dsl::optional_t<parser>> : lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_optional_v = lak::dsl::is_optional<T>::value;

		/* --- get_optional_parser --- */

		template<lak::dsl::concepts::parser par>
		struct get_optional_parser_t;

		template<lak::dsl::concepts::parser auto par>
		struct get_optional_parser_t<lak::dsl::optional_t<par>>
		{
			static constexpr auto parser = par;
		};

		/* --- remove_optional --- */

		template<lak::dsl::concepts::parser auto par>
		struct remove_optional_t;

		template<lak::dsl::concepts::parser auto par>
		requires(!lak::dsl::is_optional_v<decltype(par)>)
		struct remove_optional_t<par>
		{
			static constexpr auto parser = par;
		};

		template<lak::dsl::concepts::parser auto par>
		requires(lak::dsl::is_optional_v<decltype(par)>)
		struct remove_optional_t<par>
		{
			static constexpr auto parser =
			  lak::dsl::get_optional_parser_t<decltype(par)>::parser;
		};

		template<lak::dsl::concepts::parser auto par>
		inline constexpr auto remove_optional =
		  lak::dsl::remove_optional_t<par>::parser;

		/* --- operator~ --- */

		template<lak::dsl::concepts::parser R>
		requires(!lak::dsl::is_optional_v<R>)
		inline constexpr auto operator~(R)
		{
			return lak::dsl::optional<R{}>;
		}

		template<lak::dsl::concepts::parser auto R>
		inline constexpr auto operator~(lak::dsl::optional_t<R>)
		{
			return lak::dsl::optional<R>;
		}

		/* --- repeat --- */

		template<lak::dsl::concepts::parser auto par,
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
			requires(!is_pure_match)
			{
				lak::dsl::parse_result<value_type> result =
				  lak::dsl::parse_result<value_type>{
				    .consumed  = {},
				    .remaining = str,
				    .value     = {},
				  };

				lak::u8string err_msg;
				size_t count = 0;
				while (count < max &&
				       par.parse(result.remaining)
				         .if_ok(
				           [&]<typename T>(lak::dsl::parse_result<T> &&res)
				           {
					           result.remaining = res.remaining;
					           result.value.push_back(lak::forward<T>(res.value));
				           })
				         .if_err([&](const lak::dsl::err::parse &err)
				                 { err_msg = err.message; })
				         .is_ok())
					++count;

				if (count < min)
					return lak::err_t{
					  lak::dsl::err::parse{.message = lak::move(err_msg)}};

				result.consumed = str.first(str.size() - result.remaining.size());

				return lak::ok_t{result};
			}

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			requires(is_pure_match)
			{
				lak::dsl::parse_result<value_type> result =
				  lak::dsl::parse_result<value_type>{
				    .consumed  = {},
				    .remaining = str,
				    .value     = {},
				  };

				lak::u8string err_msg;
				size_t count = 0;
				while (count < max &&
				       par.parse(result.remaining)
				         .if_ok([&]<typename T>(lak::dsl::parse_result<T> &&res)
				                { result.remaining = res.remaining; })
				         .if_err([&](const lak::dsl::err::parse &err)
				                 { err_msg = err.message; })
				         .is_ok())
					++count;

				if (count < min)
					return lak::err_t{
					  lak::dsl::err::parse{.message = lak::move(err_msg)}};

				result.value = result.consumed =
				  str.first(str.size() - result.remaining.size());

				return lak::ok_t{result};
			}
		};

		template<lak::dsl::concepts::parser auto par>
		inline constexpr lak::dsl::repeat_t<par> repeat;

		template<lak::dsl::concepts::parser auto par, size_t count>
		inline constexpr lak::dsl::repeat_t<par, count> repeat_at_least;

		template<lak::dsl::concepts::parser auto par, size_t count>
		inline constexpr lak::dsl::repeat_t<par, count, count> repeat_exact;

		template<lak::dsl::concepts::parser auto par, size_t min, size_t max>
		inline constexpr lak::dsl::repeat_t<par, min, max> repeat_range;

		static_assert(
		  lak::dsl::concepts::parser<lak::dsl::repeat_t<lak::dsl::sequence<>>>);
		static_assert(
		  lak::dsl::concepts::parser<lak::dsl::repeat_t<lak::dsl::sequence<>, 1>>);

		/* --- is_repeat --- */

		template<typename T>
		struct is_repeat : lak::false_type
		{
		};
		template<lak::dsl::concepts::parser auto parser, size_t count>
		struct is_repeat<lak::dsl::repeat_t<parser, count>> : lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_repeat_v = lak::dsl::is_repeat<T>::value;

		/* --- operator* --- */

		template<lak::dsl::concepts::parser R>
		requires(!lak::dsl::is_repeat_v<R>)
		inline constexpr auto operator*(R)
		{
			return lak::dsl::repeat<R{}>;
		}

		template<lak::dsl::concepts::parser auto R, size_t S>
		inline constexpr auto operator*(lak::dsl::repeat_t<R, S>)
		{
			return lak::dsl::repeat<R>;
		}

		/* --- operator+ --- */

		template<lak::dsl::concepts::parser R>
		requires(!lak::dsl::is_repeat_v<R>)
		inline constexpr auto operator+(R)
		{
			return lak::dsl::repeat_at_least<R{}, 1U>;
		}

		template<lak::dsl::concepts::parser auto R, size_t S>
		inline constexpr auto operator+(lak::dsl::repeat_t<R, S>)
		{
			return lak::dsl::repeat_at_least<R, 1U>;
		}

		/* --- match --- */

		template<lak::dsl::concepts::pure_match_parser auto condition,
		         lak::dsl::concepts::parser auto par>
		struct match_t
		{
			static constexpr bool is_pure_match =
			  lak::dsl::concepts::pure_match_parser<
			    lak::remove_cvref_t<decltype(par)>>;
			using value_type = typename decltype(par)::value_type;

			lak::optional<lak::dsl::result<value_type>> parse(
			  lak::u8string_view str) const
			{
				auto rem = str;

				if (condition.parse(rem)
				      .if_ok([&](auto res) { rem = res.remaining; })
				      .is_err())
					return lak::nullopt;

				return lak::some_t{par.parse(rem)};
			}
		};

		template<lak::dsl::concepts::pure_match_parser auto condition,
		         lak::dsl::concepts::parser auto par>
		inline constexpr lak::dsl::match_t<condition, par> match;

		/* --- is_match --- */

		template<typename T>
		struct is_match : lak::false_type
		{
		};
		template<lak::dsl::concepts::pure_match_parser auto cond,
		         lak::dsl::concepts::parser auto par>
		struct is_match<lak::dsl::match_t<cond, par>> : lak::true_type
		{
		};
		template<typename T>
		inline constexpr auto is_match_v = lak::dsl::is_match<T>::value;

		static_assert(lak::dsl::is_match_v<
		              lak::dsl::match_t<lak::dsl::top, lak::dsl::bottom>>);

		/* --- match_sequence --- */

		template<auto... cases>
		requires(
		  ((lak::dsl::is_match_v<lak::remove_cvref_t<decltype(cases)>>) && ...))
		struct match_sequence_t
		{
			static constexpr bool is_pure_match =
			  ((decltype(cases)::is_pure_match) && ...);
			static constexpr bool _is_same_value_types =
			  lak::are_all_same_v<typename decltype(cases)::value_type...>;
			using value_type = lak::conditional_t<
			  is_pure_match,
			  lak::u8string_view,
			  lak::conditional_t<
			    _is_same_value_types,
			    lak::nth_type_t<0U, typename decltype(cases)::value_type...>,
			    lak::create_from_pack_t<
			      lak::variant,
			      lak::make_unique_pack_t<
			        typename decltype(cases)::value_type...>>>>;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			requires(!_is_same_value_types)
			{
				lak::optional<lak::dsl::result<value_type>> result = lak::nullopt;

				// nullopt: condition failed (keep going)
				// err: condition passed, parser didn't (return err)
				// ok: condition and parser passed (return ok)

				(((result = cases.parse(str).map(
				     []<typename T>(
				       lak::dsl::result<T> &&res) -> lak::dsl::result<value_type>
				     {
					     return lak::move(res).map(
					       [](lak::dsl::parse_result<T> &&res)
					         -> lak::dsl::parse_result<value_type>
					       {
						       return {
						         .consumed  = res.consumed,
						         .remaining = res.remaining,
						         .value     = value_type(lak::forward<T>(res.value)),
						       };
					       });
				     }))
				    .has_value()) ||
				 ...);

				if (!result.has_value())
					return lak::err_t{lak::dsl::err::parse{.message = u8"match failed"}};
				else
					return lak::move(*result);
			}

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			requires(_is_same_value_types)
			{
				lak::optional<lak::dsl::result<value_type>> result = lak::nullopt;

				// nullopt: condition failed (keep going)
				// err: condition passed, parser didn't (return err)
				// ok: condition and parser passed (return ok)

				(((result = cases.parse(str)).has_value()) || ...);

				if (!result.has_value())
					return lak::err_t{lak::dsl::err::parse{.message = u8"match failed"}};
				else
					return lak::move(*result);
			}
		};

		template<auto... cases>
		inline constexpr lak::dsl::match_sequence_t<cases...> match_sequence;

		/* --- is_match_sequence --- */

		template<typename T>
		struct is_match_sequence : lak::false_type
		{
		};
		template<auto... cases>
		struct is_match_sequence<lak::dsl::match_sequence_t<cases...>>
		: lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_match_sequence_v =
		  lak::dsl::is_match_sequence<T>::value;

		/* --- operator| ---- */

		template<typename L, typename R>
		requires(lak::dsl::is_match_v<lak::remove_cvref_t<L>> &&
		         lak::dsl::is_match_v<lak::remove_cvref_t<R>>)
		inline constexpr auto operator|(L, R)
		{
			return lak::dsl::match_sequence<L{}, R{}>;
		}

		template<auto... L, typename R>
		requires(lak::dsl::is_match_v<lak::remove_cvref_t<R>>)
		inline constexpr auto operator|(lak::dsl::match_sequence_t<L...>, R)
		{
			return lak::dsl::match_sequence<L..., R{}>;
		}

		template<typename L, auto... R>
		requires(lak::dsl::is_match_v<lak::remove_cvref_t<L>>)
		inline constexpr auto operator|(L, lak::dsl::match_sequence_t<R...>)
		{
			return lak::dsl::match_sequence<L{}, R...>;
		}

		template<auto... L, auto... R>
		inline constexpr auto operator|(lak::dsl::match_sequence_t<L...>,
		                                lak::dsl::match_sequence_t<R...>)
		{
			return lak::dsl::match_sequence<L..., R...>;
		}

		/* --- disjunction --- */

		template<lak::dsl::concepts::parser auto... parsers>
		struct disjunction_t
		{
			static constexpr bool is_pure_match =
			  ((lak::dsl::concepts::pure_match_parser<
			     lak::remove_cvref_t<decltype(parsers)>>) &&
			   ...);
			static constexpr bool _is_same_value_types =
			  lak::are_all_same_v<typename decltype(parsers)::value_type...>;
			using value_type = lak::conditional_t<
			  is_pure_match,
			  lak::u8string_view,
			  lak::conditional_t<
			    _is_same_value_types,
			    lak::nth_type_t<0U, typename decltype(parsers)::value_type...>,
			    lak::create_from_pack_t<
			      lak::variant,
			      lak::make_unique_pack_t<
			        typename decltype(parsers)::value_type...>>>>;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			requires(!is_pure_match)
			{
				lak::dsl::result<value_type> result =
				  lak::dsl::result<value_type>::make_err({});

				lak::u8string err_msg;
				(((result = parsers.parse(str).map(
				     [&]<typename T>(lak::dsl::parse_result<T> &&res)
				       -> lak::dsl::parse_result<value_type>
				     {
					     return {
					       .consumed  = res.consumed,
					       .remaining = res.remaining,
					       .value     = value_type(lak::forward<T>(res.value)),
					     };
				     }))
				    .if_err(
				      [&](const lak::dsl::err::parse &err)
				      {
					      if (err_msg.empty())
						      err_msg = u8"(" + err.message + u8")";
					      else
						      err_msg += u8" or (" + err.message + u8")";
				      })
				    .is_ok()) ||
				 ...);

				if_let_err (auto &err, result) err.message = lak::move(err_msg);

				return result;
			}

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			requires(is_pure_match)
			{
				lak::dsl::result<value_type> result =
				  lak::dsl::result<value_type>::make_err({});

				lak::u8string err_msg;
				(((result = parsers.parse(str))
				    .if_err(
				      [&](const lak::dsl::err::parse &err)
				      {
					      if (err_msg.empty())
						      err_msg = u8"(" + err.message + u8")";
					      else
						      err_msg += u8" or (" + err.message + u8")";
				      })
				    .is_ok()) ||
				 ...);

				if_let_err (auto &err, result) err.message = lak::move(err_msg);

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

		template<lak::dsl::concepts::parser auto... parsers>
		inline constexpr lak::dsl::disjunction_t<parsers...> disjunction;

		static_assert(lak::dsl::concepts::parser<lak::dsl::disjunction_t<>>);
		static_assert(
		  lak::dsl::concepts::parser<
		    lak::dsl::disjunction_t<lak::dsl::sequence<>, lak::dsl::sequence<>>>);

		/* --- is_disjunction --- */

		template<typename T>
		struct is_disjunction : lak::false_type
		{
		};
		template<lak::dsl::concepts::parser auto... parsers>
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

		template<lak::dsl::concepts::parser L, lak::dsl::concepts::parser R>
		requires(!lak::dsl::is_disjunction_v<L> && !lak::dsl::is_disjunction_v<R>)
		inline constexpr auto operator|(L, R)
		{
			return lak::dsl::disjunction<L{}, R{}>;
		}

		template<lak::dsl::concepts::parser auto... L,
		         lak::dsl::concepts::parser R>
		requires(!lak::dsl::is_disjunction_v<R>)
		inline constexpr auto operator|(lak::dsl::disjunction_t<L...>, R)
		{
			return lak::dsl::disjunction<L..., R{}>;
		}

		template<lak::dsl::concepts::parser L,
		         lak::dsl::concepts::parser auto... R>
		requires(!lak::dsl::is_disjunction_v<L>)
		inline constexpr auto operator|(L, lak::dsl::disjunction_t<R...>)
		{
			return lak::dsl::disjunction<L{}, R...>;
		}

		template<lak::dsl::concepts::parser auto... L,
		         lak::dsl::concepts::parser auto... R>
		inline constexpr auto operator|(lak::dsl::disjunction_t<L...>,
		                                lak::dsl::disjunction_t<R...>)
		{
			return lak::dsl::disjunction<L..., R...>;
		}

		/* --- conjunction --- */

		// first parser determines the maximum parse range, all other parsers must
		// then successfully parse on the same range.
		template<lak::dsl::concepts::pure_match_parser auto parser,
		         lak::dsl::concepts::pure_match_parser auto... parsers>
		struct conjunction_t
		{
			static constexpr bool is_pure_match = true;

			using value_type = lak::u8string_view;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			{
				lak::dsl::result<value_type> result1 = parser.parse(str);
				lak::dsl::result<value_type> result2 =
				  lak::dsl::result<value_type>::make_err({});

				(result1.is_ok() && ... &&
				 ((result2 = parsers.parse(result1.unsafe_unwrap().consumed)
				               .and_then(
				                 [&](const auto &value) -> lak::dsl::result<value_type>
				                 {
					                 if (value.consumed.size() ==
					                     result1.unsafe_unwrap().consumed.size())
						                 return lak::ok_t{value};
					                 else
						                 return lak::err_t<lak::dsl::err::parse>{};
				                 }))
				    .is_ok()));

				using ::operator&;
				return result2 & result1;
			}
		};

		template<lak::dsl::concepts::pure_match_parser auto... parsers>
		inline constexpr lak::dsl::conjunction_t<parsers...> conjunction;

		/* --- is_conjunction --- */

		template<typename T>
		struct is_conjunction : lak::false_type
		{
		};
		template<lak::dsl::concepts::parser auto... parsers>
		struct is_conjunction<lak::dsl::conjunction_t<parsers...>> : lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_conjunction_v =
		  lak::dsl::is_conjunction<T>::value;

		/* --- operator& --- */

		template<lak::dsl::concepts::parser L, lak::dsl::concepts::parser R>
		requires(!lak::dsl::is_conjunction_v<L> && !lak::dsl::is_conjunction_v<R>)
		inline constexpr auto operator&(L, R)
		{
			return lak::dsl::conjunction<L{}, R{}>;
		}

		template<lak::dsl::concepts::parser auto... L,
		         lak::dsl::concepts::parser R>
		requires(!lak::dsl::is_conjunction_v<R>)
		inline constexpr auto operator&(lak::dsl::conjunction_t<L...>, R)
		{
			return lak::dsl::conjunction<L..., R{}>;
		}

		template<lak::dsl::concepts::parser L,
		         lak::dsl::concepts::parser auto... R>
		requires(!lak::dsl::is_conjunction_v<L>)
		inline constexpr auto operator&(L, lak::dsl::conjunction_t<R...>)
		{
			return lak::dsl::conjunction<L{}, R...>;
		}

		template<lak::dsl::concepts::parser auto... L,
		         lak::dsl::concepts::parser auto... R>
		inline constexpr auto operator&(lak::dsl::conjunction_t<L...>,
		                                lak::dsl::conjunction_t<R...>)
		{
			return lak::dsl::conjunction<L..., R...>;
		}

		/* --- capture --- */

		template<lak::dsl::concepts::parser auto par>
		struct capture_t
		{
			static constexpr bool is_pure_match = false;
			using value_type                    = typename decltype(par)::value_type;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			{
				return par.parse(str);
			}
		};

		template<lak::dsl::concepts::parser auto par>
		inline constexpr lak::dsl::capture_t<par> capture;

		static_assert(
		  lak::dsl::concepts::parser<lak::dsl::capture_t<lak::dsl::sequence<>>>);

		/* --- is_capture --- */

		template<typename T>
		struct is_capture : lak::false_type
		{
		};
		template<lak::dsl::concepts::parser auto par>
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
		static_assert(
		  lak::dsl::is_capture_v<lak::remove_cvref_t<
		    decltype(lak::dsl::capture<lak::dsl::dummy_impure<int>>)>>);

		/* --- capture_nth --- */

		template<size_t N,
		         lak::dsl::concepts::parser auto par,
		         lak::dsl::concepts::parser auto... parsers>
		struct capture_nth_t
		{
			static_assert(N <= sizeof...(parsers));

			static constexpr bool is_pure_match =
			  lak::nth_type_t<N, decltype(par), decltype(parsers)...>::is_pure_match;
			using value_type = typename lak::
			  nth_type_t<N, decltype(par), decltype(parsers)...>::value_type;

			static force_inline lak::dsl::result<value_type> impl_parse(
			  lak::u8string_view str)
			requires(N > 0U)
			{
				RES_TRY_ASSIGN(auto res =, par.parse(str));
				return capture_nth_t<N - 1U, parsers...>::impl_parse(res.remaining);
			}

			static force_inline lak::dsl::result<value_type> impl_parse(
			  lak::u8string_view str)
			requires(N == 0U)
			{
				RES_TRY_ASSIGN(auto res =, par.parse(str));
				RES_TRY_ASSIGN(auto rem_res =,
				               lak::dsl::sequence<parsers...>.parse(res.remaining));
				res.remaining = rem_res.remaining;
				return lak::ok_t<lak::dsl::parse_result<value_type>>{res};
			}

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			{
				return impl_parse(str);
			}
		};

		template<size_t N, lak::dsl::concepts::parser auto... parsers>
		inline constexpr lak::dsl::capture_nth_t<N, parsers...> capture_nth;

		template<lak::dsl::concepts::parser auto... parsers>
		inline constexpr lak::dsl::capture_nth_t<0U, parsers...> capture_1st;

		template<lak::dsl::concepts::parser auto... parsers>
		inline constexpr lak::dsl::capture_nth_t<1U, parsers...> capture_2nd;

		static_assert(lak::dsl::concepts::parser<
		              lak::dsl::capture_nth_t<0U, lak::dsl::sequence<>>>);

		/* --- is_capture_nth --- */

		template<typename T>
		struct is_capture_nth : lak::false_type
		{
		};
		template<size_t N, lak::dsl::concepts::parser auto... parsers>
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

		/* --- as_pure --- */

		template<lak::dsl::concepts::parser auto par>
		struct as_pure_t
		{
			static constexpr bool is_pure_match = true;
			using value_type                    = lak::u8string_view;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			{
				return par.parse(str).map(
				  []<typename T>(lak::dsl::parse_result<T> &&res)
				    -> lak::dsl::parse_result<value_type>
				  {
					  return {
					    .consumed  = res.consumed,
					    .remaining = res.remaining,
					    .value     = res.consumed,
					  };
				  });
			}
		};

		template<lak::dsl::concepts::parser auto par>
		inline constexpr lak::dsl::as_pure_t<par> as_pure;

		static_assert(!lak::dsl::concepts::pure_match_parser<
		              lak::dsl::capture_t<lak::dsl::bottom>>);
		static_assert(lak::dsl::concepts::pure_match_parser<
		              lak::dsl::as_pure_t<dsl::capture<lak::dsl::bottom>>>);

		/* --- is_as_pure --- */

		template<typename T>
		struct is_as_pure : lak::false_type
		{
		};
		template<lak::dsl::concepts::parser auto par>
		struct is_as_pure<lak::dsl::as_pure_t<par>> : lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_as_pure_v = lak::dsl::is_as_pure<T>::value;

		static_assert(
		  lak::dsl::is_as_pure_v<lak::dsl::as_pure_t<lak::dsl::bottom>>);

		/* --- nth_parser --- */

		template<size_t N, lak::dsl::concepts::parser auto... parsers>
		struct _nth_parser_t;

		template<lak::dsl::concepts::parser auto par,
		         lak::dsl::concepts::parser auto... parsers>
		struct _nth_parser_t<0U, par, parsers...>
		{
			static constexpr auto parser = par;
		};

		template<size_t N,
		         lak::dsl::concepts::parser auto par,
		         lak::dsl::concepts::parser auto... parsers>
		requires(N > 0U)
		struct _nth_parser_t<N, par, parsers...>
		: public _nth_parser_t<N - 1, parsers...>
		{
		};

		template<size_t N, lak::dsl::concepts::parser auto... parsers>
		inline constexpr auto nth_parser =
		  lak::dsl::_nth_parser_t<N, parsers...>::parser;

		template<size_t N, lak::dsl::concepts::parser auto... parsers>
		using nth_parser_t =
		  lak::remove_cvref_t<decltype(lak::dsl::nth_parser<N, parsers...>)>;

		static_assert(
		  lak::dsl::concepts::parser<lak::dsl::nth_parser_t<0U,
		                                                    lak::dsl::bottom,
		                                                    lak::dsl::bottom,
		                                                    lak::dsl::bottom>>);

		/* --- nth_parsers --- */

		template<typename INDICES, lak::dsl::concepts::parser auto... parsers>
		struct _nth_parsers_t;

		template<size_t N, lak::dsl::concepts::parser auto... parsers>
		struct _nth_parsers_t<lak::index_sequence<N>, parsers...>
		{
			static constexpr auto parser = lak::dsl::nth_parser<N, parsers...>;
		};

		template<size_t N0,
		         size_t... N,
		         lak::dsl::concepts::parser auto... parsers>
		requires(sizeof...(N) > 0U)
		struct _nth_parsers_t<lak::index_sequence<N0, N...>, parsers...>
		{
			static constexpr auto parser =
			  lak::dsl::nth_parser<N0, parsers...> +
			  lak::dsl::_nth_parsers_t<lak::index_sequence<N...>,
			                           parsers...>::parser;
		};

		template<typename INDICES, lak::dsl::concepts::parser auto... parsers>
		inline constexpr auto nth_parsers =
		  lak::dsl::_nth_parsers_t<INDICES, parsers...>::parser;

		template<typename INDICES, lak::dsl::concepts::parser auto... parsers>
		using nth_parsers_t = lak::remove_cvref_t<
		  decltype(lak::dsl::nth_parsers<INDICES, parsers...>)>;

		static_assert(lak::dsl::concepts::parser<
		              lak::dsl::nth_parsers_t<lak::index_sequence<0U>,
		                                      lak::dsl::bottom,
		                                      lak::dsl::bottom,
		                                      lak::dsl::bottom>>);

		static_assert(lak::dsl::concepts::parser<
		              lak::dsl::nth_parsers_t<lak::index_sequence<0U, 1U>,
		                                      lak::dsl::bottom,
		                                      lak::dsl::bottom,
		                                      lak::dsl::bottom>>);

		/* --- capture_nths --- */

		template<typename BEGIN,
		         size_t N,
		         typename END,
		         lak::dsl::concepts::parser auto... parsers>
		struct _capture_nths_impl_t;

		template<size_t... PRE,
		         size_t N,
		         size_t... POST,
		         lak::dsl::concepts::parser auto... parsers>
		requires((sizeof...(PRE) > 0U) && (sizeof...(POST) > 0U))
		struct _capture_nths_impl_t<lak::index_sequence<PRE...>,
		                            N,
		                            lak::index_sequence<POST...>,
		                            parsers...>
		: public lak::dsl::capture_nth_t<
		    1U,
		    lak::dsl::nth_parsers<lak::index_sequence<PRE...>, parsers...>,
		    lak::dsl::nth_parser<N, parsers...>,
		    lak::dsl::nth_parsers<lak::index_sequence<POST...>, parsers...>>
		{
		};

		static_assert(lak::dsl::concepts::parser<
		              lak::dsl::_capture_nths_impl_t<lak::index_sequence<0U>,
		                                             1U,
		                                             lak::index_sequence<2U>,
		                                             lak::dsl::bottom,
		                                             lak::dsl::bottom,
		                                             lak::dsl::bottom>>);

		template<size_t N,
		         size_t... POST,
		         lak::dsl::concepts::parser auto... parsers>
		requires(sizeof...(POST) > 0U)
		struct _capture_nths_impl_t<lak::index_sequence<>,
		                            N,
		                            lak::index_sequence<POST...>,
		                            parsers...>
		: public lak::dsl::capture_nth_t<
		    0U,
		    lak::dsl::nth_parser<N, parsers...>,
		    lak::dsl::nth_parsers<lak::index_sequence<POST...>, parsers...>>
		{
		};

		static_assert(lak::dsl::concepts::parser<
		              lak::dsl::_capture_nths_impl_t<lak::index_sequence<>,
		                                             0U,
		                                             lak::index_sequence<1U>,
		                                             lak::dsl::bottom,
		                                             lak::dsl::bottom>>);

		template<size_t... PRE,
		         size_t N,
		         lak::dsl::concepts::parser auto... parsers>
		requires(sizeof...(PRE) > 0U)
		struct _capture_nths_impl_t<lak::index_sequence<PRE...>,
		                            N,
		                            lak::index_sequence<>,
		                            parsers...>
		: public lak::dsl::capture_nth_t<
		    1U,
		    lak::dsl::nth_parsers<lak::index_sequence<PRE...>, parsers...>,
		    lak::dsl::nth_parser<N, parsers...>>
		{
		};

		static_assert(lak::dsl::concepts::parser<
		              lak::dsl::_capture_nths_impl_t<lak::index_sequence<0U>,
		                                             1U,
		                                             lak::index_sequence<>,
		                                             lak::dsl::bottom,
		                                             lak::dsl::bottom>>);

		template<size_t N, lak::dsl::concepts::parser auto... parsers>
		struct _capture_nths_impl_t<lak::index_sequence<>,
		                            N,
		                            lak::index_sequence<>,
		                            parsers...>
		: public lak::dsl::nth_parser_t<N, parsers...>
		{
		};

		static_assert(lak::dsl::concepts::parser<
		              lak::dsl::_capture_nths_impl_t<lak::index_sequence<>,
		                                             0U,
		                                             lak::index_sequence<>,
		                                             lak::dsl::bottom>>);

		template<size_t BEGIN,
		         size_t N,
		         size_t END,
		         lak::dsl::concepts::parser auto... parsers>
		struct capture_nths_impl_t;

		template<size_t BEGIN,
		         size_t N,
		         size_t END,
		         lak::dsl::concepts::parser auto... parsers>
		requires((BEGIN < N) && (END > N + 1U))
		struct capture_nths_impl_t<BEGIN, N, END, parsers...>
		: public lak::dsl::_capture_nths_impl_t<
		    lak::make_index_sequence_range<BEGIN, N>,
		    N,
		    lak::make_index_sequence_range<N + 1U, END>,
		    parsers...>
		{
		};

		template<size_t BEGIN,
		         size_t N,
		         size_t END,
		         lak::dsl::concepts::parser auto... parsers>
		requires((BEGIN == N) && (END > N + 1U))
		struct capture_nths_impl_t<BEGIN, N, END, parsers...>
		: public lak::dsl::_capture_nths_impl_t<
		    lak::index_sequence<>,
		    N,
		    lak::make_index_sequence_range<N + 1U, END>,
		    parsers...>
		{
		};

		template<size_t BEGIN,
		         size_t N,
		         size_t END,
		         lak::dsl::concepts::parser auto... parsers>
		requires((BEGIN < N) && (END == N + 1U))
		struct capture_nths_impl_t<BEGIN, N, END, parsers...>
		: public lak::dsl::_capture_nths_impl_t<
		    lak::make_index_sequence_range<BEGIN, N>,
		    N,
		    lak::index_sequence<>,
		    parsers...>
		{
		};

		template<size_t BEGIN,
		         size_t N,
		         size_t END,
		         lak::dsl::concepts::parser auto... parsers>
		requires((BEGIN == N) && (END == N + 1U))
		struct capture_nths_impl_t<BEGIN, N, END, parsers...>
		: public lak::dsl::_capture_nths_impl_t<lak::index_sequence<>,
		                                        N,
		                                        lak::index_sequence<>,
		                                        parsers...>
		{
		};

		template<size_t BEGIN,
		         typename INDEX_SET,
		         lak::dsl::concepts::parser auto... parsers>
		struct _capture_nths_t;

		template<size_t BEGIN,
		         size_t N,
		         lak::dsl::concepts::parser auto... parsers>
		struct _capture_nths_t<BEGIN, lak::index_sequence<N>, parsers...>
		{
			static constexpr auto parser = lak::dsl::
			  capture_nths_impl_t<BEGIN, N, sizeof...(parsers), parsers...>{};
		};

		template<size_t BEGIN,
		         size_t N0,
		         size_t N1,
		         size_t... N,
		         lak::dsl::concepts::parser auto... parsers>
		struct _capture_nths_t<BEGIN,
		                       lak::index_sequence<N0, N1, N...>,
		                       parsers...>
		{
			static_assert(lak::dsl::concepts::parser<
			              lak::dsl::capture_nths_impl_t<BEGIN, N0, N1, parsers...>>);
			static_assert(
			  lak::dsl::concepts::parser<lak::remove_cvref_t<
			    decltype(lak::dsl::_capture_nths_t<N1,
			                                       lak::index_sequence<N1, N...>,
			                                       parsers...>::parser)>>);

			static constexpr auto parser =
			  lak::dsl::capture_nths_impl_t<BEGIN, N0, N1, parsers...>{} +
			  lak::dsl::_capture_nths_t<N1,
			                            lak::index_sequence<N1, N...>,
			                            parsers...>::parser;
		};

		template<typename INDICES, lak::dsl::concepts::parser auto... parsers>
		struct capture_nths_t;

		template<size_t... N, lak::dsl::concepts::parser auto... parsers>
		struct capture_nths_t<lak::index_sequence<N...>, parsers...>
		{
			static constexpr auto _parser = lak::dsl::
			  _capture_nths_t<0, lak::index_sequence<N...>, parsers...>::parser;

			static constexpr bool is_pure_match = decltype(_parser)::is_pure_match;

			using value_type = typename decltype(_parser)::value_type;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			{
				return _parser.parse(str);
			}
		};

		template<lak::dsl::concepts::parser auto... parsers>
		struct capture_nths_t<lak::index_sequence<>, parsers...>
		{
			static constexpr bool is_pure_match = true;

			using value_type = lak::u8string_view;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			{
				return lak::dsl::as_pure<lak::dsl::sequence<parsers...>>.parse(str);
			}
		};

		template<typename INDICES, lak::dsl::concepts::parser auto... parsers>
		inline constexpr lak::dsl::capture_nths_t<INDICES, parsers...>
		  capture_nths;

		static_assert(lak::dsl::concepts::parser<
		              lak::dsl::capture_nths_t<lak::index_sequence<0U, 1U>,
		                                       lak::dsl::bottom,
		                                       lak::dsl::bottom>>);

		static_assert(
		  lak::is_same_v<
		    int,
		    typename decltype(lak::dsl::capture_nths<
		                      lak::index_sequence<1U>,
		                      lak::dsl::bottom,
		                      lak::dsl::capture<lak::dsl::dummy_impure<int>>,
		                      lak::dsl::bottom>)::value_type>);

		static_assert(
		  lak::is_same_v<
		    lak::tuple<int, double>,
		    typename decltype(lak::dsl::capture_nths<
		                      lak::index_sequence<1U, 4U>,
		                      lak::dsl::bottom,
		                      lak::dsl::capture<lak::dsl::dummy_impure<int>>,
		                      lak::dsl::bottom,
		                      lak::dsl::top,
		                      lak::dsl::capture<lak::dsl::dummy_impure<double>>,
		                      lak::dsl::bottom>)::value_type>);

		/* --- is_capture_nths --- */

		template<typename T>
		struct is_capture_nths : lak::false_type
		{
		};
		template<size_t... N, lak::dsl::concepts::parser auto... parsers>
		struct is_capture_nths<
		  lak::dsl::capture_nths_t<lak::index_sequence<N...>, parsers...>>
		: lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_capture_nths_v =
		  lak::dsl::is_capture_nths<T>::value;

		static_assert(lak::dsl::is_capture_nths_v<
		              lak::dsl::capture_nths_t<lak::index_sequence<0U>,
		                                       lak::dsl::sequence<>>>);

		/* --- capture_sequence --- */

		// capture_sequence<capture<a>, b, capture<c>>
		// -> sequence<capture_1st<a, b>, capture_1st<c>>

		template<lak::dsl::concepts::parser auto... pars>
		struct capture_sequence_t
		: public lak::dsl::capture_nths_t<
		    lak::indices_of_filter_pack_t<
		      lak::dsl::is_capture,
		      lak::type_pack<lak::remove_cvref_t<decltype(pars)>...>>,
		    pars...>
		{
		};

		template<lak::dsl::concepts::parser auto... pars>
		inline constexpr lak::dsl::capture_sequence_t<pars...> capture_sequence;

		static_assert(
		  lak::dsl::concepts::parser<
		    lak::dsl::capture_sequence_t<lak::dsl::capture<lak::dsl::bottom>>>);

		static_assert(
		  lak::is_same_v<lak::index_sequence<1U>,
		                 lak::indices_of_filter_pack_t<
		                   lak::dsl::is_capture,
		                   lak::type_pack<lak::dsl::bottom_t,
		                                  lak::dsl::capture_t<lak::dsl::bottom>,
		                                  lak::dsl::bottom_t>>>);

		static_assert(
		  lak::is_same_v<
		    int,
		    typename decltype(lak::dsl::capture_sequence<
		                      lak::dsl::bottom,
		                      lak::dsl::capture<lak::dsl::dummy_impure<int>>,
		                      lak::dsl::bottom>)::value_type>);

		static_assert(
		  lak::is_same_v<
		    lak::tuple<int, double>,
		    typename decltype(lak::dsl::capture_sequence<
		                      lak::dsl::bottom,
		                      lak::dsl::capture<lak::dsl::dummy_impure<int>>,
		                      lak::dsl::bottom,
		                      lak::dsl::top,
		                      lak::dsl::capture<lak::dsl::dummy_impure<double>>,
		                      lak::dsl::bottom>)::value_type>);

		/* --- is_capture_sequence --- */

		template<typename T>
		struct is_capture_sequence : lak::false_type
		{
		};
		template<lak::dsl::concepts::parser auto... parsers>
		struct is_capture_sequence<lak::dsl::capture_sequence_t<parsers...>>
		: lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_capture_sequence_v =
		  lak::dsl::is_capture_sequence<T>::value;

		static_assert(lak::dsl::is_capture_sequence_v<
		              lak::dsl::capture_sequence_t<lak::dsl::sequence<>>>);

		/* --- negative_lookahead --- */

		template<lak::dsl::concepts::pure_match_parser auto par>
		struct negative_lookahead_t
		{
			static constexpr bool is_pure_match = true;

			using value_type = lak::u8string_view;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			{
				if (par.parse(str).is_err())
					return lak::ok_t{lak::dsl::parse_result<value_type>{
					  .consumed  = str.first(0),
					  .remaining = str,
					  .value     = {},
					}};
				else
					return lak::err_t<lak::dsl::err::parse>{};
			}
		};

		template<lak::dsl::concepts::pure_match_parser auto par>
		inline constexpr lak::dsl::negative_lookahead_t<par> negative_lookahead;

		static_assert(lak::dsl::concepts::parser<
		              lak::dsl::negative_lookahead_t<lak::dsl::bottom>>);
		static_assert(lak::dsl::concepts::pure_match_parser<
		              lak::dsl::negative_lookahead_t<lak::dsl::bottom>>);

		/* --- is_negative_lookahead --- */

		template<typename T>
		struct is_negative_lookahead : lak::false_type
		{
		};
		template<lak::dsl::concepts::pure_match_parser auto par>
		struct is_negative_lookahead<lak::dsl::negative_lookahead_t<par>>
		: lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_negative_lookahead_v =
		  lak::dsl::is_negative_lookahead<T>::value;

		static_assert(lak::dsl::is_negative_lookahead_v<
		              lak::dsl::negative_lookahead_t<lak::dsl::bottom>>);

		/* --- operator- --- */

		template<lak::dsl::concepts::pure_match_parser par>
		requires(!lak::dsl::is_negative_lookahead_v<par>)
		inline constexpr auto operator-(par)
		{
			return lak::dsl::negative_lookahead<par{}>;
		}

		/* --- positive_lookahead --- */

		template<lak::dsl::concepts::pure_match_parser auto par>
		struct positive_lookahead_t
		{
			static constexpr bool is_pure_match = true;

			using value_type = lak::u8string_view;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			{
				if (par.parse(str).is_ok())
					return lak::ok_t{lak::dsl::parse_result<value_type>{
					  .consumed  = str.first(0),
					  .remaining = str,
					  .value     = {},
					}};
				else
					return lak::err_t<lak::dsl::err::parse>{};
			}
		};

		template<lak::dsl::concepts::pure_match_parser auto par>
		inline constexpr lak::dsl::positive_lookahead_t<par> positive_lookahead;

		static_assert(lak::dsl::concepts::parser<
		              lak::dsl::positive_lookahead_t<lak::dsl::bottom>>);
		static_assert(lak::dsl::concepts::pure_match_parser<
		              lak::dsl::positive_lookahead_t<lak::dsl::bottom>>);

		/* --- is_positive_lookahead --- */

		template<typename T>
		struct is_positive_lookahead : lak::false_type
		{
		};
		template<lak::dsl::concepts::pure_match_parser auto par>
		struct is_positive_lookahead<lak::dsl::positive_lookahead_t<par>>
		: lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_positive_lookahead_v =
		  lak::dsl::is_positive_lookahead<T>::value;

		static_assert(lak::dsl::is_positive_lookahead_v<
		              lak::dsl::positive_lookahead_t<lak::dsl::bottom>>);

		/* --- until --- */

		template<lak::dsl::concepts::pure_match_parser auto parser>
		struct until_t
		{
			static constexpr auto is_pure_match = true;

			using value_type = lak::u8string_view;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			{
				if constexpr (lak::dsl::is_eof_v<
				                lak::remove_cvref_t<decltype(parser)>>)
				{
					if (!str.empty())
						return lak::ok_t<lak::dsl::parse_result<value_type>>{{
						  .consumed  = str,
						  .remaining = str.last(0),
						  .value     = str,
						}};
				}
				else
				{
					for (lak::u8string_view rem = str; !rem.empty();
					     rem                    = rem.substr(1U))
					{
						if (parser.parse(rem).is_ok())
						{
							str = str.first(str.size() - rem.size());
							return lak::ok_t<lak::dsl::parse_result<value_type>>{{
							  .consumed  = str,
							  .remaining = rem,
							  .value     = str,
							}};
						}
					}
				}

				return lak::err_t<lak::dsl::err::parse>{{.message = u8"out of data"}};
			}
		};

		template<lak::dsl::concepts::pure_match_parser auto parser>
		inline constexpr lak::dsl::until_t<parser> until;

		template<lak::dsl::concepts::pure_match_parser auto parser>
		inline constexpr auto until_inc = lak::dsl::until<parser> + parser;

		static_assert(lak::dsl::concepts::pure_match_parser<
		              lak::dsl::until_t<lak::dsl::bottom>>);

		/* --- is_until --- */

		template<typename T>
		struct is_until : lak::false_type
		{
		};
		template<lak::dsl::concepts::pure_match_parser auto par>
		struct is_until<lak::dsl::until_t<par>> : lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_until_v = lak::dsl::is_until<T>::value;

		static_assert(lak::dsl::is_until_v<lak::dsl::until_t<lak::dsl::bottom>>);

		/* --- conditional --- */

		template<lak::dsl::concepts::pure_match_parser auto condition,
		         lak::dsl::concepts::parser auto true_parser,
		         lak::dsl::concepts::parser auto false_parser>
		struct conditional_t
		{
			static constexpr bool is_pure_match =
			  decltype(true_parser)::is_pure_match &&
			  decltype(false_parser)::is_pure_match;

			static constexpr bool _same_type =
			  lak::is_same_v<typename decltype(true_parser)::value_type,
			                 typename decltype(false_parser)::value_type>;

			using value_type = lak::conditional_t<
			  is_pure_match,
			  lak::u8string_view,
			  lak::conditional_t<
			    _same_type,
			    typename decltype(true_parser)::value_type,
			    lak::variant<typename decltype(true_parser)::value_type,
			                 typename decltype(false_parser)::value_type>>>;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			requires(is_pure_match)
			{
				if_let_ok (auto ok, condition.parse(str))
					return true_parser.parse(ok.remaining);
				else
					return false_parser.parse(str);
			}

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			requires(!is_pure_match && !_same_type)
			{
				if_let_ok (auto ok, condition.parse(str))
					return true_parser.parse(ok.remaining)
					  .map(
					    []<typename T>(lak::dsl::parse_result<T> &&res)
					      -> lak::dsl::parse_result<value_type>
					    {
						    return {
						      .consumed  = res.consumed,
						      .remaining = res.remaining,
						      .value     = lak::var_t<0U>(lak::forward<T>(res.value)),
						    };
					    });
				else
					return false_parser.parse(str).map(
					  []<typename T>(lak::dsl::parse_result<T> &&res)
					    -> lak::dsl::parse_result<value_type>
					  {
						  return {
						    .consumed  = res.consumed,
						    .remaining = res.remaining,
						    .value     = lak::var_t<1U>(lak::forward<T>(res.value)),
						  };
					  });
			}

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			requires(!is_pure_match && _same_type)
			{
				if_let_ok (auto ok, condition.parse(str))
					return true_parser.parse(ok.remaining)
					  .map(
					    []<typename T>(lak::dsl::parse_result<T> &&res)
					      -> lak::dsl::parse_result<value_type>
					    {
						    return {
						      .consumed  = res.consumed,
						      .remaining = res.remaining,
						      .value     = lak::forward<T>(res.value),
						    };
					    });
				else
					return false_parser.parse(str).map(
					  []<typename T>(lak::dsl::parse_result<T> &&res)
					    -> lak::dsl::parse_result<value_type>
					  {
						  return {
						    .consumed  = res.consumed,
						    .remaining = res.remaining,
						    .value     = lak::forward<T>(res.value),
						  };
					  });
			}
		};

		template<lak::dsl::concepts::pure_match_parser auto condition,
		         lak::dsl::concepts::parser auto true_parser,
		         lak::dsl::concepts::parser auto false_parser>
		lak::dsl::conditional_t<condition, true_parser, false_parser> conditional;

		static_assert(
		  lak::dsl::concepts::parser<lak::dsl::conditional_t<lak::dsl::top,
		                                                     lak::dsl::top,
		                                                     lak::dsl::bottom>>);

		/* --- is_conditional --- */

		template<typename T>
		struct is_conditional : lak::false_type
		{
		};
		template<lak::dsl::concepts::pure_match_parser auto condition,
		         lak::dsl::concepts::parser auto true_parser,
		         lak::dsl::concepts::parser auto false_parser>
		struct is_conditional<
		  lak::dsl::conditional_t<condition, true_parser, false_parser>>
		: lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_conditional_v =
		  lak::dsl::is_conditional<T>::value;

		static_assert(
		  lak::dsl::is_conditional_v<lak::dsl::conditional_t<lak::dsl::top,
		                                                     lak::dsl::top,
		                                                     lak::dsl::bottom>>);

		/* --- str_literal --- */

		template<lak::u8const_string const_str>
		struct str_literal_t
		{
			// keep a copy of the string because apparently it's possible for const
			// strings lose their value at runtime?
			static constexpr auto _comp_str = const_str;

			static constexpr bool is_pure_match = true;
			using value_type                    = lak::u8string_view;

			lak::dsl::result<lak::u8string_view> parse(lak::u8string_view str) const
			{
				if (lak::compare(
				      lak::u8string_view(_comp_str.begin(), _comp_str.end()), str) ==
				    _comp_str.size())
					return lak::ok_t{lak::dsl::parse_result<value_type>{
					  .consumed  = str.first(_comp_str.size()),
					  .remaining = str.substr(_comp_str.size()),
					  .value     = str.first(_comp_str.size()),
					}};
				else
					return lak::err_t{lak::dsl::err::parse{
					  .message =
					    lak::streamify("expected '",
					                   lak::u8string(_comp_str),
					                   "' got '",
					                   str.first(std::min(str.size(), _comp_str.size())),
					                   "'")}};
			}
		};

		template<lak::u8const_string const_str>
		inline constexpr lak::dsl::str_literal_t<const_str> str_literal;

		static_assert(lak::dsl::concepts::parser<lak::dsl::str_literal_t<u8"a">>);

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

		/* --- negative_str_literal --- */

		template<lak::u8const_string const_str>
		struct negative_str_literal_t
		{
			// keep a copy of the string because apparently it's possible for const
			// strings lose their value at runtime?
			static constexpr auto _comp_str = const_str;

			static constexpr bool is_pure_match = true;
			using value_type                    = lak::u8string_view;

			lak::dsl::result<lak::u8string_view> parse(lak::u8string_view str) const
			{
				if (auto comp = lak::compare(
				      lak::u8string_view(_comp_str.begin(), _comp_str.end()), str);
				    comp != _comp_str.size())
					return lak::ok_t{lak::dsl::parse_result<value_type>{
					  .consumed  = str.first(comp),
					  .remaining = str.substr(comp),
					  .value     = str.first(comp),
					}};
				else
					return lak::err_t{lak::dsl::err::parse{
					  .message =
					    lak::streamify("expected !'",
					                   lak::u8string(_comp_str),
					                   "' got '",
					                   str.first(std::min(str.size(), _comp_str.size())),
					                   "'")}};
			}
		};

		template<lak::u8const_string const_str>
		inline constexpr lak::dsl::negative_str_literal_t<const_str>
		  negative_str_literal;

		static_assert(
		  lak::dsl::concepts::parser<lak::dsl::negative_str_literal_t<u8"a">>);

		/* --- is_negative_str_literal --- */

		template<typename T>
		struct is_negative_str_literal : lak::false_type
		{
		};
		template<auto rule>
		struct is_negative_str_literal<lak::dsl::negative_str_literal_t<rule>>
		: lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_negative_str_literal_v =
		  lak::dsl::is_negative_str_literal<T>::value;

		/* --- operator! --- */

		template<lak::u8const_string const_str>
		inline constexpr auto operator!(lak::dsl::str_literal_t<const_str>)
		{
			return lak::dsl::negative_str_literal<const_str>;
		}

		template<lak::u8const_string const_str>
		inline constexpr auto operator!(
		  lak::dsl::negative_str_literal_t<const_str>)
		{
			return lak::dsl::str_literal<const_str>;
		}

		/* --- char_literal --- */

		template<char32_t chr>
		struct char_literal_t
		{
			static constexpr bool is_pure_match = true;
			using value_type                    = lak::u8string_view;

			lak::dsl::result<lak::u8string_view> parse(lak::u8string_view str) const
			{
				if (str.empty())
					return lak::err_t{lak::dsl::err::parse{.message = u8"out of data"}};
				const uint8_t clen = lak::character_length(str);
				if (clen < 1 || clen > 4)
					return lak::err_t{lak::dsl::err::parse{
					  .message = u8"invalid unicode character length"}};
				const char32_t c = lak::codepoint(str);
				if (c != chr)
				{
					lak::codepoint_buffer_t<char8_t> buffers[2];
					return lak::err_t{lak::dsl::err::parse{
					  .message = lak::streamify(
					    "expected '",
					    lak::is_ascii_printable(chr)
					      ? lak::u8string_view(lak::from_codepoint(buffers[0], chr))
					      : lak::u8string_view(lak::streamify(chr)),
					    "' got '",
					    lak::is_ascii_printable(c)
					      ? lak::u8string_view(lak::from_codepoint(buffers[1], c))
					      : lak::u8string_view(lak::streamify(c)),
					    "'")}};
				}
				return lak::ok_t{lak::dsl::parse_result<value_type>{
				  .consumed  = str.first(clen),
				  .remaining = str.substr(clen),
				  .value     = str.first(clen),
				}};
			}
		};

		template<char32_t chr>
		inline constexpr lak::dsl::char_literal_t<chr> char_literal;

		static_assert(lak::dsl::concepts::parser<lak::dsl::char_literal_t<U'a'>>);

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

		/* --- negative_char_literal --- */

		template<char32_t chr>
		struct negative_char_literal_t
		{
			static constexpr bool is_pure_match = true;
			using value_type                    = lak::u8string_view;

			lak::dsl::result<lak::u8string_view> parse(lak::u8string_view str) const
			{
				if (str.empty())
					return lak::err_t{lak::dsl::err::parse{.message = u8"out of data"}};
				const uint8_t clen = lak::character_length(str);
				if (clen < 1 || clen > 4)
					return lak::err_t{lak::dsl::err::parse{
					  .message = u8"invalid unicode character length"}};
				const char32_t c = lak::codepoint(str);
				if (c == chr)
					return lak::err_t{lak::dsl::err::parse{
					  .message = lak::streamify("expected !'", chr, "' got '", c, "'")}};
				return lak::ok_t{lak::dsl::parse_result<value_type>{
				  .consumed  = str.first(clen),
				  .remaining = str.substr(clen),
				  .value     = str.first(clen),
				}};
			}
		};

		template<char32_t chr>
		inline constexpr lak::dsl::negative_char_literal_t<chr>
		  negative_char_literal;

		static_assert(
		  lak::dsl::concepts::parser<lak::dsl::negative_char_literal_t<U'a'>>);

		/* --- is_negative_char_literal --- */

		template<typename T>
		struct is_negative_char_literal : lak::false_type
		{
		};
		template<auto rule>
		struct is_negative_char_literal<lak::dsl::negative_char_literal_t<rule>>
		: lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_negative_char_literal_v =
		  lak::dsl::is_negative_char_literal<T>::value;

		/* --- operator! --- */

		template<char32_t chr>
		inline constexpr auto operator!(lak::dsl::char_literal_t<chr>)
		{
			return lak::dsl::negative_char_literal<chr>;
		}

		template<char32_t chr>
		inline constexpr auto operator!(lak::dsl::negative_char_literal_t<chr>)
		{
			return lak::dsl::char_literal<chr>;
		}

		/* --- char_range --- */

		template<char32_t begin, char32_t end>
		struct char_range_t
		{
			static constexpr bool is_pure_match = true;
			using value_type                    = lak::u8string_view;

			lak::dsl::result<lak::u8string_view> parse(lak::u8string_view str) const
			{
				if (str.empty())
					return lak::err_t{lak::dsl::err::parse{.message = u8"out of data"}};
				const uint8_t clen = lak::character_length(str);
				if (clen < 1 || clen > 4)
					return lak::err_t{lak::dsl::err::parse{
					  .message = u8"invalid unicode character length"}};
				const char32_t c = lak::codepoint(str);
				if (c < begin || c > end)
					return lak::err_t{lak::dsl::err::parse{
					  .message = lak::streamify(
					    "expected '", begin, "'<=c<='", end, "' got '", c, "'")}};
				return lak::ok_t{lak::dsl::parse_result<value_type>{
				  .consumed  = str.first(clen),
				  .remaining = str.substr(clen),
				  .value     = str.first(clen),
				}};
			}
		};

		template<char32_t begin, char32_t end>
		inline constexpr lak::dsl::char_range_t<begin, end> char_range;

		static_assert(
		  lak::dsl::concepts::parser<lak::dsl::char_range_t<U'a', U'b'>>);

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

		template<lak::dsl::concepts::parser auto par, auto value>
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

		template<lak::dsl::concepts::parser auto par, auto value>
		inline constexpr lak::dsl::replace_t<par, value> replace;

		template<lak::u8const_string const_str, auto value>
		inline constexpr lak::dsl::replace_t<lak::dsl::str_literal<const_str>,
		                                     value>
		  replace_str_literal;

		static_assert(lak::dsl::concepts::parser<
		              lak::dsl::replace_t<lak::dsl::str_literal<u8"a">, 0>>);

		template<char32_t chr, auto value>
		inline constexpr lak::dsl::replace_t<lak::dsl::char_literal<chr>, value>
		  replace_char_literal;

		static_assert(lak::dsl::concepts::parser<
		              lak::dsl::replace_t<lak::dsl::char_literal<U'a'>, 0>>);

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

		/* --- transform --- */

		template<typename RESULT>
		struct _transform_t
		{
			static constexpr bool _can_flatten = false;
			using value_type                   = RESULT;
		};

		template<typename RESULT>
		requires(
		  lak::is_result_v<RESULT> &&
		  lak::is_same_v<lak::result_err_type_t<RESULT>, lak::dsl::err::parse>)
		struct _transform_t<RESULT>
		{
			static constexpr bool _can_flatten = true;
			using value_type                   = lak::result_ok_type_t<RESULT>;
		};

		template<lak::dsl::concepts::parser auto par, auto func>
		struct transform_t
		{
			static constexpr bool is_pure_match = false;
			using _par_value_type               = typename decltype(par)::value_type;
			using _func_result =
			  lak::invoke_result_t<decltype(func), const _par_value_type &>;
			static constexpr bool _can_flatten =
			  _transform_t<_func_result>::_can_flatten;
			using value_type = typename _transform_t<_func_result>::value_type;
			static_assert(!lak::is_void_v<value_type>);

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			requires(_can_flatten)
			{
				return par.parse(str).and_then(
				  [](lak::dsl::parse_result<_par_value_type> res)
				    -> lak::dsl::result<value_type>
				  {
					  return func(lak::forward<_par_value_type>(res.value))
					    .map(
					      [&]<typename U>(U &&v)
					      {
						      return lak::dsl::parse_result<value_type>{
						        .consumed  = res.consumed,
						        .remaining = res.remaining,
						        .value     = lak::forward<U>(v),
						      };
					      });
				  });
			}

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			requires(!_can_flatten)
			{
				return par.parse(str).map(
				  []<typename T>(const lak::dsl::parse_result<T> &res)
				  {
					  return lak::dsl::parse_result<value_type>{
					    .consumed  = res.consumed,
					    .remaining = res.remaining,
					    .value     = func(res.value),
					  };
				  });
			}
		};

		template<lak::dsl::concepts::parser auto par, auto func>
		inline constexpr lak::dsl::transform_t<par, func> transform;

		/* --- is_transform --- */

		template<typename T>
		struct is_transform : lak::false_type
		{
		};
		template<auto rule, auto func>
		struct is_transform<lak::dsl::transform_t<rule, func>> : lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_transform_v = lak::dsl::is_transform<T>::value;

		/* --- unordered --- */

		template<lak::dsl::concepts::parser auto... parsers>
		struct unordered_t
		{
			static constexpr bool is_pure_match =
			  ((lak::dsl::concepts::pure_match_parser<
			     lak::remove_cvref_t<decltype(parsers)>>) &&
			   ...);

			using value_type = lak::conditional_t<
			  is_pure_match,
			  lak::u8string_view,
			  lak::tuple<typename decltype(parsers)::value_type...>>;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const
			{
				return parse(str, lak::index_sequence_for<decltype(parsers)...>{});
			}

			template<size_t... I>
			requires(!is_pure_match)
			lak::dsl::result<value_type> parse(lak::u8string_view str,
			                                   lak::index_sequence<I...>) const
			{
				lak::dsl::parse_result<lak::u8string_view> result{
				  .consumed  = {},
				  .remaining = str,
				  .value     = {},
				};

				lak::tuple<lak::optional<typename decltype(lak::dsl::remove_optional<
				                                           parsers>)::value_type>...>
				  values;

				lak::u8string err_msg;
				while (
				  ((values.template get<I>().has_value()
				      ? false
				      : (lak::dsl::remove_optional<parsers>.parse(result.remaining)
				           .if_ok(
				             [&]<typename T>(lak::dsl::parse_result<T> &&res)
				             {
					             result.remaining         = res.remaining;
					             values.template get<I>() = lak::forward<T>(res.value);
				             })
				           .if_err(
				             [&](const lak::dsl::err::parse &err)
				             {
					             if (err_msg.empty())
						             err_msg = err.message;
					             else
						             err_msg += u8" or " + err.message;
				             })
				           .is_ok())) ||
				   ...))
					err_msg.clear();

				if (!((lak::dsl::is_optional_v<decltype(parsers)> ||
				       values.template get<I>().has_value()) &&
				      ...))
					return lak::err_t{
					  lak::dsl::err::parse{.message = lak::move(err_msg)}};

				result.consumed = str.first(str.size() - result.remaining.size());

				auto forwarder =
				  []<typename T, size_t J, bool B>(
				    lak::optional<T> &val, lak::size_type<J>, lak::bool_type<B>)
				  -> lak::conditional_t<B,
				                        lak::optional<T> &&,
				                        lak::tuple_element_t<J, value_type> &&>
				{
					if constexpr (B)
						return lak::forward<lak::optional<T>>(val);
					else
						return lak::forward<lak::tuple_element_t<J, value_type>>(*val);
				};

				return lak::ok_t{lak::dsl::parse_result<value_type>{
				  .consumed  = result.consumed,
				  .remaining = result.remaining,
				  .value     = value_type(forwarder(
            values.template get<I>(),
            lak::size_type<I>{},
            lak::bool_type<lak::dsl::is_optional_v<decltype(parsers)>>{})...),
				}};
			}

			template<size_t... I>
			requires(is_pure_match)
			lak::dsl::result<value_type> parse(lak::u8string_view str,
			                                   lak::index_sequence<I...>) const
			{
				lak::array<bool, sizeof...(I)> succeeded;

				lak::dsl::parse_result<value_type> result{
				  .consumed  = {},
				  .remaining = str,
				  .value     = {},
				};

				lak::u8string err_msg;
				while (
				  ((succeeded[I]
				      ? false
				      : (succeeded[I] =
				           parsers.parse(result.remaining)
				             .if_ok([&]<typename T>(lak::dsl::parse_result<T> &&res)
				                    { result.remaining = res.remaining; })
				             .if_err(
				               [&](const lak::dsl::err::parse &err)
				               {
					               if (err_msg.empty())
						               err_msg = err.message;
					               else
						               err_msg += u8" or " + err.message;
				               })
				             .is_ok())) ||
				   ...))
					err_msg.clear();

				if (!((succeeded[I]) && ...))
					return lak::err_t{
					  lak::dsl::err::parse{.message = lak::move(err_msg)}};

				result.value = result.consumed =
				  str.first(str.size() - result.remaining.size());

				return lak::move_ok(result);
			}
		};

		template<>
		struct unordered_t<>
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

		template<lak::dsl::concepts::parser auto... parsers>
		inline constexpr lak::dsl::unordered_t<parsers...> unordered;

		static_assert(lak::dsl::concepts::parser<lak::dsl::unordered_t<>>);
		static_assert(lak::dsl::concepts::parser<
		              lak::dsl::unordered_t<lak::dsl::unordered<>>>);

		/* --- is_unordered --- */

		template<typename T>
		struct is_unordered : lak::false_type
		{
		};
		template<lak::dsl::concepts::parser auto... parsers>
		struct is_unordered<lak::dsl::unordered_t<parsers...>> : lak::true_type
		{
		};
		template<typename T>
		inline constexpr bool is_unordered_v = lak::dsl::is_unordered<T>::value;

		static_assert(lak::dsl::is_unordered_v<lak::dsl::unordered_t<>>);
		static_assert(
		  lak::dsl::is_unordered_v<lak::dsl::unordered_t<lak::dsl::unordered<>>>);
	}
}

template<lak::u32const_string const_str>
inline consteval auto operator""_dsl_char()
{
	static_assert(const_str.size() == 1U);
	return lak::dsl::char_literal<const_str[0]>;
}
static_assert(lak::dsl::is_char_literal_v<decltype(U"0"_dsl_char)>);

template<lak::u8const_string const_str>
inline consteval auto operator""_dsl_str()
{
	return lak::dsl::str_literal<const_str>;
}
static_assert(lak::dsl::is_str_literal_v<decltype(u8"hello"_dsl_str)>);

#endif
