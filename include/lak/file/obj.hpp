#ifndef LAK_FILE_OBJ_HPP
#define LAK_FILE_OBJ_HPP

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

		struct group
		{
			lak::u8string name;
			size_t face_offset = 0U;
			size_t face_count  = 0U;
			size_t line_offset = 0U;
			size_t line_count  = 0U;

			void visit(lak::span<const lak::obj::face> faces, auto &&func) const
			{
				for (const auto &f : faces.subspan(face_offset, face_count)) func(f);
			}

			void visit(lak::span<const lak::obj::line> lines, auto &&func) const
			{
				for (const auto &l : lines.subspan(line_offset, line_count)) func(l);
			}
		};

		struct object
		{
			lak::u8string name;
			size_t offset = 0U;
			size_t count  = 0U;

			void visit(lak::span<const lak::obj::group> groups, auto &&func) const
			{
				for (const auto &g : groups.subspan(offset, count)) func(g);
			}
		};

		enum struct smooth_shading
		{
			on,
			off,
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

			lak::array<lak::obj::group> groups;

			lak::array<lak::obj::object> objects;
		};
	}

	namespace dsl
	{
		struct obj_t
		{
			static constexpr bool is_pure_match = false;

			using value_type = lak::obj::obj;

			lak::dsl::result<value_type> parse(lak::u8string_view str) const;
		};

		inline constexpr obj_t obj;

		static_assert(lak::dsl::concepts::parser<obj_t>);
	}
}

#endif
