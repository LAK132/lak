#include "backend.inl"

#include "lak/imgui/softrender.hpp"

#if !defined(LAK_SOFTWARE_RENDER_32BIT) &&                                    \
  !defined(LAK_SOFTWARE_RENDER_24BIT) &&                                      \
  !defined(LAK_SOFTWARE_RENDER_16BIT) && !defined(LAK_SOFTWARE_RENDER_8BIT)
#	if defined(LAK_OS_APPLE) && defined(LAK_ARCH_ARM64)
#		define LAK_SOFTWARE_RENDER_32BIT
#	else
#		define LAK_SOFTWARE_RENDER_16BIT
#	endif
#endif

namespace ImGui
{
	struct _ImplSRContext
	{
#if defined(LAK_USE_WINAPI)
		//     // HBITMAP bitmap_handle = NULL;
		// #  if defined(LAK_SOFTWARE_RENDER_32BIT)
		//     using screen_format_t = lak::colour::abgr8888;
		// #  elif defined(LAK_SOFTWARE_RENDER_24BIT)
		//     using screen_format_t = lak::colour::bgr888;
		// #  elif defined(LAK_SOFTWARE_RENDER_16BIT)
		//     using screen_format_t = lak::colour::bgr565;
		// #  else
		// #    error "No software render colour bit depth specified"
		// #  endif

		decltype(lak::software_context::platform_handle) *screen_surface = nullptr;
#elif defined(LAK_USE_XLIB)
#	error "NYI"
#elif defined(LAK_USE_XCB)
#	error "NYI"
#elif defined(LAK_USE_SDL2)
		SDL_Window *window;
		SDL_Surface *screen_surface;
#	if defined(LAK_SOFTWARE_RENDER_32BIT)
		static const Uint32 screen_format = SDL_PIXELFORMAT_ABGR8888;
#	elif defined(LAK_SOFTWARE_RENDER_24BIT)
		static const Uint32 screen_format = SDL_PIXELFORMAT_RGB24;
#	elif defined(LAK_SOFTWARE_RENDER_16BIT)
		static const Uint32 screen_format = SDL_PIXELFORMAT_RGB565;
#	elif defined(LAK_SOFTWARE_RENDER_8BIT)
		SDL_Palette *palette;
		static const Uint32 screen_format = SDL_PIXELFORMAT_INDEX8;
#	else
#		error "No software render colour bit depth specified"
#	endif
#elif defined(LAK_USE_SDL3)
		SDL_Window *window;
		SDL_Surface *screen_surface;
#	if defined(LAK_SOFTWARE_RENDER_32BIT)
		static const SDL_PixelFormat screen_format = SDL_PIXELFORMAT_ABGR8888;
#	elif defined(LAK_SOFTWARE_RENDER_24BIT)
		static const SDL_PixelFormat screen_format = SDL_PIXELFORMAT_RGB24;
#	elif defined(LAK_SOFTWARE_RENDER_16BIT)
		static const SDL_PixelFormat screen_format = SDL_PIXELFORMAT_RGB565;
#	elif defined(LAK_SOFTWARE_RENDER_8BIT)
		SDL_Palette *palette;
		static const SDL_PixelFormat screen_format = SDL_PIXELFORMAT_INDEX8;
#	else
#		error "No software render colour bit depth specified"
#	endif
#else
#	error "No implementation specified"
#endif

#if defined(LAK_SOFTWARE_RENDER_32BIT)
		texture_color32_t screen_texture;
#elif defined(LAK_SOFTWARE_RENDER_24BIT)
		texture_color24_t screen_texture;
#elif defined(LAK_SOFTWARE_RENDER_16BIT)
		texture_color16_t screen_texture;
#elif defined(LAK_SOFTWARE_RENDER_8BIT)
		texture_value8_t screen_texture;
#else
#	error "No software render colour bit depth specified"
#endif
	};

	struct _ImplSRViewport
	{
	};
}

