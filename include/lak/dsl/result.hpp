#ifndef LAK_DSL_RESULT_HPP
#define LAK_DSL_RESULT_HPP

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
			struct parse
			{
				::lak::u8string message;

				inline ::lak::u8string to_string() const
				{
					return u8"parse error" +
					       (message.empty() ? u8""_str : u8": " + message);
				}
			};
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
