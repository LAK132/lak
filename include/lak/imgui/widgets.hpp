#ifndef LAK_IMGUI_WIDGETS_HPP
#define LAK_IMGUI_WIDGETS_HPP

#include "lak/macro_utils.hpp"
#include "lak/utility.hpp"

#include <imgui.h>

#include <thread>

namespace lak
{
	template<typename R, typename... T, typename... D>
	bool AwaitPopup(const char *str_id,
	                bool &open,
	                std::thread *&staticThread,
	                std::atomic<bool> &staticFinished,
	                R (*callback)(T...),
	                const std::tuple<D...> &callbackData);

	bool VertSplitter(float &left,
	                  float &right,
	                  float width,
	                  float leftMin  = 8.0f,
	                  float rightMin = 8.0f,
	                  float length   = -1.0f);

	bool HoriSplitter(float &top,
	                  float &bottom,
	                  float height,
	                  float topMin    = 8.0f,
	                  float bottomMin = 8.0f,
	                  float length    = -1.0f);

	struct vert_split_child
	{
		float left_size;
		float right_size;

		inline vert_split_child();

		inline bool begin(const char *id, bool border = true);

		inline bool split(bool border = true) const;

		inline void end() const;
	};

	struct hori_split_child
	{
		float top_size;
		float bottom_size;

		inline hori_split_child();

		inline bool begin(const char *id, bool border = true);

		inline bool split(bool border = true) const;

		inline void end() const;
	};

	bool TreeNode(const char *fmt, ...);

	struct tree_node
	{
		bool _result;

		template<typename... ARGS>
		tree_node(const char *fmt, ARGS &&...args)
		: _result(lak::TreeNode(fmt, lak::forward<ARGS>(args)...))
		{
			ImGui::Separator();
		}

		inline ~tree_node()
		{
			if (_result)
			{
				ImGui::Separator();
				ImGui::TreePop();
			}
		}

		inline operator bool() const { return _result; }
	};

#define LAK_TREE_NODE(...)                                                    \
	if (lak::tree_node UNIQUIFY(TREE_NODE_)(__VA_ARGS__); UNIQUIFY(TREE_NODE_))
}

#include "widgets.inl"

#endif
