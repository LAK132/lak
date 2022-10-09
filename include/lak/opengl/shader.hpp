#ifndef LAK_OPENGL_SHADER_HPP
#define LAK_OPENGL_SHADER_HPP

#include "lak/opengl/texture.hpp"

#include "lak/optional.hpp"
#include "lak/span.hpp"
#include "lak/string.hpp"

#include <memory>

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
			shader(shader &&other);
			~shader();
			shader &operator=(shader &&other);

			shader(const shader &other) = delete;
			shader &operator=(const shader &other) = delete;

			static shader create(const lak::astring &code, GLenum shader_type);

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

		struct shader_attribute
		{
			lak::astring name;
			GLuint position;
			GLint size;
			// GL_FLOAT, GL_FLOAT_VEC2, GL_FLOAT_VEC3, GL_FLOAT_VEC4,
			// GL_FLOAT_MAT2, GL_FLOAT_MAT3 or GL_FLOAT_MAT4.
			GLenum type;
		};

		struct shader_uniform
		{
			lak::astring name;
			GLuint position;
			GLint size;
			// There are a lot more types that this can be compared to
			// shader_attribute, see
			// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetActiveUniform.xhtml
			GLenum type;
		};

		struct program;
		using shared_program = std::shared_ptr<program>;
		struct program
		{
		private:
			GLuint _program = 0; // 0 is reserved

		public:
			program() = default;
			program(program &&other);
			~program();
			program &operator=(program &&other);

			program(const program &other) = delete;
			program &operator=(const program &other) = delete;

			static program create();
			static program create(const shader &vertex, const shader &fragment);

			static shared_program create_shared();
			static shared_program create_shared(const shader &vertex,
			                                    const shader &fragment);

			program &attach(const shader &shader);
			program &link();
			program &clear();
			program &use();

			inline operator bool() const { return _program != 0; }
			inline operator GLuint() const { return _program; }
			inline operator GLint() const { return (GLint)_program; }
			inline GLuint get() const { return _program; }
			lak::optional<lak::astring> link_error() const;

			std::vector<shader_attribute> attributes() const;
			std::vector<shader_uniform> uniforms() const;

			shader_attribute attribute(GLuint attr) const;
			shader_uniform uniform(GLuint unif) const;

			lak::optional<GLuint> attrib_location(const GLchar *name) const;
			lak::optional<GLuint> uniform_location(const GLchar *name) const;

			shader_attribute assert_attribute(const GLchar *name) const;
			shader_uniform assert_uniform(const GLchar *name) const;

			GLuint assert_attrib_location(const GLchar *name) const;
			GLuint assert_uniform_location(const GLchar *name) const;

			const program &set_uniform(
			  GLuint unif,
			  lak::span<const void> data,
			  GLsizei count       = 1 /* only used for array type */,
			  GLboolean transpose = GL_FALSE) const;
			const program &assert_set_uniform(
			  const GLchar *name,
			  lak::span<const void> data,
			  GLsizei count       = 1 /* only used for array type */,
			  GLboolean transpose = GL_FALSE) const;
		};
	}
}

#endif
