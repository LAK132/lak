#ifndef LAK_DSL_RESULT_HPP
#define LAK_DSL_RESULT_HPP

#include "lak/array.hpp"
#include "lak/errors.hpp"
#include "lak/format.hpp"
#include "lak/result.hpp"
#include "lak/string_view.hpp"

#include "lak/string_literals/string.hpp"

namespace lak
{
	namespace dsl
	{
		namespace err
		{
			struct unexpected_char
			{
				char32_t expected_min;
				char32_t expected_max;
				char32_t got;
				bool negative = false;

				inline lak::u8string to_string() const
				{
					if (expected_min == expected_max)
					{
						if (negative)
							return lak::fmt<u8"unexpected '{:A}'">(got);
						else
							return lak::fmt<u8"expected '{:A}', got '{:A}'">(expected_min,
							                                                 got);
					}
					else
					{
						if (negative)
							return lak::fmt<u8"unexpected '{:A}' (!'{:A}'..'{:A}')">(
							  got, expected_min, expected_max);
						else
							return lak::fmt<u8"expected '{:A}'..'{:A}', got '{:A}'">(
							  expected_min, expected_max, got);
					}
				}
			};

			struct unexpected_str
			{
				lak::u8string_view expected;
				lak::u8string_view got;
				bool negative = false;

				inline lak::u8string to_string() const
				{
					if (negative)
						return lak::fmt<u8"unexpected '{}'">(expected);
					else
						return lak::fmt<u8"expected '{}', got '{}'">(expected, got);
				}
			};

			struct parse;

			struct multi
			{
				lak::array<lak::dsl::err::parse> errors;

				multi();
				multi(const multi &);
				multi(multi &&);
				multi &operator=(multi &&);
				multi &operator=(const multi &);
				multi(lak::array<lak::dsl::err::parse> &&errs);

				inline lak::u8string to_string() const;
			};

			struct bottom
			{
				inline lak::u8string to_string() const { return u8"bottom"_str; }
			};
			struct dummy
			{
				inline lak::u8string to_string() const { return u8"dummy"_str; }
			};
			struct eof
			{
				inline lak::u8string to_string() const
				{
					return u8"expected end of file"_str;
				}
			};

			struct parse
			{
				lak::variant<lak::u8string,
				             lak::err::out_of_data,
				             lak::err::invalid_character_length,
				             lak::err::string_to_numeric,
				             lak::err::value_out_of_range,
				             lak::dsl::err::bottom,
				             lak::dsl::err::dummy,
				             lak::dsl::err::eof,
				             lak::dsl::err::unexpected_char,
				             lak::dsl::err::unexpected_str,
				             lak::dsl::err::multi>
				  info;

				inline lak::u8string to_string() const
				{
					lak::u8string str = u8"parse error: "_str;
					info.visit([&](const auto &err) { str += lak::fmt<u8"{}">(err); });
					return str;
				}
			};

			inline lak::u8string multi::to_string() const
			{
				if (errors.empty()) return {};
				lak::u8string err = u8"(" + errors[0].to_string() + u8")";
				for (const auto &e : lak::span(errors).subspan(1U))
					err += u8" or (" + e.to_string() + u8")";
				return err;
			}
		}

		template<typename T>
		requires(!lak::is_void_v<T>)
		struct parse_result
		{
			using value_type = T;
			lak::u8string_view consumed;
			lak::u8string_view remaining;
			T value;

			template<lak::concepts::invocable<lak::add_lvalue_reference_t<T>> F>
			auto map(F &&f) & -> parse_result<
			  lak::invoke_result_t<F, lak::add_lvalue_reference_t<T>>>
			{
				return {
				  .consumed  = consumed,
				  .remaining = remaining,
				  .value     = f(value),
				};
			}
			template<
			  lak::concepts::invocable<const lak::add_lvalue_reference_t<T>> F>
			auto map(F &&f) const & -> parse_result<
			  lak::invoke_result_t<F, const lak::add_lvalue_reference_t<T>>>
			{
				return {
				  .consumed  = consumed,
				  .remaining = remaining,
				  .value     = f(value),
				};
			}
			template<lak::concepts::invocable<T &&> F>
			auto map(F &&f) && -> parse_result<lak::invoke_result_t<F, T &&>>
			{
				return {
				  .consumed  = consumed,
				  .remaining = remaining,
				  .value     = f(value),
				};
			}
		};

		template<typename T>
		requires(!lak::is_void_v<T>)
		using result =
		  lak::result<lak::dsl::parse_result<T>, lak::dsl::err::parse>;
	}

	template<typename CHAR>
	struct format_traits<lak::dsl::err::parse, CHAR>
	{
		static constexpr lak::string<CHAR> to_string(
		  const lak::dsl::err::parse &val)
		{
			return lak::strconv<CHAR>(val.to_string());
		}
	};
}

#endif
