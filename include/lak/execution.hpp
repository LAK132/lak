#ifndef LAK_EXECUTION_HPP
#define LAK_EXECUTION_HPP

#include "lak/concepts.hpp"

namespace lak
{
	namespace execution
	{
		struct sequenced_policy
		{
		};
		inline constexpr sequenced_policy seq;

		struct unsequenced_policy
		{
		};
		inline constexpr unsequenced_policy unseq;

		struct parallel_policy
		{
		};
		inline constexpr parallel_policy par;

		struct parallel_unsequenced_policy
		{
		};
		inline constexpr parallel_unsequenced_policy par_unseq;

		namespace concepts
		{
			template<typename T>
			concept policy =
			  lak::concepts::one_of<T,
			                        lak::execution::sequenced_policy,
			                        lak::execution::unsequenced_policy,
			                        lak::execution::parallel_policy,
			                        lak::execution::parallel_unsequenced_policy>;

			template<typename T>
			concept sequenced =
			  lak::concepts::one_of<T,
			                        lak::execution::sequenced_policy,
			                        lak::execution::parallel_policy>;
			template<typename T>
			concept unsequenced =
			  lak::concepts::one_of<T,
			                        lak::execution::unsequenced_policy,
			                        lak::execution::parallel_unsequenced_policy>;

			template<typename T>
			concept parallel =
			  lak::concepts::one_of<T,
			                        lak::execution::parallel_policy,
			                        lak::execution::parallel_unsequenced_policy>;

			template<typename T>
			concept serial =
			  lak::concepts::one_of<T,
			                        lak::execution::sequenced_policy,
			                        lak::execution::unsequenced_policy>;
		}
	}
}

#endif
