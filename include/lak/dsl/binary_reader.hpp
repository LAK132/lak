#ifndef LAK_DSL_BINARY_READER_HPP
#define LAK_DSL_BINARY_READER_HPP

#include "lak/dsl/dsl.hpp"
#include "lak/dsl/utility.hpp"

#include "lak/binary_reader.hpp"
#include "lak/result.hpp"
#include "lak/string_view.hpp"
#include "lak/variant.hpp"

namespace lak
{
	namespace dsl
	{
		struct binary_reader
		{
			::lak::binary_reader &strm;

#define LAK_BINARY_READER_PEEK_MAP                                            \
	map([&]<typename T>(lak::dsl::parse_result<T> &&result) -> T                \
	    { return lak::move(result.value); })

#define LAK_BINARY_READER_READ_MAP                                            \
	map(                                                                        \
	  [&]<typename T>(lak::dsl::parse_result<T> &&result) -> T                  \
	  {                                                                         \
			strm.skip(strm.remaining().size() - result.remaining.size()).UNWRAP();  \
			return lak::move(result.value);                                         \
	  })

			auto _impl_peek(lak::dsl::concepts::parser auto par)
			{
				return par.parse(
				  lak::u8string_view(lak::span<const char8_t>(strm.remaining())));
			}

			auto peek(lak::dsl::concepts::parser auto par)
			{
				return _impl_peek(par).LAK_BINARY_READER_PEEK_MAP;
			}

			auto read(lak::dsl::concepts::parser auto par)
			{
				return _impl_peek(par).LAK_BINARY_READER_READ_MAP;
			}

			template<typename T>
			using _impl_number_parse_result =
			  lak::result<lak::dsl::parse_result<T>,
			              lak::variant<lak::err::string_to_numeric,
			                           lak::err::value_out_of_range,
			                           lak::dsl::err::parse>>;

			template<typename T>
			using number_parse_result =
			  lak::result<T,
			              lak::variant<lak::err::string_to_numeric,
			                           lak::err::value_out_of_range,
			                           lak::dsl::err::parse>>;

			_impl_number_parse_result<uintmax_t> _impl_peek_uintmax(
			  lak::dsl::concepts::substring_parser auto par = lak::dsl::dec_number,
			  lak::numeric_base base                        = lak::numeric_base::dec)
			{
				return _impl_peek(par).and_then(
				  [&](const lak::dsl::parse_result<lak::u8string_view> &result)
				    -> _impl_number_parse_result<uintmax_t>
				  {
					  return lak::string_to_uintmax(result.value, base)
					    .and_then(
					      [&]<typename T>(
					        T value) -> _impl_number_parse_result<uintmax_t>
					      {
						      return lak::ok_t{lak::dsl::parse_result<T>{
						        .consumed  = result.consumed,
						        .remaining = result.remaining,
						        .value     = value,
						      }};
					      });
				  });
			}

			number_parse_result<uintmax_t> peek_uintmax(
			  lak::dsl::concepts::substring_parser auto par = lak::dsl::dec_number,
			  lak::numeric_base base                        = lak::numeric_base::dec)
			{
				return _impl_peek_uintmax(par, base).LAK_BINARY_READER_PEEK_MAP;
			}

			number_parse_result<uintmax_t> read_uintmax(
			  lak::dsl::concepts::substring_parser auto par = lak::dsl::dec_number,
			  lak::numeric_base base                        = lak::numeric_base::dec)
			{
				return _impl_peek_uintmax(par, base).LAK_BINARY_READER_READ_MAP;
			}

			_impl_number_parse_result<intmax_t> _impl_peek_intmax(
			  lak::dsl::concepts::substring_parser auto par =
			    lak::dsl::signed_dec_number,
			  lak::numeric_base base = lak::numeric_base::dec)
			{
				return _impl_peek(par).and_then(
				  [&](const lak::dsl::parse_result<lak::u8string_view> &result)
				    -> _impl_number_parse_result<intmax_t>
				  {
					  return lak::string_to_intmax(result.value, base)
					    .and_then(
					      [&]<typename T>(T value) -> _impl_number_parse_result<intmax_t>
					      {
						      return lak::ok_t{lak::dsl::parse_result<T>{
						        .consumed  = result.consumed,
						        .remaining = result.remaining,
						        .value     = value,
						      }};
					      });
				  });
			}

