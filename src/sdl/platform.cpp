#include "lak/platform.hpp"
#include "lak/debug.hpp"

bool lak::platform_init(lak::platform_instance *handle)
{
  SDL_SetMainReady();
  return SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_TIMER) == 0;
}

bool lak::platform_quit(lak::platform_instance *handle)
{
  SDL_Quit();
  return true;
}
