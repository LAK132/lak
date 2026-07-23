#include "lak/system/opengl/mesh.hpp"
#include "lak/system/opengl/state.hpp"

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

lak::opengl::buffer lak::opengl::buffer::make(GLenum target)
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

lak::opengl::shared_buffer lak::opengl::buffer::make_shared(GLenum target)
{
	return lak::opengl::shared_buffer::make(make(target));
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

lak::opengl::buffer &lak::opengl::buffer::set_data(lak::span<const void> data,
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

void lak::opengl::vertex_attribute::apply(
  lak::opengl::location shader_location) const
{
	lak::opengl::call_checked(glEnableVertexAttribArray,
	                          static_cast<GLint>(shader_location))
	  .UNWRAP();
	// :TODO: Check if we're above OpenGL 3.3 for divisors.
	lak::opengl::call_checked(
	  glVertexAttribDivisor, static_cast<GLint>(shader_location), divisor)
	  .UNWRAP();
	lak::opengl::call_checked(glVertexAttribPointer,
	                          static_cast<GLint>(shader_location),
	                          size,
	                          type,
	                          normalised,
	                          stride,
	                          reinterpret_cast<void *>(offset))
	  .UNWRAP();
}

/* --- vertex_buffer --- */

lak::opengl::vertex_buffer::vertex_buffer(vertex_buffer &&other)
: _buffer(lak::move(other._buffer)), _attributes(lak::move(other._attributes))
{
}

lak::opengl::vertex_buffer &lak::opengl::vertex_buffer::operator=(
  vertex_buffer &&other)
{
	lak::swap(_buffer, other._buffer);
	lak::swap(_attributes, other._attributes);
	return *this;
}

lak::opengl::vertex_buffer lak::opengl::vertex_buffer::make()
{
	vertex_buffer buf;
	buf._buffer = buffer::make(GL_ARRAY_BUFFER);
	return buf;
}

lak::opengl::shared_vertex_buffer lak::opengl::vertex_buffer::make_shared()
{
	return lak::opengl::shared_vertex_buffer::make(make());
}

lak::opengl::vertex_buffer &lak::opengl::vertex_buffer::bind()
{
	_buffer.bind();
	return *this;
}

const lak::opengl::vertex_buffer &lak::opengl::vertex_buffer::bind() const
{
	_buffer.bind();
	return *this;
}

lak::opengl::vertex_buffer &lak::opengl::vertex_buffer::set_data(
  lak::span<const void> vertex_data, GLenum usage)
{
	_buffer.set_data(vertex_data, usage);
	return *this;
}

lak::opengl::vertex_buffer &lak::opengl::vertex_buffer::set_vertex_attributes(
  lak::vector<lak::opengl::vertex_attribute> vertex_attributes)
{
	_attributes = lak::move(vertex_attributes);
	return *this;
}

lak::opengl::vertex_buffer &
lak::opengl::vertex_buffer::apply_shader_attributes(
  lak::span<const lak::opengl::location> attribute_locations)
{
	ASSERT(_attributes.size() == attribute_locations.size());
	for (size_t i = 0; i < _attributes.size(); ++i)
		_attributes[i].apply(attribute_locations[i]);
	return *this;
}

lak::span<const lak::opengl::vertex_attribute>
lak::opengl::vertex_buffer::vertex_attributes() const
{
	return lak::span(_attributes);
}

/* --- index_buffer --- */

lak::opengl::index_buffer::index_buffer(index_buffer &&other)
: _buffer(lak::move(other._buffer)), _size(lak::exchange(other._size, 0U))
{
}

lak::opengl::index_buffer &lak::opengl::index_buffer::operator=(
  index_buffer &&other)
{
	lak::swap(_buffer, other._buffer);
	lak::swap(_size, other._size);
	return *this;
}

lak::opengl::index_buffer lak::opengl::index_buffer::make()
{
	index_buffer buf;
	buf._buffer = buffer::make(GL_ELEMENT_ARRAY_BUFFER);
	return buf;
}

lak::opengl::shared_index_buffer lak::opengl::index_buffer::make_shared()
{
	return lak::opengl::shared_index_buffer::make(make());
}

lak::opengl::index_buffer &lak::opengl::index_buffer::bind()
{
	_buffer.bind();
	return *this;
}

const lak::opengl::index_buffer &lak::opengl::index_buffer::bind() const
{
	_buffer.bind();
	return *this;
}

lak::opengl::index_buffer &lak::opengl::index_buffer::set_data(
  lak::span<const GLuint> index_data, GLenum usage)
{
	_buffer.set_data(index_data, usage);
	_size = index_data.size();
	return *this;
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

lak::opengl::vertex_array lak::opengl::vertex_array::make()
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
: _vertex_buffers(lak::move(other._vertex_buffers)),
  _index_buffer(lak::move(other._index_buffer)),
  _vertex_count(lak::exchange(other._vertex_count, 0)),
  _shader(lak::move(other._shader)),
  _textures(lak::move(other._textures)),
  _vertex_array(lak::move(other._vertex_array)),
  _draw_mode(lak::exchange(other._draw_mode, GL_TRIANGLES))
{
}

lak::opengl::static_object_part &lak::opengl::static_object_part::operator=(
  static_object_part &&other)
{
	lak::swap(_index_buffer, other._index_buffer);
	lak::swap(_vertex_count, other._vertex_count);
	lak::swap(_vertex_buffers, other._vertex_buffers);
	lak::swap(_shader, other._shader);
	lak::swap(_textures, other._textures);
	lak::swap(_vertex_array, other._vertex_array);
	lak::swap(_draw_mode, other._draw_mode);
	return *this;
}

lak::opengl::static_object_part lak::opengl::static_object_part::make(
  shared_program shader_program,
  GLenum draw_mode,
  lak::array<lak::pair<shared_vertex_buffer,
                       lak::span<const lak::opengl::location>>> buffers,
  GLsizei vertex_count,
  lak::array<lak::pair<lak::opengl::shared_texture, lak::opengl::location>>
    textures)
{
	static_object_part mesh;

	mesh._vertex_buffers.reserve(buffers.size());
	mesh._vertex_count = vertex_count;
	mesh._shader       = lak::move(shader_program);
	mesh._textures     = lak::move(textures);
	mesh._vertex_array = lak::opengl::vertex_array::make();
	mesh._draw_mode    = draw_mode;

	{
		mesh._vertex_array.bind();
		GL_DEFER_CALL(glBindVertexArray, 0);
		for (auto &buff : buffers)
		{
			mesh._vertex_buffers.push_back(buff.first)
			  ->bind()
			  .apply_shader_attributes(buff.second);
		}
	}

	return mesh;
}

lak::opengl::static_object_part lak::opengl::static_object_part::make(
  shared_program shader_program,
  GLenum draw_mode,
  lak::array<lak::pair<shared_vertex_buffer,
                       lak::span<const lak::opengl::location>>> buffers,
  shared_index_buffer index_buff,
  lak::array<lak::pair<lak::opengl::shared_texture, lak::opengl::location>>
    textures)
{
	static_object_part mesh;

	mesh._vertex_buffers.reserve(buffers.size());
	mesh._index_buffer = lak::move(index_buff);
	mesh._shader       = lak::move(shader_program);
	mesh._textures     = lak::move(textures);
	mesh._vertex_array = lak::opengl::vertex_array::make();
	mesh._draw_mode    = draw_mode;

	{
		mesh._vertex_array.bind();
		GL_DEFER_CALL(glBindVertexArray, 0);
		mesh._index_buffer->bind();
		for (auto &buff : buffers)
		{
			mesh._vertex_buffers.push_back(buff.first)
			  ->bind()
			  .apply_shader_attributes(buff.second);
		}
	}

	return mesh;
}

lak::opengl::shared_static_object_part
lak::opengl::static_object_part::make_shared(
  shared_program shader_program,
  GLenum draw_mode,
  lak::array<lak::pair<shared_vertex_buffer,
                       lak::span<const lak::opengl::location>>> buffers,
  GLsizei vertex_count,
  lak::array<lak::pair<lak::opengl::shared_texture, lak::opengl::location>>
    textures)
{
	return lak::opengl::shared_static_object_part::make(
	  make(lak::move(shader_program),
	       draw_mode,
	       lak::move(buffers),
	       vertex_count,
	       lak::move(textures)));
}

lak::opengl::shared_static_object_part
lak::opengl::static_object_part::make_shared(
  shared_program shader_program,
  GLenum draw_mode,
  lak::array<lak::pair<shared_vertex_buffer,
                       lak::span<const lak::opengl::location>>> buffers,
  shared_index_buffer index_buff,
  lak::array<lak::pair<lak::opengl::shared_texture, lak::opengl::location>>
    textures)
{
	return lak::opengl::shared_static_object_part::make(
	  make(lak::move(shader_program),
	       draw_mode,
	       lak::move(buffers),
	       lak::move(index_buff),
	       lak::move(textures)));
}

lak::opengl::static_object_part &lak::opengl::static_object_part::clear()
{
	return *this = static_object_part();
}

void lak::opengl::static_object_part::draw(GLuint instances) const
{
	_shader->use().UNWRAP();

	_vertex_array.bind();
	GL_DEFER_CALL(glBindVertexArray, 0);

	for (size_t texture_index = 0; const auto &[texture, sampler] : _textures)
	{
		lak::opengl::call_checked(glUniform1i, sampler, GLint(texture_index))
		  .UNWRAP();
		lak::opengl::call_checked(glActiveTexture,
		                          GLenum(GL_TEXTURE0 + GLint(texture_index)))
		  .UNWRAP();
		texture->bind();
		++texture_index;
	}

	if (_index_buffer)
	{
		if (instances == 1)
		{
			lak::opengl::call_checked(glDrawElements,
			                          _draw_mode,
			                          GLsizei(_index_buffer->size()),
			                          GL_UNSIGNED_INT,
			                          (GLvoid *)nullptr)
			  .UNWRAP();
		}
		else
		{
			lak::opengl::call_checked(glDrawElementsInstanced,
			                          _draw_mode,
			                          GLsizei(_index_buffer->size()),
			                          GL_UNSIGNED_INT,
			                          (GLvoid *)nullptr,
			                          instances)
			  .UNWRAP();
		}
	}
	else
	{
		if (instances == 1)
		{
			lak::opengl::call_checked(glDrawArrays, _draw_mode, 0, _vertex_count)
			  .UNWRAP();
		}
		else
		{
			lak::opengl::call_checked(
			  glDrawArraysInstanced, _draw_mode, 0, _vertex_count, instances)
			  .UNWRAP();
		}
	}
}

void lak::opengl::static_object_part::draw_part(const GLuint *offset,
                                                GLsizei count,
                                                GLuint instances) const
{
	_shader->use().UNWRAP();

	ASSERT(!!_index_buffer);
	ASSERT(((uintptr_t)(offset + count)) / sizeof(GLuint) <=
	       _index_buffer->size());

	_vertex_array.bind();
	GL_DEFER_CALL(glBindVertexArray, 0);

	for (size_t texture_index = 0; const auto &[texture, sampler] : _textures)
	{
		lak::opengl::call_checked(glUniform1i, sampler, GLint(texture_index))
		  .UNWRAP();
		lak::opengl::call_checked(glActiveTexture,
		                          GLenum(GL_TEXTURE0 + GLint(texture_index)))
		  .UNWRAP();
		texture->bind();
		++texture_index;
	}

	if (instances == 1)
	{
		lak::opengl::call_checked(
		  glDrawElements, _draw_mode, count, GL_UNSIGNED_INT, offset)
		  .UNWRAP();
	}
	else
	{
		lak::opengl::call_checked(glDrawElementsInstanced,
		                          _draw_mode,
		                          count,
		                          GL_UNSIGNED_INT,
		                          offset,
		                          instances)
		  .UNWRAP();
	}
}
