#ifndef LAK_SYSTEM_OPENGL_STATE_HPP
#define LAK_SYSTEM_OPENGL_STATE_HPP

#include "lak/system/opengl/gl3w.hpp"

#include "lak/system/compiler.hpp"

#include "lak/array.hpp"
#include "lak/debug.hpp"
#include "lak/format.hpp"
#include "lak/result.hpp"
#include "lak/trace.hpp"
#include "lak/type_traits.hpp"

#include "lak/string_literals/view.hpp"

namespace lak
{
	namespace opengl
	{
		inline lak::astring_view error_name(GLenum error)
		{
			switch (error)
			{
				case GL_NO_ERROR:          return "GL_NO_ERROR"_view;
				case GL_INVALID_ENUM:      return "GL_INVALID_ENUM"_view;
				case GL_INVALID_VALUE:     return "GL_INVALID_VALUE"_view;
				case GL_INVALID_OPERATION: return "GL_INVALID_OPERATION"_view;
				case GL_INVALID_FRAMEBUFFER_OPERATION:
					return "GL_INVALID_FRAMEBUFFER_OPERATION"_view;
				case GL_OUT_OF_MEMORY:   return "GL_OUT_OF_MEMORY"_view;
				case GL_STACK_UNDERFLOW: return "GL_STACK_UNDERFLOW"_view;
				case GL_STACK_OVERFLOW:  return "GL_STACK_OVERFLOW"_view;
			}
			return "unknown error code"_view;
		}

		namespace err
		{
			struct error_code
			{
				GLenum value;

				inline lak::astring to_string() const
				{
					return lak::fmt<"{} ({})">(lak::opengl::error_name(value), value);
				}
			};
		}

		// ensure glGetError is cleared before calling these

		template<typename T = lak::monostate>
		using result = lak::result<T, lak::opengl::err::error_code>;

		inline lak::opengl::result<> get_error()
		{
			const auto err{glGetError()};
			if (err == GL_NO_ERROR)
				return lak::ok_t{};
			else
				return lak::err_t{lak::opengl::err::error_code{err}};
		}

		force_inline lak::opengl::result<> enable_if(GLenum target, bool enable)
		{
			(enable ? glEnable : glDisable)(target);
			return lak::opengl::get_error();
		}

		template<typename RTN, typename... ARGS, typename... ARGS2>
		requires(!lak::is_void_v<RTN>)
		lak::opengl::result<RTN> call_checked(RTN(APIENTRYP func)(ARGS...),
		                                      ARGS2... args)
		{
			RTN result{func(args...)};
			if_let_err (const auto err, lak::opengl::get_error())
				return lak::err_t{err};
			else
				return lak::ok_t{result};
		}

		template<typename... ARGS, typename... ARGS2>
		lak::opengl::result<> call_checked(void(APIENTRYP func)(ARGS...),
		                                   ARGS2... args)
		{
			func(args...);
			return lak::opengl::get_error();
		}

		static auto trace_call_checked =
		  []<typename RTN, typename... ARGS, typename... ARGS2>(
		    lak::trace tr, RTN(APIENTRYP func)(ARGS...), ARGS2... args)
		{ return lak::opengl::call_checked(func, args...).TRACE_UNWRAP(tr); };

#define GL_DEFER_CALL(FUNC, ...)                                              \
	DEFER_CALL(lak::opengl::trace_call_checked, lak::trace{}, FUNC, __VA_ARGS__)

		template<size_t S = 1>
		force_inline auto get_boolean(GLenum target)
		{
			if constexpr (S == 1)
			{
				GLboolean result;
				return lak::opengl::call_checked(glGetBooleanv, target, &result)
				  .replace(result);
			}
			else
			{
				lak::array<GLboolean, S> result;
				return lak::opengl::call_checked(glGetBooleanv, target, result.data())
				  .replace(result);
			}
		}

		template<size_t S = 1>
		force_inline auto get_int(GLenum target)
		{
			if constexpr (S == 1)
			{
				GLint result;
				return lak::opengl::call_checked(glGetIntegerv, target, &result)
				  .replace(result);
			}
			else
			{
				lak::array<GLint, S> result;
				return lak::opengl::call_checked(glGetIntegerv, target, result.data())
				  .replace(result);
			}
		}

		template<size_t S = 1>
		force_inline auto get_uint(GLenum target)
		{
			if constexpr (S == 1)
			{
				GLuint result;
				return lak::opengl::call_checked(
				         glGetIntegerv, target, (GLint *)&result)
				  .replace(result);
			}
			else
			{
				lak::array<GLuint, S> result;
				return lak::opengl::call_checked(
				         glGetIntegerv, target, (GLint *)result.data())
				  .replace(result);
			}
		}

		template<size_t S = 1>
		force_inline auto get_enum(GLenum target)
		{
			if constexpr (S == 1)
			{
				GLenum result;
				return lak::opengl::call_checked(
				         glGetIntegerv, target, (GLint *)&result)
				  .replace(result);
			}
			else
			{
				lak::array<GLenum, S> result;
				return lak::opengl::call_checked(
				         glGetIntegerv, target, (GLint *)result.data())
				  .replace(result);
			}
		}

		template<size_t S = 1>
		force_inline auto get_int64(GLenum target)
		{
			if constexpr (S == 1)
			{
				GLint64 result;
				return lak::opengl::call_checked(glGetInteger64v, target, &result)
				  .replace(result);
			}
			else
			{
				lak::array<GLint64, S> result;
				return lak::opengl::call_checked(
				         glGetInteger64v, target, result.data())
				  .replace(result);
			}
		}

		template<size_t S = 1>
		force_inline auto get_uint64(GLenum target)
		{
			if constexpr (S == 1)
			{
				GLuint64 result;
				return lak::opengl::call_checked(
				         glGetInteger64v, target, (GLint64 *)&result)
				  .replace(result);
			}
			else
			{
				lak::array<GLuint64, S> result;
				return lak::opengl::call_checked(
				         glGetInteger64v, target, (GLint64 *)result.data())
				  .replace(result);
			}
		}

		template<size_t S = 1>
		force_inline auto get_float(GLenum target)
		{
			if constexpr (S == 1)
			{
				GLfloat result;
				return lak::opengl::call_checked(glGetFloatv, target, &result)
				  .replace(result);
			}
			else
			{
				lak::array<GLfloat, S> result;
				return lak::opengl::call_checked(glGetFloatv, target, result.data())
				  .replace(result);
			}
		}

		template<size_t S = 1>
		force_inline auto get_double(GLenum target)
		{
			if constexpr (S == 1)
			{
				GLdouble result;
				return lak::opengl::call_checked(glGetDoublev, target, &result)
				  .replace(result);
			}
			else
			{
				lak::array<GLdouble, S> result;
				return lak::opengl::call_checked(glGetDoublev, target, result.data())
				  .replace(result);
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
				return lak::opengl::call_checked(
				         glGetProgramiv, program, PNAME, (GLint *)&result)
				  .replace(result);
			}
			else
			{
				GLuint result;
				return lak::opengl::call_checked(
				         glGetProgramiv, program, PNAME, (GLint *)&result)
				  .replace(result);
			}
		}
	}

	template<typename CHAR>
	struct format_traits<lak::opengl::err::error_code, CHAR>
	{
		static constexpr lak::string<CHAR> to_string(
		  const lak::opengl::err::error_code &err)
		{
			return lak::strconv<CHAR>(err.to_string());
		}
	};
}

#endif