			number_parse_result<intmax_t> peek_intmax(
			  lak::dsl::concepts::substring_parser auto par =
			    lak::dsl::signed_dec_number,
			  lak::numeric_base base = lak::numeric_base::dec)
			{
				return _impl_peek_intmax(par, base).LAK_BINARY_READER_PEEK_MAP;
			}

			number_parse_result<intmax_t> read_intmax(
			  lak::dsl::concepts::substring_parser auto par =
			    lak::dsl::signed_dec_number,
			  lak::numeric_base base = lak::numeric_base::dec)
			{
				return _impl_peek_intmax(par, base).LAK_BINARY_READER_READ_MAP;
			}

			template<
			  lak::dsl::concepts::substring_parser auto int_part =
			    lak::dsl::signed_dec_number,
			  lak::dsl::concepts::substring_parser auto frac_part = lak::dsl::
			    capture_nth<1U, lak::dsl::char_literal<U'.'>, lak::dsl::dec_number>,
			  lak::dsl::concepts::substring_parser auto exp_part =
			    lak::dsl::capture_nth<1U,
			                          lak::dsl::one_of_chars<U'e', U'E'>,
			                          lak::dsl::signed_dec_number>>
			_impl_number_parse_result<double> _impl_peek_double(
			  lak::numeric_base base = lak::numeric_base::dec)
			{
				return _impl_peek(lak::dsl::capture<int_part> +
				                  lak::dsl::capture<~frac_part> +
				                  lak::dsl::capture<~exp_part>)
				  .and_then(
				    [&](const lak::dsl::parse_result<lak::tuple<lak::u8string_view,
				                                                lak::u8string_view,
				                                                lak::u8string_view>>
				          &result) -> _impl_number_parse_result<double>
				    {
					    return lak::string_to_double(result.value.template get<0>(),
					                                 result.value.template get<1>(),
					                                 result.value.template get<2>(),
					                                 uintmax_t(base),
					                                 base)
					      .and_then(
					        [&]<typename T>(T value) -> _impl_number_parse_result<double>
					        {
						        return lak::ok_t{lak::dsl::parse_result<T>{
						          .consumed  = result.consumed,
						          .remaining = result.remaining,
						          .value     = value,
						        }};
					        });
				    });
			}

			template<
			  lak::dsl::concepts::substring_parser auto int_part =
			    lak::dsl::signed_dec_number,
			  lak::dsl::concepts::substring_parser auto frac_part = lak::dsl::
			    capture_nth<1U, lak::dsl::char_literal<U'.'>, lak::dsl::dec_number>,
			  lak::dsl::concepts::substring_parser auto exp_part =
			    lak::dsl::capture_nth<1U,
			                          lak::dsl::one_of_chars<U'e', U'E'>,
			                          lak::dsl::signed_dec_number>>
			number_parse_result<double> peek_double(
			  lak::numeric_base base = lak::numeric_base::dec)
			{
				return _impl_peek_double<int_part, frac_part, exp_part>(base)
				  .LAK_BINARY_READER_PEEK_MAP;
			}

