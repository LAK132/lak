#include "lak/opengl/shader.hpp"
#include "lak/opengl/state.hpp"

#include "lak/debug.hpp"

namespace lak
{
	namespace opengl
	{
		shader::shader(shader &&other)
		: _shader(lak::exchange(other._shader, (GLuint)0))
		{
		}

		shader::~shader() { clear(); }

		shader &shader::operator=(shader &&other)
		{
			lak::swap(_shader, other._shader);
			return *this;
		}

		shader shader::create(const lak::astring &code, GLenum shader_type)
		{
			ASSERTF(lak::opengl::check_error(), "Already in bad state");
			shader shdr;
			shdr._type   = shader_type;
			shdr._shader = glCreateShader(shdr._type);
			ASSERT(lak::opengl::check_error());
			const GLchar *cstr = code.c_str();
			glShaderSource(shdr._shader, 1, &cstr, nullptr);
			ASSERT(lak::opengl::check_error());
			glCompileShader(shdr._shader);
			ASSERT(lak::opengl::check_error());
			GLint compiled;
			glGetShaderiv(shdr._shader, GL_COMPILE_STATUS, &compiled);
			ASSERT(lak::opengl::check_error());
			if (!compiled)
			{
				GLint msgSize = 0;
				glGetShaderiv(shdr._shader, GL_INFO_LOG_LENGTH, &msgSize);
				ASSERT(lak::opengl::check_error());
				std::vector<char> msg(msgSize, '\0');
				glGetShaderInfoLog(shdr._shader, msgSize, nullptr, msg.data());
				ASSERT(lak::opengl::check_error());
				ERROR("Shader failed to compile");
				ERROR(msg.data());

				glDeleteShader(shdr._shader);
				ASSERT(lak::opengl::check_error());
				shdr._shader = 0;
			}
			return shdr;
		}

		shader &shader::clear()
		{
			if (_shader != 0)
			{
				glDeleteShader(_shader);
				ASSERT(lak::opengl::check_error());
				_shader = 0;
			}
			return *this;
		}

		namespace literals
		{
			shader operator""_vertex_shader(const char *str, size_t)
			{
				return shader::create(str, GL_VERTEX_SHADER);
			}

			shader operator""_fragment_shader(const char *str, size_t)
			{
				return shader::create(str, GL_FRAGMENT_SHADER);
			}

			shader operator""_tess_control_shader(const char *str, size_t)
			{
				return shader::create(str, GL_TESS_CONTROL_SHADER);
			}

			shader operator""_tess_eval_shader(const char *str, size_t)
			{
				return shader::create(str, GL_TESS_EVALUATION_SHADER);
			}

			shader operator""_geometry_shader(const char *str, size_t)
			{
				return shader::create(str, GL_GEOMETRY_SHADER);
			}

			shader operator""_compute_shader(const char *str, size_t)
			{
				return shader::create(str, GL_COMPUTE_SHADER);
			}
		}

		program::program(program &&other)
		: _program(lak::exchange(other._program, (GLuint)0))
		{
		}

		program::~program() { clear(); }

		program &program::operator=(program &&other)
		{
			lak::swap(_program, other._program);
			return *this;
		}

		program program::create()
		{
			program prog;
			prog._program = glCreateProgram();
			ASSERT(lak::opengl::check_error());
			return prog;
		}

		program program::create(const shader &vertex, const shader &fragment)
		{
			ASSERT(vertex && fragment);
			auto prog = program::create();
			prog.attach(vertex);
			prog.attach(fragment);
			prog.link();
			return prog;
		}

		shared_program program::create_shared()
		{
			return std::make_shared<program>(create());
		}

		shared_program program::create_shared(const shader &vertex,
		                                      const shader &fragment)
		{
			return std::make_shared<program>(create(vertex, fragment));
		}

		program &program::attach(const shader &shader)
		{
			if (shader.get() != 0)
			{
				glAttachShader(_program, shader.get());
				ASSERT(lak::opengl::check_error());
			}
			return *this;
		}

