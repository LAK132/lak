#ifndef LAK_MESH_HPP
#define LAK_MESH_HPP

#include "lak/opengl/shader.hpp"
#include "lak/opengl/texture.hpp"

#include "lak/span.hpp"
#include "lak/string.hpp"

#include <memory>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace lak
{
	namespace opengl
	{
		// Buffers are effectively blocks of data that are stored on the GPU. They
		// can be shared between mutiple Vertex Array Objects. Vertex Array Objects
		// remember which Buffers were bound to them, so Buffers do no need to be
		// rebound when rebinding a Vertex Array Object.
		struct buffer;
		using shared_buffer = std::shared_ptr<buffer>;
		struct buffer
		{
		private:
			GLuint _buffer = 0U;
			GLenum _target;
			// glBindBuffer documentation implies that 0 is reserved.

		public:
			buffer() = default;
			buffer(buffer &&other);
			~buffer();
			buffer &operator=(buffer &&other);

			buffer(const buffer &other) = delete;
			buffer &operator=(const buffer &other) = delete;

			static buffer create(GLenum target);

			buffer &bind();
			// set_data unbinds the current vertex array and binds this buffer.
			buffer &set_data(span<const void> data, GLenum usage);

			inline operator bool() const { return _buffer != 0U; }
			inline operator GLuint() const { return _buffer; }
			inline operator GLint() const { return (GLint)_buffer; }
			inline GLuint get() const { return _buffer; }
		};

		struct vertex_attribute
		{
			GLuint size; // 1, 2, 3 or 4.
			GLenum type; // GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT,
			             // GL_INT, GL_UNSIGNED_INT, GL_HALF_FLOAT, GL_FLOAT,
			             // GL_DOUBLE, GL_FIXED, GL_INT_2_10_10_10_REV,
			             // GL_UNSIGNED_INT_2_10_10_10_REV or
			             // GL_UNSIGNED_INT_10F_11F_11F_REV.
			GLboolean normalised;
			GLsizei stride;
			void *offset;
			GLuint divisor;

			// apply vertex attribute to the bound Vertex Array Object.
			void apply(GLuint shader_position) const;
		};

		// vertex_buffer is a Buffer that can only be bound to GL_ARRAY_BUFFER of a
		// Vertex Array Object. vertex_buffer tracks the vertex attributes of the
		// data it holds.
		struct vertex_buffer;
		using shared_vertex_buffer = std::shared_ptr<vertex_buffer>;
		struct vertex_buffer
		{
		private:
			buffer _vertex_buffer;
			buffer _index_buffer;
			std::vector<vertex_attribute> _attributes;
			// number of elements in _index_buffer if it exists, otherwise the number
			// of elements in _vertex_buffer.
			size_t _vertex_count = 0;
			GLenum _draw_mode    = GL_TRIANGLES;

		public:
			vertex_buffer() = default;
			vertex_buffer(vertex_buffer &&other);
			vertex_buffer &operator=(vertex_buffer &&other);

			vertex_buffer(const vertex_buffer &other) = delete;
			vertex_buffer &operator=(const vertex_buffer &other) = delete;

			// create will unbind the current Vertex Array Object.
			static vertex_buffer create(
			  span<const void> vertex_data,
			  GLenum draw_mode,
			  size_t vertex_count,
			  span<const vertex_attribute> vertex_attributes,
			  GLenum usage);
			static vertex_buffer create(
			  span<const void> vertex_data,
			  GLenum draw_mode,
			  span<const GLuint> index_data,
			  span<const vertex_attribute> vertex_attributes,
			  GLenum usage);

			static shared_vertex_buffer create_shared(
			  span<const void> vertex_data,
			  GLenum draw_mode,
			  size_t vertex_count,
			  span<const vertex_attribute> vertex_attributes,
			  GLenum usage);
			static shared_vertex_buffer create_shared(
			  span<const void> vertex_data,
			  GLenum draw_mode,
			  span<const GLuint> index_data,
			  span<const vertex_attribute> vertex_attributes,
			  GLenum usage);

			vertex_buffer &bind();
			// apply vertex attributes to the bound Vertex Array Object.
			vertex_buffer &apply_attributes(span<const GLuint> attribute_positions);

			void draw(GLuint instances = 1);
			void draw_part(const GLuint *offset,
			               GLsizei count,
			               GLuint instances = 1);

			const std::vector<vertex_attribute> &attributes() const;

			inline operator bool() const { return _vertex_buffer; }
		};

		struct vertex_array
		{
		private:
			GLuint _array = 0U;

		public:
			vertex_array() = default;
			vertex_array(vertex_array &&other);
			~vertex_array();
			vertex_array &operator=(vertex_array &&other);

			vertex_array(const vertex_array &other) = delete;
			vertex_array &operator=(const vertex_array &other) = delete;

			static vertex_array create();

			vertex_array &bind();

			inline operator bool() const { return _array != 0U; }
			inline operator GLuint() const { return _array; }
			inline operator GLint() const { return (GLint)_array; }
			inline GLuint get() const { return _array; }
		};

		struct static_object_part
		{
		private:
			shared_vertex_buffer _vertex_buffer;
			shared_program _shader;
			std::vector<std::shared_ptr<texture>> _textures;
			vertex_array _vertex_array;

		public:
			static_object_part() = default;
			static_object_part(static_object_part &&other);
			static_object_part &operator=(static_object_part &&other);

			static static_object_part create(
			  shared_vertex_buffer vertices,
			  shared_program shader_program,
			  span<const GLuint> attribute_positions,
			  span<const std::shared_ptr<texture>> textures);

			static_object_part &clear();

			static_object_part(const static_object_part &other) = delete;
			static_object_part &operator=(const static_object_part &other) = delete;

			void draw(GLuint instanced = 1);
			void draw_part(const GLuint *offset,
			               GLsizei count,
			               GLuint instances = 1);

			inline operator bool() const { return _vertex_array; }
		};
	}
}

#endif