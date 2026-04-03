#include "lak/streamify.hpp"

#ifndef LAK_FORMAT_HPP
#	define LAK_FORMAT_HPP

#	include "lak/format_traits.hpp"

#	include "lak/format_string.hpp"

namespace lak
{
	// lak::fmt<"format {} string">(132) -> lak::astring
	// lak::fmt<u8"format {} string">(132) -> lak::u8string
	// lak::fmt<U"{:d}, {0:#x}">(132) -> U"132, 0x84"

	static_assert(
	  lak::is_same_v<lak::const_string<char8_t, 1U>::char_type, char8_t>);

	template<lak::format_string FMT, typename T, size_t I>
	static consteval auto _get_format_args(lak::size_type<I>)
	{
		using char_type = typename decltype(FMT)::char_type;
		return lak::parse_format_args<T, char_type>(
		  decltype(FMT)::specifier(I).second);
	}

	template<lak::format_string FMT, typename... T>
	static consteval auto get_format_args()
	{
		return [&]<size_t... I>(lak::index_sequence<I...>)
		{
			return lak::tuple(
			  _get_format_args<
			    FMT,
			    lak::nth_type_t<decltype(FMT)::specifier(I).first, T...>>(
			    lak::size_type<I>{})...);
		}(lak::make_index_sequence<decltype(FMT)::specifiers.size()>{});
	}

	template<lak::format_string FMT, typename... ARGS>
	lak::string<typename decltype(FMT)::char_type> format(ARGS &&...args)
	{
		using fmt_type  = decltype(FMT);
		using char_type = typename decltype(FMT)::char_type;

		static_assert(
		  ((lak::concepts::formattable<lak::remove_cvref_t<ARGS>, char_type>) &&
		   ...));

		constexpr auto format_args =
		  lak::get_format_args<FMT, lak::remove_cvref_t<ARGS>...>();
		lak::c_array<lak::string<char_type>, fmt_type::specifiers.size()>
		  parsed_results;
		[&]<size_t... Is>(lak::index_sequence<Is...>)
		{
			auto func = [&]<size_t I>(lak::size_type<I>)
			{
				constexpr size_t arg_i = fmt_type::specifier(I).first;

				static_assert(I < lak::tuple_size_v<decltype(format_args)>);
				static_assert(arg_i < sizeof...(ARGS));

				using value_type =
				  lak::remove_cvref_t<lak::nth_type_t<arg_i, ARGS...>>;

				// const value_type &arg =
				//   lak::get_nth<arg_i, lak::remove_reference_t<ARGS>...>(args...);
				const value_type &arg = *lak::tuple((&args)...).template get<arg_i>();

				if constexpr (lak::concepts::dynamic_formattable<value_type,
				                                                 char_type>)
					parsed_results[I] =
					  lak::format_traits<value_type, char_type>::to_string(
					    format_args.template get<I>(), arg);
				else
					parsed_results[I] =
					  lak::format_traits<value_type, char_type>::to_string(arg);
			};
			((func(lak::size_type<Is>{}), ...));
		}(lak::make_index_sequence<fmt_type::specifiers.size()>{});

		size_t reserve_space = fmt_type::prefix(0).size();
		for (size_t i = 0; i < fmt_type::specifiers.size(); ++i)
			reserve_space +=
			  parsed_results[i].size() + fmt_type::prefix(i + 1U).size();

		lak::string<char_type> result;
		result.reserve(reserve_space);
		result += fmt_type::prefix(0);
		for (size_t i = 0; i < fmt_type::specifiers.size(); ++i)
		{
			result += parsed_results[i];
			result += fmt_type::prefix(i + 1U);
		}
		return result;
	}

	template<lak::const_string STR, typename... ARGS>
	auto fmt(ARGS &&...args)
	{
		using CHAR = typename decltype(STR)::char_type;
		static_assert(
		  ((lak::concepts::formattable<lak::remove_cvref_t<ARGS>, CHAR>) && ...));
		constexpr lak::format_string<STR> fmt;
		return lak::format<fmt>(lak::forward<ARGS>(args)...);
	}

#	define LAK_FORMAT(CHAR, PREFIX, ...)                                       \
		template<lak::PREFIX##const_string STR, typename... ARGS>                 \
		lak::string<CHAR> PREFIX##fmt(ARGS &&...args)                             \
		{                                                                         \
			static_assert(                                                          \
			  ((lak::concepts::formattable<lak::remove_cvref_t<ARGS>, CHAR>) &&     \
			   ...));                                                               \
			return lak::format<lak::format_string<STR>{}>(                          \
			  lak::forward<ARGS>(args)...);                                         \
		};
	LAK_FOREACH_CHAR(LAK_FORMAT)
#	undef LAK_FORMAT

	template<typename CHAR, lak::aconst_string STR, typename... ARGS>
	lak::string<CHAR> fmt(ARGS &&...args)
	{
		static_assert(
		  ((lak::concepts::formattable<lak::remove_cvref_t<ARGS>, CHAR>) && ...));
		return lak::format<lak::format_string<lak::strconv<CHAR>(STR)>{}>(
		  lak::forward<ARGS>(args)...);
	}
}

#endif

#ifdef LAK_FORMAT_FORWARD_ONLY
#	undef LAK_FORMAT_FORWARD_ONLY
#else
#	ifndef LAK_FORMAT_HPP_IMPL
#		define LAK_FORMAT_HPP_IMPL
#		include "lak/format.inl"
#	endif
#endif
