#ifndef LAK_ALLOC_HPP
#define LAK_ALLOC_HPP

#include "lak/result.hpp"
#include "lak/span.hpp"

#include <ostream>

namespace lak
{
	struct bad_alloc
	{
		inline friend std::ostream &operator<<(std::ostream &strm,
		                                       const lak::bad_alloc &)
		{
			return strm << "bad alloc";
		}
	};

	namespace alloc
	{
		template<typename T>
		using result = lak::result<T, lak::bad_alloc>;

		enum locality
		{
			local, // thread_local
			global,
			container,
		};
	}

	lak::alloc::result<lak::span<byte_t>> local_alloc(size_t size,
	                                                  size_t align = 0);
	void local_free(lak::span<byte_t> data);

	lak::alloc::result<lak::span<byte_t>> global_alloc(size_t size,
	                                                   size_t align = 0);
	void global_free(lak::span<byte_t> data);

	template<lak::alloc::locality LOC>
	lak::alloc::result<lak::span<byte_t>> malloc(size_t size, size_t align = 0)
	{
		static_assert(LOC == lak::alloc::locality::local ||
		              LOC == lak::alloc::locality::global);
		if constexpr (LOC == lak::alloc::locality::local)
			return lak::local_alloc(size, align);
		else
			return lak::global_alloc(size, align);
	}

	template<lak::alloc::locality LOC>
	void free(lak::span<byte_t> data)
	{
		static_assert(LOC == lak::alloc::locality::local ||
		              LOC == lak::alloc::locality::global);
		if constexpr (LOC == lak::alloc::locality::local)
			lak::local_free(data);
		else
			lak::global_free(data);
	}
}

#endif
