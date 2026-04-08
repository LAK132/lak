#ifndef LAK_ERRORS_HPP
#define LAK_ERRORS_HPP

#include "lak/format_traits.hpp"
#include "lak/strconv.hpp"
#include "lak/string_literals/view.hpp"

namespace lak
{
	namespace err
	{
		struct out_of_data
		{
		};

		struct value_out_of_range
		{
		};

		struct invalid_character_length
		{
		};
	}

	template<typename CHAR>
	struct format_traits<lak::err::out_of_data, CHAR>
	{
		static constexpr lak::string<CHAR> to_string(const lak::err::out_of_data &)
		{
			return lak::strconv<CHAR>("out of data"_view);
		}
	};

	template<typename CHAR>
	struct format_traits<lak::err::value_out_of_range, CHAR>
	{
		static constexpr lak::string<CHAR> to_string(
		  const lak::err::value_out_of_range &)
		{
			return lak::strconv<CHAR>("value out of range"_view);
		}
	};

	template<typename CHAR>
	struct format_traits<lak::err::invalid_character_length, CHAR>
	{
		static constexpr lak::string<CHAR> to_string(
		  const lak::err::invalid_character_length &)
		{
			return lak::strconv<CHAR>("invalid character length"_view);
		}
	};
}

#endif
