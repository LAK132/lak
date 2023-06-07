#include "lak/opengl/mesh.hpp"
#include "lak/opengl/state.hpp"

#include "lak/debug.hpp"

/* --- buffer --- */

lak::opengl::buffer::buffer(buffer &&other)
: _buffer(lak::exchange(other._buffer, 0U)), _target(other._target)
{
}

lak::opengl::buffer::~buffer()
{
	if (_buffer != 0U)
		lak::opengl::call_checked(glDeleteBuffers, 1, &_buffer).UNWRAP();
}

lak::opengl::buffer &lak::opengl::buffer::operator=(buffer &&other)
{
	lak::swap(_buffer, other._buffer);
	lak::swap(_target, other._target);
	return *this;
}

lak::opengl::buffer lak::opengl::buffer::create(GLenum target)
{
	ASSERT(target == GL_ARRAY_BUFFER || target == GL_ATOMIC_COUNTER_BUFFER ||
	       target == GL_COPY_READ_BUFFER || target == GL_COPY_WRITE_BUFFER ||
	       target == GL_DISPATCH_INDIRECT_BUFFER ||
	       target == GL_DRAW_INDIRECT_BUFFER ||
	       target == GL_ELEMENT_ARRAY_BUFFER || target == GL_PIXEL_PACK_BUFFER ||
	       target == GL_PIXEL_UNPACK_BUFFER || target == GL_QUERY_BUFFER ||
	       target == GL_SHADER_STORAGE_BUFFER || target == GL_TEXTURE_BUFFER ||
	       target == GL_TRANSFORM_FEEDBACK_BUFFER ||
	       target == GL_UNIFORM_BUFFER);
	buffer buf;
	buf._target = target;
	lak::opengl::call_checked(glGenBuffers, 1, &buf._buffer).UNWRAP();
	ASSERT(buf);
	return buf;
}

lak::opengl::buffer &lak::opengl::buffer::bind()
{
	ASSERT(_buffer != 0);
	lak::opengl::call_checked(glBindBuffer, _target, _buffer).UNWRAP();
	return *this;
}

const lak::opengl::buffer &lak::opengl::buffer::bind() const
{
	ASSERT(_buffer != 0);
	lak::opengl::call_checked(glBindBuffer, _target, _buffer).UNWRAP();
	return *this;
}

lak::opengl::buffer &lak::opengl::buffer::set_data(span<const void> data,
                                                   GLenum usage)
{
	ASSERT(usage == GL_STREAM_DRAW || usage == GL_STREAM_READ ||
	       usage == GL_STREAM_COPY || usage == GL_STATIC_DRAW ||
	       usage == GL_STATIC_READ || usage == GL_STATIC_COPY ||
	       usage == GL_DYNAMIC_DRAW || usage == GL_DYNAMIC_READ ||
	       usage == GL_DYNAMIC_COPY);
	lak::opengl::call_checked(glBindVertexArray, 0).UNWRAP();
	bind();
	lak::opengl::call_checked(
	  glBufferData, _target, data.size(), data.data(), usage)
	  .UNWRAP();
	return *this;
}

/* --- vertex_attribute --- */

void lak::opengl::vertex_attribute::apply(GLuint shader_position) const
{
	lak::opengl::call_checked(glEnableVertexAttribArray, shader_position)
	  .UNWRAP();
	// :TODO: Check if we're above OpenGL 3.3 for divisors.
	lak::opengl::call_checked(glVertexAttribDivisor, shader_position, divisor)
	  .UNWRAP();
	lak::opengl::call_checked(glVertexAttribPointer,
	                          shader_position,
	                          size,
	                          type,
	                          normalised,
	                          stride,
	                          offset)
	  .UNWRAP();
}

/* --- vertex_buffer --- */

lak::opengl::vertex_buffer::vertex_buffer(vertex_buffer &&other)
: _vertex_buffer(lak::move(other._vertex_buffer)),
  _index_buffer(lak::move(other._index_buffer)),
  _attributes(lak::move(other._attributes)),
  _vertex_count(lak::exchange(other._vertex_count, 0U))
{
}

