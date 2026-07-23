#ifndef LAK_SYSTEM_OPENGL_TEXTURE_HPP
#define LAK_SYSTEM_OPENGL_TEXTURE_HPP

#include "lak/system/opengl/gl3w.hpp"

#include "lak/image.hpp"
#include "lak/memory.hpp"

#include <memory>

namespace lak
{
	namespace opengl
	{
		struct texture;
		using shared_texture = lak::tiny_shared_ptr<lak::opengl::texture>;
		struct texture
		{
		private:
			GLuint _texture     = 0; // 0 is reserved, use it as null texture
			GLenum _target      = GL_TEXTURE_2D;
			vec2<GLsizei> _size = {0, 0};

		public:
			texture() = default;
			texture(GLenum target);
			texture(texture &&other);
			~texture();
			texture &operator=(texture &&other);

			static shared_texture make_shared();
			static shared_texture make_shared(GLenum target);

			texture &init(GLenum target);
			texture &clear();

			inline GLuint get() const { return _texture; }
			inline const vec2<GLsizei> &size() const { return _size; }

			texture &bind();
			const texture &bind() const;

			texture &apply(GLenum pname, GLint value);
			texture &apply(GLenum pname, GLint *value);

			texture &applyi(GLenum pname, GLint *value);
			texture &applyi(GLenum pname, GLuint *value);

			texture &apply(GLenum pname, GLfloat value);
			texture &apply(GLenum pname, GLfloat *value);

			texture &store_mode(GLenum pname, GLint value);
			texture &store_mode(GLenum pname, GLfloat value);

			texture &build(GLint level,
			               GLint format,
			               vec2<GLsizei> size,
			               GLint border,
			               GLenum pixel_format,
			               GLenum color_type,
			               const GLvoid *pixels);

			texture &rebuild(GLint level,
			                 vec2<GLint> pos,
			                 vec2<GLsizei> size,
			                 GLenum pixel_format,
			                 GLenum color_type,
			                 const GLvoid *pixels);
		};
	}
}

#endif
