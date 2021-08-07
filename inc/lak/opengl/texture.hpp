#ifndef LAK_OPENGL_TEXTURE_HPP
#define LAK_OPENGL_TEXTURE_HPP

#include <GL/gl3w.h>

#include "lak/image.hpp"

#include <memory>

namespace lak
{
	namespace opengl
	{
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
		};
	}
}

#endif
