#include "lak/opengl/mesh.hpp"
#include "lak/opengl/state.hpp"

#include "lak/debug.hpp"

namespace lak
{
  namespace opengl
  {
    /* --- buffer --- */

    buffer::buffer(buffer &&other)
    : _buffer(std::exchange(other._buffer, 0U)), _target(other._target)
    {
    }

    buffer::~buffer()
    {
      if (_buffer != 0U)
      {
        glDeleteBuffers(1, &_buffer);
        ASSERT(lak::opengl::check_error());
      }
    }

    buffer &buffer::operator=(buffer &&other)
    {
      std::swap(_buffer, other._buffer);
      std::swap(_target, other._target);
      return *this;
    }

    buffer buffer::create(GLenum target)
    {
      ASSERT(
        target == GL_ARRAY_BUFFER || target == GL_ATOMIC_COUNTER_BUFFER ||
        target == GL_COPY_READ_BUFFER || target == GL_COPY_WRITE_BUFFER ||
        target == GL_DISPATCH_INDIRECT_BUFFER ||
        target == GL_DRAW_INDIRECT_BUFFER ||
        target == GL_ELEMENT_ARRAY_BUFFER || target == GL_PIXEL_PACK_BUFFER ||
        target == GL_PIXEL_UNPACK_BUFFER || target == GL_QUERY_BUFFER ||
        target == GL_SHADER_STORAGE_BUFFER || target == GL_TEXTURE_BUFFER ||
        target == GL_TRANSFORM_FEEDBACK_BUFFER || target == GL_UNIFORM_BUFFER);
      buffer buf;
      buf._target = target;
      glGenBuffers(1, &buf._buffer);
      ASSERT(lak::opengl::check_error());
      ASSERT(buf);
      return buf;
    }

    buffer &buffer::bind()
    {
      ASSERT(_buffer != 0);
      glBindBuffer(_target, _buffer);
      ASSERT(lak::opengl::check_error());
      return *this;
    }

    buffer &buffer::set_data(span<const void> data, GLenum usage)
    {
      ASSERT(usage == GL_STREAM_DRAW || usage == GL_STREAM_READ ||
             usage == GL_STREAM_COPY || usage == GL_STATIC_DRAW ||
             usage == GL_STATIC_READ || usage == GL_STATIC_COPY ||
             usage == GL_DYNAMIC_DRAW || usage == GL_DYNAMIC_READ ||
             usage == GL_DYNAMIC_COPY);
      glBindVertexArray(0);
      ASSERT(lak::opengl::check_error());
      bind();
      glBufferData(_target, data.size(), data.data(), usage);
      ASSERT(lak::opengl::check_error());
      return *this;
    }

    /* --- vertex_attribute --- */

    void vertex_attribute::apply(GLuint shader_position) const
    {
      glEnableVertexAttribArray(shader_position);
      ASSERT(lak::opengl::check_error());
      // :TODO: Check if we're above OpenGL 3.3 for divisors.
      glVertexAttribDivisor(shader_position, divisor);
      ASSERT(lak::opengl::check_error());
      glVertexAttribPointer(
        shader_position, size, type, normalised, stride, offset);
      ASSERT(lak::opengl::check_error());
    }

    /* --- vertex_buffer --- */

    vertex_buffer::vertex_buffer(vertex_buffer &&other)
    : _vertex_buffer(std::move(other._vertex_buffer)),
      _index_buffer(std::move(other._index_buffer)),
      _attributes(std::move(other._attributes)),
      _vertex_count(std::exchange(other._vertex_count, 0U))
    {
    }

    vertex_buffer &vertex_buffer::operator=(vertex_buffer &&other)
    {
      std::swap(_vertex_buffer, other._vertex_buffer);
      std::swap(_index_buffer, other._index_buffer);
      std::swap(_attributes, other._attributes);
      std::swap(_vertex_count, other._vertex_count);
      return *this;
    }

    vertex_buffer vertex_buffer::create(
      span<const void> vertex_data,
      GLenum draw_mode,
      size_t vertex_count,
      span<const vertex_attribute> vertex_attributes,
      GLenum usage)
    {
      vertex_buffer buf;

      buf._vertex_buffer = buffer::create(GL_ARRAY_BUFFER);

      buf._vertex_buffer.set_data(vertex_data, usage);

      buf._attributes = std::vector<vertex_attribute>(
        vertex_attributes.begin(), vertex_attributes.end());
      buf._vertex_count = vertex_count;
      buf._draw_mode    = draw_mode;

      return buf;
    }

    vertex_buffer vertex_buffer::create(
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

      buf._attributes = std::vector<vertex_attribute>(
        vertex_attributes.begin(), vertex_attributes.end());
      buf._vertex_count = index_data.size();
      buf._draw_mode    = draw_mode;

      return buf;
    }

    shared_vertex_buffer vertex_buffer::create_shared(
      span<const void> vertex_data,
      GLenum draw_mode,
      size_t vertex_count,
      span<const vertex_attribute> vertex_attributes,
      GLenum usage)
    {
      return std::make_shared<vertex_buffer>(create(
        vertex_data, draw_mode, vertex_count, vertex_attributes, usage));
    }

