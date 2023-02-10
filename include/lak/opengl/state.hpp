#ifndef LAK_OPENGL_STATE_HPP
#define LAK_OPENGL_STATE_HPP

#include "lak/opengl/gl3w.hpp"

#include "lak/compiler.hpp"
#include "lak/debug.hpp"

#include <array>

namespace lak
{
	namespace opengl
	{
		force_inline auto enable_if(GLenum target, bool enable)
		{
			return (enable ? glEnable : glDisable)(target);
		}

		// returns true when there is no errors.
		inline bool check_error()
		{
			auto error = glGetError();
			switch (error)
			{
				case GL_NO_ERROR:
					return true;
				case GL_INVALID_ENUM:
					ERROR("OpenGL: Invalid enum.");
					break;
				case GL_INVALID_VALUE:
					ERROR("OpenGL: Invalid value.");
					break;
				case GL_INVALID_OPERATION:
					ERROR("OpenGL: Invalid operation.");
					break;
				case GL_INVALID_FRAMEBUFFER_OPERATION:
					ERROR("OpenGL: Invalid framebuffer operation.");
					break;
				case GL_OUT_OF_MEMORY:
					ERROR("OpenGL: Out of memory.");
					break;
				case GL_STACK_UNDERFLOW:
					ERROR("OpenGL: Stack underflow.");
					break;
				case GL_STACK_OVERFLOW:
					ERROR("OpenGL: Stack overflow.");
					break;
				default:
					ERROR("Unknown error: ", error, ".");
					break;
			}
			return false;
		}

		template<size_t S = 1>
		force_inline auto get_boolean(GLenum target)
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

		template<size_t S = 1>
		force_inline auto get_int(GLenum target)
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

		template<size_t S = 1>
		force_inline auto get_uint(GLenum target)
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

		template<size_t S = 1>
		force_inline auto get_enum(GLenum target)
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

		template<size_t S = 1>
		force_inline auto get_int64(GLenum target)
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

		template<size_t S = 1>
		force_inline auto get_uint64(GLenum target)
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

		template<size_t S = 1>
		force_inline auto get_float(GLenum target)
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

		template<size_t S = 1>
		force_inline auto get_double(GLenum target)
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

		template<GLenum PNAME>
		force_inline auto get_program_value(GLuint program)
		{
			static_assert(
			  PNAME == GL_DELETE_STATUS || PNAME == GL_LINK_STATUS ||
			    PNAME == GL_VALIDATE_STATUS || PNAME == GL_INFO_LOG_LENGTH ||
			    PNAME == GL_ATTACHED_SHADERS || PNAME == GL_ACTIVE_ATTRIBUTES ||
			    PNAME == GL_ACTIVE_ATTRIBUTE_MAX_LENGTH ||
			    PNAME == GL_ACTIVE_UNIFORMS || PNAME == GL_ACTIVE_UNIFORM_MAX_LENGTH,
			  "PNAME must be a valid argument for glGetProgramiv");

			if constexpr (PNAME == GL_DELETE_STATUS || PNAME == GL_LINK_STATUS ||
			              PNAME == GL_VALIDATE_STATUS)
			{
				GLboolean result;
				glGetProgramiv(program, PNAME, (GLint *)&result);
				return result;
			}
			else
			{
				GLuint result;
				glGetProgramiv(program, PNAME, (GLint *)&result);
				return result;
			}
		}
	}
}

#endif
