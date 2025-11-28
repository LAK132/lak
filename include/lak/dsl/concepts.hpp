#ifndef LAK_DSP_CONCEPTS_HPP
#define LAK_DSP_CONCEPTS_HPP

#include "lak/dsl/result.hpp"

#include "lak/concepts.hpp"
#include "lak/string_view.hpp"

namespace lak
{
	namespace dsl
	{
		namespace concepts
		{
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
			concept pure_match_parser =
			  lak::dsl::concepts::parser<T> && T::is_pure_match;
		}
	}
}

#endif
