#ifndef LAK_OPENGL_STATE_HPP
#define LAK_OPENGL_STATE_HPP

#include <GL/gl3w.h>

#include <array>

#define glEnableDisable(TARGET, BOOL) ((BOOL) ? glEnable : glDisable)((TARGET))

namespace lak
{
  namespace opengl
  {
    template<size_t S>
    auto get_boolean(GLenum target)
    {
      if constexpr (S == 1)
      {
        GLboolean result;
        glGetBooleanv(target, &result);
        return result;
      }
      else
      {
        std::array<GLboolean, S> result;
        glGetBooleanv(target, result.data());
        return result;
      }
    }

    template<size_t S>
    auto get_int(GLenum target)
    {
      if constexpr (S == 1)
      {
        GLint result;
        glGetIntegerv(target, &result);
        return result;
      }
      else
      {
        std::array<GLint, S> result;
        glGetIntegerv(target, result.data());
        return result;
      }
    }

    template<size_t S>
    auto get_uint(GLenum target)
    {
      if constexpr (S == 1)
      {
        GLuint result;
        glGetIntegerv(target, (GLint *)&result);
        return result;
      }
      else
      {
        std::array<GLuint, S> result;
        glGetIntegerv(target, (GLint *)result.data());
        return result;
      }
    }

    template<size_t S>
    auto get_enum(GLenum target)
    {
      if constexpr (S == 1)
      {
        GLenum result;
        glGetIntegerv(target, (GLint *)&result);
        return result;
      }
      else
      {
        std::array<GLenum, S> result;
        glGetIntegerv(target, (GLint *)result.data());
        return result;
      }
    }

    template<size_t S>
    auto get_int64(GLenum target)
    {
      if constexpr (S == 1)
      {
        GLint64 result;
        glGetInteger64v(target, &result);
        return result;
      }
      else
      {
        std::array<GLint64, S> result;
        glGetInteger64v(target, result.data());
        return result;
      }
    }

    template<size_t S>
    auto get_uint64(GLenum target)
    {
      if constexpr (S == 1)
      {
        GLuint64 result;
        glGetInteger64v(target, (GLint64 *)&result);
        return result;
      }
      else
      {
        std::array<GLuint64, S> result;
        glGetInteger64v(target, (GLint64 *)result.data());
        return result;
      }
    }

    template<size_t S>
    auto get_float(GLenum target)
    {
      if constexpr (S == 1)
      {
        GLfloat result;
        glGetFloatv(target, &result);
        return result;
      }
      else
      {
        std::array<GLfloat, S> result;
        glGetFloatv(target, result.data());
        return result;
      }
    }

    template<size_t S>
    auto get_double(GLenum target)
    {
      if constexpr (S == 1)
      {
        GLdouble result;
        glGetDoublev(target, &result);
        return result;
      }
      else
      {
        std::array<GLdouble, S> result;
        glGetDoublev(target, result.data());
        return result;
      }
    }
  }
}

#endif
