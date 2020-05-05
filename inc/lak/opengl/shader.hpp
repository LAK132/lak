#ifndef LAK_OPENGL_SHADER_HPP
#define LAK_OPENGL_SHADER_HPP

#include "lak/opengl/texture.hpp"

#include <optional>
#include <string>

namespace lak
{
  namespace opengl
  {
    struct shader
    {
    private:
      GLuint _shader = 0; // 0 is reserved
      GLenum _type   = 0;

    public:
      shader() = default;
      shader(const std::string &code, GLenum shader_type);
      shader(shader &&other);
      ~shader();
      shader &operator=(shader &&other);

      shader &clear();

      inline operator bool() const { return _shader != 0; }
      inline GLuint get() const { return _shader; }
      inline GLenum type() const { return _type; }
    };

    namespace literals
    {
      shader operator""_vertex_shader(const char *str, size_t);
      shader operator""_fragment_shader(const char *str, size_t);
      shader operator""_tess_control_shader(const char *str, size_t);
      shader operator""_tess_eval_shader(const char *str, size_t);
      shader operator""_geometry_shader(const char *str, size_t);
      shader operator""_compute_shader(const char *str, size_t);
    }

    struct program
    {
    private:
      GLuint _program = 0; // 0 is reserved

    public:
      program() = default;
      program(const shader &vertex, const shader &fragment);
      program(program &&other);
      ~program();
      program &operator=(program &&other);

      program &init();
      program &attach(const shader &shader);
      program &link();
      program &clear();

      inline operator bool() const { return _program != 0; }
      inline GLuint get() const { return _program; }
      std::optional<std::string> link_error() const;

      GLint uniform_location(const GLchar *name) const;
      GLint attrib_location(const GLchar *name) const;
    };
  }
}

#endif