inline void ImplUpdateDisplaySize(ImGui::ImplSRContext context,
                                  const lak::window_handle *,
                                  lak::vec2l_t window_size)
{
	ImGuiIO &io                  = ImGui::GetIO();
	io.DisplayFramebufferScale.x = 1.0f;
	io.DisplayFramebufferScale.y = 1.0f;
	if ((size_t)window_size.x != context->screen_texture.w ||
	    (size_t)window_size.y != context->screen_texture.h)
	{
		context->screen_texture.init(window_size.x, window_size.y);

#if defined(LAK_USE_WINAPI)
		// context->screen_surface.resize(lak::vec2s_t(window_size));
#elif defined(LAK_USE_XLIB)
#	error "NYI"
#elif defined(LAK_USE_XCB)
#	error "NYI"
#elif defined(LAK_USE_SDL2)
		if (context->screen_surface != nullptr)
			SDL_FreeSurface(context->screen_surface);

		context->screen_surface = SDL_CreateRGBSurfaceWithFormatFrom(
		  context->screen_texture.pixels,
		  static_cast<int>(context->screen_texture.w),
		  static_cast<int>(context->screen_texture.h),
		  static_cast<int>(context->screen_texture.size * 8),
		  static_cast<int>(context->screen_texture.w *
		                   context->screen_texture.size),
		  context->screen_format);

#	ifdef LAK_SOFTWARE_RENDER_8BIT
		SDL_SetSurfacePalette(context->screen_surface, context->palette);
#	endif
#elif defined(LAK_USE_SDL3)
		if (context->screen_surface != nullptr)
			SDL_DestroySurface(context->screen_surface);

		context->screen_surface =
		  SDL_CreateSurfaceFrom(static_cast<int>(context->screen_texture.w),
		                        static_cast<int>(context->screen_texture.h),
		                        context->screen_format,
		                        context->screen_texture.pixels,
		                        static_cast<int>(context->screen_texture.w *
		                                         context->screen_texture.size));

#	ifdef LAK_SOFTWARE_RENDER_8BIT
		SDL_SetSurfacePalette(context->screen_surface, context->palette);
#	endif
#else
#	error "No implementation specified"
#endif
	}
}

void ImplInitSRContext(ImGui::ImplSRContext context, const lak::window &window)
{
	ImGuiIO &io = ImGui::GetIO();

	io.BackendRendererName = "imgui_impl_lak_softrender";

	io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;

#if defined(LAK_USE_WINAPI)
	context->screen_surface =
	  &window.handle()->software_context().platform_handle;
#elif defined(LAK_USE_XLIB)
#	error "NYI"
#elif defined(LAK_USE_XCB)
#	error "NYI"
#elif defined(LAK_USE_SDL2)
	context->window = window.handle()->sdl_window;

#	ifdef LAK_SOFTWARE_RENDER_8BIT
	context->palette = SDL_AllocPalette(256);
	SDL_Colour palette[256];
	for (size_t i = 0; i < 256; ++i)
	{
		palette[i].r = uint8_t(i);
		palette[i].g = uint8_t(i);
		palette[i].b = uint8_t(i);
		palette[i].a = uint8_t(255);
	}
	SDL_SetPaletteColors(context->palette, palette, 0, 256);
#	endif
#elif defined(LAK_USE_SDL3)
	context->window = window.handle()->sdl_window;

#	ifdef LAK_SOFTWARE_RENDER_8BIT
	context->palette = SDL_CreatePalette(256);
	SDL_Colour palette[256];
	for (size_t i = 0; i < 256; ++i)
	{
		palette[i].r = uint8_t(i);
		palette[i].g = uint8_t(i);
		palette[i].b = uint8_t(i);
		palette[i].a = uint8_t(255);
	}
	SDL_SetPaletteColors(context->palette, palette, 0, 256);
#	endif
#else
#	error "No implementation specified"
#endif

	ImplUpdateDisplaySize(context, window.handle(), window.size());

	ImGui_ImplSoftrender_Init(&context->screen_texture);
}

