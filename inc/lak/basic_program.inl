#define SDL_MAIN_HANDLED
#include <SDL.h>

#include "opengl/state.hpp"

#include "debug.hpp"
#include "window.hpp"

#ifndef APP_NAME
#  define APP_NAME "basic window"
#endif

// Implement these basic_window_* functions in your program.
void basic_window_preinit(int argc, char **argv);
void basic_window_init(lak::window &window);
void basic_window_handle_event(SDL_Event &event, lak::window &window);
void basic_window_loop(lak::window &window, uint64_t counter_delta);
void basic_window_quit(lak::window &window);
uint32_t basic_window_target_framerate = 60;
lak::window::opengl_settings basic_window_opengl_settings;

void MessageCallback(GLenum source,
                     GLenum type,
                     GLuint id,
                     GLenum severity,
                     GLsizei length,
                     const GLchar *message,
                     const void *userParam)
{
  if (type == GL_DEBUG_TYPE_OTHER &&
      severity == GL_DEBUG_SEVERITY_NOTIFICATION)
    return;

  DEBUG("GL CALLBACK: " << (type == GL_DEBUG_TYPE_ERROR ? "** ERROR **" : ""));
  DEBUG("| Error code: 0x" << glGetError());
  switch (type)
  {
    case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
      DEBUG("| Type: DEPRECATED BEHAVIOR");
      break;
    case GL_DEBUG_TYPE_ERROR: DEBUG("| Type: ERROR"); break;
    case GL_DEBUG_TYPE_MARKER: DEBUG("| Type: MARKER"); break;
    case GL_DEBUG_TYPE_OTHER: DEBUG("| Type: OTHER"); break;
    case GL_DEBUG_TYPE_PERFORMANCE: DEBUG("| Type: PERFORMANCE"); break;
    case GL_DEBUG_TYPE_POP_GROUP: DEBUG("| Type: POP GROUP"); break;
    case GL_DEBUG_TYPE_PORTABILITY: DEBUG("| Type: PORTABILITY"); break;
    case GL_DEBUG_TYPE_PUSH_GROUP: DEBUG("| Type: PUSH GROUP"); break;
    case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
      DEBUG("| Type: UNDEFINED BEHAVIOR");
      break;
    default: DEBUG("| Type: 0x" << type); break;
  }
  switch (severity)
  {
    case GL_DEBUG_SEVERITY_HIGH: DEBUG("| Severity: HIGH"); break;
    case GL_DEBUG_SEVERITY_MEDIUM: DEBUG("| Severity: MEDIUM"); break;
    case GL_DEBUG_SEVERITY_LOW: DEBUG("| Severity: LOW"); break;
    case GL_DEBUG_SEVERITY_NOTIFICATION:
      DEBUG("| Severity: NOTIFICATION");
      break;
    default: DEBUG("| Severity: 0x" << severity); break;
  }
  switch (source)
  {
    case GL_DEBUG_SOURCE_API: DEBUG("| Source: API"); break;
    case GL_DEBUG_SOURCE_APPLICATION: DEBUG("| Source: APPLICATION"); break;
    case GL_DEBUG_SOURCE_OTHER: DEBUG("| Source: OTHER"); break;
    case GL_DEBUG_SOURCE_SHADER_COMPILER:
      DEBUG("| Source: SHADER COMPILER");
      break;
    case GL_DEBUG_SOURCE_THIRD_PARTY: DEBUG("| Source: THIRD PARTY"); break;
    case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
      DEBUG("| Source: WINDOW SYSTEM");
      break;
    default: DEBUG("| Source: 0x" << source); break;
  }
  DEBUG("| Message:\n" << message << "\n");
}

int main(int argc, char **argv)
{
  /* --- Debugger initialisation --- */

  std::set_terminate(lak::terminate_handler);

  /* --- Window initialisation --- */

  basic_window_preinit(argc, argv);

  lak::core_init();

  auto window = lak::window(APP_NAME, {720, 480}, SDL_WINDOW_RESIZABLE);

  if (!window) FATAL("Failed to create window");

  /* --- Graphics initialisation --- */

  // :TODO: Add a way to selected a prefered graphics mode
  if (window.init_opengl(basic_window_opengl_settings))
  {
    glViewport(0, 0, window.size().x, window.size().y);
    glClearColor(0.0f, 0.3125f, 0.3125f, 1.0f);
    glEnable(GL_DEPTH_TEST);

#ifndef NDEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);
#endif
  }
  else if (window.init_software())
  {
    // If this isn't called here it crashes when it's called later.
    SDL_GetWindowSurface(window.sdl_window());
  }
  else
  {
    FATAL("Failed to start any known graphics mode");
  }

  basic_window_init(window);

  uint64_t last_counter = lak::performance_counter();
  uint64_t counter_delta =
    lak::performance_frequency() / basic_window_target_framerate;

  for (bool running = true; running;)
  {
    /* --- Handle SDL2 events --- */
    for (SDL_Event event; SDL_PollEvent(&event);
         basic_window_handle_event(event, window))
    {
      switch (event.type)
      {
        case SDL_QUIT:
        {
          running = false;
        }
        break;

        /* --- Window events --- */
        case SDL_WINDOWEVENT:
        {
          switch (event.window.event)
          {
            case SDL_WINDOWEVENT_RESIZED:
            case SDL_WINDOWEVENT_SIZE_CHANGED:
            {
              window.set_size(
                {(uint32_t)event.window.data1, (uint32_t)event.window.data2});
              if (window.graphics() == lak::graphics_mode::OpenGL)
                glViewport(0, 0, window.size().x, window.size().y);
            }
            break;
          }
        }
        break;
      }
    }

    if (window.graphics() == lak::graphics_mode::OpenGL)
    {
      glViewport(0, 0, window.size().x, window.size().y);
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    basic_window_loop(window, counter_delta);

    window.swap();

    const auto counter =
      lak::yield_frame(last_counter, basic_window_target_framerate);
    counter_delta = counter - last_counter;
    last_counter  = counter;
  }

#ifndef NDEBUG
  glDisable(GL_DEBUG_OUTPUT);
#endif

  basic_window_quit(window);

  window.close();

  lak::core_quit();
}