lak::opengl::vertex_buffer &lak::opengl::vertex_buffer::operator=(
  vertex_buffer &&other)
{
	lak::swap(_vertex_buffer, other._vertex_buffer);
	lak::swap(_index_buffer, other._index_buffer);
	lak::swap(_attributes, other._attributes);
	lak::swap(_vertex_count, other._vertex_count);
	return *this;
}

lak::opengl::vertex_buffer lak::opengl::vertex_buffer::create(
  span<const void> vertex_data,
  GLenum draw_mode,
  size_t vertex_count,
  span<const vertex_attribute> vertex_attributes,
  GLenum usage)
{
	vertex_buffer buf;

	buf._vertex_buffer = buffer::create(GL_ARRAY_BUFFER);

	buf._vertex_buffer.set_data(vertex_data, usage);

	buf._attributes   = lak::vector<vertex_attribute>(vertex_attributes.begin(),
                                                  vertex_attributes.end());
	buf._vertex_count = vertex_count;
	buf._draw_mode    = draw_mode;

	return buf;
}

lak::opengl::vertex_buffer lak::opengl::vertex_buffer::create(
  span<const void> vertex_data,
  GLenum draw_mode,
  span<const GLuint> index_data,
  span<const vertex_attribute> vertex_attributes,
  GLenum usage)
{
	vertex_buffer buf;

	buf._vertex_buffer = buffer::create(GL_ARRAY_BUFFER);
	buf._index_buffer  = buffer::create(GL_ELEMENT_ARRAY_BUFFER);

	buf._vertex_buffer.set_data(vertex_data, usage);
	buf._index_buffer.set_data(index_data, usage);

	buf._attributes   = lak::vector<vertex_attribute>(vertex_attributes.begin(),
                                                  vertex_attributes.end());
	buf._vertex_count = index_data.size();
	buf._draw_mode    = draw_mode;

	return buf;
}

lak::opengl::shared_vertex_buffer lak::opengl::vertex_buffer::create_shared(
  span<const void> vertex_data,
  GLenum draw_mode,
  size_t vertex_count,
  span<const vertex_attribute> vertex_attributes,
  GLenum usage)
{
	return lak::shared_ptr<vertex_buffer>::make(
	  create(vertex_data, draw_mode, vertex_count, vertex_attributes, usage));
}

lak::opengl::shared_vertex_buffer lak::opengl::vertex_buffer::create_shared(
  span<const void> vertex_data,
  GLenum draw_mode,
  span<const GLuint> index_data,
  span<const vertex_attribute> vertex_attributes,
  GLenum usage)
{
	return lak::shared_ptr<vertex_buffer>::make(
	  create(vertex_data, draw_mode, index_data, vertex_attributes, usage));
}

lak::opengl::vertex_buffer &lak::opengl::vertex_buffer::bind()
{
	_vertex_buffer.bind();
	if (_index_buffer) _index_buffer.bind();
	return *this;
}

lak::opengl::vertex_buffer &lak::opengl::vertex_buffer::apply_attributes(
  span<const GLuint> attribute_positions)
{
	ASSERT(_attributes.size() == attribute_positions.size());
	for (size_t i = 0; i < _attributes.size(); ++i)
		_attributes[i].apply(attribute_positions[i]);
	return *this;
}

void lak::opengl::vertex_buffer::draw(GLuint instances) const
{
	if (_index_buffer)
	{
		lak::opengl::call_checked(glDrawElementsInstanced,
		                          _draw_mode,
		                          GLsizei(_vertex_count),
		                          GL_UNSIGNED_INT,
		                          (GLvoid *)nullptr,
		                          instances)
		  .UNWRAP();
	}
	else
	{
		lak::opengl::call_checked(
		  glDrawArraysInstanced, _draw_mode, 0, GLsizei(_vertex_count), instances)
		  .UNWRAP();
	}
}

