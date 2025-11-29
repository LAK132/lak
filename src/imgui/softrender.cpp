#include "lak/imgui/softrender.hpp"

#include "lak/debug.hpp"

texture_base_t *Screen = nullptr;

bool ImGui_ImplSoftrender_Init(texture_base_t *screen)
{
	if (screen != nullptr)
	{
		Screen = screen;
		return true;
	}
	return false;
}

void ImGui_ImplSoftrender_Shutdown() { Screen = nullptr; }

void ImGui_ImplSoftrender_NewFrame()
{
	if (Screen == nullptr) return;

	ImGuiIO &io      = ImGui::GetIO();
	io.DisplaySize.x = static_cast<float>(Screen->w);
	io.DisplaySize.y = static_cast<float>(Screen->h);
}

void ImGui_ImplSoftrender_RenderDrawData(ImDrawData *draw_data)
{
	if (Screen == nullptr) return;

	Screen->clear();

	// using pos_t = float;
	using pos_t = int32_t;

	if (draw_data->Textures != nullptr)
		for (ImTextureData *tex : *draw_data->Textures)
			if (tex->Status != ImTextureStatus_OK)
				ImGui_ImplSoftrender_UpdateTexture(tex);

	switch (Screen->type)
	{
		case texture_type_t::ALPHA8:
			renderDrawLists<pos_t>(draw_data,
			                       *reinterpret_cast<texture_alpha8_t *>(Screen));
			break;

		case texture_type_t::VALUE8:
			renderDrawLists<pos_t>(draw_data,
			                       *reinterpret_cast<texture_value8_t *>(Screen));
			break;

		case texture_type_t::COLOR16:
			renderDrawLists<pos_t>(draw_data,
			                       *reinterpret_cast<texture_color16_t *>(Screen));
			break;

		case texture_type_t::COLOR24:
			renderDrawLists<pos_t>(draw_data,
			                       *reinterpret_cast<texture_color24_t *>(Screen));
			break;

		case texture_type_t::COLOR32:
			renderDrawLists<pos_t>(draw_data,
			                       *reinterpret_cast<texture_color32_t *>(Screen));
			break;

		default:
			return;
	}
}

void ImGui_ImplSoftrender_DestroyTexture(ImTextureData *texture)
{
	switch (texture->Format)
	{
		case ImTextureFormat_RGBA32:
			delete (texture_color32_t *)(uintptr_t)texture->TexID;
			break;
		case ImTextureFormat_Alpha8:
			delete (texture_alpha8_t *)(uintptr_t)texture->TexID;
			break;
		default:
			ASSERT_UNREACHABLE();
	}

	texture->SetStatus(ImTextureStatus_Destroyed);
}

template<typename COLOR>
void _ImGui_ImplSoftrender_UpdateTexture(ImTextureData *texture)
{
	if (texture->Status == ImTextureStatus_WantCreate)
	{
		auto tex = new texture_t<COLOR>;
		tex->copy(texture->Width, texture->Height, (COLOR *)texture->GetPixels());
		texture->SetTexID((ImTextureID)(uintptr_t)(texture_base_t *)tex);
		texture->SetStatus(ImTextureStatus_OK);
	}
	else if (texture->Status == ImTextureStatus_WantUpdates)
	{
		auto old          = (texture_t<COLOR> *)(uintptr_t)texture->TexID;
		size_t new_width  = old->w;
		size_t new_height = old->h;

		for (ImTextureRect &r : texture->Updates)
		{
			new_width  = std::max<size_t>(new_width, r.x + r.w);
			new_height = std::max<size_t>(new_height, r.y + r.h);
		}

		auto tex = new texture_color32_t;
		tex->init(new_width, new_height);

		for (size_t y = 0U; y < old->h; ++y)
			memcpy(((color32_t *)(tex->pixels)) + (y * tex->w),
			       ((const color32_t *)(old->pixels)) + (y * old->w),
			       old->w * sizeof(color32_t));

		for (ImTextureRect &r : texture->Updates)
			for (size_t y = r.y; y < r.y + r.h; ++y)
				for (size_t x = r.x; x < r.x + r.w; ++x)
					tex->at(x, y) = *(const color32_t *)texture->GetPixelsAt(x, y);

		texture->SetTexID((ImTextureID)(uintptr_t)(texture_base_t *)tex);
		delete old;
		texture->SetStatus(ImTextureStatus_OK);
	}
	else if (texture->Status == ImTextureStatus_WantDestroy &&
	         texture->UnusedFrames > 0)
	{
		ImGui_ImplSoftrender_DestroyTexture(texture);
	}
}

void ImGui_ImplSoftrender_UpdateTexture(ImTextureData *texture)
{
	switch (texture->Format)
	{
		case ImTextureFormat_RGBA32:
			_ImGui_ImplSoftrender_UpdateTexture<color32_t>(texture);
			break;
		case ImTextureFormat_Alpha8:
			_ImGui_ImplSoftrender_UpdateTexture<alpha8_t>(texture);
			break;
		default:
			ASSERT_UNREACHABLE();
	}
}
