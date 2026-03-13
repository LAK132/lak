#ifndef LAK_FORMAT_TRAITS_HPP
#define LAK_FORMAT_TRAITS_HPP

#include "lak/concepts.hpp"
#include "lak/string.hpp"
#include "lak/type_traits.hpp"
#ifndef LAK_STRING_VIEW_FORWARD_ONLY
#	define LAK_STRING_VIEW_FORWARD_ONLY
#endif
#include "lak/string_view.hpp"

namespace lak
{
	template<typename T, typename CHAR>
	struct format_traits;
	// {
	// 	dynamic:
	// 	struct format_args
	// 	{
	// 	};
	// 	static consteval format_args parse_args(lak::string_view<CHAR> args);
	// 	static constexpr lak::string<CHAR> to_string(const format_args &fmt,
	// 	                                             const T &value);

	// 	static:
	// 	static constexpr lak::string<CHAR> to_string(const T &value);
	// };

	namespace concepts
	{
		template<typename T, typename CHAR>
		concept dynamic_formattable = requires(T obj) {
			typename lak::format_traits<T, CHAR>::format_args;
			{
				lak::format_traits<T, CHAR>::parse_args(
				  lak::declval<lak::string_view<CHAR>>())
			} -> lak::concepts::same_as<
			  typename lak::format_traits<T, CHAR>::format_args>;
			{
				lak::format_traits<T, CHAR>::to_string(
				  lak::declval<
				    const typename lak::format_traits<T, CHAR>::format_args &>(),
				  lak::declval<const T &>())
			} -> lak::concepts::same_as<lak::string<CHAR>>;
		};

		template<typename T, typename CHAR>
		concept static_formattable = requires(T obj) {
			{
				lak::format_traits<T, CHAR>::to_string(lak::declval<const T &>())
			} -> lak::concepts::same_as<lak::string<CHAR>>;
		};

		template<typename T, typename CHAR>
		concept formattable = lak::concepts::dynamic_formattable<T, CHAR> ||
		                      lak::concepts::static_formattable<T, CHAR>;
	}

	template<typename T, typename CHAR>
	struct _format_args_type;

	template<typename T, typename CHAR>
	requires(lak::concepts::dynamic_formattable<T, CHAR>)
	struct _format_args_type<T, CHAR>
	{
		using type = lak::format_traits<T, CHAR>::format_args;
	};

	template<typename T, typename CHAR>
	requires(lak::concepts::formattable<T, CHAR> &&
	         !lak::concepts::dynamic_formattable<T, CHAR>)
	struct _format_args_type<T, CHAR>
	{
		using type = lak::monostate;
	};

	template<typename T, typename CHAR>
	using format_args_t = typename lak::_format_args_type<T, CHAR>::type;

	template<typename T, typename CHAR>
	consteval lak::format_args_t<T, CHAR> parse_format_args(
	  lak::string_view<CHAR> args)
	requires(lak::concepts::dynamic_formattable<T, CHAR>)
	{
		return lak::format_traits<T, CHAR>::parse_args(args);
	}

	template<typename T, typename CHAR>
	consteval lak::format_args_t<T, CHAR> parse_format_args(
	  lak::string_view<CHAR>)
	requires(lak::concepts::formattable<T, CHAR> &&
	         !lak::concepts::dynamic_formattable<T, CHAR>)
	{
		return {};
	}
}

#endif
