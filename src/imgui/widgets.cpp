#define IMGUI_DEFINE_MATH_OPERATORS
#include "lak/imgui/widgets.hpp"

#include "lak/opengl/gl3w.hpp"
#include "lak/opengl/state.hpp"

#include "lak/debug.hpp"

#include <imgui_internal.h>
#include <texture.h>

/* --- VertSplitter --- */

bool lak::VertSplitter(float &left,
                       float &right,
                       float width,
                       float leftMin,
                       float rightMin,
                       float length)
{
	const float thickness = ImGui::GetStyle().FramePadding.x * 2;
	const float maxLeft   = width - (rightMin + thickness);
	if (left > maxLeft) left = maxLeft;
	right = width - (left + thickness);

	ImGuiID id    = ImGui::GetID("##Splitter");
	ImVec2 cursor = ImGui::GetCursorScreenPos();
	ImRect bb;

	bb.Min.x = cursor.x + left;
	bb.Min.y = cursor.y;

	bb.Max = ImGui::CalcItemSize(ImVec2(thickness, length), 0.0f, 0.0f);
	bb.Max.x += bb.Min.x;
	bb.Max.y += bb.Min.y;

	return ImGui::SplitterBehavior(
	  bb, id, ImGuiAxis_X, &left, &right, leftMin, rightMin);
}

/* --- HoriSplitter --- */

bool lak::HoriSplitter(float &top,
                       float &bottom,
                       float height,
                       float topMin,
                       float bottomMin,
                       float length)
{
	const float thickness = ImGui::GetStyle().FramePadding.x * 2;
	const float maxTop    = height - (bottomMin + thickness);
	if (top > maxTop) top = maxTop;
	bottom = height - (top + thickness);

	ImGuiID id    = ImGui::GetID("##Splitter");
	ImVec2 cursor = ImGui::GetCursorScreenPos();
	ImRect bb;

	bb.Min.x = cursor.x;
	bb.Min.y = cursor.y + top;

	bb.Max = ImGui::CalcItemSize(ImVec2(length, thickness), 0.0f, 0.0f);
	bb.Max.x += bb.Min.x;
	bb.Max.y += bb.Min.y;

	return ImGui::SplitterBehavior(
	  bb, id, ImGuiAxis_Y, &top, &bottom, topMin, bottomMin);
}

/* --- vert_split_child --- */

inline lak::vert_split_child::vert_split_child()
{
	const auto content_size{ImGui::GetContentRegionAvail()};
	left_size  = content_size.x / 2;
	right_size = content_size.x / 2;
}

inline bool lak::vert_split_child::begin(const char *id, bool border)
{
	ImGui::PushID(id);
	float min_size = ImGui::GetStyle().WindowPadding.x * 4;
	lak::VertSplitter(left_size,
	                  right_size,
	                  ImGui::GetContentRegionAvail().x,
	                  min_size,
	                  min_size);
	return ImGui::BeginChild(
	  "##left", {left_size, 0}, border, ImGuiWindowFlags_NoSavedSettings);
}

inline bool lak::vert_split_child::split(bool border) const
{
	ImGui::EndChild();
	ImGui::SameLine();
	return ImGui::BeginChild(
	  "##right", {right_size, 0}, border, ImGuiWindowFlags_NoSavedSettings);
}

inline void lak::vert_split_child::end() const
{
	ImGui::EndChild();
	ImGui::PopID();
}

/* --- hori_split_child --- */

inline lak::hori_split_child::hori_split_child()
{
	const auto content_size{ImGui::GetContentRegionAvail()};
	top_size    = content_size.y / 2;
	bottom_size = content_size.y / 2;
}

inline bool lak::hori_split_child::begin(const char *id, bool border)
{
	ImGui::PushID(id);
	lak::HoriSplitter(top_size, bottom_size, ImGui::GetContentRegionAvail().y);
	return ImGui::BeginChild(
	  "##top", {0, top_size}, border, ImGuiWindowFlags_NoSavedSettings);
}

inline bool lak::hori_split_child::split(bool border) const
{
	ImGui::EndChild();
	ImGui::Spacing();
	return ImGui::BeginChild(
	  "##bottom", {0, bottom_size}, border, ImGuiWindowFlags_NoSavedSettings);
}

