#include "lak/platform.hpp"
#include "lak/debug.hpp"

bool lak::platform_init(lak::platform_instance *handle)
{
  SDL_SetMainReady();
  return SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
}

bool lak::platform_quit(lak::platform_instance *handle)
{
  SDL_Quit();
  return true;
}
