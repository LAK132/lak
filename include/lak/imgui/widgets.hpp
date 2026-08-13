#ifndef LAK_IMGUI_WIDGETS_HPP
#define LAK_IMGUI_WIDGETS_HPP

#include "lak/com_ptr.hpp"
#include "lak/const_string.hpp"
#include "lak/error_code_result.hpp"
#include "lak/macro_utils.hpp"
#include "lak/optional.hpp"
#include "lak/system/windowing/window.hpp"
#include "lak/utility.hpp"

#include "lak/imgui/backend.hpp"

#ifdef LAK_ENABLE_COBALT
#	include <Cobalt/RendererInterface/RendererInterface.pkg>
#endif

#include <imgui.h>

#include <thread>

namespace lak
{
	template<lak::u8const_string STR, typename... ARGS>
	void Text(ARGS &&...args);

	ImTextureRef CreateTexture(const lak::image<lak::vec4u8_t> &image);
	ImTextureRef CreateTexture(const lak::image<lak::vec3u8_t> &image);
	ImTextureRef CreateTexture(const lak::image<lak::vec4u16_t> &image);
	ImTextureRef CreateTexture(const lak::image<lak::vec3u16_t> &image);
	ImTextureRef CreateTexture(const lak::image<lak::vec4f_t> &image);
	ImTextureRef CreateTexture(const lak::image<lak::vec3f_t> &image);
	ImTextureRef CreateTexture(const lak::image<float> &image);
	void DestroyTexture(ImTextureRef &tex);
	void QueueDestroyTexture(ImTextureRef &tex); // queue for destruction at the
	                                             // end of the current frame
	lak::vec2s_t TextureSize(ImTextureRef tex);

	using ImViewport             = ImGui::ImplViewport;
	using ImViewportDetails      = ImGui::ImplViewportDetails;
	using ImSRViewportDetails    = ImGui::ImplSRViewportDetails;
	using ImGLViewportDetails    = ImGui::ImplGLViewportDetails;
	using ImCoViewportDetails    = ImGui::ImplCoViewportDetails;
	using ImTextureColourFormat  = ImGui::ImplTextureColourFormat;
	using ImTextureChannelFormat = ImGui::ImplTextureChannelFormat;

	ImViewport CreateViewport(lak::ImTextureColourFormat colour,
	                          lak::ImTextureChannelFormat channel);
	void DestroyViewport(ImViewport viewport);
	ImViewportDetails BeginViewport(
	  ImViewport viewport,
	  const ImVec2 &size,
	  bool *clicked          = nullptr,
	  ImGuiButtonFlags flags = ImGuiButtonFlags_None);
	void EndViewport(ImViewport viewport);

	template<>
	struct unique_com_ptr_traits<ImViewport>
	{
		using handle_type                = ImViewport;
		using exposed_type               = ImViewport;
		static constexpr auto null_value = nullptr;
		inline static lak::infallible_result<ImViewport> ctor(
		  lak::ImTextureColourFormat colour, lak::ImTextureChannelFormat channel)
		{
			return lak::ok_t{lak::CreateViewport(colour, channel)};
		}
		inline static void dtor(ImViewport &viewport)
		{
			lak::DestroyViewport(viewport);
			viewport = nullptr;
		}
		inline static bool valid(ImViewport viewport)
		{
			return viewport != nullptr;
		}
	};
	using ImUniqueViewport = lak::unique_com_ptr<ImViewport>;

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

	template<lak::u8const_string FMT, typename... ARGS>
	bool TreeNode(ARGS &&...args);

	struct tree_node
	{
	private:
		bool _result;
		tree_node(bool b) : _result(b) { ImGui::Separator(); }

	public:
		tree_node(tree_node &&o) : _result(lak::exchange(o._result, false)) {}

		template<typename... ARGS>
		tree_node(const char *fmt, ARGS &&...args)
		: _result(lak::TreeNode(fmt, lak::forward<ARGS>(args)...))
		{
			ImGui::Separator();
		}

		template<lak::u8const_string FMT, typename... ARGS>
		static tree_node make(ARGS &&...args)
		{
			return {lak::TreeNode<FMT>(lak::forward<ARGS>(args)...)};
		}

		inline ~tree_node()
		{
			if (_result)
			{
				ImGui::Separator();
				ImGui::TreePop();
			}
		}

		explicit inline operator bool() const { return _result; }
	};

#define LAK_TREE_NODE(...)                                                    \
	if (lak::tree_node UNIQUIFY(TREE_NODE_)(__VA_ARGS__); UNIQUIFY(TREE_NODE_))

#define LAK_TREE_NODE2(FMT, ...)                                              \
	if (lak::tree_node UNIQUIFY(TREE_NODE_){                                    \
	      lak::tree_node::make<u8"" FMT>(__VA_ARGS__)};                         \
	    UNIQUIFY(TREE_NODE_))

	enum struct file_open_error
	{
		INCOMPLETE,
		INVALID,
		CANCELLED,
		VALID
	};

	void init_file_modal();

	lak::error_code_result<lak::file_open_error> open_file_modal(
	  std::filesystem::path &path, bool save, const lak::astring &filter = ".*");

	lak::error_code_result<lak::file_open_error> open_folder_modal(
	  std::filesystem::path &path);

	struct path_getter
	{
		lak::optional<std::filesystem::path> _path;
		lak::astring _filter;
		bool _save;
		bool _file;

		inline void open_file(std::filesystem::path path = {},
		                      lak::astring filter        = ".*")
		{
			_path   = lak::move(path);
			_filter = lak::move(filter);
			_save   = false;
			_file   = true;
		}
		inline void save_file(std::filesystem::path path = {},
		                      lak::astring filter        = ".*")
		{
			_path   = lak::move(path);
			_filter = lak::move(filter);
			_save   = true;
			_file   = true;
		}
		inline void folder(std::filesystem::path path = {})
		{
			_path = lak::move(path);
			_file = false;
		}

		lak::optional<std::filesystem::path> operator()();
	};
}

#include "widgets.inl"

#endif
