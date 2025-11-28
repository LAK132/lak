#ifndef LAK_STRING_HPP
#define LAK_STRING_HPP

#include "lak/char.hpp"
#include "lak/type_traits.hpp"

#include <string>

namespace lak
{
	template<typename CHAR>
	using string = std::basic_string<CHAR>;

	template<typename T>
	struct is_string
	{
		static constexpr bool value = false;
	};
	template<typename T>
	inline constexpr bool is_string_v = lak::is_string<T>::value;

	template<typename T>
	struct is_lak_type<lak::is_string<T>> : lak::true_type
	{
	};

	template<typename CHAR>
	struct is_string<lak::string<CHAR>>
	{
		static constexpr bool value = true;
	};

#define LAK_DEFINE_STRING_TYPES(CHAR, PREFIX, ...)                            \
	using PREFIX##string = lak::string<CHAR>;                                   \
	template<size_t N>                                                          \
	struct is_string<CHAR[N]>                                                   \
	{                                                                           \
		static constexpr bool value = true;                                       \
	};                                                                          \
	template<size_t N>                                                          \
	struct is_string<const CHAR[N]>                                             \
	{                                                                           \
		static constexpr bool value = true;                                       \
	};                                                                          \
	template<>                                                                  \
	struct is_string<CHAR *>                                                    \
	{                                                                           \
		static constexpr bool value = true;                                       \
	};                                                                          \
	template<>                                                                  \
	struct is_string<const CHAR *>                                              \
	{                                                                           \
		static constexpr bool value = true;                                       \
	};
	LAK_FOREACH_CHAR(LAK_DEFINE_STRING_TYPES)
#undef LAK_DEFINE_STRING_TYPES
}

#endif
