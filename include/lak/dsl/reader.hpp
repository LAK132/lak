#ifndef LAK_DSL_READER_HPP
#define LAK_DSL_READER_HPP

#include "lak/dsl/dsl.hpp"

#include "lak/ptr_intrin.hpp"

namespace lak
{
	namespace dsl
	{
		struct reader
		{
			lak::u8string_view data;
			size_t cursor = 0U;

			inline lak::u8string_view consumed() const { return data.first(cursor); }
			inline lak::u8string_view remaining() const
			{
				return data.substr(cursor);
			}
			inline bool empty() const { return remaining().empty(); }

			template<lak::dsl::concepts::parser auto par>
			auto parse() -> lak::result<typename decltype(par)::value_type,
			                            lak::dsl::err::parse>
			{
				RES_TRY_ASSIGN(auto res =, par.parse(remaining()));
				if (res.remaining.empty())
				{
					cursor = data.size();
				}
				else
				{
					ASSERT(
					  lak::ptr_in_range(res.remaining.data(), data.data(), data.size()));
					cursor = static_cast<size_t>(res.remaining.data() - data.data());
				}
				return lak::move_ok(res.value);
			}
		};
	}
}

#endif