void ImplShutdownSRContext(ImGui::ImplSRContext context)
{
	ImGui_ImplSoftrender_Shutdown();

#if defined(LAK_USE_WINAPI)
	context->screen_surface = nullptr;
#elif defined(LAK_USE_XLIB)
#	error "NYI"
#elif defined(LAK_USE_XCB)
#	error "NYI"
#elif defined(LAK_USE_SDL2)
	context->window = nullptr;

	SDL_FreeSurface(context->screen_surface);
	context->screen_surface = nullptr;

#	ifdef LAK_SOFTWARE_RENDER_8BIT
	SDL_FreePalette(context->palette);
	context->palette = nullptr;
#	endif
#elif defined(LAK_USE_SDL3)
	context->window = nullptr;

	SDL_DestroySurface(context->screen_surface);
	context->screen_surface = nullptr;

#	ifdef LAK_SOFTWARE_RENDER_8BIT
	SDL_DestroyPalette(context->palette);
	context->palette = nullptr;
#	endif
#else
#	error "No implementation specified"
#endif

	context->screen_texture.init(0, 0);
}

ImTextureID ImplSRCreateTexture(ImGui::ImplContext,
                                const void *pixels,
                                lak::vec2s_t size,
                                ImGui::ImplTextureColourFormat colour,
                                ImGui::ImplTextureChannelFormat channel)
{
	ASSERT(channel == ImGui::ImplTextureChannelFormat::U8);

	auto compressor = lak::overloaded{
	  [](lak::span<const uint16_t> val) -> lak::span<uint8_t>
	  {
		  auto result =
		    lak::span<uint8_t>((uint8_t *)malloc(val.size()), val.size());
		  for (size_t i = 0U; i < result.size(); ++i) result[i] = val[i] >> 8U;
		  return result;
	  },
	  [](lak::span<const float> val) -> lak::span<uint8_t>
	  {
		  auto result =
		    lak::span<uint8_t>((uint8_t *)malloc(val.size()), val.size());
		  for (size_t i = 0U; i < result.size(); ++i)
			  result[i] = uint8_t(std::min<uint64_t>(uint64_t(val[i] * 256), 255));
		  return result;
	  }};

	texture_base_t *tex;
	switch (colour)
	{
		case ImGui::ImplTextureColourFormat::RGBA:
		{
			auto t = new texture_color32_t;
			switch (channel)
			{
				case ImGui::ImplTextureChannelFormat::U8:
					t->copy(size.x, size.y, (const color32_t *)pixels);
					break;
				case ImGui::ImplTextureChannelFormat::U16:
					t->init(size.x,
					        size.y,
					        (color32_t *)compressor(
					          lak::span<const uint16_t>(lak::span<const void>(
					            pixels, size.x * size.y * sizeof(uint16_t) * 4U)))
					          .data());
					break;
				case ImGui::ImplTextureChannelFormat::F32:
					t->init(size.x,
					        size.y,
					        (color32_t *)compressor(
					          lak::span<const float>(lak::span<const void>(
					            pixels, size.x * size.y * sizeof(float) * 4U)))
					          .data());
					break;
			}
			tex = t;
		}
		break;
		case ImGui::ImplTextureColourFormat::RGB:
		{
			auto t = new texture_color24_t;
			switch (channel)
			{
				case ImGui::ImplTextureChannelFormat::U8:
					t->copy(size.x, size.y, (const color24_t *)pixels);
					break;
				case ImGui::ImplTextureChannelFormat::U16:
					t->init(size.x,
					        size.y,
					        (color24_t *)compressor(
					          lak::span<const uint16_t>(lak::span<const void>(
					            pixels, size.x * size.y * sizeof(uint16_t) * 3U)))
					          .data());
					break;
				case ImGui::ImplTextureChannelFormat::F32:
					t->init(size.x,
					        size.y,
					        (color24_t *)compressor(
					          lak::span<const float>(lak::span<const void>(
					            pixels, size.x * size.y * sizeof(float) * 3U)))
					          .data());
					break;
			}
			tex = t;
		}
		break;
		case ImGui::ImplTextureColourFormat::R:
		{
			auto t = new texture_value8_t;
			switch (channel)
			{
				case ImGui::ImplTextureChannelFormat::U8:
					t->copy(size.x, size.y, (const value8_t *)pixels);
					break;
				case ImGui::ImplTextureChannelFormat::U16:
					t->init(size.x,
					        size.y,
					        (value8_t *)compressor(
					          lak::span<const uint16_t>(lak::span<const void>(
					            pixels, size.x * size.y * sizeof(uint16_t) * 1U)))
					          .data());
					break;
				case ImGui::ImplTextureChannelFormat::F32:
					t->init(size.x,
					        size.y,
					        (value8_t *)compressor(
					          lak::span<const float>(lak::span<const void>(
					            pixels, size.x * size.y * sizeof(float) * 1U)))
					          .data());
					break;
			}
			tex = t;
		}
		break;
		case ImGui::ImplTextureColourFormat::A:
		{
			auto t = new texture_alpha8_t;
			switch (channel)
			{
				case ImGui::ImplTextureChannelFormat::U8:
					t->copy(size.x, size.y, (const alpha8_t *)pixels);
					break;
				case ImGui::ImplTextureChannelFormat::U16:
					t->init(size.x,
					        size.y,
					        (alpha8_t *)compressor(
					          lak::span<const uint16_t>(lak::span<const void>(
					            pixels, size.x * size.y * sizeof(uint16_t) * 1U)))
					          .data());
					break;
				case ImGui::ImplTextureChannelFormat::F32:
					t->init(size.x,
					        size.y,
					        (alpha8_t *)compressor(
					          lak::span<const float>(lak::span<const void>(
					            pixels, size.x * size.y * sizeof(float) * 1U)))
					          .data());
					break;
			}
			tex = t;
		}
		break;
		default: ASSERT_UNREACHABLE();
	}

	return (ImTextureID)(uintptr_t)tex;
}