			template<
			  lak::dsl::concepts::substring_parser auto int_part =
			    lak::dsl::signed_dec_number,
			  lak::dsl::concepts::substring_parser auto frac_part = lak::dsl::
			    capture_nth<1U, lak::dsl::char_literal<U'.'>, lak::dsl::dec_number>,
			  lak::dsl::concepts::substring_parser auto exp_part =
			    lak::dsl::capture_nth<1U,
			                          lak::dsl::one_of_chars<U'e', U'E'>,
			                          lak::dsl::signed_dec_number>>
			number_parse_result<double> read_double(
			  lak::numeric_base base = lak::numeric_base::dec)
			{
				return _impl_peek_double<int_part, frac_part, exp_part>(base)
				  .LAK_BINARY_READER_READ_MAP;
			}

#define BINARY_READER_MEMBERS(TYPE, NAME, ...)                                \
	inline _impl_number_parse_result<TYPE> _impl_peek_##NAME(                   \
	  lak::dsl::concepts::substring_parser auto par,                            \
	  lak::numeric_base base = lak::numeric_base::dec)                          \
	{                                                                           \
		return _impl_peek_uintmax(par, base).and_then(                            \
		  [](const lak::dsl::parse_result<uintmax_t> &v)                          \
		    -> _impl_number_parse_result<TYPE>                                    \
		  {                                                                       \
				if (v.value > std::numeric_limits<TYPE>::max())                       \
					return lak::err_t{lak::err::value_out_of_range{}};                  \
				else                                                                  \
					return lak::ok_t{lak::dsl::parse_result<TYPE>{                      \
					  .consumed  = v.consumed,                                          \
					  .remaining = v.remaining,                                         \
					  .value     = static_cast<TYPE>(v.value),                          \
					}};                                                                 \
		  });                                                                     \
	}                                                                           \
	inline number_parse_result<TYPE> peek_##NAME(                               \
	  lak::dsl::concepts::substring_parser auto par,                            \
	  lak::numeric_base base = lak::numeric_base::dec)                          \
	{                                                                           \
		return _impl_peek_##NAME(par, base).LAK_BINARY_READER_PEEK_MAP;           \
	}                                                                           \
	inline number_parse_result<TYPE> read_##NAME(                               \
	  lak::dsl::concepts::substring_parser auto par,                            \
	  lak::numeric_base base = lak::numeric_base::dec)                          \
	{                                                                           \
		return _impl_peek_##NAME(par, base).LAK_BINARY_READER_READ_MAP;           \
	}
			LAK_FOREACH_UNSIGNED_INTEGER(BINARY_READER_MEMBERS)
#undef BINARY_READER_MEMBERS

#define BINARY_READER_MEMBERS(TYPE, NAME, ...)                                \
	inline _impl_number_parse_result<TYPE> _impl_peek_##NAME(                   \
	  lak::dsl::concepts::substring_parser auto par,                            \
	  lak::numeric_base base = lak::numeric_base::dec)                          \
	{                                                                           \
		return _impl_peek_intmax(par, base).and_then(                             \
		  [](const lak::dsl::parse_result<intmax_t> &v)                           \
		    -> _impl_number_parse_result<TYPE>                                    \
		  {                                                                       \
				if (v.value < std::numeric_limits<TYPE>::lowest() ||                  \
				    v.value > std::numeric_limits<TYPE>::max())                       \
					return lak::err_t{lak::err::value_out_of_range{}};                  \
				else                                                                  \
					return lak::ok_t{lak::dsl::parse_result<TYPE>{                      \
					  .consumed  = v.consumed,                                          \
					  .remaining = v.remaining,                                         \
					  .value     = static_cast<TYPE>(v.value),                          \
					}};                                                                 \
		  });                                                                     \
	}                                                                           \
	inline number_parse_result<TYPE> peek_##NAME(                               \
	  lak::dsl::concepts::substring_parser auto par,                            \
	  lak::numeric_base base = lak::numeric_base::dec)                          \
	{                                                                           \
		return _impl_peek_##NAME(par, base).LAK_BINARY_READER_PEEK_MAP;           \
	}                                                                           \
	inline number_parse_result<TYPE> read_##NAME(                               \
	  lak::dsl::concepts::substring_parser auto par,                            \
	  lak::numeric_base base = lak::numeric_base::dec)                          \
	{                                                                           \
		return _impl_peek_##NAME(par, base).LAK_BINARY_READER_READ_MAP;           \
	}
			LAK_FOREACH_SIGNED_INTEGER(BINARY_READER_MEMBERS)
