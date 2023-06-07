#include "widgets.hpp"

#include "lak/await.hpp"

template<typename R, typename... T, typename... D>
bool lak::AwaitPopup(const char *str_id,
                     bool &open,
                     std::thread *&staticThread,
                     std::atomic<bool> &staticFinished,
                     R (*callback)(T...),
                     const std::tuple<D...> &callbackData)
{
	if (ImGui::BeginPopup(str_id, ImGuiWindowFlags_AlwaysAutoResize))
	{
		if (lak::await(staticThread, &staticFinished, callback, callbackData))
		{
			ImGui::CloseCurrentPopup();
			ImGui::EndPopup();
			open = false;
			return false;
		}
		open = true;
	}
	else
	{
		open = false;
		ImGui::OpenPopup(str_id);
	}

	return true;
}