ImTextureRef ImplSRUpdateTexture(ImGui::ImplContext context,
                                 ImTextureID,
                                 const void *pixels,
                                 lak::vec2s_t size,
                                 ImGui::ImplTextureColourFormat colour,
                                 ImGui::ImplTextureChannelFormat channel,
                                 lak::span<const ImTextureRect>)
{
	// :TODO: do this correctly

	return ImplSRCreateTexture(context, pixels, size, colour, channel);
}

void ImplSRDestroyTexture(ImGui::ImplContext, ImTextureID tex)
{
	auto _t = (texture_base_t *)(uintptr_t)tex;
	switch (_t->type)
	{
		case texture_type_t::ALPHA8:  delete (texture_alpha8_t *)_t; break;
		case texture_type_t::VALUE8:  delete (texture_value8_t *)_t; break;
		case texture_type_t::COLOR16: delete (texture_color16_t *)_t; break;
		case texture_type_t::COLOR24: delete (texture_color24_t *)_t; break;
		case texture_type_t::COLOR32: delete (texture_color32_t *)_t; break;
		default:                      ASSERT_UNREACHABLE();
	}
}

lak::vec2s_t ImplSRTextureSize(ImGui::ImplContext, ImTextureID tex)
{
	auto _t = (texture_base_t *)(uintptr_t)tex;
	return {_t->w, _t->h};
}

void ImplSRCreateViewport(ImGui::ImplContext, ImGui::ImplViewport viewport)
{
	viewport->sr_viewport = new ImGui::_ImplSRViewport();
}

void ImplSRDestroyViewport(ImGui::ImplContext context,
                           ImGui::ImplViewport viewport)
{
	if (!viewport) return;

	ImplSRDestroyTexture(context, viewport->output.GetTexID());

	if (!viewport->sr_viewport) return;

	delete viewport->sr_viewport;
}

