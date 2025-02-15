#ifndef LAK_STRUCTURE_OBJ_HPP
#define LAK_STRUCTURE_OBJ_HPP

#include "lak/array.hpp"
#include "lak/binary_reader.hpp"
#include "lak/dsl/dsl.hpp"
#include "lak/numeric.hpp"

/*
# comment

# vertex coord. w optional, defaults to 1.0
v x y z w

# vertex texture coord. v w optional, defaults to 0.0
vt u v w

# vertex normal.
vn x y z

# param space vertex.
vp u v w

# face.
f v1/vt1/vn1 v2/vt2/vn2 v3/vt3/vn3 ...

# polyline.
l v1 v2 v3 ...
*/

namespace lak
{
	namespace obj
	{
		struct vertex_coord
		{
			double x;
			double y;
			double z;
			double w = 1.0;
		};

		struct texture_coord
		{
			double u;
			double v = 0.0;
			double w = 0.0;
		};

		struct vertex_normal
		{
			double x;
			double y;
			double z;
		};

		struct face_coord
		{
			size_t v;
			size_t vt = lak::dynamic_extent;
			size_t vn = lak::dynamic_extent;

			void visit(lak::span<const lak::obj::vertex_coord> vertex_coords,
			           lak::span<const lak::obj::texture_coord> texture_coords,
			           lak::span<const lak::obj::vertex_normal> vertex_normals,
			           auto &&func) const
			{
				func(vertex_coords[v],
				     vt != lak::dynamic_extent ? &texture_coords[vt] : nullptr,
				     vn != lak::dynamic_extent ? &vertex_normals[vn] : nullptr);
			}
		};

		struct face
		{
			size_t offset;
			size_t count;

			void visit(lak::span<const lak::obj::vertex_coord> vertex_coords,
			           lak::span<const lak::obj::texture_coord> texture_coords,
			           lak::span<const lak::obj::vertex_normal> vertex_normals,
			           lak::span<const lak::obj::face_coord> indices,
			           auto &&func) const
			{
				for (const auto &i : indices.subspan(offset, count))
					i.visit(vertex_coords, texture_coords, vertex_normals, func);
			}

			void visit_fan(lak::span<const lak::obj::vertex_coord> vertex_coords,
			               lak::span<const lak::obj::texture_coord> texture_coords,
			               lak::span<const lak::obj::vertex_normal> vertex_normals,
			               lak::span<const lak::obj::face_coord> indices,
			               auto &&func) const
			{
				if (count <= 3U)
					visit(vertex_coords, texture_coords, vertex_normals, indices, func);
				else
				{
					for (size_t i = 1U; i + 1U < count; ++i)
					{
						indices[offset].visit(
						  vertex_coords, texture_coords, vertex_normals, func);
						indices[offset + i].visit(
						  vertex_coords, texture_coords, vertex_normals, func);
						indices[offset + i + 1U].visit(
						  vertex_coords, texture_coords, vertex_normals, func);
					}
				}
			}
		};

		struct line_coord
		{
			size_t v;

			void visit(lak::span<const lak::obj::vertex_coord> vertex_coords,
			           auto &&func) const
			{
				func(vertex_coords[v]);
			}
		};

		struct line
		{
			size_t offset;
			size_t count;

			void visit(lak::span<const lak::obj::vertex_coord> vertex_coords,
			           lak::span<const lak::obj::line_coord> indices,
			           auto &&func) const
			{
				for (const auto &i : indices.subspan(offset, count))
					i.visit(vertex_coords, func);
			}
		};

		struct obj
		{
			lak::array<lak::obj::vertex_coord> vertex_coords;
			lak::array<lak::obj::texture_coord> texture_coords;
			lak::array<lak::obj::vertex_normal> vertex_normals;
			lak::array<lak::obj::face_coord> face_coords;
			lak::array<lak::obj::face> faces;
			lak::array<lak::obj::line_coord> line_coords;
			lak::array<lak::obj::line> lines;

			template<lak::endian E>
			lak::error_codes<lak::out_of_data_error,
			                 lak::value_out_of_range_error,
			                 lak::string_to_numeric_error,
			                 lak::dsl::parse_error>
			read(lak::binary_reader &strm);
		};
	}
}

#include "obj.inl"

#endif