		program &program::link()
		{
			glLinkProgram(_program);
			ASSERT(lak::opengl::check_error());
			return *this;
		}

		lak::optional<lak::astring> program::link_error() const
		{
			GLint linked;
			glGetProgramiv(_program, GL_LINK_STATUS, &linked);
			ASSERT(lak::opengl::check_error());
			if (!linked)
			{
				GLint msgSize = 0;
				glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &msgSize);
				ASSERT(lak::opengl::check_error());
				std::vector<char> msg;
				msg.resize(msgSize + 1);
				glGetProgramInfoLog(_program, msgSize, nullptr, msg.data());
				ASSERT(lak::opengl::check_error());
				msg[msgSize] = 0;
				return lak::optional<lak::astring>(lak::astring(msg.data()));
			}
			return lak::nullopt;
		}

		program &program::clear()
		{
			if (_program != 0)
			{
				glDeleteProgram(_program);
				// ASSERT(lak::opengl::check_error());
				_program = 0;
			}
			return *this;
		}

		program &program::use()
		{
			ASSERT(_program != 0);
			glUseProgram(_program);
			ASSERT(lak::opengl::check_error());
			return *this;
		}

		std::vector<shader_attribute> program::attributes() const
		{
			std::vector<shader_attribute> attributes;

			std::vector<char> name;
			name.resize(
			  lak::opengl::get_program_value<GL_ACTIVE_ATTRIBUTE_MAX_LENGTH>(
			    _program) +
			  1);
			ASSERT(lak::opengl::check_error());

			attributes.resize(
			  lak::opengl::get_program_value<GL_ACTIVE_ATTRIBUTES>(_program));
			ASSERT(lak::opengl::check_error());

			for (GLuint i = 0; i < attributes.size(); ++i)
			{
				lak::fill(lak::span(name), char(0));
				glGetActiveAttrib(_program,
				                  i,
				                  (GLsizei)name.size(),
				                  NULL,
				                  &attributes[i].size,
				                  &attributes[i].type,
				                  name.data());
				ASSERT(lak::opengl::check_error());
				attributes[i].position = glGetAttribLocation(_program, name.data());
				ASSERT(lak::opengl::check_error());
				attributes[i].name = name.data();
			}

			return attributes;
		}

		std::vector<shader_uniform> program::uniforms() const
		{
			std::vector<shader_uniform> uniforms;

			std::vector<char> name;
			name.resize(lak::opengl::get_program_value<GL_ACTIVE_UNIFORM_MAX_LENGTH>(
			              _program) +
			            1);
			ASSERT(lak::opengl::check_error());

			uniforms.resize(
			  lak::opengl::get_program_value<GL_ACTIVE_UNIFORMS>(_program));
			ASSERT(lak::opengl::check_error());

			for (GLuint i = 0; i < uniforms.size(); ++i)
			{
				lak::fill(lak::span(name), char(0));
				glGetActiveUniform(_program,
				                   i,
				                   (GLsizei)name.size(),
				                   NULL,
				                   &uniforms[i].size,
				                   &uniforms[i].type,
				                   name.data());
				ASSERT(lak::opengl::check_error());
				uniforms[i].position = glGetAttribLocation(_program, name.data());
				ASSERT(lak::opengl::check_error());
				uniforms[i].name = name.data();
			}

			return uniforms;
		}

		shader_attribute program::attribute(GLuint attr) const
		{
			shader_attribute result;

			std::vector<char> name;
			name.resize(
			  lak::opengl::get_program_value<GL_ACTIVE_ATTRIBUTE_MAX_LENGTH>(
			    _program) +
			  1);
			ASSERT(lak::opengl::check_error());

			lak::fill(lak::span(name), char(0));
			glGetActiveAttrib(_program,
			                  attr,
			                  (GLsizei)name.size(),
			                  NULL,
			                  &result.size,
			                  &result.type,
			                  name.data());
			ASSERT(lak::opengl::check_error());
			result.position = glGetAttribLocation(_program, name.data());
			ASSERT(lak::opengl::check_error());
			result.name = name.data();

			return result;
		}

