#ifndef LAK_SYSTEM_OPENGL_SHADER_HPP
#define LAK_SYSTEM_OPENGL_SHADER_HPP

#include "lak/system/opengl/state.hpp"
#include "lak/system/opengl/texture.hpp"

#include "lak/memory.hpp"
#include "lak/optional.hpp"
#include "lak/result.hpp"
#include "lak/span.hpp"
#include "lak/string.hpp"

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

			shader(const shader &other)            = delete;
			shader &operator=(const shader &other) = delete;

			static lak::opengl::result<shader> make(const lak::astring &code,
			                                        GLenum shader_type);

			shader &clear();

			explicit inline operator bool() const { return _shader != 0; }
			inline GLuint get() const { return _shader; }
			inline GLenum type() const { return _type; }
		};

		namespace literals
		{
			lak::opengl::result<shader> operator""_vertex_shader(const char *str,
			                                                     size_t);
			lak::opengl::result<shader> operator""_fragment_shader(const char *str,
			                                                       size_t);
			lak::opengl::result<shader> operator""_tess_control_shader(
			  const char *str, size_t);
			lak::opengl::result<shader> operator""_tess_eval_shader(const char *str,
			                                                        size_t);
			lak::opengl::result<shader> operator""_geometry_shader(const char *str,
			                                                       size_t);
			lak::opengl::result<shader> operator""_compute_shader(const char *str,
			                                                      size_t);
		}

		enum index : GLuint
		{
		};

		enum location : GLint
		{
		};

		struct shader_attribute
		{
			lak::astring name;
			lak::opengl::index index;
			lak::opengl::location location;
			GLint size;
			// GL_FLOAT, GL_FLOAT_VEC2, GL_FLOAT_VEC3, GL_FLOAT_VEC4,
			// GL_FLOAT_MAT2, GL_FLOAT_MAT3 or GL_FLOAT_MAT4.
			GLenum type;
		};

		struct shader_uniform
		{
			lak::astring name;
			lak::opengl::index index;
			lak::opengl::location location;
			GLint size;
			// There are a lot more types that this can be compared to
			// shader_attribute, see
			// https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/glGetActiveUniform.xhtml
			GLenum type;
		};

		struct program;
		using shared_program = lak::tiny_shared_ptr<program>;
		struct program
		{
		private:
			GLuint _program = 0; // 0 is reserved

		public:
			program() = default;
			program(program &&other);
			~program();
			program &operator=(program &&other);

			program(const program &other)            = delete;
			program &operator=(const program &other) = delete;

			static lak::opengl::result<program> make();
			static lak::opengl::result<program> make(const shader &vertex,
			                                         const shader &fragment);

			static lak::opengl::result<shared_program> make_shared();
			static lak::opengl::result<shared_program> make_shared(
			  const shader &vertex, const shader &fragment);

			lak::opengl::result<program &> attach(const shader &shader);
			lak::opengl::result<program &> link();
			lak::opengl::result<program &> clear();
			lak::opengl::result<program &> use();
			lak::opengl::result<const program &> use() const;

			explicit inline operator bool() const { return _program != 0; }
			inline operator GLuint() const { return _program; }
			inline operator GLint() const { return (GLint)_program; }
			inline GLuint get() const { return _program; }
			lak::opengl::result<lak::optional<lak::astring>> link_error() const;

			lak::vector<shader_attribute> attributes() const;
			lak::vector<shader_uniform> uniforms() const;

			shader_attribute attribute(lak::opengl::index attr_index) const;
			shader_uniform uniform(lak::opengl::index unif_index) const;

			lak::optional<lak::opengl::index> attrib_index(const GLchar *name) const;
			lak::optional<lak::opengl::index> uniform_index(
			  const GLchar *name) const;

			lak::optional<lak::opengl::location> attrib_location(
			  const GLchar *name) const;
			lak::optional<lak::opengl::location> uniform_location(
			  const GLchar *name) const;

			shader_attribute assert_attribute(const GLchar *name) const;
			shader_uniform assert_uniform(const GLchar *name) const;

			lak::opengl::index assert_attrib_index(const GLchar *name) const;
			lak::opengl::index assert_uniform_index(const GLchar *name) const;

			lak::opengl::location assert_attrib_location(const GLchar *name) const;
			lak::opengl::location assert_uniform_location(const GLchar *name) const;

			const program &set_uniform(
			  lak::opengl::index unif_index,
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
