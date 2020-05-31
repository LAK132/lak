/*
Typical usage for an OpenGL program:

int main()
{
  lak::core_init();

  lak::window window(...);

  window.init_opengl(...);

  uint32_t framerate = 60;

  auto last_counter = lak::performance_counter();

  // main loop
  while(...)
  {
    // event handlers

    // update code

    // draw code

    window.swap();

    last_counter = lak::yield_frame(last_counter, framerate);
  }

  window.close();

  lak::core_quit();
}
*/

#ifndef LAK_WINDOW_HPP
#define LAK_WINDOW_HPP

#include <SDL/SDL.h>

#include <lak/string.hpp>
#include <lak/vec.hpp>

#include <memory>

namespace lak
{
  void core_init();

  void core_quit();

  enum struct graphics_mode
  {
    None     = 0,
    Software = 1,
    OpenGL   = 2,
    Vulkan   = 3
  };

  struct window_impl;
  extern template std::unique_ptr<window_impl>;

  struct window
  {
  private:
    std::unique_ptr<window_impl> _impl;

    std::string _title;

    vec2u32_t _size = {0, 0};

    graphics_mode _graphics_mode = graphics_mode::None;

  public:
    window(const std::string &title, vec2u32_t size, uint32_t flags);
    ~window();

    void close();

    window()          = default;
    window(window &&) = default;
    window &operator=(window &&) = default;

    inline operator bool() { return _impl.operator bool(); }

    inline vec2u32_t size() const { return _size; }
    inline window &set_size(vec2u32_t size)
    {
      _size = size;
      return *this;
    }

    inline const std::string &title() const { return _title; }
    inline window &set_title(const std::string &title)
    {
      _title = title;
      return *this;
    }

    inline graphics_mode graphics() const { return _graphics_mode; }

    struct opengl_settings
    {
      bool double_buffered = false;
      uint8_t depth_size   = 24;
      uint8_t colour_size  = 8;
      uint8_t stencil_size = 8;
      int major            = 3;
      int minor            = 2;
    };

    bool init_opengl(const opengl_settings &settings);
    void stop_opengl();

    bool init_software();
    void stop_software();

    // Update back buffer.
    void swap();

    SDL_Window *sdl_window() const;
  };

  uint64_t performance_frequency();

  uint64_t performance_counter();

  // Yield this thread until the target framerate is achieved.
  uint64_t yield_frame(const uint64_t last_counter,
                       const uint32_t target_framerate);
}

#include <ostream>

static std::ostream &operator<<(std::ostream &strm, lak::graphics_mode mode)
{
  switch (mode)
  {
    case lak::graphics_mode::OpenGL: strm << "OpenGL"; break;
    case lak::graphics_mode::Software: strm << "Software"; break;
    case lak::graphics_mode::Vulkan: strm << "Vulkan"; break;
    default: strm << "None"; break;
  }
  return strm;
}

#endif