ImGui::ImplSRViewportDetails ImplSRBeginViewport(ImGui::ImplContext context,
                                                 ImGui::ImplViewport viewport,
                                                 lak::vec2s_t size)
{
	auto id = viewport->output.GetTexID();

	bool rebuild = false;

	if (id == ImTextureID_Invalid)
	{
		rebuild = true;
	}
	else if (auto *tex = (texture_base_t *)(uintptr_t)id;
	         tex->w != size.x || tex->h != size.y)
	{
		rebuild = true;
	}

	if (rebuild)
	{
		if (id != ImTextureID_Invalid) ImplSRDestroyTexture(context, id);

		switch (viewport->colour)
		{
			case ImGui::ImplTextureColourFormat::A:
			{
				auto t = new texture_alpha8_t;
				t->init(size.x, size.y);
				viewport->output = (ImTextureID)(uintptr_t)(texture_base_t *)t;
			}
			break;
			case ImGui::ImplTextureColourFormat::R:
			{
				auto t = new texture_value8_t;
				t->init(size.x, size.y);
				viewport->output = (ImTextureID)(uintptr_t)(texture_base_t *)t;
			}
			break;
			case ImGui::ImplTextureColourFormat::RGB:
			{
				auto t = new texture_color24_t;
				t->init(size.x, size.y);
				viewport->output = (ImTextureID)(uintptr_t)(texture_base_t *)t;
			}
			break;
			case ImGui::ImplTextureColourFormat::RGBA:
			{
				auto t = new texture_color32_t;
				t->init(size.x, size.y);
				viewport->output = (ImTextureID)(uintptr_t)(texture_base_t *)t;
			}
			break;
			default: ASSERT_UNREACHABLE();
		}

		id = viewport->output.GetTexID();
	}

	return {
	  .framebuffer = (texture_base_t *)(uintptr_t)id,
	};
}

void ImplSREndViewport(ImGui::ImplContext, ImGui::ImplViewport)
{
	//
}

void ImplSRRender(ImGui::ImplContext context, ImDrawData *draw_data)
{
	ASSERT(context != nullptr);
	ASSERT(context->sr_context != nullptr);
	auto *sr_context = context->sr_context;

	ImGui_ImplSoftrender_RenderDrawData(draw_data);

#if defined(LAK_USE_WINAPI)
#	if defined(LAK_SOFTWARE_RENDER_32BIT)
	using texture_colour_t = color32_t; // lak::colour::rgba8888;
#	elif defined(LAK_SOFTWARE_RENDER_24BIT)
	using texture_colour_t = color24_t; // lak::colour::rgb888;
#	elif defined(LAK_SOFTWARE_RENDER_16BIT)
	using texture_colour_t = color16_t; // lak::colour::rgb565;
#	elif defined(LAK_SOFTWARE_RENDER_8BIT)
	using texture_colour_t = alpha8_t; // lak::colour::v8;
#	else
#		error "No software render colour bit depth specified"
#	endif
	auto screen_texture_pixels = lak::span<void>(
	  sr_context->screen_texture.pixels,
	  sr_context->screen_texture.w * sr_context->screen_texture.h *
	    sr_context->screen_texture.size);
	{
		lak::blit(
		  lak::image_subview(*sr_context->screen_surface),
		  lak::image_subview(lak::image_view(
		    lak::span<texture_colour_t>(screen_texture_pixels),
		    {sr_context->screen_texture.w, sr_context->screen_texture.h})));
	}
#elif defined(LAK_USE_XLIB)
#	error "NYI"
#elif defined(LAK_USE_XCB)
#	error "NYI"
#elif defined(LAK_USE_SDL2)
	ASSERT(sr_context->window != nullptr);

	SDL_Surface *window = SDL_GetWindowSurface(sr_context->window);

	if (window != nullptr)
	{
		SDL_Rect clip;
		SDL_GetClipRect(window, &clip);
		SDL_FillRect(
		  window, &clip, SDL_MapRGBA(window->format, 0x00, 0x00, 0x00, 0xFF));
		if (SDL_BlitSurface(sr_context->screen_surface, nullptr, window, nullptr))
			ERROR(SDL_GetError());
	}
#elif defined(LAK_USE_SDL3)
	ASSERT(sr_context->window != nullptr);

	SDL_Surface *window = SDL_GetWindowSurface(sr_context->window);

	if (window != nullptr)
	{
		SDL_Rect clip;
		SDL_GetSurfaceClipRect(window, &clip);
		SDL_FillSurfaceRect(window,
		                    &clip,
		                    SDL_MapRGBA(SDL_GetPixelFormatDetails(window->format),
		                                NULL,
		                                0x00,
		                                0x00,
		                                0x00,
		                                0xFF));
		if (!SDL_BlitSurface(sr_context->screen_surface, nullptr, window, nullptr))
			ERROR(SDL_GetError());
	}
#else
#	error "No implementation specified"
#endif
}