#undef BINARY_READER_MEMBERS

#define BINARY_READER_MEMBERS(TYPE, NAME, ...)                                \
	template<                                                                   \
	  lak::dsl::concepts::substring_parser auto int_part =                      \
	    lak::dsl::signed_dec_number,                                            \
	  lak::dsl::concepts::substring_parser auto frac_part = lak::dsl::          \
	    capture_nth<1U, lak::dsl::char_literal<U'.'>, lak::dsl::dec_number>,    \
	  lak::dsl::concepts::substring_parser auto exp_part =                      \
	    lak::dsl::capture_nth<1U,                                               \
	                          lak::dsl::one_of_chars<U'e', U'E'>,               \
	                          lak::dsl::signed_dec_number>>                     \
	inline _impl_number_parse_result<TYPE> _impl_peek_##NAME(                   \
	  lak::numeric_base base = lak::numeric_base::dec)                          \
	{                                                                           \
		return _impl_peek_double<int_part, frac_part, exp_part>(base).and_then(   \
		  [](const lak::dsl::parse_result<double> &v)                             \
		    -> _impl_number_parse_result<TYPE>                                    \
		  {                                                                       \
				if (v.value < std::numeric_limits<TYPE>::lowest() ||                  \
				    v.value > std::numeric_limits<TYPE>::max())                       \
					return lak::err_t{lak::err::value_out_of_range{}};                  \
				else                                                                  \
					return lak::ok_t{lak::dsl::parse_result<TYPE>{                      \
					  .consumed  = v.consumed,                                          \
					  .remaining = v.remaining,                                         \
					  .value     = static_cast<TYPE>(v.value),                          \
					}};                                                                 \
		  });                                                                     \
	}                                                                           \
	template<                                                                   \
	  lak::dsl::concepts::substring_parser auto int_part =                      \
	    lak::dsl::signed_dec_number,                                            \
	  lak::dsl::concepts::substring_parser auto frac_part = lak::dsl::          \
	    capture_nth<1U, lak::dsl::char_literal<U'.'>, lak::dsl::dec_number>,    \
	  lak::dsl::concepts::substring_parser auto exp_part =                      \
	    lak::dsl::capture_nth<1U,                                               \
	                          lak::dsl::one_of_chars<U'e', U'E'>,               \
	                          lak::dsl::signed_dec_number>>                     \
	inline number_parse_result<TYPE> peek_##NAME(lak::numeric_base base =       \
	                                               lak::numeric_base::dec)      \
	{                                                                           \
		return _impl_peek_##NAME<int_part, frac_part, exp_part>(base)             \
		  .LAK_BINARY_READER_READ_MAP_STIP;                                       \
	}                                                                           \
	template<                                                                   \
	  lak::dsl::concepts::substring_parser auto int_part =                      \
	    lak::dsl::signed_dec_number,                                            \
	  lak::dsl::concepts::substring_parser auto frac_part = lak::dsl::          \
	    capture_nth<1U, lak::dsl::char_literal<U'.'>, lak::dsl::dec_number>,    \
	  lak::dsl::concepts::substring_parser auto exp_part =                      \
	    lak::dsl::capture_nth<1U,                                               \
	                          lak::dsl::one_of_chars<U'e', U'E'>,               \
	                          lak::dsl::signed_dec_number>>                     \
	inline number_parse_result<TYPE> read_##NAME(lak::numeric_base base =       \
	                                               lak::numeric_base::dec)      \
	{                                                                           \
		return _impl_peek_##NAME<int_part, frac_part, exp_part>(base)             \
		  .LAK_BINARY_READER_READ_MAP;                                            \
	}
			LAK_FOREACH_FLOAT(BINARY_READER_MEMBERS)
#undef BINARY_READER_MEMBERS

#undef LAK_BINARY_READER_READ_MAP
		};
	}
}

#endif