void lak::opengl::vertex_buffer::draw_part(const GLuint *offset,
                                           GLsizei count,
                                           GLuint instances) const
{
	ASSERT(_index_buffer);
	ASSERT(((uintptr_t)(offset + count)) / sizeof(GLuint) <= _vertex_count);
	glDrawElementsInstanced(
	  _draw_mode, count, GL_UNSIGNED_INT, offset, instances);
}

const lak::vector<lak::opengl::vertex_attribute> &
lak::opengl::vertex_buffer::attributes() const
{
	return _attributes;
}

/* --- vertex_array --- */

lak::opengl::vertex_array::vertex_array(vertex_array &&other)
: _array(lak::exchange(other._array, 0U))
{
}

lak::opengl::vertex_array::~vertex_array()
{
	if (_array != 0U)
	{
		lak::opengl::call_checked(glDeleteVertexArrays, 1, &_array).UNWRAP();
	}
}

lak::opengl::vertex_array &lak::opengl::vertex_array::operator=(
  vertex_array &&other)
{
	lak::swap(_array, other._array);
	return *this;
}

lak::opengl::vertex_array lak::opengl::vertex_array::create()
{
	vertex_array arr;
	lak::opengl::call_checked(glGenVertexArrays, 1, &arr._array).UNWRAP();
	ASSERT(arr);
	return arr;
}

lak::opengl::vertex_array &lak::opengl::vertex_array::bind()
{
	ASSERT(_array != 0);
	lak::opengl::call_checked(glBindVertexArray, _array).UNWRAP();
	return *this;
}

const lak::opengl::vertex_array &lak::opengl::vertex_array::bind() const
{
	ASSERT(_array != 0);
	lak::opengl::call_checked(glBindVertexArray, _array).UNWRAP();
	return *this;
}

/* --- static_object_part --- */

lak::opengl::static_object_part::static_object_part(static_object_part &&other)
: _vertex_buffer(lak::move(other._vertex_buffer)),
  _shader(lak::move(other._shader)),
  _textures(lak::move(other._textures)),
  _vertex_array(lak::move(other._vertex_array))
{
}

lak::opengl::static_object_part &lak::opengl::static_object_part::operator=(
  static_object_part &&other)
{
	lak::swap(_vertex_array, other._vertex_array);
	lak::swap(_vertex_buffer, other._vertex_buffer);
	lak::swap(_shader, other._shader);
	lak::swap(_textures, other._textures);
	return *this;
}

lak::opengl::static_object_part lak::opengl::static_object_part::create(
  shared_vertex_buffer vertices,
  shared_program shader_program,
  span<const GLuint> attribute_positions,
  span<const lak::shared_ptr<texture>> textures)
{
	static_object_part mesh;

	mesh._vertex_array  = lak::opengl::vertex_array::create();
	mesh._vertex_buffer = vertices;
	mesh._shader        = shader_program;
	mesh._textures =
	  lak::vector<lak::shared_ptr<texture>>(textures.begin(), textures.end());

	mesh._vertex_array.bind();
	mesh._vertex_buffer->bind();
	mesh._vertex_buffer->apply_attributes(attribute_positions);

	return mesh;
}

lak::opengl::static_object_part &lak::opengl::static_object_part::clear()
{
	return *this = static_object_part();
}

void lak::opengl::static_object_part::draw(GLuint instances) const
{
	_shader->use().UNWRAP();

	_vertex_array.bind();

	size_t texture_index = 0;
	for (const auto &texture : _textures)
	{
		lak::opengl::call_checked(glActiveTexture,
		                          GLenum(GL_TEXTURE0 + (texture_index++)))
		  .UNWRAP();
		texture->bind();
	}

	_vertex_buffer->draw(instances);
}

void lak::opengl::static_object_part::draw_part(const GLuint *offset,
                                                GLsizei count,
                                                GLuint instances) const
{
	_shader->use().UNWRAP();

	_vertex_array.bind();

	size_t texture_index = 0;
	for (const auto &texture : _textures)
	{
		lak::opengl::call_checked(glActiveTexture,
		                          GLenum(GL_TEXTURE0 + (texture_index++)))
		  .UNWRAP();
		texture->bind();
	}

	_vertex_buffer->draw_part(offset, count, instances);
}