		shader_uniform program::uniform(GLuint unif) const
		{
			shader_uniform result;

			std::vector<char> name;
			name.resize(lak::opengl::get_program_value<GL_ACTIVE_UNIFORM_MAX_LENGTH>(
			              _program) +
			            1);
			ASSERT(lak::opengl::check_error());

			lak::fill(lak::span(name), char(0));
			glGetActiveUniform(_program,
			                   unif,
			                   (GLsizei)name.size(),
			                   NULL,
			                   &result.size,
			                   &result.type,
			                   name.data());
			ASSERT(lak::opengl::check_error());
			result.position = glGetAttribLocation(_program, name.data());
			ASSERT(lak::opengl::check_error());
			result.name = name.data();

			return result;
		}

		lak::optional<GLuint> program::attrib_location(const GLchar *name) const
		{
			auto loc = glGetAttribLocation(_program, name);
			ASSERT(lak::opengl::check_error());
			return loc == -1 ? lak::nullopt : lak::optional<GLuint>{(GLuint)loc};
		}

		lak::optional<GLuint> program::uniform_location(const GLchar *name) const
		{
			auto loc = glGetUniformLocation(_program, name);
			ASSERT(lak::opengl::check_error());
			return loc == -1 ? lak::nullopt : lak::optional<GLuint>{(GLuint)loc};
		}

		shader_attribute program::assert_attribute(const GLchar *name) const
		{
			return attribute(assert_attrib_location(name));
		}

		shader_uniform program::assert_uniform(const GLchar *name) const
		{
			return uniform(assert_uniform_location(name));
		}

		GLuint program::assert_attrib_location(const GLchar *name) const
		{
			auto loc = glGetAttribLocation(_program, name);
			ASSERT(lak::opengl::check_error());
			ASSERT(loc != -1);
			return (GLuint)loc;
		}

		GLuint program::assert_uniform_location(const GLchar *name) const
		{
			auto loc = glGetUniformLocation(_program, name);
			ASSERT(lak::opengl::check_error());
			ASSERT(loc != -1);
			return (GLuint)loc;
		}

