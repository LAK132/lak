#ifndef LAK_IMGUI_TEXTURE_HPP
#define LAK_IMGUI_TEXTURE_HPP

#include "lak/com_ptr.hpp"
#include "lak/result.hpp"

#include "lak/imgui/widgets.hpp"

namespace lak
{
	template<>
	struct unique_com_ptr_traits<ImTextureRef>
	{
		using handle_type  = ImTextureRef;
		using exposed_type = ImTextureRef;

		static constexpr ImTextureID null_value = ImTextureID_Invalid;

		template<typename... ARGS>
		inline static lak::infallible_result<ImTextureRef> ctor(ARGS &&...args)
		{
			return lak::ok_t{lak::CreateTexture(lak::forward<ARGS>(args)...)};
		}

		inline static void dtor(handle_type &handle)
		{
			lak::DestroyTexture(handle);
		}

		inline static bool valid(const handle_type &handle)
		{
			return handle.GetTexID() != null_value;
		}
	};

	static_assert(lak::concepts::unique_com_ptr_traits<
	              lak::unique_com_ptr_traits<ImTextureRef>>);

	using ImUniqueTexture = lak::unique_com_ptr<ImTextureRef>;
	using ImSharedTexture = lak::ref_count_com_ptr<ImTextureRef>;
}

#endif
