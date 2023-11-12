#define IMGUI_DEFINE_MATH_OPERATORS
#include "lak/imgui/widgets.hpp"

#include "lak/opengl/gl3w.hpp"
#include "lak/opengl/state.hpp"

#include "lak/debug.hpp"

#include "lak/file.hpp"

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

#ifdef LAK_USE_NATIVE_FILE_DIALOG
#	include "lak/os.hpp"
#	if defined(LAK_OS_WINDOWS)
#		define LAK_USE_WIN_NATIVE_FILE_DIALOG
#	else
#		define LAK_USE_IMFILEDIALOG
#	endif
#else
#	define LAK_USE_IMFILEDIALOG
#endif

#ifdef LAK_USE_IMFILEDIALOG
#	include <ImFileDialog.h>

lak::optional<ifd::FileDialog> _file_dialog;

lak::array<void *> _textures_to_destroy;
void (*_texture_destroyer)(void *) = nullptr;

void lak::init_file_modal(lak::graphics_mode graphics)
{
	lak::flush_file_modal();

	if (!_file_dialog) _file_dialog.emplace(ifd::FileDialog{});

	_file_dialog->DeleteTexture = [](void *tex)
	{ _textures_to_destroy.push_back(tex); };

#	ifdef LAK_ENABLE_SOFTRENDER
	if (graphics == lak::graphics_mode::Software)
	{
		_file_dialog->CreateTexture =
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
#	endif
#	ifdef LAK_ENABLE_OPENGL
	  if (graphics == lak::graphics_mode::OpenGL)
	{
		_file_dialog->CreateTexture =
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
#	endif
	{
		ASSERT_UNREACHABLE();
	}
}

void lak::flush_file_modal()
{
	if (_textures_to_destroy.empty()) return;

	for (auto tex : _textures_to_destroy) _texture_destroyer(tex);
	_textures_to_destroy.clear();
}
#endif

#ifdef LAK_USE_WIN_NATIVE_FILE_DIALOG
#	include "../win32/wrapper.hpp"

void lak::init_file_modal(lak::graphics_mode) {}
void lak::flush_file_modal() {}

lak::error_code_result<lak::file_open_error> open_file_dialog(
  std::filesystem::path &path,
  bool folder,
  bool save,
  const lak::astring &filter)
{
	RES_TRY_COM(CoInitializeEx(
	  nullptr, COINIT_APARTMENTTHREADED | COINIT_DISABLE_OLE1DDE));
	DEFER(CoUninitialize());

	IFileDialog *fd;
	if (folder || !save)
	{
		IFileOpenDialog *fod;
		RES_TRY_COM(CoCreateInstance(CLSID_FileOpenDialog,
		                             nullptr,
		                             CLSCTX_ALL,
		                             IID_IFileOpenDialog,
		                             reinterpret_cast<void **>(&fod)));
		fd = fod;
	}
	else
	{
		IFileSaveDialog *fsd;
		RES_TRY_COM(CoCreateInstance(CLSID_FileSaveDialog,
		                             nullptr,
		                             CLSCTX_ALL,
		                             IID_IFileSaveDialog,
		                             reinterpret_cast<void **>(&fsd)));
		fd = fsd;
	}
	DEFER(fd->Release());

	FILEOPENDIALOGOPTIONS opts = FOS_NOCHANGEDIR | FOS_DONTADDTORECENT;
	if (folder) opts |= FOS_PICKFOLDERS;
	RES_TRY_COM(fd->SetOptions(opts));

	if_let_ok (auto res, lak::deepest_folder(path))
	{
		auto p = res.folder.wstring();
		IShellItem *psi;
		RES_TRY_COM(::SHCreateItemFromParsingName(
		  p.c_str(), nullptr, IID_IShellItem, reinterpret_cast<void **>(&psi)));
		RES_TRY_COM(fd->SetDefaultFolder(psi));
		RES_TRY_COM(fd->SetFileName(res.file.wstring().c_str()));
	}

	auto parsed_filter =
	  [&filter]() -> lak::pair<lak::array<COMDLG_FILTERSPEC>,
	                           lak::array<lak::array<wchar_t>>>
	{
		lak::array<lak::array<wchar_t>> strings;
		lak::array<COMDLG_FILTERSPEC> result;

		auto add_string = [&strings](const std::string &str) -> LPCWSTR
		{
			auto wstr = lak::to_wstring(str);
			auto &arr = strings.emplace_back();
			arr.resize(wstr.size() + 1U);
			lak::copy(wstr.begin(), wstr.end(), arr.begin(), arr.end());
			arr.back() = 0U;
			return arr.data();
		};

		if (filter.empty()) return {};

		size_t lastSplit = 0, lastExt = 0;
		bool inExtList     = false;
		LPCWSTR filterName = nullptr;
		lak::astring filterExtension;

		for (size_t i = 0; i < filter.size(); i++)
		{
			if (filter[i] == '{')
			{
				filterName = add_string(filter.substr(lastSplit, i - lastSplit));

				inExtList = true;
				lastExt   = i + 1;
			}
			else if (filter[i] == '}')
			{
				ASSERT(!!filterName);

				if (!filterExtension.empty()) filterExtension += ";";
				filterExtension += "*" + filter.substr(lastExt, i - lastExt);

				result.push_back(COMDLG_FILTERSPEC{
				  .pszName = filterName,
				  .pszSpec = add_string(filterExtension),
				});

				filterName = nullptr;
				filterExtension.clear();

				inExtList = false;
			}
			else if (filter[i] == ',')
			{
				if (!inExtList)
				{
					// file category separator
					lastSplit = i + 1;
				}
				else
				{
					// file extension separator
					if (!filterExtension.empty()) filterExtension += ";";
					filterExtension += "*" + filter.substr(lastExt, i - lastExt);
					lastExt = i + 1;
				}
			}
		}

		if (lastSplit != 0)
		{
			if (std::string str = filter.substr(lastSplit); str == ".*")
			{
				result.push_back(COMDLG_FILTERSPEC{
				  .pszName = L"All Files",
				  .pszSpec = L"*.*",
				});
			}
			else
				FATAL("Filter Missing Extension List");
		}

		return {lak::move(result), lak::move(strings)};
	}();

	if (!parsed_filter.first.empty())
	{
		DEBUG_EXPR(parsed_filter.first.size());
		for (const auto [name, ext] : parsed_filter.first)
		{
			DEBUG_EXPR(name, ext);
		}
		RES_TRY_COM(fd->SetFileTypes(static_cast<UINT>(parsed_filter.first.size()),
		                             parsed_filter.first.data()));
	}

	if (HRESULT hr = fd->Show(nullptr); FAILED(hr))
	{
		if (hr == HRESULT_FROM_WIN32(ERROR_CANCELLED))
			return lak::ok_t{lak::file_open_error::CANCELED};
		else
			return lak::err_t{lak::winapi::make_com_error(hr)};
	}

	IShellItem *files;
	RES_TRY_COM(fd->GetResult(&files));
	DEFER(files->Release());

	PWSTR pwstr;
	RES_TRY_COM(files->GetDisplayName(SIGDN_FILESYSPATH, &pwstr));
	DEFER(CoTaskMemFree(pwstr));

	path = pwstr;
	return lak::ok_t{lak::file_open_error::VALID};
}
#endif

lak::error_code_result<lak::file_open_error> lak::open_file_modal(
  std::filesystem::path &path, bool save, const lak::astring &filter)
{
#ifdef LAK_USE_WIN_NATIVE_FILE_DIALOG
	return open_file_dialog(path, false, save, filter);
#endif

#ifdef LAK_USE_IMFILEDIALOG
	const std::string name = save ? "Save File" : "Open File";

	if (!ImGui::IsPopupOpen((name + "###" + name).c_str()))
	{
		if (save)
		{
			if (!_file_dialog->Save(name, name, filter, path.string()))
				ASSERT_UNREACHABLE();
		}
		else
		{
			if (!_file_dialog->Open(name, name, filter, false, path.string()))
				ASSERT_UNREACHABLE();
		}
	}

	if (_file_dialog->IsDone(name))
	{
		const bool has_result = _file_dialog->HasResult();
		if (has_result) path = _file_dialog->GetResult();
		_file_dialog->Close();
		return lak::ok_t{has_result ? lak::file_open_error::VALID
		                            : lak::file_open_error::CANCELED};
	}

	return lak::ok_t{lak::file_open_error::INCOMPLETE};
#endif
}

lak::error_code_result<lak::file_open_error> lak::open_folder_modal(
  std::filesystem::path &path)
{
#ifdef LAK_USE_WIN_NATIVE_FILE_DIALOG
	return open_file_dialog(path, true, false, {});
#endif

#ifdef LAK_USE_IMFILEDIALOG
	const std::string name = "Open Folder";

	if (!ImGui::IsPopupOpen((name + "###" + name).c_str()))
		if (!_file_dialog->Open(name, name, "", false, path.string()))
			ASSERT_UNREACHABLE();

	if (_file_dialog->IsDone(name))
	{
		const bool has_result = _file_dialog->HasResult();
		if (has_result) path = _file_dialog->GetResult();
		_file_dialog->Close();
		return lak::ok_t{has_result ? lak::file_open_error::VALID
		                            : lak::file_open_error::CANCELED};
	}

	return lak::ok_t{lak::file_open_error::INCOMPLETE};
#endif
}
