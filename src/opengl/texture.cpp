#include "lak/opengl/texture.hpp"

lak::opengl::texture::texture(GLenum target) : _target(target), _size({0, 0})
{
	glGenTextures(1, &_texture);
}

lak::opengl::texture::texture(texture &&other)
: _texture(lak::exchange(other._texture, 0U)),
  _target(other._target),
  _size(lak::exchange(other._size, {0, 0}))
{
}

lak::opengl::texture::~texture() { clear(); }

lak::opengl::texture &lak::opengl::texture::operator=(texture &&other)
{
	lak::swap(_texture, other._texture);
	lak::swap(_target, other._target);
	lak::swap(_size, other._size);
	other.clear();
	return *this;
}

lak::opengl::texture &lak::opengl::texture::init(GLenum target)
{
	if (_texture != 0) clear();
	_target = target;
	glGenTextures(1, &_texture);
	_size = {0, 0};
	return *this;
}

lak::opengl::texture &lak::opengl::texture::clear()
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

lak::opengl::texture &lak::opengl::texture::bind()
{
	glBindTexture(_target, _texture);
	return *this;
}

const lak::opengl::texture &lak::opengl::texture::bind() const
{
	glBindTexture(_target, _texture);
	return *this;
}

lak::opengl::texture &lak::opengl::texture::apply(GLenum pname, GLint value)
{
	glTexParameteri(_target, pname, value);
	return *this;
}

lak::opengl::texture &lak::opengl::texture::apply(GLenum pname, GLint *value)
{
	glTexParameteriv(_target, pname, value);
	return *this;
}

lak::opengl::texture &lak::opengl::texture::applyi(GLenum pname, GLint *value)
{
	glTexParameterIiv(_target, pname, value);
	return *this;
}

lak::opengl::texture &lak::opengl::texture::applyi(GLenum pname, GLuint *value)
{
	glTexParameterIuiv(_target, pname, value);
	return *this;
}

lak::opengl::texture &lak::opengl::texture::apply(GLenum pname, GLfloat value)
{
	glTexParameterf(_target, pname, value);
	return *this;
}

lak::opengl::texture &lak::opengl::texture::apply(GLenum pname, GLfloat *value)
{
	glTexParameterfv(_target, pname, value);
	return *this;
}

lak::opengl::texture &lak::opengl::texture::store_mode(GLenum pname,
                                                       GLint value)
{
	glPixelStorei(pname, value);
	return *this;
}

lak::opengl::texture &lak::opengl::texture::store_mode(GLenum pname,
                                                       GLfloat value)
{
	glPixelStoref(pname, value);
	return *this;
}

lak::opengl::texture &lak::opengl::texture::build(GLint level,
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
