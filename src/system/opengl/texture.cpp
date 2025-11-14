#include "lak/system/opengl/texture.hpp"

#include "lak/system/opengl/state.hpp"

lak::opengl::texture::texture(GLenum target) : _target(target), _size({0, 0})
{
	lak::opengl::call_checked(glGenTextures, 1, &_texture).UNWRAP();
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
	lak::opengl::call_checked(glGenTextures, 1, &_texture).UNWRAP();
	_size = {0, 0};
	return *this;
}

lak::opengl::texture &lak::opengl::texture::clear()
{
	if (_texture != 0)
	{
		lak::opengl::call_checked(glDeleteTextures, 1, &_texture).UNWRAP();
		_texture = 0;
	}
	_target = GL_TEXTURE_2D;
	_size   = {0, 0};
	return *this;
}

lak::opengl::texture &lak::opengl::texture::bind()
{
	lak::opengl::call_checked(glBindTexture, _target, _texture).UNWRAP();
	return *this;
}

const lak::opengl::texture &lak::opengl::texture::bind() const
{
	lak::opengl::call_checked(glBindTexture, _target, _texture).UNWRAP();
	return *this;
}

lak::opengl::texture &lak::opengl::texture::apply(GLenum pname, GLint value)
{
	lak::opengl::call_checked(glTexParameteri, _target, pname, value).UNWRAP();
	return *this;
}

lak::opengl::texture &lak::opengl::texture::apply(GLenum pname, GLint *value)
{
	lak::opengl::call_checked(glTexParameteriv, _target, pname, value).UNWRAP();
	return *this;
}

lak::opengl::texture &lak::opengl::texture::applyi(GLenum pname, GLint *value)
{
	lak::opengl::call_checked(glTexParameterIiv, _target, pname, value).UNWRAP();
	return *this;
}

lak::opengl::texture &lak::opengl::texture::applyi(GLenum pname, GLuint *value)
{
	lak::opengl::call_checked(glTexParameterIuiv, _target, pname, value)
	  .UNWRAP();
	return *this;
}

lak::opengl::texture &lak::opengl::texture::apply(GLenum pname, GLfloat value)
{
	lak::opengl::call_checked(glTexParameterf, _target, pname, value).UNWRAP();
	return *this;
}

lak::opengl::texture &lak::opengl::texture::apply(GLenum pname, GLfloat *value)
{
	lak::opengl::call_checked(glTexParameterfv, _target, pname, value).UNWRAP();
	return *this;
}

lak::opengl::texture &lak::opengl::texture::store_mode(GLenum pname,
                                                       GLint value)
{
	lak::opengl::call_checked(glPixelStorei, pname, value).UNWRAP();
	return *this;
}

lak::opengl::texture &lak::opengl::texture::store_mode(GLenum pname,
                                                       GLfloat value)
{
	lak::opengl::call_checked(glPixelStoref, pname, value).UNWRAP();
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
	lak::opengl::call_checked(glTexImage2D,
	                          _target,
	                          level,
	                          format,
	                          size.x,
	                          size.y,
	                          border,
	                          pixel_format,
	                          color_type,
	                          pixels)
	  .UNWRAP();
	return *this;
}
