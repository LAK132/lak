#include "lak/debug.hpp"

#include "impl.hpp"

#ifdef LAK_OS_APPLE
#	include "lak/system/file.hpp"
#	include <stdlib.h>
#endif

bool lak::platform_init()
{
#ifdef LAK_OS_APPLE
	{
		lak::fs::path current_prefix = lak::exe_path().parent_path().parent_path();
		lak::array<lak::fs::path> files;
#	ifdef LAK_MOLTENVK_JSON
		files.push_back(current_prefix / LAK_MOLTENVK_JSON);
#		ifdef LAK_VK_DRIVER_PREFIX
		if (auto prefix = lak::fs::path(LAK_VK_DRIVER_PREFIX);
				prefix != current_prefix)
			files.push_back(prefix / LAK_MOLTENVK_JSON);
# 	endif
#	endif
#	ifdef LAK_VK_VALIDATION_JSON
		files.push_back(current_prefix / LAK_VK_VALIDATION_JSON);
#		ifdef LAK_VK_DRIVER_PREFIX
		if (auto prefix = lak::fs::path(LAK_VK_DRIVER_PREFIX);
		    prefix != current_prefix)
			files.push_back(prefix / LAK_VK_VALIDATION_JSON);
# 	endif
#	endif
		if (!files.empty())
		{
			lak::astring str = files[0].string();
			for (const auto& p : lak::span(files).subspan(1U))
				str += ":" + p.string();
			ASSERT_EQUAL(setenv("VK_DRIVER_FILES", str.c_str(), 1), 0);
		}
	}
#endif

	bool failed = false;
	SDL_SetMainReady();
	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		ERROR(lak::fmt<u8"SDL_INIT_VIDEO failed ({})">(SDL_GetError()));
		failed = true;
	}
	if (SDL_Init(SDL_INIT_AUDIO) != 0)
	{
		ERROR(lak::fmt<u8"SDL_INIT_AUDIO failed ({})">(SDL_GetError()));
		// :TODO: just ignore audio subsystem initialisation failure for now
		// failed = true;
	}
	if (SDL_Init(SDL_INIT_TIMER) != 0)
	{
		ERROR(lak::fmt<u8"SDL_INIT_TIMER failed ({})">(SDL_GetError()));
		failed = true;
	}
	return !failed;
}

bool lak::platform_quit()
{
	SDL_Quit();
	return true;
}

bool lak::get_clipboard(lak::u8string *s)
{
	auto *clip = SDL_GetClipboardText();
	*s         = (const char8_t *)clip;
	SDL_free(clip);
	// impossible to tell the difference between an empty clipboard and an error
	return true;
}

bool lak::set_clipboard(lak::u8string_view s)
{
	return SDL_SetClipboardText((const char *)s.begin()) == 0;
}

bool lak::cursor_visible() { ASSERT_NYI(); }

void lak::show_cursor() { ASSERT_NYI(); }

void lak::hide_cursor() { ASSERT_NYI(); }

void lak::set_cursor_pos(lak::vec2l_t p)
{
	ASSERT(p.x < INT_MAX && p.x > INT_MIN);
	ASSERT(p.y < INT_MAX && p.y > INT_MIN);
	// :TODO: clamp instead of cast
	ASSERT_EQUAL(SDL_WarpMouseGlobal((int)p.x, (int)p.y), 0);
}

lak::vec2l_t lak::get_cursor_pos() { ASSERT_NYI(); }

void lak::set_cursor(lak::cursor *c)
{
	LAK_UNUSED(c);
	ASSERT_NYI();
}

#include "../common/platform.inl"