inline void lak::hori_split_child::end() const
{
	ImGui::EndChild();
	ImGui::PopID();
}

/* --- TreeNode --- */

bool lak::TreeNode(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);

	ImGuiWindow *window = ImGui::GetCurrentWindow();
	if (window->SkipItems) return false;

	ImGuiContext &g = *GImGui;

	ImFormatStringV(g.TempBuffer.begin(), g.TempBuffer.size(), fmt, args);

	bool is_open = ImGui::TreeNodeBehavior(
	  window->GetID(g.TempBuffer.begin()), 0, g.TempBuffer.begin());

	va_end(args);
	return is_open;
}

/* --- Open* --- */

lak::optional<ifd::FileDialog> lak::file_dialog;

lak::array<void *> _textures_to_destroy;
void (*_texture_destroyer)(void *) = nullptr;

void lak::ConfigureFileDialog(lak::graphics_mode graphics)
{
	lak::FlushFileDialogTextures();

	if (!lak::file_dialog) lak::file_dialog.emplace(ifd::FileDialog{});

	lak::file_dialog->DeleteTexture = [](void *tex)
	{ _textures_to_destroy.push_back(tex); };

#ifdef LAK_ENABLE_SOFTRENDER
	if (graphics == lak::graphics_mode::Software)
	{
		lak::file_dialog->CreateTexture =
		  [](uint8_t *data, int w, int h, char fmt) -> void *
		{
			texture_color32_t *result = new texture_color32_t{};
			result->init(w, h);
			if (fmt == 0)
			{
				// BGRA8888
				color32_t *pixels = (color32_t *)result->pixels;
				for (size_t i = size_t(w * h); i-- > 0; ++pixels)
				{
					pixels->b = *(data++);
					pixels->g = *(data++);
					pixels->r = *(data++);
					pixels->a = *(data++);
				}
			}
			else
			{
				// RGBA8888
				color32_t *pixels = (color32_t *)result->pixels;
				for (size_t i = size_t(w * h); i-- > 0; ++pixels)
				{
					pixels->r = *(data++);
					pixels->g = *(data++);
					pixels->b = *(data++);
					pixels->a = *(data++);
				}
			}
			return (void *)result;
		};

		_texture_destroyer = [](void *tex) { delete (texture_color32_t *)tex; };
	}
	else
#endif
#ifdef LAK_ENABLE_OPENGL
	  if (graphics == lak::graphics_mode::OpenGL)
	{
		lak::file_dialog->CreateTexture =
		  [](uint8_t *data, int w, int h, char fmt) -> void *
		{
			GLuint tex;

			lak::opengl::call_checked(glGenTextures, 1, &tex).UNWRAP();
			lak::opengl::call_checked(glBindTexture, GL_TEXTURE_2D, tex).UNWRAP();
			lak::opengl::call_checked(
			  glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
			  .UNWRAP();
			lak::opengl::call_checked(
			  glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)
			  .UNWRAP();
			lak::opengl::call_checked(
			  glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE)
			  .UNWRAP();
			lak::opengl::call_checked(
			  glTexParameteri, GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE)
			  .UNWRAP();
			lak::opengl::call_checked(glTexImage2D,
			                          GL_TEXTURE_2D,
			                          0,
			                          GL_RGBA,
			                          w,
			                          h,
			                          0,
			                          (fmt == 0) ? GL_BGRA : GL_RGBA,
			                          GL_UNSIGNED_BYTE,
			                          data)
			  .UNWRAP();
			lak::opengl::call_checked(glGenerateMipmap, GL_TEXTURE_2D).UNWRAP();
			lak::opengl::call_checked(glBindTexture, GL_TEXTURE_2D, 0).UNWRAP();

			return (void *)tex;
		};

		_texture_destroyer = [](void *tex)
		{
			GLuint texID = (GLuint)((uintptr_t)tex);
			lak::opengl::call_checked(glDeleteTextures, 1, &texID).UNWRAP();
		};
	}
	else
#endif
	{
		ASSERT_UNREACHABLE();
	}
}

void lak::FlushFileDialogTextures()
{
	if (_textures_to_destroy.empty()) return;

	for (auto tex : _textures_to_destroy) _texture_destroyer(tex);
	_textures_to_destroy.clear();
}
