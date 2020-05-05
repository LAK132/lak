#include "lak/opengl/texture.hpp"

namespace lak
{
  namespace opengl
  {
    texture::texture(GLenum target) : _target(target), _size({0, 0})
    {
      glGenTextures(1, &_texture);
    }

    texture::texture(texture &&other)
    {
      std::swap(_texture, other._texture);
      std::swap(_size, other._size);
    }

    texture::~texture() { clear(); }

    texture &texture::operator=(texture &&other)
    {
      std::swap(_texture, other._texture);
      std::swap(_size, other._size);
      return *this;
    }

    texture &texture::init(GLenum target)
    {
      if (_texture != 0) clear();
      _target = target;
      glGenTextures(1, &_texture);
      _size = {0, 0};
      return *this;
    }

    texture &texture::clear()
    {
      if (_texture != 0)
      {
        glDeleteTextures(1, &_texture);
        _texture = 0;
      }
      _target = GL_TEXTURE_2D;
      _size   = {0, 0};
      return *this;
    }

    texture &texture::bind()
    {
      glBindTexture(_target, _texture);
      return *this;
    }

    const texture &texture::bind() const
    {
      glBindTexture(_target, _texture);
      return *this;
    }

    texture &texture::apply(GLenum pname, GLint value)
    {
      glTexParameteri(_target, pname, value);
      return *this;
    }

    texture &texture::apply(GLenum pname, GLint *value)
    {
      glTexParameteriv(_target, pname, value);
      return *this;
    }

    texture &texture::applyi(GLenum pname, GLint *value)
    {
      glTexParameterIiv(_target, pname, value);
      return *this;
    }

    texture &texture::applyi(GLenum pname, GLuint *value)
    {
      glTexParameterIuiv(_target, pname, value);
      return *this;
    }

    texture &texture::apply(GLenum pname, GLfloat value)
    {
      glTexParameterf(_target, pname, value);
      return *this;
    }

    texture &texture::apply(GLenum pname, GLfloat *value)
    {
      glTexParameterfv(_target, pname, value);
      return *this;
    }

    texture &texture::store_mode(GLenum pname, GLint value)
    {
      glPixelStorei(pname, value);
      return *this;
    }

    texture &texture::store_mode(GLenum pname, GLfloat value)
    {
      glPixelStoref(pname, value);
      return *this;
    }

    texture &texture::build(GLint level,
                            GLint format,
                            vec2<GLsizei> size,
                            GLint border,
                            GLenum pixel_format,
                            GLenum color_type,
                            const GLvoid *pixels)
    {
      _size = size;
      glTexImage2D(_target,
                   level,
                   format,
                   size.x,
                   size.y,
                   border,
                   pixel_format,
                   color_type,
                   pixels);
      return *this;
    }
  }
}