    shared_vertex_buffer vertex_buffer::create_shared(
      span<const void> vertex_data,
      GLenum draw_mode,
      span<const GLuint> index_data,
      span<const vertex_attribute> vertex_attributes,
      GLenum usage)
    {
      return std::make_shared<vertex_buffer>(
        create(vertex_data, draw_mode, index_data, vertex_attributes, usage));
    }

    vertex_buffer &vertex_buffer::bind()
    {
      _vertex_buffer.bind();
      if (_index_buffer) _index_buffer.bind();
      return *this;
    }

    vertex_buffer &vertex_buffer::apply_attributes(
      span<const GLuint> attribute_positions)
    {
      ASSERT(_attributes.size() == attribute_positions.size());
      for (size_t i = 0; i < _attributes.size(); ++i)
        _attributes[i].apply(attribute_positions[i]);
      return *this;
    }

    void vertex_buffer::draw(GLuint instances)
    {
      if (_index_buffer)
      {
        glDrawElementsInstanced(
          _draw_mode, _vertex_count, GL_UNSIGNED_INT, NULL, instances);
        ASSERT(lak::opengl::check_error());
      }
      else
      {
        glDrawArraysInstanced(_draw_mode, 0, _vertex_count, instances);
        ASSERT(lak::opengl::check_error());
      }
    }

    void vertex_buffer::draw_part(const GLuint *offset,
                                  GLsizei count,
                                  GLuint instances)
    {
      ASSERT(_index_buffer);
      ASSERT(((uintptr_t)(offset + count)) / sizeof(GLuint) <= _vertex_count);
      glDrawElementsInstanced(
        _draw_mode, count, GL_UNSIGNED_INT, offset, instances);
    }

    const std::vector<vertex_attribute> &vertex_buffer::attributes() const
    {
      return _attributes;
    }

    /* --- vertex_array --- */

    vertex_array::vertex_array(vertex_array &&other)
    : _array(std::exchange(other._array, 0U))
    {
    }

    vertex_array::~vertex_array()
    {
      if (_array != 0U)
      {
        glDeleteVertexArrays(1, &_array);
        ASSERT(lak::opengl::check_error());
      }
    }

    vertex_array &vertex_array::operator=(vertex_array &&other)
    {
      std::swap(_array, other._array);
      return *this;
    }

    vertex_array vertex_array::create()
    {
      vertex_array arr;
      glGenVertexArrays(1, &arr._array);
      ASSERT(lak::opengl::check_error());
      ASSERT(arr);
      return arr;
    }

    vertex_array &vertex_array::bind()
    {
      ASSERT(_array != 0);
      glBindVertexArray(_array);
      ASSERT(lak::opengl::check_error());
      return *this;
    }

    /* --- static_object_part --- */

    static_object_part::static_object_part(static_object_part &&other)
    : _vertex_array(std::move(other._vertex_array)),
      _vertex_buffer(std::move(other._vertex_buffer)),
      _shader(std::move(other._shader)),
      _textures(std::move(other._textures))
    {
    }

    static_object_part &static_object_part::operator=(
      static_object_part &&other)
    {
      std::swap(_vertex_array, other._vertex_array);
      std::swap(_vertex_buffer, other._vertex_buffer);
      std::swap(_shader, other._shader);
      std::swap(_textures, other._textures);
      return *this;
    }

    static_object_part static_object_part::create(
      shared_vertex_buffer vertices,
      shared_program shader_program,
      span<const GLuint> attribute_positions,
      span<const std::shared_ptr<texture>> textures)
    {
      static_object_part mesh;

      mesh._vertex_array  = lak::opengl::vertex_array::create();
      mesh._vertex_buffer = vertices;
      mesh._shader        = shader_program;
      mesh._textures = std::vector<std::shared_ptr<texture>>(textures.begin(),
                                                             textures.end());

      mesh._vertex_array.bind();
      mesh._vertex_buffer->bind();
      mesh._vertex_buffer->apply_attributes(attribute_positions);

      return mesh;
    }

    static_object_part &static_object_part::clear()
    {
      return *this = static_object_part();
    }

    void static_object_part::draw(GLuint instances)
    {
      _shader->use();

      _vertex_array.bind();

      size_t texture_index = 0;
      for (const auto &texture : _textures)
      {
        glActiveTexture(GL_TEXTURE0 + (texture_index++));
        ASSERT(lak::opengl::check_error());
        texture->bind();
      }

      _vertex_buffer->draw(instances);
    }

    void static_object_part::draw_part(const GLuint *offset,
                                       GLsizei count,
                                       GLuint instances)
    {
      _shader->use();

      _vertex_array.bind();

      size_t texture_index = 0;
      for (const auto &texture : _textures)
      {
        glActiveTexture(GL_TEXTURE0 + (texture_index++));
        ASSERT(lak::opengl::check_error());
        texture->bind();
      }

      _vertex_buffer->draw_part(offset, count, instances);
    }
  }
}