		const program &program::set_uniform(GLuint unif,
		                                    lak::span<const void> data,
		                                    GLsizei count,
		                                    GLboolean transpose) const
		{
			ASSERT(lak::opengl::get_uint(GL_CURRENT_PROGRAM) == _program);
			const auto info = uniform(unif);
			switch (info.type)
			{
				case GL_FLOAT:
					ASSERT(static_cast<GLsizei>(1 * data.size() / sizeof(GLfloat)) >=
					       count);
					glUniform1fv(unif, count, (GLfloat *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_FLOAT_VEC2:
					ASSERT(static_cast<GLsizei>(2 * data.size() / sizeof(GLfloat)) >=
					       count);
					glUniform2fv(unif, count, (GLfloat *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_FLOAT_VEC3:
					ASSERT(static_cast<GLsizei>(3 * data.size() / sizeof(GLfloat)) >=
					       count);
					glUniform3fv(unif, count, (GLfloat *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_FLOAT_VEC4:
					ASSERT(static_cast<GLsizei>(4 * data.size() / sizeof(GLfloat)) >=
					       count);
					glUniform4fv(unif, count, (GLfloat *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_FLOAT_MAT2:
					ASSERT(static_cast<GLsizei>(2 * 2 * data.size() / sizeof(GLfloat)) >=
					       count);
					glUniformMatrix2fv(unif, count, transpose, (GLfloat *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_FLOAT_MAT2x3:
					ASSERT(static_cast<GLsizei>(2 * 3 * data.size() / sizeof(GLfloat)) >=
					       count);
					glUniformMatrix2x3fv(unif, count, transpose, (GLfloat *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_FLOAT_MAT2x4:
					ASSERT(static_cast<GLsizei>(2 * 4 * data.size() / sizeof(GLfloat)) >=
					       count);
					glUniformMatrix2x4fv(unif, count, transpose, (GLfloat *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_FLOAT_MAT3:
					ASSERT(static_cast<GLsizei>(3 * 3 * data.size() / sizeof(GLfloat)) >=
					       count);
					glUniformMatrix3fv(unif, count, transpose, (GLfloat *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_FLOAT_MAT3x2:
					ASSERT(static_cast<GLsizei>(3 * 2 * data.size() / sizeof(GLfloat)) >=
					       count);
					glUniformMatrix3x2fv(unif, count, transpose, (GLfloat *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_FLOAT_MAT3x4:
					ASSERT(static_cast<GLsizei>(3 * 4 * data.size() / sizeof(GLfloat)) >=
					       count);
					glUniformMatrix3x4fv(unif, count, transpose, (GLfloat *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_FLOAT_MAT4:
					ASSERT(static_cast<GLsizei>(4 * 4 * data.size() / sizeof(GLfloat)) >=
					       count);
					glUniformMatrix4fv(unif, count, transpose, (GLfloat *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_FLOAT_MAT4x2:
					ASSERT(static_cast<GLsizei>(4 * 2 * data.size() / sizeof(GLfloat)) >=
					       count);
					glUniformMatrix4x2fv(unif, count, transpose, (GLfloat *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_FLOAT_MAT4x3:
					ASSERT(static_cast<GLsizei>(4 * 3 * data.size() / sizeof(GLfloat)) >=
					       count);
					glUniformMatrix4x3fv(unif, count, transpose, (GLfloat *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_DOUBLE:
					ASSERT(static_cast<GLsizei>(1 * data.size() / sizeof(GLdouble)) >=
					       count);
					glUniform1dv(unif, count, (GLdouble *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_DOUBLE_VEC2:
					ASSERT(static_cast<GLsizei>(2 * data.size() / sizeof(GLdouble)) >=
					       count);
					glUniform2dv(unif, count, (GLdouble *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_DOUBLE_VEC3:
					ASSERT(static_cast<GLsizei>(3 * data.size() / sizeof(GLdouble)) >=
					       count);
					glUniform3dv(unif, count, (GLdouble *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_DOUBLE_VEC4:
					ASSERT(static_cast<GLsizei>(4 * data.size() / sizeof(GLdouble)) >=
					       count);
					glUniform4dv(unif, count, (GLdouble *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_DOUBLE_MAT2:
					ASSERT(static_cast<GLsizei>(2 * 2 * data.size() /
					                            sizeof(GLdouble)) >= count);
					glUniformMatrix2dv(unif, count, transpose, (GLdouble *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_DOUBLE_MAT2x3:
					ASSERT(static_cast<GLsizei>(2 * 3 * data.size() /
					                            sizeof(GLdouble)) >= count);
					glUniformMatrix2x3dv(
					  unif, count, transpose, (GLdouble *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_DOUBLE_MAT2x4:
					ASSERT(static_cast<GLsizei>(2 * 4 * data.size() /
					                            sizeof(GLdouble)) >= count);
					glUniformMatrix2x4dv(
					  unif, count, transpose, (GLdouble *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_DOUBLE_MAT3:
					ASSERT(static_cast<GLsizei>(3 * 3 * data.size() /
					                            sizeof(GLdouble)) >= count);
					glUniformMatrix3dv(unif, count, transpose, (GLdouble *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_DOUBLE_MAT3x2:
					ASSERT(static_cast<GLsizei>(3 * 2 * data.size() /
					                            sizeof(GLdouble)) >= count);
					glUniformMatrix3x2dv(
					  unif, count, transpose, (GLdouble *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_DOUBLE_MAT3x4:
					ASSERT(static_cast<GLsizei>(3 * 4 * data.size() /
					                            sizeof(GLdouble)) >= count);
					glUniformMatrix3x4dv(
					  unif, count, transpose, (GLdouble *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_DOUBLE_MAT4:
					ASSERT(static_cast<GLsizei>(4 * 4 * data.size() /
					                            sizeof(GLdouble)) >= count);
					glUniformMatrix4dv(unif, count, transpose, (GLdouble *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_DOUBLE_MAT4x2:
					ASSERT(static_cast<GLsizei>(4 * 2 * data.size() /
					                            sizeof(GLdouble)) >= count);
					glUniformMatrix4x2dv(
					  unif, count, transpose, (GLdouble *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_DOUBLE_MAT4x3:
					ASSERT(static_cast<GLsizei>(4 * 3 * data.size() /
					                            sizeof(GLdouble)) >= count);
					glUniformMatrix4x3dv(
					  unif, count, transpose, (GLdouble *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_INT:
				case GL_SAMPLER_1D:
				case GL_SAMPLER_2D:
				case GL_SAMPLER_3D:
				case GL_SAMPLER_CUBE:
				case GL_SAMPLER_1D_SHADOW:
				case GL_SAMPLER_2D_SHADOW:
				case GL_SAMPLER_CUBE_SHADOW:
				case GL_SAMPLER_1D_ARRAY:
				case GL_SAMPLER_2D_ARRAY:
				case GL_SAMPLER_1D_ARRAY_SHADOW:
				case GL_SAMPLER_2D_ARRAY_SHADOW:
				case GL_SAMPLER_2D_MULTISAMPLE:
				case GL_SAMPLER_2D_MULTISAMPLE_ARRAY:
				case GL_SAMPLER_BUFFER:
				case GL_SAMPLER_2D_RECT:
				case GL_SAMPLER_2D_RECT_SHADOW:
					ASSERT(static_cast<GLsizei>(1 * data.size() / sizeof(GLint)) >=
					       count);
					glUniform1iv(unif, count, (GLint *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_INT_VEC2:
					ASSERT(static_cast<GLsizei>(2 * data.size() / sizeof(GLint)) >=
					       count);
					glUniform2iv(unif, count, (GLint *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_INT_VEC3:
					ASSERT(static_cast<GLsizei>(3 * data.size() / sizeof(GLint)) >=
					       count);
					glUniform3iv(unif, count, (GLint *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_INT_VEC4:
					ASSERT(static_cast<GLsizei>(4 * data.size() / sizeof(GLint)) >=
					       count);
					glUniform4iv(unif, count, (GLint *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_UNSIGNED_INT:
					ASSERT(static_cast<GLsizei>(1 * data.size() / sizeof(GLuint)) >=
					       count);
					glUniform1uiv(unif, count, (GLuint *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_UNSIGNED_INT_VEC2:
					ASSERT(static_cast<GLsizei>(2 * data.size() / sizeof(GLuint)) >=
					       count);
					glUniform2uiv(unif, count, (GLuint *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_UNSIGNED_INT_VEC3:
					ASSERT(static_cast<GLsizei>(3 * data.size() / sizeof(GLuint)) >=
					       count);
					glUniform3uiv(unif, count, (GLuint *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				case GL_UNSIGNED_INT_VEC4:
					ASSERT(static_cast<GLsizei>(4 * data.size() / sizeof(GLuint)) >=
					       count);
					glUniform4uiv(unif, count, (GLuint *)data.data());
					ASSERT(lak::opengl::check_error());
					break;
				default: FATAL("Unsupported type ", info.type);
			}
			return *this;
		}

		const program &program::assert_set_uniform(const GLchar *unif,
		                                           lak::span<const void> data,
		                                           GLsizei count,
		                                           GLboolean transpose) const
		{
			return set_uniform(
			  assert_uniform_location(unif), data, count, transpose);
		}
	}
}
